#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"

void DEV_Cls(void);
void LED_Disp(u8 out);
void T1_Int(void);   // 12T 1ms
void T0_Int(void);   // cnt
void Uart_Int(void); // 9600bps@12.000MHz
void Uart_Send(u8 *pstr);
void Seg_Tran(u8 *pDip, u8 *pCod);
void SEG_Disp(u8 *pCod, u8 pos);
u8 KEY_Matrix(void);
void Relay(u8 st);

#endif