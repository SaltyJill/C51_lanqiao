#ifndef __ONEWIRE_H
#define __ONEWIRE_H
#include "main.h"
sbit DQ=P1^4;
// 启动温度转换
void DS18b20_CV(void);
// 温度读取
u16 DS18b20_RD(void);
#endif