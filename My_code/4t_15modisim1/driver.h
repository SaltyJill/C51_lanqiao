#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"

void DEV_Cls(void);
void LED_Disp(u8 out);
void T1_Int(void);
void SEG_Tran(u8 *DP, u8*CD);
void SEG_Disp(u8 *CD, u8 PS);
u8 KEY_Martix(void);

#endif