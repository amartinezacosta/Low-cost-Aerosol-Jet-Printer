/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef ADBI_H_
#define ADBI_H_

#include "vector.h"

#define ADBI_MAX_QUEUE  128
#define GET_COUNT       0
#define DELAY           1

int adbi_init(void);
void adbi_linear(vector3_t p0, vector3_t p1, float feedrate);
int adbi_ioctl(int req, void *args);

#endif /* ADBI_H_ */
