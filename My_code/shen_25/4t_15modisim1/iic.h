#ifndef __IIC_H
#define __IIC_H
#include "main.h"
sbit scl=P2^0;
sbit sda=P2^1;
void E2_RD(u8 *pdate,u8 add,u8 n);
void E2_WR(u8 *pdate,u8 add,u8 n);
#endif