#include "main.h"
#include "tim.h"
#include "iic.h"
#include <stdio.h>
#include <string.h>

#define somenops {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_(); _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

//#define 15nops {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

u8 ucLed=0;
u8 ucLed_En=1;
u8 ucMode=0;
u8 ucChIndex=0;	//0-Ain1£¬1-Ain3
u8 ucAdc[2];
u8 uc10ms_Cnt=0,uc10ms_Flag=0;
u8 ucAdc_Cnt=0,ucAdc_Flag=0;

u32 uwMs=0;
u16 uiSec=0;
u8 pucSeg_Buff[10]="12345678",pucSeg_Code[8]={0},ucSeg_Pos=0;

u16 uiFreq=0;
u16 uiFreq_Ref;
u8 ucAdc3_Ref;



void Key_Proc(void);
void Seg_Proc(void);
void Adc_Proc(void);
void Led_Proc(void);

void main(void)
{
	Cls_Waishe();
	T1_Init();
	T0_Init();
	
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
	static u8 ucKey_Old=0;
	static u32 uwMs_Key=0;
	static u8 ucKeyPress1s=0;
	u8 ucKey_Now;
	//ucKey_Now=KBD_Read();
	ucKey_Now=BTN_Read();
	if(ucKey_Now !=ucKey_Old)
	{
		uwMs_Key=uwMs;
		switch(ucKey_Now)
		{
			case 0:		
				if(ucKeyPress1s)
				{
					ucLed_En=!ucLed_En;
					ucKeyPress1s=0;
				}
				break;
			case 1:		//S4
				ucMode=(++ucMode) % 3;
				if(ucMode==2) ucChIndex=0;
				break;
			case 2:		//S5
				ucChIndex=(++ucChIndex) % 2;				
				break;
			case 3:		//S6
				ucAdc3_Ref=ucAdc[ucChIndex];
				break;
			case 4:		//S7
				uiFreq_Ref=uiFreq;
				break;						
		}
		ucKey_Old=ucKey_Now;
	}
	if(uwMs-uwMs_Key>=1000)
	{
		switch(ucKey_Old)
		{
			case 4:
				ucKeyPress1s=1;
				break;
		}
	}
}

void Seg_Proc(void)
{
	u8 ucChId_Seg;	
	if(!uc10ms_Flag) return;
	uc10ms_Flag=0;
	if(!ucMode)
	{
		sprintf(pucSeg_Buff, "F%7u",(u16)uiFreq);	
	}
	else if(ucMode==1)
	{
		sprintf(pucSeg_Buff, "N%7u",(u16)1000000/uiFreq);	
	}
	else
	{
		if(!ucChIndex)
			ucChId_Seg=1;
		else
			ucChId_Seg=3;
		sprintf(pucSeg_Buff, "U-%u  %4.2f",(u16)ucChId_Seg, ucAdc[ucChIndex]*5.0/255);
		//sprintf(pucSeg_Buff, "U-%u  %3u",(u16)ucChId_Seg, (u16)ucAdc[ucChIndex]);
	}
		//sprintf(pucSeg_Buff, "-%u- %4u",(u16)ucMode, (u16)usSec);	
		//sprintf(pucSeg_Buff, "-%u- %4u",(u16)ucMode, (u16)uiFreq);
		//sprintf(pucSeg_Buff, "-%u- %5.1f",(u16)ucMode, fWaveLen);	
		//sprintf(pucSeg_Buff, "%4u%4u",(u16)ucMem[0], (u16)ucAdc);
		//sprintf(pucSeg_Buff, "%02d.%02d.%02d",(unsigned int)pucTime_Now[0], (unsigned int)pucTime_Now[1],(unsigned int)pucTime_Now[2]);
		//sprintf(pucSeg_Buff, "%4.1f%4u", fWendu,(u16)ucAdc);
		Seg_Tran(pucSeg_Buff,pucSeg_Code);	
  
}
void ISR_T1() interrupt 3		//1ms
{
	//static u8 ucMsCnt=0;
	uwMs++;
		
	if(++uc10ms_Cnt==100)   //100ms
	{
		uc10ms_Cnt=0;
		uc10ms_Flag=1;
	}
	
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
		uiFreq=(u16)(TH0<<8)+TL0;
		TR0=0;
		TH0=0;
		TL0=0;
		TR0=1;   
			
		//P34 Freq in Capture
		
	}
	Seg_Disp(pucSeg_Code,ucSeg_Pos);
	if(++ucSeg_Pos==8) ucSeg_Pos=0;		
}
void Adc_Proc(void)
{
 	u8 ucChId_Get;	
  static u8 ucChIndex_Get=0;  
	if(!ucAdc_Flag) return;
	ucAdc_Flag=0;
	if(!ucChIndex_Get)
		ucChId_Get=1;
	else
		ucChId_Get=3;
	ucAdc[ucChIndex_Get]=PCF8591_Adc(ucChId_Get);
	
	ucChIndex_Get=!ucChIndex_Get;
}
void Led_Proc(void)
{
	if(!uc10ms_Flag) return;
	uc10ms_Flag=0;	
	if(ucLed_En)
	{
		if(ucAdc[1]>ucAdc3_Ref) 
			ucLed |=1;
		else
			ucLed &=~1;
		if(uiFreq>uiFreq_Ref) 
			ucLed |=1<<1;
		else
			ucLed &=~(1<<1);
		ucLed &=~(7<<2);
		ucLed |=1<<(2+ucMode);		
	}
	else
		ucLed=0;
	Led_Disp(ucLed);
}