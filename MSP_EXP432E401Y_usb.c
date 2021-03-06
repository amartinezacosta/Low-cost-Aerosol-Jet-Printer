/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== MSP_EXP432E401Y.c ========
 *  This file is responsible for setting up the USB board specific items for the
 *  MSP_EXP432E401Y board.
 */

#include <ti/devices/msp432e4/inc/msp432.h>

#include <ti/devices/msp432e4/driverlib/interrupt.h>
#include <ti/drivers/power/PowerMSP432E4.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/display/Display.h>
#include "MSP_EXP432E401Y_usb.h"
/*
 *  ======== MSP_EXP432E401Y_usbBusFaultHwi ========
 */
static void MSP_EXP432E401Y_usbBusFaultHwi(uintptr_t arg)
{
    /*
     *  This function should be modified to appropriately manage handle
     *  a USB bus fault.
     */
    static Display_Handle display;

    Display_init();

    /* Open the display for output */
    display = Display_open(Display_Type_UART, NULL);

    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }

    Display_printf(display, 0, 0, "USB bus fault detected.\n");
    HwiP_clearInterrupt(INT_GPIOQ4);
}

/*
 *  =============================== USB ===============================
 */
/*
 *  ======== MSP_EXP432E401Y_initUSB ========
 *  This function just turns on the USB
 */
#include <ti/devices/msp432e4/driverlib/inc/hw_gpio.h>
#include <ti/devices/msp432e4/driverlib/driverlib.h>
#include <ti/drivers/uart/UARTMSP432E4.h>
void MSP_EXP432E401Y_initUSB(MSP_EXP432E401Y_USBMode usbMode)
{
    HwiP_Params hwiParams;

    /* Enable the GPIOs for USB device, host, full */
    /* speed and high speed */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOR);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOT);

    /* Power to USB is turned off in Power_init() and in .cfg file. */
    /* With this API call, the power is enabled to USB module */
    Power_setDependency(PowerMSP432E4_PERIPH_USB0);  
	
    /* Enable the USB peripheral */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    /* Setup pins for USB operation */
    if (usbMode == MSP_EXP432E401Y_USBHOST || usbMode == MSP_EXP432E401Y_USBDEVICE) {
        GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
        GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    }

    /* Additional configurations for Host mode */
    if (usbMode == MSP_EXP432E401Y_USBHOST) {
        /* Configure the pins needed */
        HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
        HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0xff;
        GPIOPinConfigure(GPIO_PD6_USB0EPEN);
        GPIOPinTypeUSBDigital(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

        /*
         *  USB bus fault is routed to pin PQ4.  We create a Hwi to allow us
         *  to detect power faults and recover gracefully or terminate the
         *  program.  PQ4 is active low; set the pin as input with a weak
         *  pull-up.
         */
        GPIOPadConfigSet(GPIO_PORTQ_BASE, GPIO_PIN_4,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
        GPIOIntTypeSet(GPIO_PORTQ_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
        GPIOIntClear(GPIO_PORTQ_BASE, GPIO_PIN_4);

        /* Create a Hwi for PQ4 pin. */
        HwiP_Params_init(&hwiParams);
        HwiP_create(INT_GPIOQ4,
                      MSP_EXP432E401Y_usbBusFaultHwi, &hwiParams);
    }

    /* Mode is High Speed */
    if (usbMode == MSP_EXP432E401Y_USBULPI) {
        GPIOPinConfigure(GPIO_PL0_USB0D0);
        GPIOPinConfigure(GPIO_PL1_USB0D1);
        GPIOPinConfigure(GPIO_PL2_USB0D2);
        GPIOPinConfigure(GPIO_PL3_USB0D3);
        GPIOPinConfigure(GPIO_PL4_USB0D4);
        GPIOPinConfigure(GPIO_PL5_USB0D5);
        GPIOPinConfigure(GPIO_PP5_USB0D6);
        GPIOPinConfigure(GPIO_PP4_USB0D7);
        GPIOPinConfigure(GPIO_PB3_USB0CLK);
        GPIOPinConfigure(GPIO_PB2_USB0STP);
        GPIOPinConfigure(GPIO_PP3_USB0DIR);
        GPIOPinConfigure(GPIO_PP2_USB0NXT);

        GPIOPinTypeUSBDigital(GPIO_PORTL_BASE, GPIO_PIN_0);
        GPIOPinTypeUSBDigital(GPIO_PORTL_BASE, GPIO_PIN_1);
        GPIOPinTypeUSBDigital(GPIO_PORTL_BASE, GPIO_PIN_2);
        GPIOPinTypeUSBDigital(GPIO_PORTL_BASE, GPIO_PIN_3);
        GPIOPinTypeUSBDigital(GPIO_PORTL_BASE, GPIO_PIN_4);
        GPIOPinTypeUSBDigital(GPIO_PORTL_BASE, GPIO_PIN_5);
        GPIOPinTypeUSBDigital(GPIO_PORTP_BASE, GPIO_PIN_5);
        GPIOPinTypeUSBDigital(GPIO_PORTP_BASE, GPIO_PIN_4);
        GPIOPinTypeUSBDigital(GPIO_PORTB_BASE, GPIO_PIN_3);
        GPIOPinTypeUSBDigital(GPIO_PORTB_BASE, GPIO_PIN_2);
        GPIOPinTypeUSBDigital(GPIO_PORTP_BASE, GPIO_PIN_3);
        GPIOPinTypeUSBDigital(GPIO_PORTP_BASE, GPIO_PIN_2);
    }
}
