#ifndef __DS1302_H
#define __DS1302_H
#include "main.h"
sbit RST = P1 ^ 3;
sbit SCK = P1 ^ 7;
sbit SDA = P2 ^ 3;
// DS1302 初值设置
void DS1302_SET(u8 *pTime_begin);
// DS1302 读取时间
void DS1302_RD(u8 *pTime_now);
#endif