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
void Delay(u8 n)
{
	u32 Dtime;
	Dtime=T1_1MS;
	while(T1_1MS-Dtime<n);
}
void SEG_Tran(u8 *DP, u8*CD)
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
			case 'C':
				*(CD+j)=0xC6;
				break;
			case 'E':
				*(CD+j)=0x86;
				break;
			default:
				*(CD+j)=0xFF;
				break;
		}
	}
}
void SEG_Disp(u8 *CD, u8 PS)
{
	P0=0xFF;
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
	
	P0=(u8)(1<<PS);
	P2=(P2&0x1F)|0xC0;
	P2&=0x1F;	
	
	P0=*(CD+PS);
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
}
u8 KEY_Martix(void)
{
	int RD0,RD1;
	u8 btn=0;
	P44=0;P42=1;P35=1;P34=1;
	RD0=(P3&0x0F);
	P44=1;P42=0;P35=1;P34=1;
	RD0=(RD0<<4)|(P3&0x0F);
	P44=1;P42=1;P35=0;P34=1;
	RD0=(RD0<<4)|(P3&0x0F);
	P44=1;P42=1;P35=1;P34=0;
	RD0=(RD0<<4)|(P3&0x0F);
	if(RD0==0xFFFF){return 0;}
	Delay(20);
	P44=0;P42=1;P35=1;P34=1;
	RD1=(P3&0x0F);
	P44=1;P42=0;P35=1;P34=1;
	RD1=(RD1<<4)|(P3&0x0F);
	P44=1;P42=1;P35=0;P34=1;
	RD1=(RD1<<4)|(P3&0x0F);
	P44=1;P42=1;P35=1;P34=0;
	RD1=(RD1<<4)|(P3&0x0F);
	if(RD1==RD0)
	{
		switch(~RD1)
		{
			case 0x8000:
				btn=4;
				break;
			case 0x4000:
				btn=5;
				break;
			case 0x2000:
				btn=6;
				break;
			case 0x1000:
				btn=7;
				break;
			case 0x0800:
				btn=8;
				break;
			case 0x0400:
				btn=9;
				break;
			case 0x0200:
				btn=10;
				break;
			case 0x0100:
				btn=11;
				break;
			case 0x0080:
				btn=12;
				break;
			case 0x0040:
				btn=13;
				break;
			case 0x0020:
				btn=14;
				break;
			case 0x0010:
				btn=15;
				break;
			case 0x0008:
				btn=16;
				break;
			case 0x0004:
				btn=17;
				break;
			case 0x0002:
				btn=18;
				break;
			case 0x0001:
				btn=19;
				break;
			default:
				btn=0;
				break;
		}
	}
	return btn;
}

