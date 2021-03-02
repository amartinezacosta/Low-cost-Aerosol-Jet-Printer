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

#include "timer.h"

typedef void(*timer_fxninner)(Timer_Handle);
typedef void(*timer_fxnouter)(void);

void Timer0_Callback(Timer_Handle timer);
void Timer1_Callback(Timer_Handle timer);
void Timer2_Callback(Timer_Handle timer);
void Timer3_Callback(Timer_Handle timer);

typedef struct
{
    timer_fxninner inner;
    timer_fxnouter outer;
    Timer_Handle handle;
    uint32_t freq;
}timer_t;

timer_t timers[] =
{
 {Timer0_Callback, NULL, NULL, 1000},
 {Timer1_Callback, NULL, NULL, 1000},
 {Timer2_Callback, NULL, NULL, 1000},
 {Timer3_Callback, NULL, NULL, 1000},
};


int Timer_Open(int device)
{
    Timer_Params params;
    timer_t *t = &timers[device];

    Timer_Params_init(&params);
    params.period = t->freq;
    params.periodUnits = Timer_PERIOD_HZ;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = t->inner;

    t->handle = Timer_open(device, &params);

    if (t->handle == NULL) {
        return 1;
    }

    return 0;
}

int Timer_Write(int device, char *buf, int len)
{
    return 0;
}

int Timer_Read(int device, char *buf, int len)
{
    return 0;
}

int Timer_IOctl(int device, int req, void *arg)
{
    timer_t *t = &timers[device];

    Timer_Params params;

    switch(req)
    {
    case START:
        Timer_start(t->handle);
        break;
    case STOP:
        Timer_stop(t->handle);
        break;
    case TIMER_SET_CALLBACK:
        t->outer = (timer_fxnouter)arg;
        break;
    case SET_FREQUENCY:
        t->freq = *((uint32_t*)arg);

        Timer_close(t->handle);

        Timer_Params_init(&params);
        params.period = t->freq;
        params.periodUnits = Timer_PERIOD_HZ;
        params.timerMode = Timer_CONTINUOUS_CALLBACK;
        params.timerCallback = t->inner;

        t->handle = Timer_open(device, &params);
        if(t->handle == 0){while(1);}
        break;
    default:
        break;
    }

    return 0;
}

void Timer0_Callback(Timer_Handle timer)
{
    timer_t *t = &timers[0];

    if(t->outer)
    {
        t->outer();
    }
    else
    {
        while(1);
    }
}

void Timer1_Callback(Timer_Handle timer)
{
    timer_t *t = &timers[1];

    if(t->outer)
    {
        t->outer();
    }
    else
    {
        while(1);
    }
}

void Timer2_Callback(Timer_Handle timer)
{
    timer_t *t = &timers[2];

    if(t->outer)
    {
        t->outer();
    }
    else
    {
        while(1);
    }
}

void Timer3_Callback(Timer_Handle timer)
{
    timer_t *t = &timers[3];

    if(t->outer)
    {
        t->outer();
    }
    else
    {
        while(1);
    }
}

