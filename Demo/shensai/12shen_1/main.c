#include "main.h"
#include "tim.h"
#include "onewire.h"
#include "iic.h"
#include "stdio.h"
u8 ucLed=0;
u8 ucState=0;
u8 ucMode=0;

u32 ulMs=0;
//u32 ulMs10=0; 
u16 usSec=0;
u8 uc100ms_Cnt=0,ucDac_Flag=0;
u8 uc10ms_Cnt=0,ucSeg_Flag=0,ucLed_Flag=0;
//u16 uiSec_Cnt=0;
//u8 ucSec_Flag=0;

u8 pucSeg_Buff[10]="12345678",pucSeg_Code[8]={0},ucSeg_Pos=0;

float fWd;  //defined in uart,used in  ds18b20
u8 ucPara=25;
u8 ucDac;




void Key_Proc(void);
void Seg_Proc(void);
void Dac_Proc(void);
void Wd_Proc(void);
void Led_Proc(void);

void main(void)
{
	Cls_Waishe();
	T1_Init();
	
	while(1)
	{
		Key_Proc();
		
		Seg_Proc();
		
		Dac_Proc();
	
		Wd_Proc();
		Led_Proc();
	}
}
void Key_Proc(void)
{
	static u8 ucKey_Old=0;
	u8 ucKey_Now;
	//ucKey_Now=KBD_Read();
	ucKey_Now=KBD_Read();
	if(ucKey_Now !=ucKey_Old)
	{
		switch(ucKey_Now)
		{
			//case 0:				
			//	break;
			case 1:		//S4
				ucState=(++ucState) % 3;   //why not ucState++
				
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
					ucPara++;
					if(ucPara>99) ucPara=0;
				}
				break;		
			case 6:		//S9
				if(ucState==1)
				{
					if(!ucPara)
					{
						ucPara=99;
					}
					else
						ucPara--;
				}
				break;						
		}
		ucKey_Old=ucKey_Now;
	}
}

void Seg_Proc(void)
{
	static u32 ulMs10Temp=0; 		
	
	if(!uc10ms_Flag) return;
	uc10ms_Flag=0;
	//if(ulMs10Temp !=ulMs10)   //method 3
	//{
		//ulMs10Temp =ulMs10;
		if(!ucState)
		{
			sprintf(pucSeg_Buff, "C   %5.2f",fWd);	
		}
		else if(ucState==1)
		{
			sprintf(pucSeg_Buff, "P     %2u",(u16)ucPara);	
		}
		else
		{
			sprintf(pucSeg_Buff, "A    %4.2f",ucDac*5.0/255);
		}
		
		Seg_Tran(pucSeg_Buff,pucSeg_Code);	
  //}
}
void ISR_T1() interrupt 3		//1ms
{
	static u8 ucMsCnt=0;
	ulMs++;
	
	//if(ulMs % 10==0)   
	//	ulMs10++;
	if(++uc10ms_Cnt==10)
	{
		uc10ms_Cnt=0;
		ucSeg_Flag=1;
		ucLed_Flag=1;
	}
	if(++uc100ms_Cnt==100)
	{
		uc100ms_Cnt=0;
		ucDac_Flag=1;
	}
	
	if(ulMs % 1000==0)
	{
		usSec++;			
	}
	/*if(++uiSec_Cnt==1000)
	{
		uiSec_Cnt=0;
		ucSec_Flag=1;
	}*/
	Seg_Disp(pucSeg_Code,ucSeg_Pos);
	if(++ucSeg_Pos==8) ucSeg_Pos=0;		
}
void Wd_Proc(void)
{	
  static u32 ulStepBegin=0;
  static u8 ucStep=0;
  s16 sWd;
  
	switch(ucStep)
  {
    case 0:
      Wd_Convert();
      ulStepBegin=ulMs;
      ucStep=1;
      break;
    case 1:
      if(ulMs-ulStepBegin>760)		//760
      {				
				sWd=Wd_Read();
				fWd=sWd/16.0;	
				ulStepBegin=ulMs;
				ucStep=2;
      }
      break;
		case 2:			
				if(ulMs-ulStepBegin>5)		//100 »ò10
				{
					ucStep=0;
				}	
				break;
  }		
}
void Dac_Proc(void)
{
    float fAdc;
    if(!ucDac_Flag) return;
		ucDac_Flag=0;
		if(!ucMode)
		{
			if( fWd < ucPara)
				ucDac=0;
			else
				ucDac=255;
		}
		else
		{
			if(fWd<20)
			{
				ucDac=1*255/5;
			}
			else if(fWd>40)
			{
				ucDac=4*255/5;
			}
			else
			{
				ucDac=((fWd-20)*3.0/20+1)*255/5;
			}
		}
		PCF8591_Dac(ucDac);
    //ucDac=PCF8591_Adc();		
    
}
void Led_Proc(void)
{
	if(!uc10ms_Flag) return;
	uc10ms_Flag=0;
	if(!ucMode)
		ucLed |=1;
	else
		ucLed &=1;
	ucLed &=~(7<<1);
	ucLed |=1<<(ucState+1);
	Led_Disp(ucLed);
}