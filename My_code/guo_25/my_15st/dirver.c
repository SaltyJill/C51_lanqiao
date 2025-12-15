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
    P0 = ~out;
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
    TMOD |= 0x04;
    AUXR &= ~(1 << 7);
    TH0 = 0x00;
    TL0 = 0x00;
    TF0 = 0;
    TR0 = 1;
}

void Uart_Int(void) // 9600bps@12.000MHz
{
    SCON = 0x50;  // 8位数据,可变波特率
    AUXR |= 0x01; // 串口1选择定时器2为波特率发生器
    AUXR |= 0x04; // 定时器时钟1T模式
    T2L = 0xC7;   // 设置定时初始值
    T2H = 0xFE;   // 设置定时初始值
    AUXR |= 0x10; // 定时器2开始计时
		ES=1;
	  EA=1;
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
				TI=0;
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
            *(pCod + j) = 0xC0;
            break;
        case '1':
            *(pCod + j) = 0xF9;
            break;
        case '2':
            *(pCod + j) = 0xA4;
            break;
        case '3':
            *(pCod + j) = 0xB0;
            break;
        case '4':
            *(pCod + j) = 0x99;
            break;
        case '5':
            *(pCod + j) = 0x92;
            break;
        case '6':
            *(pCod + j) = 0x82;
            break;
        case '7':
            *(pCod + j) = 0xF8;
            break;
        case '8':
            *(pCod + j) = 0x80;
            break;
        case '9':
            *(pCod + j) = 0x90;
            break;
        case 'L':
            *(pCod + j) = 0xC7;
            break;
        case 'E':
            *(pCod + j) = 0x86;
            break;
        case 'P':
            *(pCod + j) = 0x8C;
            break;
        case '-':
            *(pCod + j) = 0xBF;
            break;
        case '.':
            if (j > 0)
            {
                j--;
            }
            *(pCod + j) &= 0x7F;
						break;
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

u8 KEY_Matrix(void)
{
    u16 RD0=0, RD1=0;
    u8 btn=0;
    P44 = 0;
    P42 = 1;
    P35 = 1;
    RD0 = P3 & 0x0F;
    P44 = 1;
    P42 = 0;
    P35 = 1;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
    P44 = 1;
    P42 = 1;
    P35 = 0;
    RD0 = (RD0 << 4) | (P3 & 0x0F);
    if (RD0 == 0x0FFF)
    {
        return 0;
    }
    Delay(10);
      P44 = 0;
    P42 = 1;
    P35 = 1;
    RD1 = P3 & 0x0F;
    P44 = 1;
    P42 = 0;
    P35 = 1;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    P44 = 1;
    P42 = 1;
    P35 = 0;
    RD1 = (RD1 << 4) | (P3 & 0x0F);
    if (RD0 == RD1)
    {
        switch (~RD1)
        {
        case 0xF800:
            btn = 4;
            break;
        case 0xF400:
            btn = 5;
            break;
        case 0xF080:
            btn = 8;
            break;
        case 0xF040:
            btn = 9;
            break;
        case 0xF008:
            btn = 12;
            break;
        case 0xF004:
            btn = 13;
            break;
        default:
            btn = 0;
            break;
        }
    }
    return btn;
}

void Relay(u8 st)
{
	P0=0x00;
	if(st)
	{
		P0|=(1<<4);
	}
	else
	{
		P0&=~(1<<4);
	}
	P2=(P2&0x1F)|0xA0;
	P2&=0x1F;
}