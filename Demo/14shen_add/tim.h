#ifndef __TIM_H
#define __TIM_H

#include "main.h"

void Led_Disp(u8 ucOut);
void Cls_Waishe(void);
void Relay(u8 ucVal);

void T1_Init(void);
void T0_Init(void);
void Delay(u16 usIn);

void Seg_Tran(u8 *pucBuff,u8 *pucCode);
void Seg_Disp(u8 *pCode,u8 ucPos);

u8 KBD_Read(void);

u8 MaoPao_Filter(u8 *pucBuff,u8 ucLen);
u8 ComPare_Filter(u8 *pucBuff,u8 ucLen);

void UartInit(void);	  //9600bps@12.000MHz
void UartSend(u8 *str1);
#endif