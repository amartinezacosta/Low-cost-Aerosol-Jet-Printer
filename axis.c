#include <FreeRTOS.h>
#include <task.h>
#include <ti/drivers/GPIO.h>
#include <string.h>
#include <mqueue.h>
#include <semaphore.h>
#include <pthread.h>

#include "device.h"
#include "timer.h"
#include "Board.h"
#include "axis.h"
#include "USBCDCD.h"

void axis0_timeout(void);
void axis1_timeout(void);
void axis2_timeout(void);

void *axis0_task(void *args);
void *axis1_task(void *args);
void *axis2_task(void *args);

typedef struct
{
   char *step_pin;
   char *dir_pin;
   char *en_pin;
   char *timer;
}axis_hwr_t;

typedef struct
{
    mqd_t queue;
    axis_hwr_t *hwr;
    sem_t *self;
    sem_t *sync0;
    sem_t *sync1;
    pthread_t axis_thread;
    axis_req_t *current;
    void*(*axis_task)(void*);
    void(*timer_fxn)(void);
    int tid;
    int sid;
    int did;
    int eid;
    char *queue_name;
    int status;
    long int count;
}axis_t;

/*STEPPER MOTOR BUSY SEMAPHORES----------------------------------------*/
sem_t self[AXIS_COUNT];
sem_t sync0[AXIS_COUNT];
sem_t sync1[AXIS_COUNT];
/*--------------------------------------------------------------------*/

/*AXIS HARDWARE OBJECTS-----------------------------------------------*/
axis_hwr_t axis_hwr[] =
{
 {"/dev/dio0", "/dev/dio1", "/dev/dio2", "/dev/ti0"},
 {"/dev/dio3", "/dev/dio4", "/dev/dio5", "/dev/ti1"},
 {"/dev/dio6", "/dev/dio7", "/dev/dio8", "/dev/ti2"}

};
/*--------------------------------------------------------------------*/


/*AXIS_DEVICE_OBJECTS--------------------------------------------------------------------------------*/
axis_t axis_device[] =
{
 {NULL, &axis_hwr[AXIS0], &self[AXIS0], &sync0[AXIS0], &sync1[AXIS0], NULL, NULL, axis0_task, axis0_timeout, -1, -1, -1, -1, "axis0_queue", 0},
 {NULL, &axis_hwr[AXIS1], &self[AXIS1], &sync0[AXIS1], &sync1[AXIS1], NULL, NULL, axis1_task, axis1_timeout, -1, -1, -1, -1, "axis1_queue", 0},
 {NULL, &axis_hwr[AXIS2], &self[AXIS2], &sync0[AXIS2], &sync1[AXIS2], NULL, NULL, axis2_task, axis2_timeout, -1, -1, -1, -1, "axis2_queue", 0}
};
/*---------------------------------------------------------------------------------------------------*/

int axis_open(int device)
{
    axis_t *a = &axis_device[device];

    //setup timer
    a->tid = open(a->hwr->timer);
    ioctl(a->tid, TIMER_SET_CALLBACK, a->timer_fxn);

    //setup digital Outputs
    a->sid = open(a->hwr->step_pin);
    a->did = open(a->hwr->dir_pin);
    a->eid = open(a->hwr->en_pin);

    write(a->eid, NULL, 0);

    a->count = 0;

    //Create Queue
    mq_attr attr;
    attr.mq_curmsgs = 0;
    attr.mq_msgsize = sizeof(axis_req_t);
    attr.mq_maxmsg = MAX_QUEUE_REQ;
    attr.mq_flags = 0;
    a->queue = mq_open(a->queue_name, O_CREAT|O_RDWR, 0644, &attr);

    //Create binary semaphore to wait for motion to finish
    sem_init(a->self, 0, 0);
    sem_init(a->sync0, 0, 0);
    sem_init(a->sync1, 0, 0);

    //Create thread and initialize task
    pthread_attr_t      attrs;
    struct sched_param  priParam;

    pthread_attr_init(&attrs);
    pthread_attr_setschedparam(&attrs, &priParam);
    attrs.priority = 2;

    pthread_create(&a->axis_thread, &attrs, a->axis_task, NULL);

    return device;
}

int axis_write(int device, char *buf, int len)
{
    axis_t *a = &axis_device[device];

    mq_send(a->queue, buf, len, 0);

    return 0;
}

int axis_read(int device, char *buf, int len)
{
    axis_t *a = &axis_device[device];

    return a->status;
}

int axis_ioctl(int device, int req, void *args)
{
    axis_t *a = &axis_device[device];

    switch(req)
    {
    case GET_COUNT:
        *((int*)args) = a->count;
        break;
    }

    return 0;
}

