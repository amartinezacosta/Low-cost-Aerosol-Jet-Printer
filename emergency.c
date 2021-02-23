#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <ti/devices/msp432e4/driverlib/sysctl.h>

#include "emergency.h"
#include "device.h"
#include "dio.h"

void emergency_callback(uint8_t index);

typedef struct
{
    int sid;
    int did;
}emergency_t;

emergency_t emergency_button;

int emergency_open(char *did, int sid)
{
    emergency_t *e = &emergency_button;

    e->did = open(did);
    ioctl(e->did, DIO_SET_CALLBACK, emergency_callback);
    ioctl(e->did, INPUT_INTERRUPT, NULL);

    e->sid = sid;

    return 0;
}

void emergency_callback(uint8_t index)
{
    emergency_t *e = &emergency_button;

    write(e->sid, "EMERGENCY!\n", 11);

    __delay_cycles(1000000);

    SysCtlReset();
}


void emergency_check(void)
{
    emergency_t *e = &emergency_button;

    while(!read(e->did, NULL, 0))
    {
        write(e->sid, "EMERGENCY!\n", 11);
        sleep(1);
    }
}
