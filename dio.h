#ifndef DIO_H_
#define DIO_H_

#define INPUT_INTERRUPT     0
#define DIO_SET_CALLBACK    1
#define INPUT               2

int DIO_Open(int device);
int DIO_Write(int device, char *arg, int len);
int DIO_Read(int device, char *arg, int len);
int DIO_IOctl(int device, int req, void *args);



#endif /* DIO_H_ */
