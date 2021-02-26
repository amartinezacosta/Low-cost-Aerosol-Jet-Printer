/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#include <string.h>
#include <stdlib.h>

#include "axis.h"
#include "delay.h"
#include "rsync.h"
#include "adbi.h"


int delay_cmd(int argc, char **argv)
{
    int delay = atoi(argv[0]);

    adbi_ioctl(DELAY, &delay);

    return 0;
}


