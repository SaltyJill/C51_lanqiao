#ifndef __DS18B20_H
#define __DS18B20_H
#include "main.h"
sbit DQ = P1 ^ 4;
void DS18B20_ST(void);
unsigned int DS18B20_RD(void);
#endif