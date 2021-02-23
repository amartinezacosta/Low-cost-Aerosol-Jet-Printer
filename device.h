#ifndef DEVICE_H_
#define DEVICE_H_

int open(char *path);
int read(int fd, char *buf, int len);
int write(int fd, char *buf, int len);
int ioctl(int fd, int rqst, void *args);

#endif /* DEVICE_H_ */
