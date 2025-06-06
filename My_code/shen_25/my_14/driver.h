#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"
void DEV_Cls(void);
void LED_Disp(u8 out);
void T1_Int(void);
void T0_Int(void);
void SEG_Tran(u8 *pDP,u8 *pCD);
void SEG_Disp(u8 *pCD,u8 PS);
unsigned KEY_Martix(void);
#endif
