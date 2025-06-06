#include "main.h"
#include "tim.h"

#include "iic.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define somenops {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

//#define 15nops {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

u8 ucLed=0;
//volatile u8 ucMode=0;
u8 ucMode=0;
u8 ucAdc_Cnt=0,ucAdc_Flag=0;

u32 uwMs=0;
u32 uwMs10=0; 
u16 usSec=0;
u8 pucSeg_Buff[10]="12345678",pucSeg_Code[8]={0},ucSeg_Pos=0;

//u8 ucAdc=0;		//debug
//float fAdc;		//debug
u16 uiAdc;
s16 siPar;
u8 ucNum=0;

u8 ucKey_Num=0;		//debug
u8 ucL3_ON=0;
u8 ucL1_Flag=0;  //Adc<Para
u32 uwL1_Time=0;
void Key_Proc(void);
void Seg_Proc(void);
void Adc_Proc(void);
void Led_Proc(void);

void main(void)
{
	u8 ucMem;
	Cls_Waishe();
	T1_Init();
	
	EE_Read(&ucMem,0,1);
	if(ucMem >50) ucMem=0;
	siPar=ucMem*10;
	
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Adc_Proc();
		Led_Proc();
	}
}
void Key_Proc(void)
{
	u8 ucMem;
	static u8 ucKey_Old=0;
	u8 ucKey_Now;
	//static u8 ucKey_Num=0;
	//ucKey_Now=KBD_Read();
	ucKey_Now=KBD_Read();
	if(ucKey_Now !=ucKey_Old)
	{
		switch(ucKey_Now)
		{
			case 0:				
				break;
			case 9:				//S12
				ucMode=(++ucMode)% 3;  //ucMode++     
				if(ucMode==2)
				{
					ucMem=siPar/10;
					EE_Write(&ucMem,0,1);
				}
				ucKey_Num=0;
				ucL3_ON=0;
				break;
			case 10:		//S13
				if(ucMode==2)
				{
					ucNum=0;
					ucKey_Num=0;
					ucL3_ON=0;
				}
				else
				{
					ucKey_Num++;
					if(ucKey_Num>=3) 
					{
						ucL3_ON=1;
					}
				}
				
				break;
			case 13:		//S16
				if(ucMode==1)
				{
					siPar +=50;
					if(siPar>500) siPar=0;
					ucKey_Num=0;
					ucL3_ON=0;
				}
				else
				{
					ucKey_Num++;
					if(ucKey_Num>=3) 
					{
						ucL3_ON=1;
					}
				}
				break;
			case 14:		//S17
				if(ucMode==1)
				{
					siPar -=50;
					if(siPar<0) siPar=500;
					ucKey_Num=0;
					ucL3_ON=0;
				}
				else
				{
					ucKey_Num++;
					if(ucKey_Num>=3) 
					{
						ucL3_ON=1;
					}
				}
				break;			
			default:				
				break;			
		}
		ucKey_Old=ucKey_Now;
	}
}

void Seg_Proc(void)
{
	static u32 uwMs10Temp=0; 		
	/*   method 2/3
	if(!ucSeg_Flag) return;
	ucSeg_Flag=0;*/
	if(uwMs10Temp !=uwMs10)   //method 3
	{
		uwMs10Temp =uwMs10;
		if(!ucMode)
		{
			sprintf(pucSeg_Buff, "U    %4.2f", uiAdc/100.0);
			//sprintf(pucSeg_Buff, "U%2u%2u%4.2f",(u16)ucKey_Num,(u16)ucL3_ON, (float)(uiAdc/100.0));
			//sprintf(pucSeg_Buff, "U%2u%2u", (u16)ucKey_Num,(u16)ucL3_ON);
			//sprintf(pucSeg_Buff, "%.3f%3u", fAdc,(u16)uiAdc);
		}
		else if(ucMode==1)
		{
			sprintf(pucSeg_Buff, "P    %4.2f", siPar/100.0);
		}
		else
		{
			sprintf(pucSeg_Buff, "N     %2u", (u16)ucNum);
		}
		//sprintf(pucSeg_Buff, "U    %3d", (u16)ucMode);
		Seg_Tran(pucSeg_Buff,pucSeg_Code);	
  }
}
void ISR_T1() interrupt 3		//1ms
{
	static u8 ucMsCnt=0;
	uwMs++;
	/* method 1
	if(++ucSeg_Cnt==10)		//
	{
		ucSeg_Cnt=0;
		ucSeg_Flag=1;
	}*/
	/* method 2
	if(ulMs % 10==0)
		ucSeg_Flag=1;*/
	if(uwMs % 10==0)   //method 3
		uwMs10++;
	
	if(++ucAdc_Cnt==100)
	{
		ucAdc_Cnt=0;
		ucAdc_Flag=1;
	}
	
	
	//PWM  target:1
	/* P34 ^=1; */
	//PWM  target:2
	/*
	if(++ucMsCnt==5)
	{
		ucMsCnt=0;
		P34 ^=1;
	}*/
	//PWM  target:3
	/*
	if(++ucMsCnt==10) ucMsCnt=0;
	if(!ucMsCnt)
		P34=1;
	else if(ucMsCnt==3)
		P34=0;
	*/
	if(uwMs % 1000==0)
	{
		usSec++;
		
		
	}
	Seg_Disp(pucSeg_Code,ucSeg_Pos);
	if(++ucSeg_Pos==8) ucSeg_Pos=0;		
}
void Adc_Proc(void)
{
    u8 ucAdc;
		float fAdc;
		static u16 uiAdc_Old=0;
    if(!ucAdc_Flag) return;
		ucAdc_Flag=0;
		
    ucAdc=PCF8591_Adc();
		//if(ucAdc==0) ucAdc=199;
    fAdc=ucAdc*5.0/255;
		uiAdc=fAdc*100+0.5;   //自定义四舍五入
		if(uiAdc_Old > siPar && uiAdc<=siPar)
			ucNum++;
		if(uiAdc_Old >= siPar && uiAdc<siPar)
		{
			ucL1_Flag=1;
			uwL1_Time=uwMs;			
		}
		if(uiAdc>=siPar) ucL1_Flag=0;
		uiAdc_Old=uiAdc;
}
void Led_Proc(void)
{
	static u32 uwMs10Temp=0; 
	if(uwMs10Temp !=uwMs10)   //method 3
	{
		uwMs10Temp =uwMs10;
		if(ucL1_Flag && (uwMs-uwL1_Time)>=5000)
			ucLed |=1;
		else
			ucLed &=~1;
		if(ucNum % 2)
			ucLed |=1<<1;
		else
			ucLed &=~(1<<1);
		if(ucL3_ON)
			ucLed |=1<<2;
		else
			ucLed &=~(1<<2);
		Led_Disp(ucLed);
	}
}