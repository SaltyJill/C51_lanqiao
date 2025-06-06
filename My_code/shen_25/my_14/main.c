#include "main.h"
#include "driver.h"
#include "iic.h"
#include "ds1302.h"
#include "onewire.h"
#include <stdio.h>
// LED
u8 FLAG_FLASH = 0;
u8 FLAG_L6=0;
//ISR
u16 FREQ=0;
u32 T1_1MS=0;
// SEG
u8 SEG_DP[10]="12345",
	SEG_CD[8]={0};
u8 SEG_PS=0,
	FLAG_SEG=0;
u8 page=0,
	page_back=0;
u8 CD_3S=0,
	STEP_GET=0;
// TIM
u8 TIME_NOW[3]={0},
	TIME_TRIG[3]={0};
u8 FLAG_TIM=0;
// TEP
u8 FLAG_TEP=0;
u16 TEP=0,
	TEP_SAVE=0,
	TEP_PARA=25,
	TEP_MAX=0,
	TEP_SUM=0;
// RH
u16 RH=0,
	RH_SAVE=0,
	RH_MAX=0,
	RH_SUM=0;
// ADC
u8 FLAG_ADC=0;
u16 TRIG_CNT=0;

void SEG_Fuc(void);
void KEY_Fuc(void);
void TIM_Fuc(void);
void ADC_Fuc(void);
void TEP_Fuc(void);
void RH_Fuc(void);
void LED_Fuc(void);

