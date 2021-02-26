/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef DEVIO_H_
#define DEVIO_H_

typedef int(*devio_read_fxn)(int, char**);
typedef int(*devio_write_fxn)(int, char**);

int devio_register(devio_read_fxn read_fxn, devio_write_fxn write_fxn);

#endif /* DEVIO_H_ */
