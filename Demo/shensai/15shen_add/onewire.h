#ifndef __ONEWIRE_H
#define __ONEWIRE_H
#include "main.h"
sbit DQ=P1^4;
void Wd_Convert(void);
signed int Wd_Read(void);
#endif