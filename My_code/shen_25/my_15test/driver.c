#include "driver.h"
extern volatile u32 T1_1ms;
void DEV_cls(void)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;

    P0 = 0x00;
    P2 = (P2 & 0x1F) | 0xA0;
    P2 &= 0x1F;
}
void T1_int(void)
{
    TMOD &= 0x0F;
    AUXR &= ~(1 << 6);
    TH1 = 0xFC;
    TL1 = 0x18;
    EA = 1;
    ET1 = 1;
    TF1 = 0;
    TR1 = 1;
}
/*
void T0_int(void)
{
    TMOD &= 0xF0;
    AUXR &= ~(1 << 7);
    TH0 = 0x00;
    TL0 = 0x00;
    TF0 = 0;
    TR0 = 0;
}
*/
void LED_disp(u8 outp)
{
    P0 = ~outp;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
}
void Delay(u8 n)
{
    u32 DelayT;
    DelayT = T1_1ms;
    while (T1_1ms - DelayT < n)
        ;
}
void SEG_tran(u8 *pDP, u8 *pCD)
{
    u8 i, j;
    for (j = 0, i = 0; i < 8; j++, i++)
    {
        switch (*(pDP + j))
        {
        case '0':
            *(pCD + i) = 0xC0;
            break;
        case '1':
            *(pCD + i) = 0xF9;
            break;
        case '2':
            *(pCD + i) = 0xA4;
            break;
        case '3':
            *(pCD + i) = 0xB0;
            break;
        case '4':
            *(pCD + i) = 0x99;
            break;
        case '5':
            *(pCD + i) = 0x92;
            break;
        case '6':
            *(pCD + i) = 0x82;
            break;
        case '7':
            *(pCD + i) = 0xF8;
            break;
        case '8':
            *(pCD + i) = 0x80;
            break;
        case '9':
            *(pCD + i) = 0x90;
            break;
        case '-':
            *(pCD + i) = 0xBF;
            break;
        case 'F':
            *(pCD + i) = 0x8E;
            break;
        case 'P':
            *(pCD + i) = 0x8C;
            break;
        case 'H':
            *(pCD + i) = 0x89;
            break;
        case 'A':
            *(pCD + i) = 0x88;
            break;
        case 'L':
            *(pCD + i) = 0xC7;
            break;
        case '.':
            if (i > 0)
            {
                i--;
            }
            *(pCD + i) &= 0x7F;
            break;
        default:
            *(pCD + i) = 0xFF;
            break;
        }
    }
}
void SEG_disp(u8 *pCD, u8 PS)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;

    P0 = (u8)1 << PS;
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;

    P0 = *(pCD + PS);
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}
unsigned char KEY_martix(void)
{
    u8 RD0 = 0;
    u8 RD1 = 0;
    u8 button = 0;
    P42 = 1;
    P44 = 0;
    RD0 = (P3 & 0x0F);
    P42 = 0;
    P44 = 1;
    RD0 = (RD0 << 4) | P3 & 0x0F;
    if (RD0 == 0xFF)
    {
        return 0;
    }
    Delay(20);
    P42 = 1;
    P44 = 0;
    RD1 = (P3 & 0x0F);
    P42 = 0;
    P44 = 1;
    RD1 = (RD1 << 4) | P3 & 0x0F;
    if (RD0 != RD1)
    {
        return 0;
    }
    switch (~RD1)
    {
    case 0x80:
        button = 4;
        break;
    case 0x40:
        button = 5;
        break;
    case 0x20:
        button = 6;
        break;
    case 0x10:
        button = 7;
        break;
    case 0x08:
        button = 8;
        break;
    case 0x04:
        button = 9;
        break;
    case 0x02:
        button = 10;
        break;
    case 0x01:
        button = 11;
        break;
    default:
        button = 0;
        break;
    }
    return button;
}
void URT_int(void)
{
    SCON = 0x50;
    T2L = 0x64;
    T2H = 0xFF;
    AUXR |= 0x15;
    EA = 1;
    ES = 1;
}
void URT_send(u8 *str)
{
    u8 i = 0;
    while (str[i])
    {
        SBUF = str[i];
        while (!TI)
            ;
        TI = 0;
        i++;
    }
}
void DEV_out(u8 which, u8 how)
{
    if (which)
    {
        if (how)
        {
            P0 |= (1 << 4);
        }
        else
        {
            P0 &= ~(1 << 4);
        }
    }
    else
    {
        if (how)
        {
            P0 |= (1 << 6);
        }
        else
        {
            P0 &= ~(1 << 6);
        }
    }
		P2 = (P2 & 0x1F) | 0xA0;
    P2 &= 0x1F;
}