#ifndef __ONEWIRE_H
#define __ONEWIRE_H
#include "main.h"
sbit DQ=P1^4;
// �����¶�ת��
void DS18b20_CV(void);
// �¶ȶ�ȡ
u16 DS18b20_RD(void);
#endif