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



