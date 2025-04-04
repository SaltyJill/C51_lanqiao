#include "tim.h"

extern volatile u32 uwMs;
void Led_Disp(u8 ucOut)
{
	P0=~ucOut;
	P2=(P2 & 0x1F) | 0x80;
	P2=P2 & 0x1F;
}

void Cls_Waishe(void)
{
	P0=0xFF;
	P2=(P2 & 0x1F) | 0x80;
	P2=P2 & 0x1F;
	P0=0x0;
	P2=(P2 & 0x1F) | 0xA0;
	P2=P2 & 0x1F;
}

void T1_Init(void)   //12T 12M
{
	TH1=0xFC;
	TL1=0x18;
	TMOD &=0x0F;
	AUXR &=~(1<<6);
	TF1=0;
	ET1=1;
	EA=1;
	TR1=1;
}

void T0_Init(void)		//12T  @12.000MHz   ---P34 for Capture 
{
	AUXR &= 0x7F;		//12T
	TMOD |= 0x04;		//mode 0
	TL0 = 0x00;		
	TH0 = 0x00;		
	TF0 = 0;		
	TR0 = 1;		
	//ET0=1;
	//EA=1;
}

void Delay(u16 usIn)
{
	u32 uwInit;
	uwInit=uwMs;
	while(uwMs-uwInit<usIn);	
}

void Seg_Tran(u8 *pBuff,u8 *pCode)
{
	u8 i,j;
	for(i=0,j=0;j<8;i++,j++)
	{
		switch(*(pBuff+i))
		{
			case '0':
				*(pCode+j)=0xc0;
				break;
			case '1':
				*(pCode+j)=0xf9;
				break;
			case '2':
				*(pCode+j)=0xa4;
				break;
			case '3':
				*(pCode+j)=0xb0;
				break;
			case '4':
				*(pCode+j)=0x99;
				break;
			case '5':
				*(pCode+j)=0x92;
				break;
			case '6':
				*(pCode+j)=0x82;
				break;
			case '7':
				*(pCode+j)=0xf8;
				break;
			case '8':
				*(pCode+j)=0x80;
				break;
			case '9':
				*(pCode+j)=0x90;
				break;
			case 'A':
				*(pCode+j)=0x88;
				break;
			case 'B':
				*(pCode+j)=0x83;
				break;
			case 'C':
				*(pCode+j)=0xc6;
				break;
			case 'D':
				*(pCode+j)=0xa1;
				break;
			case 'E':
				*(pCode+j)=0x86;
				break;
			case 'F':
				*(pCode+j)=0x8e;
				break;
			case 'U':
				*(pCode+j)=0xc1;
				break;
			case 'N':
				*(pCode+j)=0xc8;
				break;
			case '-':
				*(pCode+j)=0xbf;
				break;
			case ' ':
				*(pCode+j)=0xff;
				break;
			case '.':
				j=j-1;
				*(pCode+j) &=0x7f;
				break;
			default:
				*(pCode+j)=0xff;				
		}	
	}
}
void Seg_Disp(u8 *pCode,u8 ucPos)
{
	P0=0xFF;
	P2=(P2 & 0x1F)|0xe0;
	P2 =P2 & 0x1F;
	P0=(u8)1<<ucPos;
	P2=(P2 & 0x1F)|0xc0;
	P2 =P2 & 0x1F;
	P0=*(pCode+ucPos);
	P2=(P2 & 0x1F)|0xe0;
	P2 =P2 & 0x1F;
}

u8 BTN_Read(void)
{
   u8 ucKey1,ucKey2;
   u8 ucRtn=0;
   ucKey1=P3 & 0x0F;
   if(ucKey1==0x0F) return 0;
   Delay(10);
   ucKey2=P3 & 0x0F;
   if(ucKey1!=ucKey2) return 0;
   switch(~ucKey1)
   {
			case 0xF8:	//S4
					ucRtn=1;
					break;
			case 0xF4:	//S5
					ucRtn=2;
					break;
			case 0xF2:	//S6
					ucRtn=3;
					break;
			case 0xF1:	//S7
					ucRtn=4;
					break;
			default:
					ucRtn=0;
   }
   return ucRtn;
}