#ifndef __IIC_H
#define __IIC_H
#include "main.h"
sbit RST=P1^3;
sbit SCK=P1^7;
sbit SDA=P2^3;

void DS1302_SET(u8 *ptimeSET);
void DS1302_RED(u8 *ptimeRED);
#endif