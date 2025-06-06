#ifndef __IIC_H
#define __IIC_H

#include "main.h"

sbit scl=P2^0;
sbit sda=P2^1;
unsigned char PCF8591_ADC(void);

#endif