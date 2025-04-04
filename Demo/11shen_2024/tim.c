#include "tim.h"

extern volatile u32 ulMs;
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
void Delay(u16 usIn)
{
	u32 ulInit;
	ulInit=ulMs;
	while(ulMs-ulInit<usIn);	
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
			case 'P':
				*(pCode+j)=0x8c;
				break;
			case 'N':
				*(pCode+j)=0xab;
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

u8 KBD_Read(void)
{
	u16 uiArray1,uiArray2;
	u8 ucRtn=0;
	P44=0;P42=1;P35=1;P34=1;
	uiArray1=P3 & 0x0F;
	P44=1;P42=0;
	uiArray1=(uiArray1 <<4) | P3 & 0x0F;
	P42=1;P35=0;
	uiArray1=(uiArray1 <<4) | P3 & 0x0F;
	P35=1;P34=0;
	uiArray1=(uiArray1 <<4) | P3 & 0x0F;
	if(uiArray1==0xFFFF) return 0;
	Delay(10);
	P44=0;P42=1;P35=1;P34=1;
	uiArray2=P3 & 0x0F;
	P44=1;P42=0;
	uiArray2=(uiArray2 <<4) | P3 & 0x0F;
	P42=1;P35=0;
	uiArray2=(uiArray2 <<4) | P3 & 0x0F;
	P35=1;P34=0;
	uiArray2=(uiArray2 <<4) | P3 & 0x0F;
	if(uiArray1!=uiArray2) return 0;
	switch(~uiArray2)
	{
		case 0x8000:
			ucRtn=1;
			break;
		case 0x4000:
			ucRtn=2;
			break;
		case 0x2000:
			ucRtn=3;
			break;
		case 0x1000:
			ucRtn=4;
			break;
		case 0x0800:
			ucRtn=5;
			break;
		case 0x0400:
			ucRtn=6;
			break;
		case 0x0200:
			ucRtn=7;
			break;
		case 0x0100:
			ucRtn=8;
			break;
		case 0x0080:		//S12
			ucRtn=9;
			break;
		case 0x0040:
			ucRtn=10;
			break;
		case 0x0020:
			ucRtn=11;
			break;
		case 0x0010:
			ucRtn=12;
			break;
		case 0x0008:		//S16
			ucRtn=13;
			break;
		case 0x0004:		//S17
			ucRtn=14;
			break;
		case 0x0002:
			ucRtn=15;
			break;
		case 0x0001:
			ucRtn=16;
			break;
		default:
			ucRtn=0;
	}
	return ucRtn;
}