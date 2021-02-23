#include <FreeRTOS.h>
#include <task.h>
#include <mqueue.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "device.h"
#include "timer.h"
#include "adbi.h"
#include "axis.h"

#define PROFILE_COUNT   1

void *adbi_thread(void *args);
//void adbi_tipo(void);

typedef enum
{
    ACCELERATING,
    CRUISING,
    DECELERATING,
    DONE
}state_e;

typedef struct
{
    float xr;
    float xt;
    float yr;
    float yt;
    float zr;
    float zt;
}adbi_error_t;

typedef struct
{
    int na;
    int nc;
    int nd;
    float dx;
    float dy;
    float dz;
    float l;
    float v_last;
    float feedrate;
    int delay;
    adbi_error_t error;
    state_e mstate;
}adbi_req_t;


typedef struct
{
    float acceleration;
    float deceleration;
    float tipo;
    float spmm;
    int xid;
    int yid;
    int zid;
    //int tid;
    mqd_t queue;
    char *queue_name;
    pthread_t thread_id;
    void*(*adbi_thread)(void*);
    adbi_req_t *current;
    int count;
}adbi_t;


//sem_t tipo_sem[PROFILE_COUNT];
adbi_t acontrol = { 200000.0, 200000.0, 0.020, 66.666, -1, -1, -1, NULL, "ACCEL0", NULL, adbi_thread, NULL };

int adbi_init(void)
{
    adbi_t *a = &acontrol;

    a->xid = axis_open(AXIS0);
    a->yid = axis_open(AXIS1);
    a->zid = axis_open(AXIS2);

    a->acceleration = a->acceleration/3600;
    a->deceleration = a->deceleration/3600;

    a->count = 0;

    //Create Queue
    mq_attr attr;
    attr.mq_curmsgs = 0;
    attr.mq_msgsize = sizeof(adbi_req_t);
    attr.mq_maxmsg = ADBI_MAX_QUEUE;
    attr.mq_flags = 0;
    a->queue = mq_open(a->queue_name, O_CREAT|O_RDWR, 0644, &attr);

    //Create thread and initialize task
    pthread_create(&a->thread_id, NULL, a->adbi_thread, NULL);

    return 0;
}

void adbi_interp(adbi_req_t *m, float tipo, float acceleration, float deceleration)
{
    float ln;
    float vn;
    float xn, yn, zn;

    float v_last = m->v_last;

    //Compute velocity and linear displacement according to the motion state in which the motion is in
    if(m->mstate == ACCELERATING)
    {
        vn = v_last + tipo*acceleration;
        ln = (vn*vn - v_last*v_last)/(2*acceleration);
        m->v_last = vn;
    }
    else if(m->mstate == CRUISING)
    {
        ln = m->feedrate*tipo;
    }
    else if(m->mstate == DECELERATING)
    {
        vn = v_last - tipo*deceleration;
        ln = (v_last*v_last - vn*vn)/(2*deceleration);
        m->v_last = vn;
    }

    //rough interpolation and axis write
    xn = (ln*m->dx)/m->l;
    yn = (ln*m->dy)/m->l;
    zn = (ln*m->dz)/m->l;

    //accumulate distance for error calculation
    m->error.xt += xn;
    m->error.yt += yn;
    m->error.zt += zn;
}


