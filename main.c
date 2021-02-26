/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#include <stdint.h>
#include <pthread.h>
#include <FreeRTOS.h>
#include <task.h>
#include <ti/drivers/Board.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

#include "Board.h"
#include "USBCDCD.h"

extern void *server(void *args);

int main(void)
{
    pthread_t server_thread;

    /* Call driver initialization functions */
    Board_init();
    Timer_init();
    GPIO_init();
    UART_init();
    I2C_init();

    MSP_EXP432E401Y_initUSB(Board_USB0);
    USBCDCD_init(Board_USB0);

    pthread_create(&server_thread, NULL, server, NULL);

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}


void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
