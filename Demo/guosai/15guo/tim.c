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
void Out_Waishe(u8 ucVal)
{
	P0=ucVal;
	P2=(P2 & 0x1F) | 0xA0;
	P2=P2 & 0x1F;
}
/*
void T1_Init(void)   //12T 12M     1ms  
{
	TL1 = 0x9C;		//??????
	TH1 = 0xFF;		//??????
	TMOD &=0x0F;
	AUXR &=~(1<<6);
	TF1=0;
	ET1=1;
	EA=1;
	TR1=1;
}*/
void T1_Init(void)   //12T 12M     100us  
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
/*
void T0_Init(void)		//12T  @12.000MHz   ---for Wave P10,P11
{
	AUXR &= 0x7F;		//12T
	TMOD &= 0xF0;		
	TL0 = 0x00;		
	TH0 = 0x00;		
	//TF0 = 0;		
	TR0 = 0;		
}*/
void UartInit(void)	  //9600bps@12.000MHz
{
   SCON = 0x50;	//????1
   AUXR |= 0x01;	//T2-->Serial BardRate
   AUXR |= 0x04;      //1T
   T2L = 0xC7;	
   T2H = 0xFE;	
   AUXR |= 0x10;      //TR2
   ES=1;
   EA=1;
}
void UartSend(u8 *str1)
{
	u8 i=0;
	while(str1[i])
	{
		SBUF=str1[i];
		i++;
		while(!TI);
		TI=0;
	}
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
			case 'H':
				*(pCode+j)=0x89;
				break;
			case 'P':
				*(pCode+j)=0x8c;
				break;
			case 'L':
				*(pCode+j)=0xc7;
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
	//P44=0;P42=1;P35=1;P34=1;     P34 used as Pwm out,in Capture
	P44=0;P42=1;P35=1;
	uiArray1=P3 & 0x0F;
	P44=1;P42=0;P35=1;
	uiArray1=(uiArray1 <<4) | P3 & 0x0F;
	P44=1;P42=1;P35=0;
	uiArray1=(uiArray1 <<4) | P3 & 0x0F;
	if(uiArray1==0x0FFF) return 0;
	Delay(10);
	//P44=0;P42=1;P35=1;P34=1;
	P44=0;P42=1;P35=1;
	uiArray2=P3 & 0x0F;
	P44=1;P42=0;P35=1;
	uiArray2=(uiArray2 <<4) | P3 & 0x0F;
	P44=1;P42=1;P35=0;
	uiArray2=(uiArray2 <<4) | P3 & 0x0F;
	
	if(uiArray1!=uiArray2) return 0;
	switch(~uiArray1)
	{
		case 0xF800:			//S4
			ucRtn=4;
			break;
		case 0xF400:			//S5
			ucRtn=5;
			break;		
		case 0xF080:			//S8
			ucRtn=8;
			break;		
		case 0xF040:			//S9
			ucRtn=9;
			break;		
		case 0xF008:			//S12
			ucRtn=12;
			break;
		case 0xF004:			//S13
			ucRtn=13;
			break;		
		
		default:
			ucRtn=0;
	}
	return ucRtn;
}