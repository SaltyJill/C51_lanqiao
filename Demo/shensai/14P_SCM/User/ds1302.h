#ifndef __DS1302_H
#define __DS1302_H

#include "stc15f2k60s2.h"
#include "intrins.h"

sbit SCK=P1^7;		
sbit SDA=P2^3;		
sbit RST = P1^3;   // DS1302复位

typedef struct{
	unsigned char rtc[6];
	unsigned char set[6];

	void (code *Read)(void);
	void (code *Set)(void);
}RTC;

void initRTCStruct(void);
RTC *getRTCStruct(void);

void ClearSecond(void);
void ClearMinute(void);



#endif
