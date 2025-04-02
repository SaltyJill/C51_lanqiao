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
void T1_Int(void)
{
    AUXR &= ~(1 << 6);
    TMOD &= 0x0F;
    TH1 = 0xFC;
    TL1 = 0x18;
    ET1 = 1;
    EA = 1;
    TF1 = 0;
    TR1 = 1;
}
void T0_Int(void)
{
    AUXR &= ~(1 << 7);
    TMOD |= 0x04;
    TH0 = 0x0;
    TL0 = 0x0;
    TF0 = 0;
    TR0 = 1;
}
void LED_Disp(u8 LEDisp)
{
    P0 = ~LEDisp;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
}
void SEG_Tran(u8 *pSEGDisp, u8 *pSEGCode)
{
    u8 i, j;
    for (j = 0, i = 0; i < 8; j++, i++)
    {
        switch (*(pSEGDisp + j))
        {
        case '0':
            *(pSEGCode + i) = 0xC0;
            break;
        case '1':
            *(pSEGCode + i) = 0xf9;
            break;
        case '2':
            *(pSEGCode + i) = 0xa4;
            break;
        case '3':
            *(pSEGCode + i) = 0xb0;
            break;
        case '4':
            *(pSEGCode + i) = 0x99;
            break;
        case '5':
            *(pSEGCode + i) = 0x92;
            break;
        case '6':
            *(pSEGCode + i) = 0x82;
            break;
        case '7':
            *(pSEGCode + i) = 0xf8;
            break;
        case '8':
            *(pSEGCode + i) = 0x80;
            break;
        case '9':
            *(pSEGCode + i) = 0x90;
            break;
        case '-':
            *(pSEGCode + i) = 0xBF;
            break;
        case 'C':
            *(pSEGCode + i) = 0xC6;
            break;
        case 'H':
            *(pSEGCode + i) = 0x89;
            break;
        case 'F':
            *(pSEGCode + i) = 0x8E;
            break;
        case 'P':
            *(pSEGCode + i) = 0x8C;
            break;
        case 'E':
            *(pSEGCode + i) = 0x86;
            break;
        case 'A':
            *(pSEGCode + i) = 0x88;
            break;
        case '.':
            if (i > 0)
            {
                i = i - 1;
            }
            *(pSEGCode + i) &= 0x7F;
            break;
        default:
            *(pSEGCode + i) = 0xFF;
            break;
        }
    }
}
void SEG_Disp(u8 *pSEGCode, u8 SEGPosi)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;

    P0 = (u8)1 << SEGPosi;
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;

    P0 = *(pSEGCode + SEGPosi);
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}
void DELAY(u8 dct)
{
    u32 Dtime;
    Dtime = T1_1MS;
    while (T1_1MS - Dtime < dct)
    {
        ;
    }
}
unsigned char KEY_Martix(void)
{
    u8 RD0, RD1;
    u8 BTN = 0;
    P44 = 0;
    P42 = 1;
    RD0 = P3 & 0x0F;
    P44 = 1;
    P42 = 0;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
    if (RD0 == 0x0F)
    {
        return 0;
    }
    DELAY(10);
    P44 = 0;
    P42 = 1;
    RD1 = P3 & 0x0F;
    P44 = 1;
    P42 = 0;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    if (RD0 == RD1)
    {
        switch (~RD1)
        {
        case 0x80:
            BTN = 4;
            break;
        case 0x40:
            BTN = 5;
            break;
        case 0x20:
            BTN = 6;
            break;
        case 0x10:
            BTN = 7;
            break;
        case 0x08:
            BTN = 8;
            break;
        case 0x04:
            BTN = 9;
            break;
        case 0x02:
            BTN = 10;
            break;
        case 0x01:
            BTN = 11;
            break;
        default:
            BTN = 0;
            break;
        }
    }
    return BTN;
}