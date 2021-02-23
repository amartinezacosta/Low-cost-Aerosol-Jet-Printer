#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include <ti/drivers/UART.h>

#include "relay.h"
#include "rsync.h"
#include "device.h"
#include "adbi.h"
#include "devio.h"

typedef int (*relay_write_cmd)(int, char**);
typedef int (*relay_read_cmd)(int, char**);
typedef void (*relay_sync)(char*);

int relay0_write_cmd(int argc, char **argv);
int relay0_read_cmd(int argc, char **argv);
void relay0_sync_fxn(char *data);

typedef struct
{
    char *device;
    int sid;
    int sync_id;
    char channels;
    relay_write_cmd write_cmd;
    relay_read_cmd read_cmd;
    relay_sync sync_fxn;
}relay_t;

relay_t relay_objs[] =
{
 {"/dev/i2c0", -1, -1, 0, relay0_write_cmd, relay0_read_cmd, relay0_sync_fxn},
};

int relay_count = sizeof(relay_objs)/sizeof(relay_t);

int relay_open(int device)
{
    relay_t *a = &relay_objs[device];
    a->sid = open(a->device);

    a->sync_id = rsync_register(a->sync_fxn);
    devio_register(a->read_cmd, a->write_cmd);

    return 0;
}

int relay0_write_cmd(int argc, char **argv)
{
    rsync_req_t req;
    relay_t *r = &relay_objs[0];
    int on_off;
    int n;

    on_off = atoi(argv[0]);
    n = atoi(argv[1]);

    if(on_off)
    {
        r->channels |= (1 << n);
    }
    else
    {
        r->channels &= ~(1 << n);
    }

    req.data[0] = 0x10;
    req.data[1] = r->channels;

    adbi_ioctl(GET_COUNT, &req.sync);
    req.type = r->sync_id;

    return rsync_write((char*)&req, sizeof(rsync_req_t));
}


int relay0_read_cmd(int argc, char **argv)
{
    return 0;
}

void relay0_sync_fxn(char *data)
{
    relay_t *a = &relay_objs[0];

    write(a->sid, data, 2);
}



