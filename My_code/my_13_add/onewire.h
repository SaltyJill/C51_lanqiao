#ifndef __ONEWIRE_H
#define __ONEWIRE_H
#include "main.h"
sbit DQ = P1 ^ 4;
void DS18B20_START_CV(void);
signed int DS18B20_RD(void);
#endif