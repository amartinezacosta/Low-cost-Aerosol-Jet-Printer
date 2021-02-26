/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#include <string.h>

#include "encoder.h"
#include "device.h"
#include "dio.h"

void cha0_callback(void);
void cha1_callback(void);

typedef struct
{
    char *cha_dev;
    char *chb_dev;
    void(*cha_callback)(void);
    int cha;
    int chb;
    long int count;
}encoder_t;

encoder_t encoder_list[] =
{
 {"/dev/dioa", "/dev/diob", cha0_callback, -1, -1, 0},
 {"/dev/dioc", "/dev/diod", cha1_callback, -1, -1, 0},
};

int encoder_count = sizeof(encoder_list)/sizeof(encoder_t);


int encoder_open(int device)
{
    encoder_t *e = &encoder_list[device];

    e->cha = open(e->cha_dev);
    e->chb = open(e->chb_dev);

    ioctl(e->chb, INPUT, NULL);

    ioctl(e->cha, DIO_SET_CALLBACK, e->cha_callback);
    ioctl(e->cha, INPUT_INTERRUPT, NULL);

    return 0;
}

int encoder_read(int device)
{
    encoder_t *e = &encoder_list[device];

    return e->count;
}

void cha0_callback(void)
{
    encoder_t *e = &encoder_list[0];

    int chb = read(e->chb, NULL, 0);

    e->count += (chb == 0) ? -1 : 1;;
}

void cha1_callback(void)
{
    encoder_t *e = &encoder_list[1];

    int chb = read(e->chb, NULL, 0);

    e->count += (chb == 0) ? -1 : 1;;
}

int encoder_cmd(int argc, char **argv)
{
    return 0;
}


