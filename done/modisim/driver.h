#ifndef __DRIVER_H
#define __DRIVER_H

#include "main.h"
// led 显示
void Led_Out(u8 ouT);
// 外设关闭
void Other_Cls(void);
// T1设置
void T1_int(void); // 12T,1Mhz
// 数码管转换
void Seg_TR(u8 *pSegDP, u8 *pSegCD);
// 数码管显示
void Seg_DP(u8 *pSegCD, u8 SEG_PS);
// 继电器控制
void Relay(u8 nof);
// 矩阵键盘读取
unsigned char Key_MARTIX(void);

#endif