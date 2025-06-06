#include "driver.h"
extern volatile u32 T_1MS;
// LED 显示
void Led_DP(u8 out)
{
    P0 = ~out;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
}
// 关闭蜂鸣器，LED 数码管等外设
void Other_CLS(void)
{
    /*LED close*/
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
    /*Speaker butter close*/
    P0 = 0x00;
    P2 = (P2 & 0x1F) | 0xA0;
    P2 &= 0x1F;
}
// 定时器T1设置
void T1_INT(void) // 12T 12M
{
    TMOD &= 0x0F;
    AUXR &= ~(1 << 6);
    TH1 = 0xFC;
    TL1 = 0x18;
    TF1 = 0;
    EA = 1;
    ET1 = 1;
    TR1 = 1;
}
// 定时器T0设置
void T0_INT(void)
{
    TMOD &= 0xF0;
    AUXR &= ~(1 << 7);
    TH0 = 0x00;
    TL0 = 0x00;
    TF0 = 0;
    TR0 = 0;
}
void UART_INT(void)
{
    SCON = 0x50;
    AUXR |= 0x01;
    AUXR |= 0x04;
    T2H = 0xFE;
    T2L = 0xC7;
    AUXR |= 0x10;
    EA = 1;
    ES = 1;
}
void UART_SEND(u8 *pstr)
{
    u8 i = 0;
    while (pstr[i])
    {
        SBUF = pstr[i];
        while (!TI)
            ;
        TI = 0;
        i++;
    }
}
// 数码管显示码转换
void Seg_TR(u8 *pDisp, u8 *pCode)
{
    u8 i, j;
    for (i = 0, j = 0; j < 8; i++, j++)
    {
        switch (*(pDisp + i))
        {
        case '0':
            *(pCode + j) = 0xc0;
            break;
        case '1':
            *(pCode + j) = 0xf9;
            break;
        case '2':
            *(pCode + j) = 0xa4;
            break;
        case '3':
            *(pCode + j) = 0xb0;
            break;
        case '4':
            *(pCode + j) = 0x99;
            break;
        case '5':
            *(pCode + j) = 0x92;
            break;
        case '6':
            *(pCode + j) = 0x82;
            break;
        case '7':
            *(pCode + j) = 0xf8;
            break;
        case '8':
            *(pCode + j) = 0x80;
            break;
        case '9':
            *(pCode + j) = 0x90;
            break;
        case 'A':
            *(pCode + j) = 0x88;
            break;
        case 'B':
            *(pCode + j) = 0x83;
            break;
        case 'C':
            *(pCode + j) = 0xc6;
            break;
        case 'D':
            *(pCode + j) = 0xa1;
            break;
        case 'E':
            *(pCode + j) = 0x86;
            break;
        case 'F':
            *(pCode + j) = 0x8e;
            break;
        case 'U': // 0011 1110->1100 0001
            *(pCode + j) = 0xc1;
            break;
        case '-':
            *(pCode + j) = 0xbf;
            break;
        case '.':
            j = j - 1;
            *(pCode + j) &= 0x7f;
            break;
        default:
            *(pCode + j) = 0xff;
            break;
        }
    }
}
// 数码管显示码输出
void Seg_DP(u8 *pCode, u8 Pos)
{
    /*熄灭数据端*/
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
    /*选择数码管位*/
    P0 = (u8)1 << Pos;
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;
    /*数据输出*/
    P0 = *(pCode + Pos);
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}
// 延时函数
void DELAY(u16 ct)
{
    u32 Tst;
    Tst = T_1MS;
    while (T_1MS - Tst < ct)
        ;
}
// 矩阵键盘读取
unsigned char Key_MARTIX(void)
{
    /*定义两个读取变量*/
    u8 key_RD0;
    u8 key_RD1;
    u8 key_NUM;
    P34 = 1;
    P35 = 0;
    key_RD0 = P3 & 0x0F;
    P34 = 0;
    P35 = 1;
    key_RD0 = (key_RD0 << 4) | (P3 & 0x0F);
    if (key_RD0 == 0xFF)
    {
        return 0;
    }
    DELAY(10);
    P34 = 1;
    P35 = 0;
    key_RD1 = P3 & 0x0F;
    P34 = 0;
    P35 = 1;
    key_RD1 = (key_RD1 << 4) | (P3 & 0x0F);
    if (key_RD1 != key_RD0)
    {
        return 0;
    }
    switch (~key_RD1)
    {
    case 0x80:
        key_NUM = 12;
        break;
    case 0x40:
        key_NUM = 13;
        break;
    case 0x08:
        key_NUM = 16;
        break;
    case 0x04:
        key_NUM = 17;
        break;
    default:
        key_NUM = 0;
        break;
    }

    return key_NUM;
}
// 继电器状态
void Relay(u8 sw)
{
    P0 = 0x0;
    P2 &= 0x1F;
    if (sw)
    {
        P0 |= (1 << 4);
    }
    else
    {
        P0 &= ~(1 << 4);
    }
    P2 = (P2 & 0x1F) | 0xA0;
    P2 &= 0x1F;
}