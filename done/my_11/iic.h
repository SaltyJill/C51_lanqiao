#ifndef __I2C_H
#define __I2C_H
#include "main.h"
sbit scl = P2 ^ 0;
sbit sda = P2 ^ 1;
// E^2ROM读取
void E2_RD(u8 *pDataER, u8 add, u8 nbyte);
// E^2ROM写入
void E2_WR(u8 *pDataEW, u8 add, u8 nbyte);
// ADC转换
unsigned char PCF8591_ADC(void);
// DAC输入
void PCF8591_DAC(u8 DACout);
#endif