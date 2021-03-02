/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). 
Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains 
certain rights in this software.

----------------------------------------------------------------------------------

MIT License

Copyright (c) 2021 Alejandro Martinez-Acosta
Copyright (c) 2021 Ethan B. Secor

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include <ti/drivers/UART.h>

#include "alicat.h"
#include "rsync.h"
#include "device.h"
#include "adbi.h"
#include "Serial_UART.h"
#include "USBCDCD.h"
#include "devio.h"

typedef int (*alicat_write_cmd)(int, char**);
typedef int (*alicat_read_cmd)(int, char**);
typedef void (*alicat_sync)(char*);

int alicat0_write_cmd(int argc, char **argv);
int alicat0_read_cmd(int argc, char **argv);
void alicat0_sync_fxn(char *data);

int alicat1_write_cmd(int argc, char **argv);
int alicat1_read_cmd(int argc, char **argv);
void alicat1_sync_fxn(char *data);

int alicat2_write_cmd(int argc, char **argv);
int alicat2_read_cmd(int argc, char **argv);
void alicat2_sync_fxn(char *data);

typedef struct
{
    char *device;
    int sid;
    int sync_id;
    alicat_write_cmd write_cmd;
    //alicat_read_cmd read_cmd;
    alicat_sync sync_fxn;
}alicat_t;

alicat_t alicat_objs[] =
{
 {"/dev/tty1", -1, -1, alicat0_write_cmd , alicat0_sync_fxn},
 {"/dev/tty2", -1, -1, alicat1_write_cmd , alicat1_sync_fxn},
 {"/dev/tty3", -1, -1, alicat2_write_cmd, alicat2_sync_fxn},
};

int alicat_count = sizeof(alicat_objs)/sizeof(alicat_t);

int alicat_open(int device)
{
    alicat_t *a = &alicat_objs[device];
    a->sid = open(a->device);

    a->sync_id = rsync_register(a->sync_fxn);
    devio_register(NULL, a->write_cmd);

    return 0;
}

int alicat_read(int device, char *buf, int len)
{
    alicat_t *a = &alicat_objs[device];

    return read(a->sid, buf, len);
}

int alicat_write(int device, char *buf, int len)
{
    alicat_t *a = &alicat_objs[device];

    return write(a->sid, buf, len);
}


int alicat0_write_cmd(int argc, char **argv)
{
    alicat_t *a = &alicat_objs[0];
    char *data = argv[0];
    int size = strlen(data);
    rsync_req_t req;

    data[size++] = '\r';
    data[size++] = '\0';
    memcpy(req.data, data, size);

    adbi_ioctl(GET_COUNT, &req.sync);
    req.type = a->sync_id;

    return rsync_write((char*)&req, sizeof(rsync_req_t));
}


void alicat0_sync_fxn(char *data)
{
    alicat_t *a = &alicat_objs[0];

    write(a->sid, data, strlen(data));
}

int alicat1_write_cmd(int argc, char **argv)
{
    alicat_t *a = &alicat_objs[1];
    char *data = argv[0];
    int size = strlen(data);
    rsync_req_t req;

    data[size++] = '\r';
    data[size++] = '\0';
    memcpy(req.data, data, size);

    adbi_ioctl(GET_COUNT, &req.sync);
    req.type = a->sync_id;

    return rsync_write((char*)&req, sizeof(rsync_req_t));
}


void alicat1_sync_fxn(char *data)
{
    alicat_t *a = &alicat_objs[1];

    write(a->sid, data, strlen(data));
}

int alicat2_write_cmd(int argc, char **argv)
{
    alicat_t *a = &alicat_objs[2];
    char *data = argv[0];
    int size = strlen(data);
    rsync_req_t req;

    data[size++] = '\r';
    data[size++] = '\0';
    memcpy(req.data, data, size);

    adbi_ioctl(GET_COUNT, &req.sync);
    req.type = a->sync_id;

    return rsync_write((char*)&req, sizeof(rsync_req_t));
}


void alicat2_sync_fxn(char *data)
{
    alicat_t *a = &alicat_objs[2];

    write(a->sid, data, strlen(data));
}
