#ifndef SERIAL_UART_H_
#define SERIAL_UART_H_

#define UART_SET_CALLBACK        0
#define CHANGE_BAUDRATE     1
#define SET_MODE         2

int SerialUART_Open(int device);
int SerialUART_Read(int device, char *data, int size);
int SerialUART_Write(int device, char *data, int size);
int SerialUART_IOctl(int device, int req, void *arg);

#endif /* SERIAL_UART_H_ */