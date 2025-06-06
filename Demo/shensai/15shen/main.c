#include "main.h"
#include "tim.h"
#include "ds1302.h"
#include "iic.h"
#include <stdio.h>

u8 ucLed=0;
//volatile u8 ucMode=0;
u8 ucState=0;  //0,1,2,3
u8 ucMode=0;   //0,1


u32 ulMs=0;
u16 us1000_Cnt=0;
u8 ucSec_Flag=0;
u8 uc10_Cnt=0,uc1302_Flag=0;
u8 uc200_Cnt=0,uc200_Flag_L1=0,uc200_Flag_L2=0;
u8 ucAdc_Flag=0;
u16 us250_Cnt=0;
u8 pucSeg_Buff[10]="123456789",pucSeg_Code[8]={0},ucSeg_Pos=0;

u16 uiFq=0;   //raw

u16 uiFq_Max=0;
u16 uiFq_Pf=2000;
s16 usFq_Jz=0;
u8 pucTime_Trig[3]={10,20,30};
u8 ucAdc=0;  //debug
u8 ucDac=80;

//DS1302
u8 pucTime_Start[3]={9,0,0};
u8 pucTime_Now[3]={10,20,30};

void Key_Proc(void);
void Seg_Proc1(void);

void Dac_Proc(void);
void Time_Proc(void);
void Led_Proc(void);


void main(void)
{
	Cls_Waishe();
	T1_Init();
	T0_Init();
	
	PCF8591_Write(120);	
	Wr_Time(pucTime_Start);	
	
	while(1)
	{
		Key_Proc();
		Seg_Proc1();
		Led_Proc();
		Dac_Proc();
		Time_Proc();
	}
}
void Key_Proc(void)
{
	static u8 ucKey_Old=0;
	u8 ucKey_Now;
	ucKey_Now=KBD_Read();
	if(ucKey_Now !=ucKey_Old)
	{
		switch(ucKey_Now)
		{
			//case 0:				
			//	break;
			case 1:		//S4
					ucState=(++ucState) % 4;    //++ucState ?
					if(ucState==1 || ucState==3) ucMode=0;
					break;
			case 2:		//S5
					ucMode=!ucMode;
					break;
			case 3:		//S6
					break;
			case 4:		//S7		
					break;			
			case 5:		//S8
					if(ucState==1)
					{
						if(!ucMode)
						{
							
							uiFq_Pf +=1000;
							if(uiFq_Pf>9000) uiFq_Pf=9000;
						}
						else
						{
							usFq_Jz +=100;
							if(usFq_Jz>900) usFq_Jz=900;
						}
					}
					break;			
			case 6:		//S9
					if(ucState==1)
					{
						if(!ucMode)
						{
							
							uiFq_Pf -=1000;
							if(uiFq_Pf<1000) uiFq_Pf=1000;
						}
						else
						{
							usFq_Jz -=100;
							if(usFq_Jz<-900) usFq_Jz=-900;
						}
					}
				break;			
		}	
		ucKey_Old=ucKey_Now;
	}
	
}

