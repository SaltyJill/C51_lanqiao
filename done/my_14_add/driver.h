#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"
void DEV_Cls(void);
void LED_Disp(u8 LEDisp);
void SEG_Tran(u8 *pSEGDisp, u8 *pSEGCode);
void SEG_Disp(u8 *pSEGCode, u8 SEGPosi);
unsigned char KEY_Martix(void);
void UART_Send(u8 *str);
void UART_Int(void);
void T1_Int(void);
void T0_Int(void);
#endif