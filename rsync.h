#ifndef RSYNC_H_
#define RSYNC_H_

#define RSYNC_QUEUE_SIZE    48
#define DATA_SIZE           12
#define MAX_HANDLERS        10

typedef struct
{
    int type;
    int sync;
    char data[DATA_SIZE];
}rsync_req_t;

typedef void (*rsync_fxn)(char*);

int rsync_open(void);
int rsync_write(char *buf, int len);
int rsync_register(rsync_fxn handler);

#endif /* RSYNC_H_ */
