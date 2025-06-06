#include "driver.h"
extern volatile u32 T1_1MS;

void DEV_Cls(void)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;

    P0 = 0x00;
    P2 = (P2 & 0x1F) | 0xA0;
    P2 &= 0x1F;
}

void LED_Disp(u8 out)
{
    P0 = (u8)~out;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
}

void T1_Int(void) // 12T 1ms
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

void T0_Int(void) // cnt
{
    TMOD &= 0x04;
    AUXR &= ~(1 << 7);
    TH0 = 0x00;
    TL0 = 0x00;
    TF0 = 0;
    TR0 = 0;
}

void Uart_Int(void) // 9600bps@12.000MHz
{
    SCON = 0x50;  // 8位数据,可变波特率
    AUXR |= 0x01; // 串口1选择定时器2为波特率发生器
    AUXR |= 0x04; // 定时器时钟1T模式
    T2L = 0xC7;   // 设置定时初始值
    T2H = 0xFE;   // 设置定时初始值
    AUXR |= 0x10; // 定时器2开始计时
}

void Uart_Send(u8 *pstr)
{
    u8 i = 0;
    while (pstr[i])
    {
        SBUF = pstr[i];
        while (!TI)
        {
            ;
        }
        i++;
    }
}

void Delay(u8 n)
{
    u32 Dtime;
    Dtime = T1_1MS;
    while (T1_1MS - Dtime < n)
    {
        ;
    }
}

void Seg_Tran(u8 *pDip, u8 *pCod)
{
    u8 i, j;
    for (j = 0, i = 0; j < 8; i++, j++)
    {
        switch (*(pDip + i))
        {
        case '0':
            *(pCD + j) = 0xC0;
            break;
        case '1':
            *(pCD + j) = 0xF9;
            break;
        case '2':
            *(pCD + j) = 0xA4;
            break;
        case '3':
            *(pCD + j) = 0xB0;
            break;
        case '4':
            *(pCD + j) = 0x99;
            break;
        case '5':
            *(pCD + j) = 0x92;
            break;
        case '6':
            *(pCD + j) = 0x82;
            break;
        case '7':
            *(pCD + j) = 0xF8;
            break;
        case '8':
            *(pCD + j) = 0x80;
            break;
        case '9':
            *(pCD + j) = 0x90;
            break;
        case 'L':
            *(pCD + j) = 0x87;
            break;
        case 'E':
            *(pCD + j) = 0x86;
            break;
        case 'P':
            *(pCD + j) = 0x8C;
            break;
        case '-':
            *(pCD + j) = 0xBF;
            break;
        case '.':
            if (j > 0)
            {
                j--;
            }
            *(pCD + j) &= 0x7F;
        default:
            *(pCod + j) = 0xFF;
            break;
        }
    }
}

void SEG_Disp(u8 *pCod, u8 pos)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;

    P0 = (u8)(1 << pos);
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;

    P0 = *(pCod + pos);
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}

unsigned char KEY_Matrix(void)
{
    u16 RD0, RD1;
    u8 btn;
    P44 = 0;
    P42 = 1;
    P35 = 1;
    P34 = 1;
    RD0 = P3 & 0x0F;
    P44 = 1;
    P42 = 0;
    P35 = 1;
    P34 = 1;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
    P44 = 1;
    P42 = 1;
    P35 = 0;
    P34 = 1;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
    P44 = 1;
    P42 = 1;
    P35 = 1;
    P34 = 0;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
    if (RD0 == 0xFFFF)
    {
        return 0;
    }
    Delay(10);
    P44 = 0;
    P42 = 1;
    P35 = 1;
    P34 = 1;
    RD1 = P3 & 0x0F;
    P44 = 1;
    P42 = 0;
    P35 = 1;
    P34 = 1;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    P44 = 1;
    P42 = 1;
    P35 = 0;
    P34 = 1;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    P44 = 1;
    P42 = 1;
    P35 = 1;
    P34 = 0;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    if (RD0 == RD1)
    {
        switch (~RD1)
        {
        case 0x8000:
            btn = 4;
            break;
        case 0x4000:
            btn = 5;
            break;
        case 0x0800:
            btn = 8;
            break;
        case 0x0400:
            btn = 9;
            break;
        case 0x0080:
            btn = 12;
            break;
        case 0x0040:
            btn = 13;
            break;

        default:
            btn = 0;
            break;
        }
    }
    return btn;
}