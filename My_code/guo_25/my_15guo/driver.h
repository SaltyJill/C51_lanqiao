#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"

void T1_Int(void);
void T0_Int(void);
void DEV_Cls(void);
void LED_Disp(u8 out);
void SEG_Tran(u8 *pDP, u8 *pCD);
void SEG_Disp(u8 *pCD, u8 PS);
u8 KEY_Matrix(void);
void Relay(u8 a);
void Uart_Send(u8 *pstr);
void Uart_Int(void);

#endif