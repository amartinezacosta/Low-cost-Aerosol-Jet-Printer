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

#include "device.h"
#include "peripherals.h"

/*Thanks to https://github.com/pridolfi/uPOSIX/blob/master/core/uposix/src/device.c*/

int getDeviceFromPath(const char * path, device_t *dev)
{
    int i = devices_count;

    if(strstr(path, "/dev/"))
    {
        for(i=0; i<devices_count; i++)
        {
            if(strstr(path, devices[i].name))
            {
                *dev = devices[i];
                break;
            }
        }
    }

    if(i == devices_count)
    {
        i = -1;
        dev = 0;
    }

    return i;
}

device_t *getDeviceFromDesc(int fd)
{
    device_t *dev = 0;

    if(fd < devices_count)
    {
        dev = &devices[fd];
    }

    return dev;
}


int open(char *path)
{
    int rv = -1;
    device_t dev;

    int fd = getDeviceFromPath(path, &dev);

    if(&dev)
    {
        rv = dev.open(dev.hwr);
        if(rv)
        {
            fd = -1;
        }
    }
    else
    {
        fd = -1;
    }

    return fd;
}

int read(int fd, char *buf, int len)
{
    int rv = -1;

    if(fd < devices_count)
    {
        device_t *dev = getDeviceFromDesc(fd);
        if(dev)
        {
            rv = dev->read(dev->hwr, buf, len);
        }
    }

    return rv;
}

int write(int fd, char *buf, int len)
{
    int rv = -1;

    if(fd < devices_count)
    {
        device_t *dev = getDeviceFromDesc(fd);
        if(dev)
        {
            rv = dev->write(dev->hwr, buf, len);
        }
    }

    return rv;
}

int ioctl(int fd, int rqst, void *args)
{
    int rv = -1;

    if(fd < devices_count)
    {
        device_t *dev = getDeviceFromDesc(fd);
        if(dev)
        {
            rv = dev->ioctl(dev->hwr, rqst, args);
        }
    }

    return rv;
}

