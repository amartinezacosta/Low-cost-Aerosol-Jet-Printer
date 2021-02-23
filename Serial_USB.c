#include "Serial_USB.h"
#include "USBCDCD.h"

int SerialUSB_Open(int device)
{
    //Wait for one second to connect
    return !USBCDCD_waitForConnect(CONNECT_DELAY);
}


int SerialUSB_IOctl(int device, int req, void *arg)
{
    return 0;
}

int SerialUSB_Read(int device, char *data, int size)
{
    return USBCDCD_receiveData((unsigned char*)data, size, READ_TIMEOUT);
}

int SerialUSB_Write(int device, char *data, int size)
{
    return USBCDCD_sendData(data, size, WRITE_TIMEOUT);
}










