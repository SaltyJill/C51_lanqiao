#ifndef __I2C_H
#define __I2C_H
#include "main.h"

sbit scl=P2^0;  //P20
sbit sda=P2^1;	//P21

unsigned char PCF8591_Adc(u8 ucCh);

#endif