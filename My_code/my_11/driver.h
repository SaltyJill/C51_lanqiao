#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"
// led 显示
void led_Disp(u8 ledD);
// 外设关闭
void dev_Cls(void);
// 定时器设置
void T1_int(void);
// 数码管转换
void seg_Tran(u8 *pseg_Disp, u8 *pseg_Code);
// 数码管显示
void seg_Disp(u8 *pseg_Code, u8 segPos);
// 矩阵键盘读取
unsigned char key_Martix(void);
#endif