#ifndef __IIC_H
#define __IIC_H
#include "main.h"
sbit scl=P2^0;
sbit sda=P2^1;
void PCF8591_DAC(u8 DACo);

#endif