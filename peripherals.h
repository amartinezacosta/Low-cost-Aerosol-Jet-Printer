#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include "Board.h"
#include "Serial_UART.h"
#include "dio.h"
#include "timer.h"
#include "Serial_USB.h"
#include "i2c.h"

/*Board specific, DO NOT INCLUDE THIS FILE ANYWHERE BUT IN devices.c*/

typedef struct
{
    char *name;
    int hwr;
    int (*open)(int);
    int (*write)(int, char*, int);
    int (*read)(int, char*, int);
    int (*ioctl)(int, int, void*);
}device_t;

device_t devices[] =
{
 {"usb0", Board_USB0, SerialUSB_Open, SerialUSB_Write, SerialUSB_Read, SerialUSB_IOctl},
 {"tty0", Board_UART0, SerialUART_Open, SerialUART_Write, SerialUART_Read, SerialUART_IOctl},
 {"tty1", Board_UART2, SerialUART_Open, SerialUART_Write,  SerialUART_Read, SerialUART_IOctl},
 {"tty2", Board_UART7, SerialUART_Open, SerialUART_Write,  SerialUART_Read, SerialUART_IOctl},
 {"tty3", Board_UART6, SerialUART_Open, SerialUART_Write,  SerialUART_Read, SerialUART_IOctl},
 {"i2c0", Board_I2C0, I2C_Open, I2C_Write,  I2C_Read, I2C_IOctl},
 {"ti0",  Board_TIMER0, Timer_Open, Timer_Write, Timer_Read, Timer_IOctl},
 {"ti1",  Board_TIMER1, Timer_Open, Timer_Write, Timer_Read, Timer_IOctl},
 {"ti2",  Board_TIMER2, Timer_Open, Timer_Write, Timer_Read, Timer_IOctl},
 {"ti3",  Board_TIMER3, Timer_Open, Timer_Write, Timer_Read, Timer_IOctl},
 {"dio0", Board_GPIO_STEPX, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio1", Board_GPIO_DIRX, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio2", Board_GPIO_ENX, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio3", Board_GPIO_STEPY, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio4", Board_GPIO_DIRY, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio5", Board_GPIO_ENY, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio6", Board_GPIO_STEPZ, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio7", Board_GPIO_DIRZ, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio8", Board_GPIO_ENZ, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dio9", Board_GPIO_EMERGENCY, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dioa", Board_GPIO_CHA0, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"diob", Board_GPIO_CHB0, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"dioc", Board_GPIO_CHA1, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
 {"diod", Board_GPIO_CHB1, DIO_Open, DIO_Write, DIO_Read, DIO_IOctl},
};

int device_index = 0;
int devices_count = sizeof(devices)/sizeof(device_t);



#endif /* PERIPHERALS_H_ */
