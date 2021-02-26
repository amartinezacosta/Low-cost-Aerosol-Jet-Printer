/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef I2C_H_
#define I2C_H_

int I2C_Open(int device);
int I2C_IOctl(int device, int req, void *arg);
int I2C_Read(int device, char *data, int size);
int I2C_Write(int device, char *data, int size);

#endif /* I2C_H_ */
