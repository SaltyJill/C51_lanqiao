#ifndef __DS1302_H
#define __DS1302_H
#include "main.h"
sbit RST = P1 ^ 3;
sbit SCK = P1 ^ 7;
sbit SDA = P2 ^ 3;
// 时钟芯片初值设定
void DS1302_SET(u8 *ptime);
// 时钟芯片读写
void DS1302_RED(u8 *prdtime);
#endif