#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"
// LED 显示
void Led_DP(u8 out);
// 关闭蜂鸣器，LED 数码管等外设
void Other_CLS(void);
// 定时器T1设置
void T1_INT(void);// 12T 12M
// 定时器T0设置
void T0_INT(void);
// 串口设置
void UART_INT(void);
// 数码管显示码转换
void Seg_TR(u8 *pDisp, u8 *pCode);
// 数码管显示码输出
void Seg_DP(u8 *pCode, u8 Pos);
// 延时函数
void DELAY(u16 ct);
// 矩阵键盘读取
unsigned char Key_MARTIX(void);
// 继电器状态
void Relay(u8 sw);
// 串口发送
void UART_SEND(u8 *pstr);
#endif