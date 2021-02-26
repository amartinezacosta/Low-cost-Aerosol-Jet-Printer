/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

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
