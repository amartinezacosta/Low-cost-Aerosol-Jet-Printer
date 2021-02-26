/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef TIMER_H_
#define TIMER_H_

#include <ti/drivers/Timer.h>

#define TIMER_COUNT     3

#define START           0
#define STOP            1
#define SET_FREQUENCY   2
#define TIMER_SET_CALLBACK    3

int Timer_Open(int device);
int Timer_Write(int device, char *buf, int len);
int Timer_Read(int device, char *buf, int len);
int Timer_IOctl(int device, int req, void *arg);



#endif /* TIMER_H_ */
