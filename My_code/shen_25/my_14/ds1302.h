#ifndef __DS1302_H
#define __DS1302_H
#include "main.h"
sbit SCK=P1^7;
sbit SDA=P2^3;
sbit RST=P1^3;
void DS1302_ST(u8 *pStime);
void DS1302_RD(u8 *pRtime);
#endif