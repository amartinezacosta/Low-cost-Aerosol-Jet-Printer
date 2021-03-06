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

#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "rpc.h"
#include "adbi.h"
#include "alicat.h"
#include "device.h"
#include "emergency.h"
#include "relay.h"
#include "rsync.h"
#include "encoder.h"

extern int motion_cmd(int argc, char **argv);
extern int delay_cmd(int argc, char **argv);
extern int acceleration_cmd(int argc, char **argv);
extern int deceleration_cmd(int argc, char **argv);
extern int spm_cmd(int argc, char **argv);
extern int tipo_cmd(int argc, char **argv);
extern int encoder_cmd(int argc, char **argv);

extern int devo_cmd(int argc, char **argv);

void *encoder_send(void *args)
{
    int count0, count1;
    int usbid = *((int*)args);
    char buf[64];

    while(1)
    {
        count0 = encoder_read(0);
        count1 = encoder_read(1);

        sprintf(buf, "ENC: %i,%i\n\r", count0, count1);
        write(usbid, buf, strlen(buf));
        usleep(100000);
    }
}

extern int alicat_count;

void *alicat_send(void *args)
{
    int usbid = *((int*)args);
    int i;
    int bytes_read;
    char data[164];

    for(i = 0; i < alicat_count; i++)
    {
        //alicat_write(i, "@@=A\r", 5);
        alicat_write(i, "AW91=500\r", 9);
        sleep(1);
        alicat_write(i, "A@=@\r", 5);
    }

    while(1)
    {
        for(i = 0; i < alicat_count; i++)
        {
            memset(data, 0, 164);

            data[0] = 'A';
            data[1] = 'L';
            data[2] = 'I';
            data[3] = ':';
            data[4] = i + 48;

            bytes_read = alicat_read(i, data + 5, 158);

            write(usbid, data, bytes_read + 5);
            usleep(50);
        }

        usleep(500000);
    }
}

void *server(void *args)
{
    adbi_init();
    rsync_open();

    alicat_open(ALICAT0);   //DEV ID0
    alicat_open(ALICAT1);   //DEV ID1
    alicat_open(ALICAT2);   //DEV ID2

    relay_open(RELAY0);     //DEV ID3

    encoder_open(0);
    encoder_open(1);

    int sid = open("/dev/tty0");
    int usbid = open("/dev/usb0");

    emergency_open("/dev/dio9", sid);

    rpc_register("MOV", motion_cmd);
    rpc_register("DLY", delay_cmd);
    rpc_register("ENC", encoder_cmd);
    rpc_register("ACC", acceleration_cmd);
    rpc_register("DEC", deceleration_cmd);
    rpc_register("SPM", spm_cmd);
    rpc_register("TIP", tipo_cmd);

    rpc_register("DEVO", devo_cmd);

    pthread_t encoder_thread_id, alicat_thread_id;
    pthread_create(&encoder_thread_id, NULL, encoder_send, &usbid);
    pthread_create(&alicat_thread_id, NULL, alicat_send, &usbid);

    emergency_check();

    char buffer[128];
    int bytes_read;
    int status;

    while(1)
    {
        //Wait for serial port data (PC)
        bytes_read = read(sid, buffer, 128);
        status = rpc_interpret(buffer, bytes_read);
        rpc_ack(sid, status);
    }
}




