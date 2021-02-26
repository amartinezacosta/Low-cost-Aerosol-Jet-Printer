/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#include <mqueue.h>
#include <pthread.h>

#include <unistd.h>

#include "rsync.h"
#include "device.h"
#include "axis.h"

void *rsync_thread(void*);

typedef struct
{
    mqd_t queue;
    char *queue_name;
    pthread_t thread_id;
    void*(*thread)(void*);
    rsync_fxn *rsync_handlers;
    int handler_count;
    int count;
    int sleep;
}rsync_t;

rsync_fxn rsync0_handlers[MAX_HANDLERS];

rsync_t sync_obj = {NULL, "QUEUEREQ", NULL, rsync_thread, rsync0_handlers, 0, 0, 100000};


int rsync_open(void)
{
    rsync_t *r = &sync_obj;

    mq_attr attr;
    attr.mq_curmsgs = 0;
    attr.mq_msgsize = sizeof(rsync_req_t);
    attr.mq_maxmsg = RSYNC_QUEUE_SIZE;
    attr.mq_flags = 0;
    r->queue = mq_open(r->queue_name, O_CREAT|O_RDWR, 0644, &attr);

    pthread_create(&r->thread_id, NULL, r->thread, NULL);

    r->handler_count = 0;

    return 0;
}

int rsync_write(char *buf, int len)
{
    rsync_t *r = &sync_obj;

    mq_send(r->queue, buf, len, 0);

    return 0;
}

int rsync_register(rsync_fxn handler)
{
    rsync_t *r = &sync_obj;
    int count = r->handler_count;

    if(count < MAX_HANDLERS)
    {
        r->rsync_handlers[count] = handler;
        r->handler_count++;

        return count;
    }

    return -1;
}

void *rsync_thread(void *args)
{
    rsync_t *r = &sync_obj;

    rsync_req_t new;
    int type;

    while(1)
    {
        mq_receive(r->queue, (char*)&new, sizeof(rsync_req_t), 0);

        //wait here until axis motion has caught up with reques sync count
        while(new.sync > r->count)
        {
            axis_ioctl(AXIS0, GET_COUNT, &r->count);
            usleep(r->sleep);
        }

        //call its handler once the axis count exceeds the sync count of this request
        type = new.type;
        r->rsync_handlers[type](new.data);
    }
}
