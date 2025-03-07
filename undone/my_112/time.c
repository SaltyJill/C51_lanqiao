#include "time.h"
extern volatile u32 ISR_1Ms;
void led_dis(u8 led_out)
{
    P0 = ~led_out;
    P2 = (P2 & 0x1f) | 0x80;
    P2 = P2 & 0x1f;
}
void outer_cls(void)
{
    P0 = 0xff;
    P2 = (P2 & 0x1f) | 0x80;
    P2 = P2 & 0x1f;
    P0 = 0x40;
    P2 = (P2 & 0x1f) | 0xA0;
    P2 = P2 & 0x1f;
}
void delay_1ms(u16 ts)
{
    u32 Time_now;
    Time_now = ISR_1Ms;
    while ((ISR_1Ms - Time_now) < ts)
    {
        ;
    }
}
void Seg_trans(u8 *pseg_Disp, u8 *pseg_Code)
{
    u8 i, j;
    for (i = 0, j = 0; j < 8; i++, j++)
    {
        switch (*(pseg_Disp + i))
        {
        case '0':
            *(pseg_Code + j) = 0xc0;
            break;
        case '1':
            *(pseg_Code + j) = 0xf9;
            break;
        case '2':
            *(pseg_Code + j) = 0xa4;
            break;
        case '3':
            *(pseg_Code + j) = 0xb0;
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
            *(pseg_Code + j) = 0xf8;
            break;
        case '8':
            *(pseg_Code + j) = 0x80;
            break;
        case '9':
            *(pseg_Code + j) = 0x90;
            break;
        case 'A':
            *(pseg_Code + j) = 0x88;
            break;
        case 'B':
            *(pseg_Code + j) = 0x83;
            break;
        case 'C':
            *(pseg_Code + j) = 0xc6;
            break;
        case 'D':
            *(pseg_Code + j) = 0xa1;
            break;
        case 'E':
            *(pseg_Code + j) = 0x86;
            break;
        case 'F':
            *(pseg_Code + j) = 0x8e;
            break;
        case 'U':
            *(pseg_Code + j) = 0xc1;
            break;
        case 'P':
            *(pseg_Code + j) = 0x8c;
            break;
        case 'N':
            *(pseg_Code + j) = 0xab;
            break;
        case '-':
            *(pseg_Code + j) = 0xbf;
            break;
        case ' ':
            *(pseg_Code + j) = 0xff;
            break;
        case '.':
            j = j - 1;
            *(pseg_Code + j) &= 0x7f;
            break;
        default:
            *(pseg_Code + j) = 0xff;
        }
    }
}
void Seg_Disp(u8 *pseg_Code, u8 seg_Add)
{
    P0 = 0xff;
    P2 = (P2 & 0x1f) | 0xE0;
    P2 &= 0x1f;
    P0 = (u8)1 << seg_Add;
    P2 = (P2 & 0x1f) | 0xC0;
    P2 = P2 & 0x1F;
    P0 = *(pseg_Code + seg_Add);
    P2 = (P2 & 0x1F) | 0xE0;
    P2 = P2 & 0x1F;
}
void T1_Init(void) // 12T 12M 1ms
{
    TH1 = 0xFC;
    TL1 = 0x18;
    TMOD &= 0x0F;
    AUXR &= ~(1 << 6);
    TF1 = 0;
    ET1 = 1;
    EA = 1;
    TR1 = 1;
}
unsigned char key_MatrixRead(void)
{
    u16 keyRD1, keyRD2;
    u8 button = 0;
    P44 = 0;
    P42 = 1;
    P35 = 1;
    P34 = 1;
    keyRD1 = P3 & 0x0f;
    P44 = 1;
    P42 = 0;
    keyRD1 = (keyRD1 << 4) | P3 & 0x0f;
    P42 = 1;
    P35 = 0;
    keyRD1 = (keyRD1 << 4) | P3 & 0x0f;
    P35 = 1;
    P34 = 0;
    keyRD1 = (keyRD1 << 4) | P3 & 0x0f;
    if (keyRD1 == 0x0ffff)
    {
        return 0;
    }
    delay_1ms(10);
    P44 = 0;
    P42 = 1;
    P35 = 1;
    P34 = 1;
    keyRD2 = P3 & 0x0f;
    P44 = 1;
    P42 = 0;
    keyRD2 = (keyRD2 << 4) | P3 & 0x0f;
    P42 = 1;
    P35 = 0;
    keyRD2 = (keyRD2 << 4) | P3 & 0x0f;
    P35 = 1;
    P34 = 0;
    keyRD2 = (keyRD2 << 4) | P3 & 0x0f;
    if (keyRD1 != keyRD2)
    {
        return 0;
    }

    switch (~keyRD2)
    {
    case 0x8000:
        button = 1;
        break;
    case 0x4000:
        button = 2;
        break;
    case 0x2000:
        button = 3;
        break;
    case 0x1000:
        button = 4;
        break;
    case 0x0800:
        button = 5;
        break;
    case 0x0400:
        button = 6;
        break;
    case 0x0200:
        button = 7;
        break;
    case 0x0100:
        button = 8;
        break;
    case 0x0080: // S12
        button = 9;
        break;
    case 0x0040:
        button = 10;
        break;
    case 0x0020:
        button = 11;
        break;
    case 0x0010:
        button = 12;
        break;
    case 0x0008: // S16
        button = 13;
        break;
    case 0x0004: // S17
        button = 14;
        break;
    case 0x0002:
        button = 15;
        break;
    case 0x0001:
        button = 16;
        break;
    default:
        break;
    }
    return button;
}