axis_req_t adbi_mapping(adbi_req_t *m, int axis, float spmm, float tipo)
{
    //mapping between steps and millimeters. Calculate frequency given steps and time interpolation
    float error;
    int steps;
    axis_req_t r;

    switch(axis)
    {
    case AXIS0:
        error = m->error.xt - m->error.xr;
        steps = round(error*spmm);
        m->error.xr += steps/spmm;
        r.direction = m->dx < 0 ? 0 : 1;

        r.steps = abs(steps);
        r.freq = r.steps == 0 ? round((1/tipo)) : round(r.steps/tipo);
        break;
    case AXIS1:
        error = m->error.yt - m->error.yr;
        steps = round(error*spmm);
        m->error.yr += steps/spmm;
        r.direction = m->dy < 0 ? 0 : 1;

        r.steps = abs(steps);
        r.freq = r.steps == 0 ? round((1/tipo) * 1.2) : round(r.steps/tipo);
        break;
    case AXIS2:
        error = m->error.zt - m->error.zr;
        steps = round(error*spmm);
        m->error.zr += steps/spmm;
        r.direction = m->dz < 0 ? 0 : 1;

        r.steps = abs(steps);
        r.freq = r.steps == 0 ? round((1/tipo) * 1.4) : round(r.steps/tipo);
        break;
    }

    r.count = 0;

    return r;
}

void adbi_init_motion(adbi_req_t *m, float l, float dx, float dy, float dz, int na, int nc, int nd, float feedrate)
{
    m->mstate = ACCELERATING;
    m->dx = dx;
    m->dy = dy;
    m->dz = dz;
    m->feedrate = feedrate;

    m->na = na;
    m->nc = nc;
    m->nd = nd;

    m->l = l;
    m->v_last = 0.0;

    m->error.xr = 0.0;
    m->error.xt = 0.0;
    m->error.yr = 0.0;
    m->error.yt = 0.0;
    m->error.zr = 0.0;
    m->error.zt = 0.0;

    m->delay = 0.0;
}

float adbi_actual_distance(adbi_t *a, int na, int nc, int nd, float feedrate)
{
    float li, vn;
    float v_last = 0.0;

    int i;
    for(i = 0; i < na; i++)
    {
        vn = v_last + a->tipo*a->acceleration;
        li += (vn*vn - v_last*v_last)/(2*a->acceleration);
        v_last = vn;
    }

    for(i = 0; i < nc; i++)
    {
        li += feedrate*a->tipo;
    }

    for(i = 0; i < nd; i++)
    {
        vn = v_last - a->tipo*a->deceleration;
        li += (v_last*v_last - vn*vn)/(2*a->deceleration);
        v_last = vn;
    }

    return li;
}

void adbi_next(adbi_t *a)
{
    axis_req_t x;
    axis_req_t y;
    axis_req_t z;

    adbi_interp(a->current, a->tipo, a->acceleration, a->deceleration);
    x = adbi_mapping(a->current, AXIS0, a->spmm, a->tipo);
    y = adbi_mapping(a->current, AXIS1, a->spmm, a->tipo);
    z = adbi_mapping(a->current, AXIS2, a->spmm, a->tipo);

    //write to axis, for step/dir pulse generation
    axis_write(a->xid, (char*)&x, sizeof(axis_req_t));
    axis_write(a->yid, (char*)&y, sizeof(axis_req_t));
    axis_write(a->zid, (char*)&z, sizeof(axis_req_t));
}

void adbi_linear(vector3_t p0, vector3_t p1, float feedrate)
{
    adbi_t *a = &acontrol;

    /*calculate relative distance and path lenght*/
    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    float dz = p1.z - p0.z;
    float l = sqrt(dx*dx + dy*dy + dz*dz);

    /*convert from mm/min to mm/s*/
    feedrate = feedrate / 60;

    /*calculate distance to accelerate and decelerate*/
    float ad = (feedrate*feedrate)/(2*a->acceleration);
    float dd = (feedrate*feedrate)/(2*a->deceleration);

    /*check if normal block or short block*/
    if(l < (ad + dd) || l == (ad + dd))
    {
        //Short block. There is not time to cruise, find optimal feedrate with no cruising
        float num = 2*l*a->acceleration*a->deceleration;
        float den = a->acceleration + a->deceleration;

        feedrate = sqrt(num/den);
    }

    /*Optimal feedrate found. Calculate acceleration, deceleration and cruising times*/
    float ta = feedrate / a->acceleration;
    float td = feedrate / a->deceleration;
    float tc = (l - ad - dd)/feedrate;

    /*compute total time and cruising count*/
    int nc = 0;

    if(tc > 0)
    {
        nc = round(tc/a->tipo);
    }

    /*calculate acceleration, deceleration and cruise count*/
    int na = round(ta/a->tipo);
    int nd = round(td/a->tipo);

    //Count to synchronize motion with external events
    a->count += (nc + na + nd);


    //Find total interpolated disance
    float li = adbi_actual_distance(a, na, nc, nd, feedrate);

    adbi_req_t motion;
    adbi_init_motion(&motion, li, dx, dy, dz, na, nc, nd, feedrate);

    mq_send(a->queue, (char*)&motion, sizeof(adbi_req_t), 0);
}

