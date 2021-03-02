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

#include <stdlib.h>
#include <math.h>

#include "motion.h"
#include "vector.h"
#include "adbi.h"

typedef struct
{
    vector3_t pt;
    double feedrate;
}motion_t;

/*CURRENT POSITION AND FEEDRATE---------------------------*/
motion_t current = {{0.0, 0.0, 0.0}, 1000.0};
/*--------------------------------------------------------*/

int atof_point(motion_t *m, char **coord)
{
    m->pt.x = atof(coord[0]);
    m->pt.y = atof(coord[1]);
    m->pt.z = atof(coord[2]);
    m->feedrate = atof(coord[3]);

    return 0;
}

void compute_distance_error(vector3_t *current, vector3_t *new, vector3_t *p0_n, vector3_t *p1_n)
{
    vector3_t rt_1 = {0.0, 0.0, 0.0};
    vector3_t rr_1;

    rt_1.x = new->x - current->x;
    rt_1.y = new->y - current->y;
    rt_1.z = new->z - current->z;

    rr_1.x = round(rt_1.x*66.66)/66.66;
    rr_1.y = round(rt_1.y*66.66)/66.66;
    rr_1.z = round(rt_1.z*66.66)/66.66;

    p0_n->x = rr_1.x-rt_1.x;
    p0_n->y = rr_1.y-rt_1.y;
    p0_n->z = rr_1.z-rt_1.z;

    p1_n->x = new->x+p0_n->x;
    p1_n->y = new->y+p0_n->y;
    p1_n->z = new->z+p0_n->z;
}

int motion_cmd(int argc, char **argv)
{
    motion_t new;
    vector3_t p0_n;
    vector3_t p1_n;

    atof_point(&new, argv);
    compute_distance_error(&current.pt, &new.pt, &p0_n, &p1_n);

    adbi_linear(current.pt, p1_n, new.feedrate);

    current.pt = p0_n;
    current.feedrate = new.feedrate;

    return MOTION_OK;
}


