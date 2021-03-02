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

#include "rpc.h"
#include "device.h"

typedef struct
{
 char *cmd;
 cmd_fxn fxn;
}cmd_t;


cmd_t cmd_list[] =
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
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
 {NULL, NULL},
};

int cmd_count = sizeof(cmd_list)/sizeof(cmd_t);
int cmd_index = 0;

int rpc_interpret(char *rcv, int len)
{
    //overwrite new line character with null character
    rcv[len - 1] = '\0';

    //parse null terminated command
    char *cmd = strtok(rcv, " ");
    char *argv[MAX_ARGS];
    int ret;

    int argc = 0;
    argv[argc++] = strtok(NULL, ",");

    while(argv[argc - 1])
    {
        argv[argc++] = strtok(NULL, ",");

    }

    int i;
    for(i = 0; i < cmd_count; i++)
    {
        if(!strcmp(cmd, cmd_list[i].cmd))
        {
            ret = cmd_list[i].fxn(argc, argv);
            break;
        }
    }

    return ret;
}

void rpc_ack(int sid, int status)
{
    char ack[64];

    sprintf(ack, "ack: %i OK\n", status);
    write(sid, ack, strlen(ack));
}

int rpc_register(char *cmd, cmd_fxn fxn)
{
    if(cmd_index < cmd_count)
    {
        cmd_list[cmd_index].cmd = cmd;
        cmd_list[cmd_index].fxn = fxn;

        cmd_index++;
        return 0;
    }

    return 1;
}




