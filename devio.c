#include <string.h>
#include <stdlib.h>

#include "devio.h"

typedef struct
{
    devio_read_fxn read_fxn;
    devio_write_fxn write_fxn;
}devio_t;


devio_t devio_list[] =
{
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
};

int devio_count = sizeof(devio_list)/sizeof(devio_t);
int devio_index = 0;

extern int devo_cmd(int argc, char **argv)
{
    int id = atoi(argv[0]);

    devio_t *d = &devio_list[id];

    return d->write_fxn(argc - 1, argv + 1);
}


int devio_register(devio_read_fxn read_fxn, devio_write_fxn write_fxn)
{
    if(devio_index < devio_count)
    {
        devio_list[devio_index].read_fxn = read_fxn;
        devio_list[devio_index].write_fxn = write_fxn;

        devio_index++;

        return 0;
    }

    return 1;
}



