/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
*/

#ifndef ALICAT_H_
#define ALICAT_H_

#define ALICAT0     0
#define ALICAT1     1
#define ALICAT2     2

int alicat_open(int device);
int alicat_write(int device, char *buf, int len);
int alicat_read(int device, char *buf, int len);
int alicat_sync_request(int device, char *buf, int len);

#endif /* ALICAT_H_ */