void Seg_Proc1(void)
{
	s32 swFq2;
	switch(ucState)
	{
		case 0:			
			swFq2=(s32)(uiFq)+usFq_Jz;
			if(swFq2>=0)
				//sprintf(pucSeg_Buff, "F  %5u", (u16)(uiFq+usFq_Jz));
				sprintf(pucSeg_Buff, "%3u%5d",(u16)ucAdc, (s16)(uiFq+usFq_Jz));
			else
				sprintf(pucSeg_Buff, "F     LL");			
			break;
		case 1:
			if(!ucMode)
				sprintf(pucSeg_Buff, "P1  %4u", (u16)(uiFq_Pf));
			else
				sprintf(pucSeg_Buff, "P2  %4d", (s16)(usFq_Jz));
			break;
		case 2:
			sprintf(pucSeg_Buff, "%02u-%02u-%02u", (u16)pucTime_Now[0],(u16)pucTime_Now[1],(u16)pucTime_Now[2]);
			break;
		case 3:
			if(!ucMode)
				sprintf(pucSeg_Buff, "HF %5u", (u16)(uiFq_Max));
			else
				sprintf(pucSeg_Buff, "HA%02u%02u%02u", (u16)pucTime_Trig[0],(u16)pucTime_Trig[1],(u16)pucTime_Trig[2]);
			break;
	}
	//sprintf(pucSeg_Buff, "%3u",(u16)ucAdc);	
		//sprintf(pucSeg_Buff, "1234567.8");	
		//if(ucAdc <50)
			//sprintf(pucSeg_Buff, "%5.1f%4u", fWd,(u16)ucAdc);	
		//else
			//sprintf(pucSeg_Buff, "%5.1f", fWd);	
		
		//sprintf(pucSeg_Buff, "%5u%2u", uiFq,(u16)ucTrigFlag);		
		Seg_Tran(pucSeg_Buff,pucSeg_Code);	
}
void ISR_T1() interrupt 3		//1ms
{
	//static u8 ucMsCnt=0;
	ulMs++;
	
	if(++uc10_Cnt==10)		//
	{
		uc10_Cnt=0;
		uc1302_Flag=1;
	}
	if(++uc200_Cnt==200)		//
	{
		uc200_Cnt=0;
		uc200_Flag_L1=1;
		uc200_Flag_L2=1;
	}
	
	if(++us1000_Cnt ==1000)
	{
		us1000_Cnt=0;
		ucSec_Flag=1;
		us250_Cnt=0;		//debug
			
		uiFq=(u16)(TH0<<8)+TL0;
		TR0=0;
		TH0=0;
		TL0=0;
		TR0=1;   
			
		//P34 Freq in Capture
		
	}
	if(++us250_Cnt==250)		//debug
	{
		ucAdc_Flag=1;
	}
	Seg_Disp(pucSeg_Code,ucSeg_Pos);
	if(++ucSeg_Pos==8) ucSeg_Pos=0;		
}
void Time_Proc(void)
{
	if(!uc1302_Flag) return;
	uc1302_Flag=0;
	Rd_Time(pucTime_Now);	
}
void Dac_Proc(void)
{	
	if(ucSec_Flag) 
	{
		ucSec_Flag=0;
		if((s32)uiFq+usFq_Jz>=0)
		{
			if(uiFq+usFq_Jz>uiFq_Max)
			{
				uiFq_Max=uiFq+usFq_Jz;
				pucTime_Trig[0]=pucTime_Now[0];
				pucTime_Trig[1]=pucTime_Now[1];
				pucTime_Trig[2]=pucTime_Now[2];
			}
			if(uiFq+usFq_Jz>uiFq_Pf)
				ucDac=255;
			else if(uiFq+usFq_Jz<500)
				ucDac=255/5;
			else
				ucDac=((uiFq+usFq_Jz-500)*4/(uiFq_Pf-500)+1)*255/5;			
		}
		else
		{
			ucDac=0;
		}
		PCF8591_Write(ucDac);
	}
	if(ucAdc_Flag)
	{
		ucAdc_Flag=0;
		ucAdc=PCF8591_Read(0);
	}
}
void Led_Proc(void)
{
	s32 swFq1;
	static u8 ucLed_Old=0;
	if(!ucState)
	{
		if(uc200_Flag_L1)
		{
			uc200_Flag_L1=0;
			ucLed ^=1<<0;
		}
	}
	else
		ucLed &=~(1<<0);
	swFq1=(s32)uiFq+usFq_Jz;
	if(swFq1<0)
	{
		ucLed |=1<<1;		
	}
	else if(swFq1>uiFq_Pf)   
	{
		if(uc200_Flag_L2)
		{
			uc200_Flag_L2=0;
			ucLed ^=1<<1;
		}
	}
	else
		ucLed &=~(1<<1);
	if(ucLed!=ucLed_Old)
	{
		ucLed_Old=ucLed;
		Led_Disp(ucLed);
	}
}
