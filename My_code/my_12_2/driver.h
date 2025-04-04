#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"
void LED_Disp(u8 Lout);
void DEV_Cls(void);
void T1_Int(void);
void T0_Int(void);
void SEG_Tran(u8 *pstr,u8 *pcod);
void SEG_Disp(u8 *pcod,u8 poci);
u8 KEY_Independ(void);

#endif