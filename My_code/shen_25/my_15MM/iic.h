#ifndef __IIC_H
#define __IIC_H
#include "main.h"
sbit scl = P2 ^ 0;
sbit sda = P2 ^ 1;
// 读取chanl通道电压值
unsigned char PCF8591_ADC(u8 chanl);
// 对通道三进行DAC输入
void PCF8591_DAC(u8 DACp);
void EE_WR(u8 *pData, u8 add, u8 n);
void EE_RD(u8 *pData, u8 add, u8 n);

#endif