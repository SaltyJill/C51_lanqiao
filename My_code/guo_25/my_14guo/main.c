#include "main.h"
#include <stdio.h>
#include "iic.h"
#include "driver.h"
#include "onewire.h"

//ISR
u32 T1_1MS=0;
// SEG
u8 SEG_DP[10]="3333";
u8 SEG_CD[8]={0};
u8 SEG_PS=0;
u8 FLAG_SEG=0;

u8 page=0;
u8 page_M=0;//0 cm;1 m
u8 page_P=0;//0 D;1 T
u8 page_S=0;//0 F1;1 F2;3 F3 

u8 PARA_D=0;
u8 PARA_T=0;
//TEP
u16 TEMP//*10

//WAVE
sbit waveP10=P1^0;
sbit waveP11=P1^1;
u8 FALG_WAVE=0;
u8 FALG_Dget=0;
u8 FALG_Dout=0;
u16 DISTEMT_RC[60]={0};
u16 DISTENT=0;//cm
u16 WAVE_V=340;
s16 ADJ_V=0;

//DAC
u8 BT_V=0;//10*
u8 FLAG_DAC=0;
void main(void)
{
	DEV_Cls();
	T1_Int();
	while(1)
	{
		;
	}
}
void SEG_Fuc(void)
{
	if(FLAG_SEG)
	{
		FLAG_SEG=0;
		switch(page)
		{
			case 0:
				if(page_M==0)
				{
					sprintf(SEG_DP,"%3.1f-%4u",TEMP/10.0,(u16)DISTENT);
				}
				else
				{
					sprintf(SEG_DP,"%3.1f-%4.2f",TEMP/10.0,DISTENT/100.0);
				}
				break;
			case 1:
				if(page_P==0)
				{
					sprintf(SEG_DP,"P1````%2u",(u16)page_M);
				}
				else
				{
					sprintf(SEG_DP,"P2````%2u",(u16)page_T);
				}
				break;
			case 2:
				if(page_S==0)
				{
					sprintf(SEG_DP,"F1```%3d",ADJ_V);
				}
				else if(page_S==1)
				{
					sprintf(SEG_DP,"F2``%4u",(u16)WAVE_V);
				}
				else
				{
					sprintf(SEG_DP,"F3````%2.1f",BT_V/10.0);
				}
				break;
		}
		SEG_Tran(SEG_DP,SEG_CD);
	}
}

void KEY_Fuc(void)
{
	static u8 key_past;
	static u32 key_time=0;
	u8 key_now;
	key_now=KEY_Matrix();
	if(key_now!=key_past&&FALG_Dget==0)
	{
		switch(key_now)
		{
			case 0:
				break;
			case 4:
				page_M=0;
			  page_P=0;
				page_S=0;
				page=(++page)%3;
				break;
			case 5:
				page_M=(page==0)?(!page_M):page_M;
				page_P=(page==1)?(!page_P):page_P;
				page_S=(page==2)?(!page_S):page_S;
				break;
			case 8:
				// distent
				if(page=0)
				{
					FALG_Dget=1;
				}
				// para
				if(page==1)
				{
					if(page_P==0)
					{
						PARA_D=(PARA_D>80)?90:(PARA_D+10);
					}
					else
					{
						PARA_T=(PARA_T>70)?80:(PARA_T+1);
					}
				}
				// factory
				if(page==2)
				{
					if(page_S==0)
					{
						ADJ_V=(ADJ_V>85)?90:(ADJ_V+5);
					}
					else if(page_S==1)
					{
						WAVE_V=(WAVE_V>9980)?9990:(WAVE_V+10);
					}
					else
					{
						BT_V=(BT_V>19)?20:(BT_V+1);
					}
				}
				break;
			case 9:
				// distent
				if(page=0)
				{
					if(DISTEMT_RC[0]!=0)
					{
						FALG_Dout=1;
					}
				}
				// para
				if(page==1)
				{
					if(page_P==0)
					{
						PARA_D=(PARA_D<20)?10:(PARA_D-10);
					}
					else
					{
						PARA_T=(PARA_T<=1)?0:(PARA_T-1);
					}
				}
				// factory
				if(page==2)
				{
					if(page_S==0)
					{
						ADJ_V=(ADJ_V<-85)?-90:(ADJ_V-5);
					}
					else if(page_S==1)
					{
						WAVE_V=(WAVE_V<20)?10:(WAVE_V-10);
					}
					else
					{
						BT_V=(BT_V<2)?1:(BT_V-1);
					}
				}
				break;
			case 89:
				key_time=T1_1MS;
				break
		}
		key_past=key_now;
	}
	if(T1_1MS-key_time>=2000)
	{
		if(key_past==89)
		{
			FALG_Dget=0;
			page=0;
			page_M=0;
			PARA_D=40;
			PARA_T=30;
			ADJ_V=0;
			WAVE_V=340;
			BT_V=10;
		}
	}
}

void DAC_Fuc(void)
{
	float fK;
	u8 dac_out=0;//10*
	u8 i=0;
	fK=(5-BT_V)/80.0;
	if(FALG_Dout)
	{
		if(FLAG_DAC)
		{
			FLAG_DAC=0;
			if(i<=60)
			{
				if(DISTEMT_RC[i]<=10)
				{
					dac_out=BT_V/10.0;
				}
				else if(DISTEMT_RC[i]=>90)
				{
					dac_out=BT_V/10.0+fK*80;
				}
				else
				{
					dac_out=BT_V/10.0+fK*(DISTEMT_RC[i]-10);
				}
				PCF8591_DAC(dac_out*255/5)
				i++;
			}
			else
			{
				FALG_Dout=0;
				i=0;
			}
		}
	}
}

void WAVE_Fuc(void)
{
	u8 n=4;
	u8 j=0;
	u16 CHL=0;
	if(FALG_WAVE)
	{
		FALG_WAVE=0;
		CR=0;
		CH=0xFF;
		CL=0xF3;
		CF=0;
		CR=1;
		while(n--)
		{
			while(!CF);
			CF=0;
			waveP10^=1;
		}
		CR=0;
		CH=0x00;
		CL=0x00;
		CF=0;
		CR=1;
		while(!CF&& waveP11);
		CR=0;
		if(CF)
		{
			DISTENT=65536*(WAVE_V/20000)+ADJ_V;
		}
		else
		{
			CHL=(u16)(CH<<8)+CL;
			DISTENT=CHL*(WAVE_V/20000)+ADJ_V;
		}
		if(FALG_Dget)
		{
			if(j<=60)
			{
				DISTEMT_RC[j]=DISTENT;
				j++;
			}
			else
			{
				FALG_Dget=0;
				j=0;
			}
		}
	}
}

void LED_Fuc(void)
{
	
}

void T1_ISR(void) interrupt 3
{
	static u32 T1_100MS;
	T1_1MS++;
	if(++T1_100MS==100)
	{
		T1_100MS=0;
		FLAG_SEG=1;
		FLAG_WAVE=1;
		FLAG_DAC=1;
	}
	SEG_Disp(SEG_CD,SEG_PS);
	SEG_PS=(++SEG_PS)&0x07;
}