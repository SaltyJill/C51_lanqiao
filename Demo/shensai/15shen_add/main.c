#include "main.h"
#include "tim.h"
#include "ds1302.h"
#include "iic.h"
#include "onewire.h"
#include <stdio.h>

u8 ucLed=0;
//volatile u8 ucMode=0;
u8 ucState=0;  //0,1,2,3
u8 ucMode=0;   //0,1


u32 ulMs=0;

u8 uc10_Cnt=0,uc1302_Flag=0;
u8 uc100_Cnt=0,ucAdc_Flag=0,ucWave_Flag=0;
u8 uc200_Cnt=0,uc200_Flag_L1=0,uc200_Flag_L2=0;

u8 pucSeg_Buff[10]="123456789",pucSeg_Code[8]={0},ucSeg_Pos=0;

s16 siDist=0;   //after Jiaozheng

u16 uiDist_Max=0;
u8 ucDist_Pf=100;
s8 scDist_Jz=0;
u8 pucTime_Trig[3]={10,20,30};
u8 ucDac=80;
u8 ucAdc0,ucAdc1,ucAdc3;  //debug


//DS1302
u8 pucTime_Start[3]={9,0,0};
u8 pucTime_Now[3]={10,20,30};

float fWd=24.5;
sbit Tx_Wave=P1^0;
sbit Rx_Wave=P1^1;
s16 siDist;  

u8 ucUartFlag=0;
u8 ucDuty=0;  		//duty / 10
u32 ulMs_Duty=0;	//duty start time
u8 pucMem[3];  //ucDist_Pf,scDist_Jz, flag--1/0,  1--data saved
//u8 pucMem1[4];

void Key_Proc(void);
void Seg_Proc1(void);

