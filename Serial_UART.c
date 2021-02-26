/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#include <Serial_UART.h>
#include <ti/drivers/UART.h>

/*Failed to initialize UART peripheral*/
#define ASSERT_UART(id) if(id == 0){ while(1); }

//typedef void(*uart_fxninner)(UART_Handle,char*,size_t);
typedef void(*uart_fxnouter)(int, char*, int);

void UART0_Callback(UART_Handle uart, void *buf, size_t count);
void UART2_Callback(UART_Handle uart, void *buf, size_t count);
void UART6_Callback(UART_Handle uart, void *buf, size_t count);
void UART7_Callback(UART_Handle uart, void *buf, size_t count);

typedef struct
{
    UART_Handle handle;
    //UART_Params uartParams;
    UART_Callback inner;
    uart_fxnouter outer;
    int baudrate;
}uart_t;

uart_t uarts[] =
{
 {NULL, UART0_Callback, NULL, 115200},
 {NULL, UART2_Callback, NULL, 19200},
 {NULL, UART7_Callback, NULL, 19200},
 {NULL, UART6_Callback, NULL, 19200},
};


int SerialUART_Open(int device)
{
    uart_t *u = &uarts[device];
    UART_Params uartParams;

    UART_Params_init(&uartParams);
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_NEWLINE;
    uartParams.dataLength = UART_LEN_8;
    uartParams.parityType = UART_PAR_NONE;
    uartParams.stopBits = UART_STOP_ONE;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = u->baudrate;

    u->handle = UART_open(device, &uartParams);
    ASSERT_UART(u->handle);

    return 0;
}


int SerialUART_IOctl(int device, int req, void *arg)
{
    uart_t *u = &uarts[device];
    UART_Params uartParams;

    /*close uart*/
    UART_close(u->handle);

    switch(req)
    {
    case UART_SET_CALLBACK:
        u->outer = (uart_fxnouter)arg;
        break;
    case CHANGE_BAUDRATE:
        UART_Params_init(&uartParams);
        uartParams.baudRate = *((uint32_t*)arg);
        break;
    case SET_MODE:
        UART_Params_init(&uartParams);
        uartParams.readMode = *((UART_Mode*)arg);
        uartParams.readDataMode = UART_DATA_TEXT;
        uartParams.writeDataMode = UART_DATA_BINARY;
        uartParams.readEcho = UART_ECHO_OFF;
        uartParams.baudRate = u->baudrate;
        uartParams.readCallback = u->inner;
        break;
    default:
        break;
    }

    u->handle = UART_open(device, &uartParams);
    ASSERT_UART(u->handle);

    return 0;
}

int SerialUART_Read(int device, char *data, int size)
{
    uart_t *u = &uarts[device];

    return UART_read(u->handle, data, size);
}

int SerialUART_Write(int device, char *data, int size)
{
    uart_t *u = &uarts[device];

    return UART_write(u->handle, data, size);
}


void UART0_Callback(UART_Handle uart, void *buf, size_t count)
{
    uart_t *u = &uarts[0];

    if(u->outer)
    {
        u->outer(0, buf, count);
    }
}

void UART2_Callback(UART_Handle uart, void *buf, size_t count)
{
    uart_t *u = &uarts[1];

    if(u->outer)
    {
        u->outer(1, buf, count);
    }
}

void UART7_Callback(UART_Handle uart, void *buf, size_t count)
{
    uart_t *u = &uarts[2];

    if(u->outer)
    {
        u->outer(2, buf, count);
    }
}

void UART6_Callback(UART_Handle uart, void *buf, size_t count)
{
    uart_t *u = &uarts[3];

    if(u->outer)
    {
        u->outer(3, buf, count);
    }
}





