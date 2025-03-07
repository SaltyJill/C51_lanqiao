#ifndef __TIME_H
#define __TIME_H
#include "main.h"
void led_dis(u8 led_out);
void outer_cls(void);
void Seg_trans(u8 *pseg_Disp, u8 *pseg_Code);
void Seg_Disp(u8 *pseg_Code, u8 seg_Add);
void T1_Init(void);
unsigned char key_MatrixRead(void);

#endif