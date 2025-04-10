#ifndef __DS1302_H
#define __DS1302_H

#include "stc15f2k60s2.h"
#include "intrins.h"

/* DS1302时钟芯片接口定义 */
sbit SCK = P1^7;		
sbit SDA = P2^3;		
sbit RST = P1^3;   

void ReadRTC(void);
void InitRTC(void);
unsigned char* GetRTC(void);

#endif
