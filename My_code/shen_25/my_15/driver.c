#include "driver.h"
extern volatile u32 T1_1MS;
void DEV_Cls(void)
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
void LED_Disp(u8 out)
{
	P0=~out;
	P2=(P2&0x1F)|0x80;
	P2&=0x1F;	
}
void T1_Int(void)
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
void T0_Int(void)
{
	TMOD|=0x04;
	AUXR&=(u8)~(1<<7);
	TH0=0x00;
	TL0=0x00;
	TF0=0;
	TR0=1;
	TMOD|=0x04;
	AUXR&=(u8)~(1<<7);
	TH0=0x00;
	TL0=0x00;
	TF0=0;
	TR0=1;
}
void Delay(u8 n)
{
	u32 Dtime=0;
	Dtime=T1_1MS;
	while(T1_1MS-Dtime<n);
	u32 Dtime=0;
	Dtime=T1_1MS;
	while(T1_1MS-Dtime<n);
}
void SEG_Tran(u8 *DP,u8 *CD)
void SEG_Tran(u8 *DP,u8 *CD)
{
	u8 i,j;
	for(i=0,j=0;j<8;i++,j++)
	{
		switch(*(DP+i))
		{
			case '0':
				*(CD+j)=0xC0;
				break;
			case '1':
				*(CD+j)=0xF9;
				break;
			case '2':
				*(CD+j)=0xA4;
				break;
			case '3':
				*(CD+j)=0xB0;
				break;
			case '4':
				*(CD+j)=0x99;
				break;
			case '5':
				*(CD+j)=0x92;
				break;
			case '6':
				*(CD+j)=0x82;
				break;
			case '7':
				*(CD+j)=0xF8;
				break;
			case '8':
				*(CD+j)=0x80;
				break;
			case '9':
				*(CD+j)=0x90;
				break;
			case '-':
				*(CD+j)=0xBF;
				break;
			case 'F':
				*(CD+j)=0x8E;
				break;
			case 'P':
				*(CD+j)=0x8C;
				break;
			case 'H':
				*(CD+j)=0x89;
				break;
			case 'A':
				*(CD+j)=0x88;
				break;
			case 'L':
				*(CD+j)=0xC7;
				break;
			case '.':
				if(j>0)
				{
					j--;
				}
				*(CD+j)&=0x7F;
				break;
			default:
				*(CD+j)=0xFF;
				break;
		}
	}
	u8 i,j;
	for(i=0,j=0;j<8;i++,j++)
	{
		switch(*(DP+i))
		{
			case '0':
				*(CD+j)=0xC0;
				break;
			case '1':
				*(CD+j)=0xF9;
				break;
			case '2':
				*(CD+j)=0xA4;
				break;
			case '3':
				*(CD+j)=0xB0;
				break;
			case '4':
				*(CD+j)=0x99;
				break;
			case '5':
				*(CD+j)=0x92;
				break;
			case '6':
				*(CD+j)=0x82;
				break;
			case '7':
				*(CD+j)=0xF8;
				break;
			case '8':
				*(CD+j)=0x80;
				break;
			case '9':
				*(CD+j)=0x90;
				break;
			case '-':
				*(CD+j)=0xBF;
				break;
			case 'F':
				*(CD+j)=0x8E;
				break;
			case 'P':
				*(CD+j)=0x8C;
				break;
			case 'H':
				*(CD+j)=0x89;
				break;
			case 'A':
				*(CD+j)=0x88;
				break;
			case 'L':
				*(CD+j)=0xC7;
				break;
			case '.':
				if(j>0)
				{
					j--;
				}
				*(CD+j)&=0x7F;
				break;
			default:
				*(CD+j)=0xFF;
				break;
		}
	}
}

void SEG_Disp(u8 *CD,u8 PS)

void SEG_Disp(u8 *CD,u8 PS)
{
	P0=0xFF;
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
	
	P0=(1<<PS);
	P2=(P2&0x1F)|0xC0;
	P2&=0x1F;
	
	P0=*(CD+PS);
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
	P0=0xFF;
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
	
	P0=(1<<PS);
	P2=(P2&0x1F)|0xC0;
	P2&=0x1F;
	
	P0=*(CD+PS);
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
}

unsigned char KEY_Martix(void)

unsigned char KEY_Martix(void)
{
	u8 RD0,RD1;
	u8 btn=0;
	P44=0;
	P42=1;
	RD0=(P3&0x0F);
	P44=1;
	P42=0;
	RD0=(RD0<<4)|(P3&0x0F);
	if(RD0==0xFF){return 0;}
	Delay(20);
	P44=0;
	P42=1;
	RD1=(P3&0x0F);
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
			case 0x20:
				btn=6;
				break;
			case 0x10:
				btn=7;
				break;
			case 0x08:
				btn=8;
				break;
			case 0x04:
				btn=9;
				break;
			case 0x02:
				btn=10;
				break;
			case 0x01:
				btn=11;
				break;			
		}
	}
	return btn;
	u8 RD0,RD1;
	u8 btn=0;
	P44=0;
	P42=1;
	RD0=(P3&0x0F);
	P44=1;
	P42=0;
	RD0=(RD0<<4)|(P3&0x0F);
	if(RD0==0xFF){return 0;}
	Delay(20);
	P44=0;
	P42=1;
	RD1=(P3&0x0F);
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
			case 0x20:
				btn=6;
				break;
			case 0x10:
				btn=7;
				break;
			case 0x08:
				btn=8;
				break;
			case 0x04:
				btn=9;
				break;
			case 0x02:
				btn=10;
				break;
			case 0x01:
				btn=11;
				break;			
		}
	}
	return btn;
}

