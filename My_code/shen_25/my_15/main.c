#include "main.h"
#include "iic.h"
#include "driver.h"
#include "ds1302.h"
#include <stdio.h>
#include <string.h>
// LED
u8 LED_DP=0;
// T1
u32 T1_1MS=0;
// SEG
u8 SEG_DP[12]="12989";
u8 SEG_CD[8]={0};
u8 SEG_PS=0;
u8 FLAG_SEG=0;
u8 page=0;
u8 page_para=0;
u8 page_back=0;
// FREQ
s16 FREQ=0;
s16	FREQ_ADJ=0;
s16	FREQ_PARA=2000;
s16	FREQ_MAX=0;
s16	FREQ_WITH_ADJ=0;
u8 FLAG_FREQ=0;
// TIM
u8 TIME_NOW[3]={0};
	TIME_TRIG[3]={0};
u8 FLAG_TIM=0;
// DAC
u8 FLAG_DAC=0;
void LED_Fuc(void);
void SEG_Fuc(void);
void KEY_Fuc(void);
void DAC_Fuc(void);
void TIM_Fuc(void);
void FREQ_Fuc(void);
void main(void)
{
	u8 Time_set[3]={12,59,45};
	DS1302_SET(Time_set);
	DEV_Cls();
	T1_Int();
	T0_Int();
	while(1)
	{
		TIM_Fuc();
		FREQ_Fuc();		
		KEY_Fuc();
		LED_Fuc();
		SEG_Fuc();
		DAC_Fuc();
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
					if(FREQ_WITH_ADJ>=0)
					{
					sprintf(SEG_DP,"F``%5u",(u16)FREQ_WITH_ADJ);
					}
					else
					{
					sprintf(SEG_DP,"F`````LL");
					}
					break;
				case 1:
					if(!page_para)
					{
						sprintf(SEG_DP,"P1``%4u",(u16)FREQ_PARA);		
					}
					else
					{
						sprintf(SEG_DP,"P2``%4d",FREQ_ADJ);
					}
					break;
				case 2:
					sprintf(SEG_DP,"%02u-%02u-%02u",(u16)TIME_NOW[0],(u16)TIME_NOW[1],(u16)TIME_NOW[2]);
					break;
				case 3:
					if(!page_back)
					{
						sprintf(SEG_DP,"HF`%5u",(u16)FREQ_MAX);						
					}
					else
					{
						sprintf(SEG_DP,"HA%02u%02u%02u",(u16)TIME_TRIG[0],(u16)TIME_TRIG[1],(u16)TIME_TRIG[2]);						
					}
					break;
			}
			SEG_Tran(SEG_DP,SEG_CD);
		}
}
void KEY_Fuc(void)
{
	u8 key_now;
	static u8 key_past=0;
	key_now=KEY_Martix();
	if(key_now!=key_past)
		{
			switch(key_now)
			{
				case 4:
					page=(++page)&0x03;
					break;
				case 5:
					if(page==1)
					{
						page_para=!page_para;
					}
					if(page==3)
					{
						page_back=!page_back;
					}
					break;
				case 8:
					if(page==1)
						{
							if(!page_para)
							{
								FREQ_PARA+=1000;
								if(FREQ_PARA>9000)
								{
									FREQ_PARA=9000;
								}
							}
							else
							{
								FREQ_ADJ+=100;
								if(FREQ_ADJ>900)
								{
									FREQ_ADJ=900;
								}								
							}
						}
					break;
				case 9:
					if(page==1)
						{
							if(!page_para)
							{
								FREQ_PARA-=1000;
								if(FREQ_PARA<1000)
								{
									FREQ_PARA=1000;
								}
							}
							else
							{
								FREQ_ADJ-=100;
								if(FREQ_ADJ<-900)
								{
									FREQ_ADJ=-900;
								}								
							}
						}
					break;
					default:
						break;
			}
			key_past=key_now;
		}
}
void DAC_Fuc(void)
{
	u8 dac_out;
	float fdac;
	if(FLAG_DAC)
	{
		FLAG_DAC=0;
		if(FREQ_WITH_ADJ>0)
		{
			if(FREQ_WITH_ADJ<500)
			{
				dac_out=1*255/5;
			}
			else if(FREQ_WITH_ADJ>FREQ_PARA)
			{
				dac_out=1*255/5;
			}
			else
			{
				fdac=(4.0*FREQ_WITH_ADJ)/(FREQ_PARA-500)+1-(4.0*500/(FREQ_PARA-500));
				dac_out=fdac*255/5;
			}
		}
		else
		{
			dac_out=0;
		}
		DS8591_DAC(dac_out);
	}
}
void TIM_Fuc(void)
{
	if(FLAG_TIM)
	{
		FLAG_TIM=0;
		DS1302_RED(TIME_NOW);
	}
}
void FREQ_Fuc(void)
{
	if(FLAG_FREQ)
	{
		FLAG_FREQ=0;
		FREQ_WITH_ADJ=FREQ+FREQ_ADJ;
		if(FREQ_WITH_ADJ>FREQ_MAX)
		{
			FREQ_MAX=FREQ_WITH_ADJ;
			TIME_TRIG[0]=TIME_NOW[0];
			TIME_TRIG[1]=TIME_NOW[1];
			TIME_TRIG[2]=TIME_NOW[2];
		}
	}
}
void LED_Fuc(void)
{
	static u8 led_flag1=0;
	static u8 led_flag2=0;
	static u8 led_past=0;
	static u32 led_time=0;
	if(T1_1MS-led_time>200)
	{
		led_time=T1_1MS;
		led_flag1=!led_flag1;
		led_flag2=!led_flag2;
	}
	// L1	
	if(!page)
	{
		if(led_flag1)
		{
			LED_DP|=(u8)(1);
		}
		else
		{
			LED_DP&=(u8)~(1);
		}
	}
	else
	{
		LED_DP&=(u8)~(1);	
	}
	// L2
	if(FREQ_WITH_ADJ>FREQ_PARA)
	{
			if(led_flag2)
			{
				LED_DP|=(u8)(1<<1);
			}
			else
			{
				LED_DP&=(u8)~(1<<1);
			}
	}
	else
	{
		LED_DP&=(u8)~(1<<1);	
	}
	if(FREQ_WITH_ADJ<0)
	{
		LED_DP|=(u8)(1<<1);
	}
	if(led_past!=LED_DP)
	{
		LED_Disp(LED_DP);
		led_past=LED_DP;
	}
}

void T1_ISR(void) interrupt 3
{
	static u32 T1_100MS=0,T1_1S=0;
	T1_1MS++;
	if(++T1_100MS==100)
	{
		T1_100MS=0;
		FLAG_SEG=1;
		FLAG_DAC=1;
		FLAG_TIM=1;
	}
	if(++T1_1S==1000)
	{
		T1_1S=0;
		FLAG_FREQ=1;
		FREQ=(u16)(TH0<<8)|TL0;
		TR0=0;
		TH0=0x00;
		TL0=0x00;
		TF0=0;
		TR0=1;
	}
	SEG_Disp(SEG_CD,SEG_PS);
	SEG_PS=(++SEG_PS)&0x07;
}