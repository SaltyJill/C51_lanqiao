#ifndef __DS18B20_H
#define __DS18B20_H

#include "stc15f2k60s2.h"

sbit DQ = P1^4;

float getTemperature(void);

#endif
