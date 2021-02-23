#ifndef DEVIO_H_
#define DEVIO_H_

typedef int(*devio_read_fxn)(int, char**);
typedef int(*devio_write_fxn)(int, char**);

int devio_register(devio_read_fxn read_fxn, devio_write_fxn write_fxn);

#endif /* DEVIO_H_ */
