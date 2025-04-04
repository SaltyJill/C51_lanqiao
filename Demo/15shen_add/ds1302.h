#ifndef __DS1302_H
#define __DS1302_H
#include "main.h"

sbit SCK=P1^7;
sbit SDA=P2^3;
sbit RST=P1^3;

void Wr_Time(u8 *pucTime);
void Rd_Time(u8 *pucTime);
#endif