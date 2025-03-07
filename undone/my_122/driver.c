#include "driver.h"
extern volatile u32 ISR_1Ms;
void led_Disp(u8 led_out)
{
    P0 = ~(led_out);
    P2 = (P2 & 0x1f) | 0x80;
    P2 = P2 & 0x1f;
}
void outside_Cls(void)
{
    P0 = 0xff;
    P2 = (P2 & 0x1f) | 0x80;
    P2 = P2 & 0x1f;

    P0 = 0x40;
    P2 = P2 | 0xA0;
    P2 = P2 & 0x1f;
}
void segTrans(u8 *pDisp, u8 *pCode)
{
    u8 i, j;
    for (j = 0, i = 0; j < 8; j++, i++)
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
        case 'U':
            *(pCode + j) = 0xc1;
            break;
        case 'P':
            *(pCode + j) = 0x8c;
            break;
        case 'N':
            *(pCode + j) = 0xab;
            break;
        case 'M':
            *(pCode + j) = 0xc8;
            break;
        case '-':
            *(pCode + j) = 0xbf;
            break;
        case ' ':
            *(pCode + j) = 0xff;
            break;
        case '.':
            j = j - 1;
            *(pCode + j) &= 0x7f;
            break;
        default:
            *(pCode + j) = 0xff;
        }
    }
}
void segDisp(u8 *pCode, u8 Adds)
{
    P0 = 0xFF; // 灭灯
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;

    P0 = (u8)1 << Adds; // 选择数码管导通
    P2 |= 0xC0;
    P2 &= 0x1F;

    P0 = *(pCode + Adds); // 输出对应的数组
    P2 |= 0xE0;
    P2 &= 0x10;
}
void delay_1ms(u8 dlt1)
{
    u32 del_strt;
    del_strt = ISR_1Ms;
    while ((ISR_1Ms - del_strt) < dlt1)
    {
        /* code */
    }
}
unsigned char key_IndividualRead(void)
{
    u8 keyRD1, keyRD2;
    u8 button = 0;
    keyRD1 = P3 & 0x0f;
    if (keyRD1 == 0x0f)
    {
        return 0;
    }
    delay_1ms(10);
    keyRD2 = P3 & 0x0f;
    if (keyRD1 != keyRD2)
    {
        return 0;
    }
    switch (~keyRD2)
    {
    case 0xF8:
        button = 1;
        break;
    case 0xF4:
        button = 2;
        break;
    case 0xF2:
        button = 3;
        break;
    case 0xF1:
        button = 4;
        break;
    default:
        button = 0;
        break;
    }
    return button;
}