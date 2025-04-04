#include "driver.h"
extern volatile u32 T1_1MS; 
void LED_Disp(u8 Lout)
{
	P0=~Lout;
	P2=(P2&0x1F)|0x80;
	P2&=0x1F;
}
void DEV_Cls(void)
{
	P0=0xFF;
	P2=(P2&0x1F)|0x80;
	P2&=0x1F;
	
	P0=0x00;
	P2=(P2&0x1F)|0xA0;
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
	TMOD|=0x40;
	AUXR&=~(1<<7);
	TH0=0x00;
	TL0=0x00;
	TF0=0;
	TR0=1;
}
void SEG_Tran(u8 *pstr,u8 *pcod){
	u8 i,j;
	for(i=0,j=0;j<8;i++,j++){
		switch(*(pstr+i)){
			case '0':
				*(pcod+j)=0xC0;
				break;
			case '1':
				*(pcod+j)=0xf9;
				break;
			case '2':
				*(pcod+j)=0xa4;
				break;
			case '3':
				*(pcod+j)=0xb0;
				break;
			case '4':
				*(pcod+j)=0x99;
				break;
			case '5':
				*(pcod+j)=0x92;
				break;
			case '6':
				*(pcod+j)=0x82;
				break;
			case '7':
				*(pcod+j)=0xf8;
				break;
			case '8':
				*(pcod+j)=0x80;
				break;
			case '9':
				*(pcod+j)=0x90;
				break;
			case 'F':
				*(pcod+j)=0x8e;
				break;
			case 'N':
				*(pcod+j)=0xC8;
				break;
			case 'U':
				*(pcod+j)=0xC1;
				break;
			case '-':
				*(pcod+j)=0xBF;
				break;
			case '.':
				if(j>0){
					j--;
				}
				*(pcod+j)&=0x7F;
				break;
			default:
				*(pcod+j)=0xFF;
				break;
}
}
}
void SEG_Disp(u8 *pcod,u8 poci){
	P0=0xFF;
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
	
	P0=(u8)(1<<poci);
	P2=(P2&0x1F)|0xC0;
	P2&=0x1F;
	
	P0=*(pcod+poci);
	P2=(P2&0x1F)|0xE0;
	P2&=0x1F;
}
void Delay(u8 Time){
	u32 DTime;
	DTime=T1_1MS;
	while(T1_1MS-DTime<Time);
}
u8 KEY_Independ(void){
	u8 RD1,RD2;
	u8 btn=0;
	RD1=(P3&0x0F);
	if(RD1==0x0F)return 0;
	Delay(20);
	RD2=(P3&0x0F);
	if(RD1!=RD2)return 0;
		switch (~RD2){
			case 0x08:
				btn=4;
			break;
			case 0x04:
				btn=5;
			break;
			case 0x02:
				btn=6;
			break;
			case 0x01:
				btn=7;
			break;
			default:
				btn=0;
			break;
}
 return btn;
}
