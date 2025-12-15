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
void T1_Int(void)
{
    TMOD &= 0x0F;
    AUXR &= ~(1 << 6);
    TH0 = 0xFC;
    TL0 = 0x18;
    TF0 = 0;
    EA = 1;
    ET1 = 1;
    TR = 1;
}

void SEG_Tran(u8 *pDP, u8 *pCD)
{
    u8 i, j;
    for (i = 0, j = 0; i < 8; i++, j++)
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
        case 'P':
            *(pCD + i) = 0x8C;
            break;
        case 'F':
            *(pCD + i) = 0x8E;
            break;
        case '.':
						if(i>0)
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
void SEG_Disp(u8 *pCD,u8 PS)
{
    P0 = 0xFF;
    P2 = (P2 & 0x1F) | 0xE0;
    P2 &= 0x1F;
	
	  P0 = (1<<PS);
    P2 = (P2 & 0x1F) | 0xC0;
    P2 &= 0x1F;
	
	  P0 = *(pCD+PS);
    P2 = (P2 & 0x1F) | 0x80;
    P2 &= 0x1F;
}

void Delay(u8 n)
{
	u32 Dtime;
	Dtime=T1_1MS;
	while(T1_1MS-Dtime<n);
}
void Relay(u8 on)
{
	if(on)
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

u8 KEY_Matrix(void)
{
	u8 RD0,RD1;
	u8 btn=0;
	P44=0;
	P42=1;
	RD0=P3&0x0F;
	P44=1;
	P42=0;
	RD0=(RD0<<4)|(P3&0x0F);
	if(RD0==0xFF)
	{
		break;
	}
	
	Delay(10);
	
	P44=0;
	P42=1;
	RD1=P3&0x0F;
	P44=1;
	P42=0;
	RD1=(RD1<<4)|(P3&0x0F);
	if(RD0==RD1)
	{
		switch(~RD1)
		{
			case 0x80:
				btn=4;
				break;
			case 0x40:
				btn=5;
				break;
			case 0x08:
				btn=9;
				break;
			case 0x04:
				btn=8;
				break;
			case 0x0C:
				btn=89;
				break;
		}
	}
	return btn;
}