void Dac_Proc(void);
void Time_Proc(void);
void Led_Proc(void);
void Wd_Proc(void);
void Wave_Proc(void);
void Uart_Proc(void);
void Waishe_Proc(void);
void main(void)
{
	//u8 pucTemp[20]={0};
	Cls_Waishe();
	T1_Init();
	T0_Init();
	UartInit();
	//PCF8591_Write(120);	
	Wr_Time(pucTime_Start);	
	Delay(1000);     //very important
	EE_Read(pucMem,0,3);	
	
	//sprintf(pucTemp, "[%3u,%3u,%3u,%3u]", (u16)pucMem[0],(u16)pucMem[1],(u16)pucMem[2],(u16)pucMem[3]);
	//sprintf(pucTemp, "[%3u,%3u]", (u16)pucMem[0],(u16)pucMem[1]);
	//UartSend(pucTemp);	
	
	if(pucMem[2]==1)
	{
		ucDist_Pf=pucMem[0];
		scDist_Jz=(s8)pucMem[1];
	}
	else
	{
		ucDist_Pf=100;
		scDist_Jz=0;
		pucMem[0]=100;
		pucMem[1]=0;
		pucMem[2]=1;
		Delay(10);
		EE_Write(pucMem,0,3);
	}
	while(1)
	{
		Key_Proc();
		Seg_Proc1();
		Led_Proc();
		Wave_Proc();
		Dac_Proc();
		Time_Proc();
		
		Wd_Proc();
		Uart_Proc();
		Waishe_Proc();
	}
}
void Key_Proc(void)
{
	static u8 ucKey_Old=0;
	u8 ucKey_Now;
	//u8 pucTemp[20]={0};
	ucKey_Now=KBD_Read();
	if(ucKey_Now !=ucKey_Old)
	{
		switch(ucKey_Now)
		{
			//case 0:				
			//	break;
			case 1:		//S4
					ucState=(++ucState) % 4;    //++ucState ?
					if(ucState!=2) ucMode=0;
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
							
							ucDist_Pf +=20;
							if(ucDist_Pf>200) ucDist_Pf=200;
						}
						else
						{
							scDist_Jz +=10;
							if(scDist_Jz>30) scDist_Jz=30;
						}
						pucMem[0]=ucDist_Pf;
						pucMem[1]=scDist_Jz;						
						EE_Write(pucMem,0,3);
						
						//Delay(10);			
						//EE_Read(pucMem1,1,4);	
						//sprintf(pucTemp, "[%3u,%3u,%3u,%3u]", (u16)pucMem1[0],(u16)pucMem1[1],(u16)pucMem1[2],(u16)pucMem1[3]);
						//UartSend(pucTemp);						
					}
					break;			
			case 6:		//S9
					if(ucState==1)
					{
						if(!ucMode)
						{
							
							ucDist_Pf -=20;
							if(ucDist_Pf<60) ucDist_Pf=60;
						}
						else
						{
							scDist_Jz -=10;
							if(scDist_Jz<-30) scDist_Jz=-30;
						}
						pucMem[0]=ucDist_Pf;
						pucMem[1]=scDist_Jz;						
						EE_Write(pucMem,0,3);
						//Delay(10);
						//EE_Read(pucMem1,1,4);	
						//sprintf(pucTemp, "[%3u,%3u,%3u,%3u]", (u16)pucMem1[0],(u16)pucMem1[1],(u16)pucMem1[2],(u16)pucMem1[3]);
						//UartSend(pucTemp);				
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
			if(!ucMode)
			{				
				if(siDist>=0)
					sprintf(pucSeg_Buff, "F%4.1f %3u", fWd,(u16)(siDist));				
				else
					sprintf(pucSeg_Buff, "F%4.1f  LL", fWd);		
			}
			else
				sprintf(pucSeg_Buff, "%3.1f%3u%3u",ucAdc0*3.3/255, (u16)ucAdc1,(u16)ucAdc3);
			break;
		case 1:
			if(!ucMode)
				sprintf(pucSeg_Buff, "P1   %3u", (u16)(ucDist_Pf));
			else
				sprintf(pucSeg_Buff, "P2   %3d", (s16)(scDist_Jz));
			break;
		case 2:
			sprintf(pucSeg_Buff, "%02u-%02u-%02u", (u16)pucTime_Now[0],(u16)pucTime_Now[1],(u16)pucTime_Now[2]);
			break;
		case 3:
			if(!ucMode)
				sprintf(pucSeg_Buff, "HF   %3u", (u16)(uiDist_Max));
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
	static u8 ucMsCnt=0;
	ulMs++;
	
	if(ucDuty)
	{
		if(ulMs-ulMs_Duty<=5000)
		{
			if(++ucMsCnt==10) ucMsCnt=0;
			if(!ucMsCnt)
				P34=1;
			else if(ucMsCnt==ucDuty)
				P34=0;
		}
		else
			ucDuty=0;
	}	
	if(++uc10_Cnt==10)		//
	{
		uc10_Cnt=0;
		uc1302_Flag=1;
	}
	if(++uc100_Cnt==100)		//
	{
		uc100_Cnt=0;
		ucAdc_Flag=1;
		ucWave_Flag=1;
	}
	if(++uc200_Cnt==200)		//
	{
		uc200_Cnt=0;
		uc200_Flag_L1=1;
		uc200_Flag_L2=1;
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
	static u8 pucArr[5]={0},ucArrPos=0,ucAdcTemp;
	static u8 ucStep=0;
	if(!ucAdc_Flag) return;
	ucAdc_Flag=0;
	switch(ucStep) 
	{		
		case 0:
			if(siDist>=0)
			{
				if(siDist>uiDist_Max)
				{
					uiDist_Max=siDist;
					pucTime_Trig[0]=pucTime_Now[0];
					pucTime_Trig[1]=pucTime_Now[1];
					pucTime_Trig[2]=pucTime_Now[2];
				}
				if(siDist>ucDist_Pf)
					ucDac=255;
				else if(siDist<40)
					ucDac=255/5;
				else
					ucDac=((siDist-40)*4/(ucDist_Pf-40)+1)*255/5;			
			}
			else
			{
				ucDac=0;
			}
			PCF8591_Write(ucDac);
			break;
		case 1:
			ucAdc0=PCF8591_Read(0);
			break;
		case 2:			
			ucAdcTemp=PCF8591_Read(1);
			pucArr[ucArrPos++]=ucAdcTemp;
			if(ucArrPos==5) 
			{
				ucArrPos=0;
				ucAdc1=MaoPao_Filter(pucArr,5); 
			}	
			break;
		case 3:
			ucAdc3=PCF8591_Read(3);
			break;		
	}
	ucStep=(ucStep+1) %4 ;
}
void Led_Proc(void)
{
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
	
	if(siDist<0)
	{
		ucLed |=1<<1;		
	}
	else if(siDist>ucDist_Pf)   
	{
		if(uc200_Flag_L2)
		{
			uc200_Flag_L2=0;
			ucLed ^=1<<1;
		}
	}
	else
		ucLed &=~(1<<1);
	
	if(ucAdc1<5)
		ucLed |=1<<7;
	else
		ucLed &=~(1<<7);
	
	if(ucLed!=ucLed_Old)
	{
		ucLed_Old=ucLed;
		Led_Disp(ucLed);
	}
}

void Waishe_Proc(void)
{
	static u8 ucWs_Old=0;
	static u8 ucWs=0;
	if(ucAdc1<5)
	{
		if(siDist>ucDist_Pf)
			ucWs |=1<<4;
		else
			ucWs &=~(1<<4);
		if(siDist<0)
			ucWs |=1<<6;
		else
			ucWs &=~(1<<6);
	}
	if(ucWs !=ucWs_Old)
	{
		ucWs_Old=ucWs;
		Out_Waishe(ucWs);
	}
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
void Wave_Proc(void)
{
	u8 i=4;   //or i=6
	u16 TH0TL0=0;
	if(!ucWave_Flag) return;
	ucWave_Flag=0;

	TL0=0xF4;
	TH0=0xFF;
	TF0=0;
	TR0=1;
	while(i--)
	{
		while(!TF0);
		TF0=0;
		Tx_Wave ^=1;
	}	
	TR0=0;
	
	TL0=0;
	TH0=0;
	TF0=0;
	TR0=1;
	while(!TF0 && Rx_Wave);
	TR0=0;
	if(TF0)
	{
		TF0=0;	
		siDist=255+scDist_Jz;		//ok	
	}
	else
	{
		TH0TL0=(u16)TH0;
		TH0TL0=(u16)(TH0TL0<<8) + TL0;					
		siDist=TH0TL0*0.017+scDist_Jz;			//ok	
	}		
}		
void ISR_Uart1(void) interrupt 4 
{              
	static u8 pucRecv[16]={0};
	static u8 ucPos=0;
	u8 c1,c2;
	int rst;
	s16 x,y;
	if(RI)
	{
		RI=0;
		pucRecv[ucPos++]=SBUF;
		pucRecv[ucPos]=0;
		if(ucPos==7)  //  "PWM20\r\n"
		{
			rst=sscanf(pucRecv,"PWM%2d%c%c",&x,&c1,&c2);
			if(rst==3)
			{
				if(x>0 && x %10 ==0 && c1=='\r' && c2=='\n')
				{
					ucUartFlag=1;
					ucDuty=x/10;   
					ulMs_Duty=ulMs;
					ucPos=0;
				}				
			}	
		}		
		else if(ucPos==15)   //"P1:100,P2:-30\r\n"
		{
			rst=sscanf(pucRecv,"P1:%3d,P2:%3d%c%c",&x,&y,&c1,&c2);
			if(rst==4)
			{				
				if(x>=60 && x<=200 && y>=-30 && y<=30 && c1=='\r' && c2=='\n')
				{
					ucDist_Pf=x;
					scDist_Jz=y;
					ucUartFlag=1;
				}
				else
					ucUartFlag=3;				
			}
			else
				ucUartFlag=2;
			ucPos=0;
		}
	}
}

void Uart_Proc(void)
{   
   switch(ucUartFlag)
   {
      case 1:	          
          UartSend("Ok1");
          break;     
			case 2:	          
          UartSend("Err2");
          break;    
			case 3:	          
          UartSend("Err3");
          break;     			
    }
   ucUartFlag=0;		
}