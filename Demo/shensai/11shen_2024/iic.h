#ifndef __I2C_H
#define __I2C_H
#include "main.h"

sbit scl=P2^0;  //P20
sbit sda=P2^1;	//P21
void EE_Write(u8* pucBuf, u8 addr, u8 num);
void EE_Read(u8* pucBuf, u8 addr,u8 num);
unsigned char PCF8591_Adc(void);
void PCF8591_Dac(unsigned char dat);
#endif