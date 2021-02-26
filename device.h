/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef DEVICE_H_
#define DEVICE_H_

int open(char *path);
int read(int fd, char *buf, int len);
int write(int fd, char *buf, int len);
int ioctl(int fd, int rqst, void *args);

#endif /* DEVICE_H_ */