/*PULSE GENERATORS-------------------------------------------------------------*/
void axis0_timeout(void)
{
    axis_t *a0 = &axis_device[AXIS0];
    axis_t *a1 = &axis_device[AXIS1];
    axis_t *a2 = &axis_device[AXIS2];

    if(a0->current->count + 1 == a0->current->steps)
    {
        write(a0->sid, NULL, 0);
        __delay_cycles(200);
        write(a0->sid, NULL, 1);

        //a0->current->count++;

        ioctl(a0->tid, STOP, NULL);
        sem_post(a0->self);
        sem_post(a1->sync0);
        sem_post(a2->sync1);
    }
    else if(a0->current->steps == 0)
    {
        ioctl(a0->tid, STOP, NULL);

        sem_post(a0->self);
        sem_post(a1->sync0);
        sem_post(a2->sync1);
    }
    else
    {
        write(a0->sid, NULL, 0);
        __delay_cycles(200);
        write(a0->sid, NULL, 1);

        a0->current->count++;
    }
}

void axis1_timeout(void)
{
    axis_t *a1 = &axis_device[AXIS1];
    axis_t *a0 = &axis_device[AXIS0];
    axis_t *a2 = &axis_device[AXIS2];

    if(a1->current->count + 1 == a1->current->steps)
    {
        write(a1->sid, NULL, 0);
        __delay_cycles(200);
        write(a1->sid, NULL, 1);

        a1->current->count++;

        ioctl(a1->tid, STOP, NULL);
        sem_post(a1->self);
        sem_post(a0->sync0);
        sem_post(a2->sync0);
    }
    else if(a1->current->steps == 0)
    {
        ioctl(a1->tid, STOP, NULL);
        sem_post(a1->self);
        sem_post(a0->sync0);
        sem_post(a2->sync0);
    }
    else
    {
        write(a1->sid, NULL, 0);
        __delay_cycles(200);
        write(a1->sid, NULL, 1);

        a1->current->count++;
    }
}

void axis2_timeout(void)
{
    axis_t *a2 = &axis_device[AXIS2];
    axis_t *a0 = &axis_device[AXIS0];
    axis_t *a1 = &axis_device[AXIS1];

    if(a2->current->count + 1 == a2->current->steps)
    {
        write(a2->sid, NULL, 0);
        __delay_cycles(200);
        write(a2->sid, NULL, 1);

        a2->current->count++;

        ioctl(a2->tid, STOP, NULL);
        sem_post(a2->self);
        sem_post(a1->sync1);
        sem_post(a0->sync1);
    }
    else if(a2->current->steps == 0)
    {
        ioctl(a2->tid, STOP, NULL);
        sem_post(a2->self);
        sem_post(a1->sync1);
        sem_post(a0->sync1);
    }
    else
    {
        write(a2->sid, NULL, 0);
        __delay_cycles(200);
        write(a2->sid, NULL, 1);

        a2->current->count++;
    }
}

/*--------------------------------------------------------------------------------*/

/*AXIS QUEUE TASKS----------------------------------------------------------------*/
void *axis0_task(void *arg0)
{
    axis_t *a = &axis_device[AXIS0];
    axis_req_t rcv;
    int dir;

    while(1)
    {
        //check if there is anything on the queue for the axis
        mq_receive(a->queue, (char*)&rcv, sizeof(axis_req_t), 0);

        //set request, initialize direction
        a->current = &rcv;
        dir = a->current->direction;
        write(a->did, NULL, dir);

        //Set the frequency, start the timer
        ioctl(a->tid, SET_FREQUENCY, &a->current->freq);
        ioctl(a->tid, START, NULL);

        //wait until motion is done, synchronize all axis
        sem_wait(a->self);
        sem_wait(a->sync0);
        sem_wait(a->sync1);

        a->count++;
    }
}

void *axis1_task(void *arg1)
{
    axis_t *a = &axis_device[AXIS1];
    axis_req_t rcv;
    int dir;

    while(1)
    {
        //Wait to receive something from the queue
        mq_receive(a->queue, (char*)&rcv, sizeof(axis_req_t), 0);

        //set request, initialize direction
        a->current = &rcv;
        dir = a->current->direction;
        write(a->did, NULL, dir);

        //Set the frequency, start the timer
        ioctl(a->tid, SET_FREQUENCY, &a->current->freq);
        ioctl(a->tid, START, NULL);

        //wait until motion is done, synchronize all axis
        sem_wait(a->self);
        sem_wait(a->sync0);
        sem_wait(a->sync1);

        a->count++;
    }
}

void *axis2_task(void *arg1)
{
    axis_t *a = &axis_device[AXIS2];
    axis_req_t rcv;
    int dir;

    while(1)
    {
        //Wait to receive something from the queue
        mq_receive(a->queue, (char*)&rcv, sizeof(axis_req_t), 0);

        //set request
        a->current = &rcv;
        dir = a->current->direction;
        write(a->did, NULL, dir);

        //Set the frequency, start the timer
        ioctl(a->tid, SET_FREQUENCY, &a->current->freq);
        ioctl(a->tid, START, NULL);

        //wait until motion is done, synchronize all axis
        sem_wait(a->self);
        sem_wait(a->sync0);
        sem_wait(a->sync1);

        a->count++;
    }
}

/*---------------------------------------------------------------------------------*/