void adbi_delay(adbi_t *a, int delay)
{
    axis_req_t req;

    req.steps = 0;
    req.count = 0;
    req.direction = 0;
    req.freq = 1;

    int i;
    for(i = 0; i < delay; i++)
    {
        //write to axis, for step/dir pulse generation
        axis_write(a->xid, (char*)&req, sizeof(axis_req_t));
        axis_write(a->yid, (char*)&req, sizeof(axis_req_t));
        axis_write(a->zid, (char*)&req, sizeof(axis_req_t));
    }
}

int adbi_ioctl(int req, void *args)
{
    adbi_t *a = &acontrol;
    adbi_req_t request;

    switch(req)
    {
    case GET_COUNT:
        *((int*)args) = a->count;
        break;
    case DELAY:
        request.delay = *(int*)args;
        mq_send(a->queue, (char*)&request, sizeof(adbi_req_t), 0);
        break;
    default:
        break;
    }

    return 0;
}

void *adbi_thread(void *args)
{
    adbi_t *a = &acontrol;
    adbi_req_t new;

    while(1)
    {
        //check if there is a new motion request on the queue
        mq_receive(a->queue, (char*)&new, sizeof(adbi_req_t), 0);

        //set new request
        a->current = &new;

        if(a->current->delay)
        {
            adbi_delay(a, a->current->delay);
            a->current->mstate = DONE;
        }

        while(a->current->mstate != DONE)
        {
            //Motion control state machine for accelerating, cruising and decelerating .
            //Every interpolation time do the following:
             //1. check acceleration/cruising/deceleration count. change state if necessary
             //2. calculate next time interpolation distance displacement
             //3. decrement acceleration/cruising/deceleration count
            switch(a->current->mstate)
            {
            case ACCELERATING:
                if(a->current->na)
                {
                    adbi_next(a);
                    a->current->na--;
                }
                else
                {
                    a->current->mstate = CRUISING;
                }
                break;
            case CRUISING:
                if(a->current->nc)
                {
                    adbi_next(a);
                    a->current->nc--;
                }
                else
                {
                    a->current->mstate = DECELERATING;
                }
                break;
            case DECELERATING:
                if(a->current->nd)
                {
                    adbi_next(a);
                    a->current->nd--;
                }
                else
                {
                    a->current->mstate = DONE;
                }
                break;
            }
        }
    }
}


/*CMD for adbi--------------------------------*/
extern int acceleration_cmd(int argc, char **argv)
{
    adbi_t *a = &acontrol;

    float acc = atof(argv[0]);

    a->acceleration = acc/60.0;

    return 0;
}

extern int deceleration_cmd(int argc, char **argv)
{
    adbi_t *a = &acontrol;

    float dec = atof(argv[0]);

    a->deceleration = dec/60.0;

    return 0;
}

extern int spm_cmd(int argc, char **argv)
{
    adbi_t *a = &acontrol;

    float spm = atof(argv[0]);

    a->spmm = spm;

    return 0;
}

extern int tipo_cmd(int argc, char **argv)
{
    adbi_t *a = &acontrol;

    float tipo = atof(argv[0]);

    a->tipo = tipo/60.0;

    return 0;
}

/*--------------------------------------------*/
