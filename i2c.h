#ifndef I2C_H_
#define I2C_H_

int I2C_Open(int device);
int I2C_IOctl(int device, int req, void *arg);
int I2C_Read(int device, char *data, int size);
int I2C_Write(int device, char *data, int size);

#endif /* I2C_H_ */
