/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef SERIAL_USB_H_
#define SERIAL_USB_H_

#define CONNECT_DELAY       120000000
#define CONNECTED           1
#define DISCONNECTED        0
#define WRITE_TIMEOUT       12000000
#define READ_TIMEOUT        12000000

int SerialUSB_Open(int device);
int SerialUSB_Read(int device, char *data, int size);
int SerialUSB_Write(int device, char *data, int size);
int SerialUSB_IOctl(int device, int req, void *arg);

#endif /* SERIAL_USB_H_ */
