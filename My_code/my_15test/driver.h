#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"
void DEV_cls(void);
void T1_int(void);
void T0_int(void);
void LED_disp(u8 outp);
void Delay(u8 n);
void SEG_tran(u8 *pDP, u8 *pCD);
void SEG_disp(u8 *pCD, u8 PS);
unsigned char KEY_martix(void);
void URT_int(void);
void URT_send(u8 *str);
// which 1¼ÌµçÆ÷0·äÃùÆ÷£¬how1Æô¶¯0¹Ø±Õ
void DEV_out(u8 which,u8 how);
#endif