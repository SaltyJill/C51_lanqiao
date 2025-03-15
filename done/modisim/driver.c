#include "driver.h"
extern volatile u32 ISR_1MS;
void Led_Out(u8 ouT)
{
    P0 = ~ouT;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
}
void Other_Cls(void)
{
    P0 = 0x0;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
    P0 = 0x00;
    P2 = (P2 & 0x1F) | 0xA0;
    P2 &= 0x1F;
}
void T1_int(void) // 12T,1Mhz
{
    TH1 = 0xFC;
    TL1 = 0x18;
    AUXR &= ~(1 << 6);
    TMOD &= 0x0F;
    TF1 = 0;
    EA = 1;
    ET1 = 1;
    TR1 = 1;
}
void Seg_TR(u8 *seg_DISP, u8 *seg_CODE)
{
    u8 i, j;
    for (j = 0, i = 0; j < 8; i++, j++)
    {
        switch (*(seg_DISP + i))
        {
        case '0':
            *(seg_CODE + j) = 0xC0;
            break;
        case '1':
            *(seg_CODE + j) = 0xF9;
            break;
        case '2':
            *(seg_CODE + j) = 0xA4;
            break;
        case '3':
            *(seg_CODE + j) = 0xB0;
            break;
        case '4':
            *(seg_CODE + j) = 0x99;
            break;
        case '5':
            *(seg_CODE + j) = 0x92;
            break;
        case '6':
            *(seg_CODE + j) = 0x82;
            break;
        case '7':
            *(seg_CODE + j) = 0xF8;
            break;
        case '8':
            *(seg_CODE + j) = 0x80;
            break;
        case '9':
            *(seg_CODE + j) = 0x90;
            break;
        case 'C':
            *(seg_CODE + j) = 0xC6;
            break;
        case 'E':
            *(seg_CODE + j) = 0x86;
            break;
        case 'H':
            *(seg_CODE + j) = 0x89;
            break;
        default:
            *(seg_CODE + j) = 0xFF;
            break;
        }
    }
}
void Seg_DP(u8 *seg_CODE, u8 seg_POSI)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
    P0 = (u8)1 << seg_POSI;
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;
    P0 = *(seg_CODE + seg_POSI);
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}
void Delay(u8 uk)
{
    u32 uTime = 0;
    uTime = ISR_1MS;
    while (ISR_1MS - uTime < uk)
    {
        /* code */
    }
}
void Relay(u8 nof)
{
    P0 = 0x00;
    P2 &= 0x1F;
    if (nof)
    {
        P0 |= (1 << 4);
    }
    else
    {
        P0 &= ~(1 << 4);
    }
    P2 |= 0xA0;
    P2 &= 0x1F;
}
unsigned char Key_MARTIX(void)
{
    u16 RD0, RD1;
    u8 button;
    P44 = 0;
    P42 = 1;
    P35 = 1;
    P34 = 1;
    RD0 = (P3 & 0x0F);
    P44 = 1;
    P42 = 0;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
    P42 = 1;
    P35 = 0;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
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
    RD1 = (P3 & 0x0F);
    P44 = 1;
    P42 = 0;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    P42 = 1;
    P35 = 0;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    P35 = 1;
    P34 = 0;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    if (RD0 != RD1)
    {
        return 0;
    }
    switch (~RD1)
    {
    case 0x8000:
        button = 4;
        break;
    case 0x4000:
        button = 5;
        break;
    case 0x2000:
        button = 6;
        break;
    case 0x1000:
        button = 7;
        break;
    case 0x0800:
        button = 8;
        break;
    case 0x0400:
        button = 9;
        break;
    case 0x0200:
        button = 10;
        break;
    case 0x0100:
        button = 11;
        break;
    case 0x0080:
        button = 12;
        break;
    case 0x0040:
        button = 13;
        break;
    case 0x0020:
        button = 14;
        break;
    case 0x0010:
        button = 15;
        break;
    case 0x0008:
        button = 16;
        break;
    case 0x0004:
        button = 17;
        break;
    case 0x0002:
        button = 18;
        break;
    case 0x0001:
        button = 19;
        break;
    default:
        button = 0;
        break;
    }
    return button;
}
