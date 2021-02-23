#ifndef AXIS_H_
#define AXIS_H_

#define AXIS_COUNT      3
#define MAX_QUEUE_REQ   512

#define AXIS0          0
#define AXIS1          1
#define AXIS2          2

#define GET_COUNT       0

typedef struct
{
    int steps;
    int freq;
    int count;
    int direction;
}axis_req_t;

int axis_open(int device);
int axis_write(int device, char *buf, int len);
int axis_read(int device, char *buf, int len);
int axis_ioctl(int device, int req, void *args);


#endif /* AXIS_H_ */