void main(void)
{
	u8 Stime[3]={12,59,45};
	DEV_Cls();
	T1_Int();
	T0_Int();
	DS1302_ST(Stime);
	while(1)
	{
		KEY_Fuc();
		TIM_Fuc();
		TEP_Fuc();
		RH_Fuc();
		ADC_Fuc();
		SEG_Fuc();
		LED_Fuc();
	}
}
void SEG_Fuc(void)
{
	static u32 lighT=0;
	if(FLAG_SEG)
	{
		FLAG_SEG=0;
		if(CD_3S)
		{
			switch(STEP_GET)
			{
				case 0:
					lighT=T1_1MS;
					STEP_GET=1;
					break;
				case 1:
					if(RH)
					{
						sprintf(SEG_DP,"E``%2u-%2u",(u16)TEP_SAVE,(u16)RH_SAVE);
					}
					else
					{
						sprintf(SEG_DP,"E``%2u-AA",(u16)TEP_SAVE);
					}
					if(T1_1MS-lighT>3000)
					{
						CD_3S=0;
						STEP_GET=0;
					}						
					break;				
			}
		}
		else
		{
			switch(page)
			{
				case 0:
					sprintf(SEG_DP,"%02u-%02u-%02u",(u16)TIME_NOW[0],(u16)TIME_NOW[1],(u16)TIME_NOW[2]);
					break;
				case 1:
					switch(page_back)
					{
						case 0:
							if(TRIG_CNT)
							{
								sprintf(SEG_DP,"C`%2u-%2.1f",(u16)TEP_MAX,TEP_SUM/1.0/TRIG_CNT);
							}
							else
							{
								sprintf(SEG_DP,"C```````");
							}
							break;
						case 1:
							if(TRIG_CNT)
							{
								sprintf(SEG_DP,"H`%2u-%2.1f",(u16)RH_MAX,RH_SUM/1.0/TRIG_CNT);
							}
							else
							{
								sprintf(SEG_DP,"H```````");
							}
							break;
						case 2:
							if(TRIG_CNT)
							{
							sprintf(SEG_DP,"F%02u%02u-%02u",(u16)TRIG_CNT,(u16)TIME_TRIG[0],(u16)TIME_TRIG[1]);
							}
							else
							{
								sprintf(SEG_DP,"F```````");								
							}
							break;
					}
					break;
				case 2:
					sprintf(SEG_DP,"P`````%2u",(u16)TEP_PARA);
					break;
			}
		}
		SEG_Tran(SEG_DP,SEG_CD);
	}
}
void KEY_Fuc(void)
{
	static u8 FLAG_K9=0;
	static u8 key_past=0;
	static u32 key_time=0;
	u8 key_now;
	key_now=KEY_Martix();
	if(key_past!=key_now)
	{
		key_time=T1_1MS;
		switch(key_now)
		{
			case 0:
				if(FLAG_K9)
				{
					FLAG_K9=0;
					TRIG_CNT=0;
					TEP_MAX=0;
					TEP_SUM=0;
					RH_MAX=0;
					RH_SUM=0;
					TIME_TRIG[0]=0;
					TIME_TRIG[1]=0;
				}
				break;
			case 4:
				page=(++page)%3;
				break;
			case 5:
				if(page==1)
				{
					page_back=(++page_back)%3;
				}
				break;
			case 8:
				if(page==2)
				{
					if(TEP_PARA>=98)
					{
						TEP_PARA=99;
					}
					else
					{
						TEP_PARA++;
					}
				}
				break;
			case 9:
				if(page==2)
				{
					if(TEP_PARA<=1)
					{
						TEP_PARA=0;
					}
					else
					{
						TEP_PARA--;
					}
				}
				break;			
		}
		key_past=key_now;
	}
	if(T1_1MS-key_time>2000)
	{
		if(key_past==9&&page==1&&page_back==2)
		{
			FLAG_K9=1;
		}
	}
}
void TIM_Fuc(void)
{
	if(FLAG_TIM)
	{
		FLAG_TIM=0;
		DS1302_RD(TIME_NOW);
	}
}
void ADC_Fuc(void)
{
	static u8 V_light_past=0;
	static u16 tep_past=0;
	static u16 rh_past=0;
	u8 ADCget;
	u8 V_light=20;
	if(FLAG_ADC)
	{
		FLAG_ADC=0;
		ADCget=PCF_ADC();
		V_light=(ADCget*5.0/255+0.05)*10;
		if(V_light<10&&V_light_past>=10)
		{
			CD_3S=1;
			if(RH)
			{
				TRIG_CNT++;
				TIME_TRIG[0]=TIME_NOW[0];
				TIME_TRIG[1]=TIME_NOW[1];
				TEP_SAVE=TEP;
				RH_SAVE=RH;
				TEP_SUM=(TEP_SUM+TEP_SAVE);
				RH_SUM=(RH_SUM+RH_SAVE);
				if(TEP_SAVE>TEP_MAX)
				{
					TEP_MAX=TEP_SAVE;
				}
				if(RH_SAVE>RH_MAX)
				{
					RH_MAX=RH_SAVE;
				}
				FLAG_L6=(RH_SAVE>rh_past&&TEP_SAVE>tep_past)?(1):(0);
				tep_past=TEP_SAVE;
				rh_past=RH_SAVE;
			}
		}
		V_light_past=V_light;
	}
}
void TEP_Fuc(void)
{
	if(FLAG_TEP)
	{
		FLAG_TEP=0;
		TEP=DS18B20_TEP();
		TEP=TEP/16.0;
	}
}
void RH_Fuc(void)
{
	if(FREQ<200||FREQ>2000)
	{
		RH=0;
	}
	else
	{
		RH=10+(FREQ-200)*(2.0/45);
	}
}
void LED_Fuc(void)
{
	static u8 led_now=0;
	static u8 led_past=0;
	//L1,L2
	if(page!=2)
	{
		led_now=(led_now&0xFC)|(1<<page);
	}
	else
	{
		led_now&=0xFC;
	}
	//L3
	if(CD_3S)
	{
		led_now|=(u8)(1<<2);
	}
	else
	{
		led_now&=(u8)~(1<<2);
	}
	//L4
	if(TEP_SAVE>TEP_PARA)
	{
		if(FLAG_FLASH)
		{
			led_now|=(u8)(1<<3);
		}
		else
		{
			led_now&=(u8)~(1<<3);			
		}
	}
	else
	{
		led_now&=(u8)~(1<<3);	
	}
	//L5
	if(RH||TRIG_CNT==0)
	{
		led_now&=(u8)~(1<<4);	
	}
	else
	{
		led_now|=(u8)(1<<4);		
	}
	//L6
	if(FLAG_L6&&TRIG_CNT>=2)
	{
		led_now|=(u8)(1<<5);	
	}
	else
	{
		led_now&=(u8)~(1<<5);	
	}
	if(led_past!=led_now)
	{
		LED_Disp(led_now);
		led_past=led_now;
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
		FLAG_TIM=1;
		FLAG_TEP=1;
		FLAG_ADC=1;
		FLAG_FLASH=!FLAG_FLASH;
	}
	if(++T1_1S==1000)
	{
		T1_1S=0;
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