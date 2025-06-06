#include "driver.h"
extern volatile u32 T1_1MS;
void DEV_Cls(void)
{
	P0=0xFF;
	P2=(P2&0x1F)|0x80;
	P2&=0x1F;
	
	P0=0x00;
	P2=(P2&0x1F)|0xA0;
	P2&=0x1F;
}
void Delay(u8 n)
{
	u32 Dtime;
	Dtime=T1_1MS;
	while(T1_1MS-Dtime<n);
}
void LED_Disp(u8 out)
{
	P0=~out;
	P2=(P2&0x1F)|0x80;
	P2&=0x1F;
}
void T1_Int(void)
{
	TMOD&=0x0F;
	AUXR&=~(1<<6);
	TH1=0xFC;
	TL1=0x18;
	TF1=0;
	EA=1;
	ET1=1;
	TR1=1;
}
void T0_Int(void)
{
	TMOD|=0x04;
	AUXR&=~(1<<7);
	TH0=0x00;
	TL0=0x00;
	TF0=0;
	TR0=1;
}
void SEG_Tran(u8 *pDP,u8 *pCD)
{
	u8 i,j;
	for(i=0,j=0;j<8;j++,i++)
	{
		switch(*(pDP+i))
		{
			case '0':
				*(pCD+j)=0xC0;
				break;
			case '1':
				*(pCD+j)=0xF9;
				break;
			case '2':
				*(pCD+j)=0xA4;
				break;
			case '3':
				*(pCD+j)=0xB0;
				break;
			case '4':
				*(pCD+j)=0x99;
				break;
			case '5':
				*(pCD+j)=0x92;
				break;
			case '6':
				*(pCD+j)=0x82;
				break;
			case '7':
				*(pCD+j)=0xF8;
				break;
			case '8':
				*(pCD+j)=0x80;
				break;
			case '9':
				*(pCD+j)=0x90;
				break;
			case '-':
				*(pCD+j)=0xBF;
				break;
			case 'C':
				*(pCD+j)=0xC6;
				break;
			case 'H':
				*(pCD+j)=0x89;
				break;
			case 'F':
				*(pCD+j)=0x8E;
				break;
			case 'P':
				*(pCD+j)=0x8C;
				break;
			case 'E':
				*(pCD+j)=0x86;
				break;
			case 'A':
				*(pCD+j)=0x88;
				break;
			case '.':
				if(j>0)
				{
					j--;
				}	
				*(pCD+j)&=0x7F;
				break;
			default:
				*(pCD+j)=0xFF;
				break;
					
		}
	}
}
void SEG_Disp(u8 *pCD,u8 PS)
{
	P0=0xFF;
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
	
	P0=(u8)(1<<PS);
	P2=(P2&0x1F)|0xC0;
	P2&=0x1F;

	P0=*(pCD+PS);
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;	
}
unsigned KEY_Martix(void)
{
	u8 RD0,RD1;
	u8 btn=0;
	P44=0;P42=1;
	RD0=(P3&0x0F);
	P44=1;P42=0;
	RD0=(RD0<<4)|(P3&0x0F);
	if(RD0==0xFF){return 0;}
	Delay(10);
	P44=0;P42=1;
	RD1=(P3&0x0F);
	P44=1;P42=0;
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
				btn=8;
				break;
			case 0x04:
				btn=9;
				break;
			default:
				btn=0;
				break;
		}
	}
	return btn;
}