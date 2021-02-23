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
