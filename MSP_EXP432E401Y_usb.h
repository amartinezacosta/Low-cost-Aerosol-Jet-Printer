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
/** ============================================================================
 *  @file       MSP_EXP432E401Y.h
 *
 *  @brief      MSP_EXP432E401Y USB Board APIs
 *
 *  ============================================================================
 */

#ifndef MSP_EXP432E401Y_H_
#define MSP_EXP432E401Y_H_

/*!
 *  @def    MSP_EXP432E401Y_USBMode
 *  @brief  Enum of USB setup function on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_USBMode {
    MSP_EXP432E401Y_USBULPI,
    MSP_EXP432E401Y_USBDEVICE,
    MSP_EXP432E401Y_USBHOST
} MSP_EXP432E401Y_USBMode;

/*!
 *  @def    MSP_EXP432E401Y_USBMSCHFatFsName
 *  @brief  Enum of USBMSCHFatFs names on the MSP_EXP432E401Y dev board
 */
typedef enum MSP_EXP432E401Y_USBMSCHFatFsName {   //only required if MSC is selected
    MSP_EXP432E401Y_USBMSCHFATFS0 = 0,

    MSP_EXP432E401Y_USBMSCHFATFSCOUNT
} MSP_EXP432E401Y_USBMSCHFATFSName;

/*!
 *  @brief  Initialize board specific USB settings
 *
 *  This function initializes the board specific USB settings and pins based on
 *  the USB mode of operation.
 *
 *  @param      usbMode    USB mode of operation
 */
extern void MSP_EXP432E401Y_initUSB(MSP_EXP432E401Y_USBMode usbMode);

/*!
 *  @brief  Initialize board specific USBMSCHFatFs settings
 *
 *  This function initializes the board specific USBMSCHFatFs settings and then
 *  calls the USBMSCHFatFs_init API to initialize the USBMSCHFatFs module.
 *
 *  The USBMSCHFatFs peripherals controlled by the USBMSCHFatFs module are
 *  determined by the USBMSCHFatFs_config variable.
 */
extern void MSP_EXP432E401Y_initUSBMSCHFatFs(void);    //only if MSC is selected

#endif /* MSP_EXP432E401Y_H_ */
