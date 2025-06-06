#include "driver.h"
extern volatile u32 ISR_T1;
void led_Disp(u8 ledD)
{
    P0 = ~ledD;
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
}
void dev_Cls(void)
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
    AUXR &= ~(1 << 6);
    TMOD &= 0x0F;
    TH1 = 0xFC;
    TL1 = 0x18;
    TF1 = 0;
    EA = 1;
    ET1 = 1;
    TR1 = 1;
}
void seg_Tran(u8 *pseg_Disp, u8 *pseg_Code)
{
    u8 i, j;
    for (j = 0, i = 0; j < 8; i++, j++)
    {
        switch (*(pseg_Disp + i))
        {
        case '0':
            *(pseg_Code + j) = 0xC0;
            break;
        case '1':
            *(pseg_Code + j) = 0xF9;
            break;
        case '2':
            *(pseg_Code + j) = 0xA4;
            break;
        case '3':
            *(pseg_Code + j) = 0xB0;
            break;
        case '4':
            *(pseg_Code + j) = 0x99;
            break;
        case '5':
            *(pseg_Code + j) = 0x92;
            break;
        case '6':
            *(pseg_Code + j) = 0x82;
            break;
        case '7':
            *(pseg_Code + j) = 0xF8;
            break;
        case '8':
            *(pseg_Code + j) = 0x80;
            break;
        case '9':
            *(pseg_Code + j) = 0x90;
            break;
        case 'U':
            *(pseg_Code + j) = 0xC1;
            break;
        case 'P':
            *(pseg_Code + j) = 0x8C;
            break;
        case 'N':
            *(pseg_Code + j) = 0xAB;
            break;
        case '.':
            if (j > 0)
            {
                j--;
            }
            *(pseg_Code + j) &= 0x7F;
            break;
        default:
            *(pseg_Code + j) = 0xFF;
            break;
        }
    }
}
void seg_Disp(u8 *pseg_Code, u8 segPos)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
    P0 = (u8)1 << segPos;
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;
    P0 = *(pseg_Code + segPos);
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
}
void DELAY(u8 Dms)
{
    u32 DelayT;
    DelayT = ISR_T1;
    while ((ISR_T1 - DelayT) < Dms)
    {
        ;
    }
}
unsigned char key_Martix(void)
{
    u8 keyRD0, keyRD1;
    u8 btN = 0;
    P35 = 0;
    P34 = 1;
    keyRD0 = (P3 & 0x0F);
    P35 = 1;
    P34 = 0;
    keyRD0 = (keyRD0 << 4) | (P3 & 0x0F);
    if (keyRD0 == 0xFF)
    {
        return 0;
    }
    DELAY(10);
    P35 = 0;
    P34 = 1;
    keyRD1 = (P3 & 0x0F);
    P35 = 1;
    P34 = 0;
    keyRD1 = (keyRD1 << 4) | (P3 & 0x0F);
    if (keyRD0 == keyRD1)
    {
        switch (~keyRD1)
        {
        case 0x80:
            btN = 12;
            break;
        case 0x40:
            btN = 13;
            break;
        case 0x08:
            btN = 16;
            break;
        case 0x04:
            btN = 17;
            break;
        default:
            btN = 0;
            break;
        }
    }
    return btN;
}