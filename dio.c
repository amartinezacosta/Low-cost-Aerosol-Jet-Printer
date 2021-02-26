/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#include <ti/drivers/GPIO.h>
#include <string.h>

#include "dio.h"

typedef void (*dio_fxn)(uint8_t);

#define ASSERT_DIO_CALLBACK(d, i)  if(d->outer){d->outer(i);}

void DIO0_Callback(uint8_t index);
void DIO1_Callback(uint8_t index);
void DIO2_Callback(uint8_t index);
void DIO3_Callback(uint8_t index);
void DIO4_Callback(uint8_t index);
void DIO5_Callback(uint8_t index);
void DIO6_Callback(uint8_t index);
void DIO7_Callback(uint8_t index);

typedef struct
{
    GPIO_CallbackFxn inner;
    dio_fxn outer;
}dio_t;

dio_t dio_list[] =
{
 {DIO0_Callback, NULL},
 {DIO1_Callback, NULL},
 {DIO2_Callback, NULL},
 {DIO3_Callback, NULL},
 {DIO4_Callback, NULL},
 {DIO5_Callback, NULL},
 {DIO6_Callback, NULL},
 {DIO7_Callback, NULL},
};

int DIO_Open(int device)
{
    GPIO_setConfig(device, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);

    return 0;
}

int DIO_Write(int device, char *buf, int len)
{
    GPIO_write(device, len);

    return 0;
}

int DIO_Read(int device, char *buf, int len)
{
    return GPIO_read(device);
}

int DIO_IOctl(int device, int req, void *args)
{
    dio_t *d = &dio_list[device - 4];

    switch(req)
    {
    case INPUT_INTERRUPT:
        GPIO_setConfig(device, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
        GPIO_setCallback(device, d->inner);
        GPIO_enableInt(device);
        break;
    case INPUT:
        GPIO_setConfig(device, GPIO_CFG_IN_PU);
        break;
    case DIO_SET_CALLBACK:
        d->outer = (dio_fxn)args;
        break;
    }

    return 0;
}

void DIO0_Callback(uint8_t index)
{
    dio_t *d = &dio_list[0];
    ASSERT_DIO_CALLBACK(d, 0);
}

void DIO1_Callback(uint8_t index)
{
    dio_t *d = &dio_list[1];
    ASSERT_DIO_CALLBACK(d, 1);
}

void DIO2_Callback(uint8_t index)
{
    dio_t *d = &dio_list[2];
    ASSERT_DIO_CALLBACK(d, 2);
}

void DIO3_Callback(uint8_t index)
{
    dio_t *d = &dio_list[3];
    ASSERT_DIO_CALLBACK(d, 3);
}

void DIO4_Callback(uint8_t index)
{
    dio_t *d = &dio_list[4];
    ASSERT_DIO_CALLBACK(d, 4);
}

void DIO5_Callback(uint8_t index)
{
    dio_t *d = &dio_list[5];
    ASSERT_DIO_CALLBACK(d, 5);
}

void DIO6_Callback(uint8_t index)
{
    dio_t *d = &dio_list[6];
    ASSERT_DIO_CALLBACK(d, 6);
}

void DIO7_Callback(uint8_t index)
{
    dio_t *d = &dio_list[7];
    ASSERT_DIO_CALLBACK(d, 7);
}

