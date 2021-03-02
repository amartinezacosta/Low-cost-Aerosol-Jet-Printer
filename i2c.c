/*
Copyright 2021 National Technology & Engineering Solutions of Sandia, LLC (NTESS). 
Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains 
certain rights in this software.

----------------------------------------------------------------------------------

MIT License

Copyright (c) 2021 Alejandro Martinez-Acosta
Copyright (c) 2021 Ethan B. Secor

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <i2c.h>
#include <ti/drivers/I2C.h>

/*Failed to initialize UART peripheral*/
#define ASSERT_I2C(id) if(id == 0){ while(1); }

typedef struct
{
    I2C_Handle handle;
    uint8_t address;
}i2c_t;

i2c_t i2c_list[] =
{
 {NULL, 0x11}
};

int i2c_count = sizeof(i2c_list)/sizeof(i2c_t);

int I2C_Open(int device)
{
    i2c_t *i = &i2c_list[device];

    I2C_Params i2cParams;

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_100kHz;
    i->handle = I2C_open(device, &i2cParams);

    ASSERT_I2C(i->handle);
    return 0;
}


int I2C_IOctl(int device, int req, void *arg)
{
    return 0;
}

int I2C_Read(int device, char *data, int size)
{
    i2c_t *i = &i2c_list[device];

    I2C_Transaction transaction;

    transaction.slaveAddress = i->address;
    transaction.writeBuf = 0;
    transaction.writeCount = 0;
    transaction.readBuf = data;
    transaction.readCount = size;

    return I2C_transfer(i->handle, &transaction);
}

int I2C_Write(int device, char *data, int size)
{
    i2c_t *i = &i2c_list[device];

    I2C_Transaction transaction;

    transaction.slaveAddress = i->address;
    transaction.readBuf = 0;
    transaction.readCount = 0;
    transaction.writeBuf = data;
    transaction.writeCount = size;

    return I2C_transfer(i->handle, &transaction);
}









