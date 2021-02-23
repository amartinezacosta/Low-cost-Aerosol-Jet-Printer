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

