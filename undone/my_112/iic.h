#ifndef __IIC_H
#define __IIC_H
#include "main.h"
sbit scl=P2^0;
sbit sda=P2^1;
void EE_WR(u8 *pBuf, u8 addr, u8 n);
void EE_RD(u8 *pBuf, u8 addr, u8 n);

void PCF8591_Dac(u8 dac_Data);
unsigned char PCF8591_ADC(void);

#endif