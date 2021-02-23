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


