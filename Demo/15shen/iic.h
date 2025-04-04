#ifndef __IIC_H
#define __IIC_H
#include "main.h"
sbit scl=P2^0;
sbit sda=P2^1;

u8 PCF8591_Read(u8 ucCh);
void PCF8591_Write(u8 ucVal);
#endif