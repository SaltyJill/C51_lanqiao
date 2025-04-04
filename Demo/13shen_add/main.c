#include "main.h"
#include "tim.h"
#include "onewire.h"
#include "ds1302.h"
#include "stdio.h"
#include "string.h"
u8 ucLed=0;
u8 ucState=0;
u8 ucMode=0;
u8 ucS17_Press=0;
u32 uwMs=0;
//u32 ulMs10=0; 
u8 ucSeg_Flag=0,ucTim_Flag,uc100ms_Flag=0,ucWave_Flag=0;
//u16 uiSec_Cnt=0;
//u8 ucSec_Flag=0;

u8 pucSeg_Buff[10]="12345678",pucSeg_Code[8]={0},ucSeg_Pos=0;

u16 usWd;  //defined in uart,used in  ds18b20
u8 ucPara=25;
u8 pucTim_Now[3]={10,20,30};

u8 ucHour_5s=0,ucRly_On=0;

sbit Tx_Wave=P1^0;
sbit Rx_Wave=P1^1;
u16 usDist=0;

u8 ucUartFlag=0;

void Key_Proc(void);
void Seg_Proc(void);
void Tim_Proc(void);
void Wd_Proc(void);
void Led_Proc(void);
void Relay_Proc(void);
void Wave_Proc(void);
void Uart_Proc(void);

void main(void)
{
	u8 pucTim_Start[3]={8,59,50};
	Cls_Waishe();
	T1_Init();
  T0_Init();
	UartInit();
	Wr_Time(pucTim_Start);
	UartSend("hello");
	while(1)
	{
		Key_Proc();		
		Seg_Proc();		
		Tim_Proc();	
		Wd_Proc();
		Relay_Proc();
		Led_Proc();
		Wave_Proc();
		Uart_Proc();
	}
}
void Key_Proc(void)
{
	static u8 ucKey_Old=0;
	static u32 uwMs_Key=0;
	u8 ucKey_Now;
	ucKey_Now=KBD_Read();
	if(ucKey_Now !=ucKey_Old)
	{
		uwMs_Key=uwMs;
		switch(ucKey_Now)
		{
			case 0:				
				if(ucKey_Old==6)
					ucS17_Press=0;
				break;
			case 1:		//S12
				ucState=(++ucState) % 4;   //why not ucState++
				
				break;
			case 2:		//S13
				ucMode=!ucMode;
				
				break;
			case 3:		//
				
				break;
			case 4:		//
				
				break;			
			case 5:		//S6
				if(ucState==2)
				{
					ucPara++;
					if(ucPara>99) ucPara=10;
				}
				break;		
			case 6:		//S17
				if(ucState==2)
				{
					if(!ucPara)
					{
						ucPara=99;
					}
					else
						ucPara--;
				}
				ucS17_Press=1;
				break;						
		}
		ucKey_Old=ucKey_Now;
	}
	if(uwMs-uwMs_Key>=800)
	{
		switch(ucKey_Old)
		{
			case 5:		//S6
				if(ucState==2)
				{
					ucPara++;
					if(ucPara>99) ucPara=10;
				}
				break;		
			case 6:		//S17
				if(ucState==2)
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
	}
}

void Seg_Proc(void)
{
	//if(ucSeg_Flag) 
	//{
		//ucSeg_Flag=0;
	
		if(!ucState)
		{
			sprintf(pucSeg_Buff, "U1   %4.1f",usWd/10.0);	
		}
		else if(ucState==1)
		{
			if(!ucS17_Press)
				sprintf(pucSeg_Buff, "U2 %02u-%02u",(u16)pucTim_Now[0],(u16)pucTim_Now[1]);	
			else
				sprintf(pucSeg_Buff, "U2 %02u-%02u",(u16)pucTim_Now[1],(u16)pucTim_Now[2]);	
		}
		else if(ucState==2)
		{
			sprintf(pucSeg_Buff, "U3    %02u",(u16)ucPara);
		}
		else
		{
			sprintf(pucSeg_Buff, "U4  %04u",(u16)usDist);
		}		
		Seg_Tran(pucSeg_Buff,pucSeg_Code);	
  //}
}
void ISR_T1() interrupt 3		//1ms
{
	static u8 uc10ms_Cnt=0,uc100ms_Cnt=0;
	uwMs++;
	
	if(++uc10ms_Cnt==10)
	{
		uc10ms_Cnt=0;
		ucSeg_Flag=1;
		ucTim_Flag=1;
	}
	if(++uc100ms_Cnt==100)
	{
		uc100ms_Cnt=0;
		uc100ms_Flag=1;
		ucWave_Flag=1;
	}
	Seg_Disp(pucSeg_Code,ucSeg_Pos);
	if(++ucSeg_Pos==8) ucSeg_Pos=0;		
}

void ISR_Uart1(void) interrupt 4 
{              
	static u8 pucRecv[7]={0};
	static u8 ucPos=0;
	u8 str1[3]={0};
	u8 str2[3]={0};
  u8 c1,c2;
	int rst;
	u16 x;
	if(RI)
	{
		RI=0;
		pucRecv[ucPos++]=SBUF;
		pucRecv[ucPos]=0;
		if(ucPos==6)
		{			
			
			//rst=sscanf(pucRecv,"%2s%2d%2s",str1,&x,str2);      Err
			//rst=sscanf(pucRecv,"WD%2d\r\n",&x);   rst<--1,  \r\n
			rst=sscanf(pucRecv,"WD%2d%c%c",&x,&c1,&c2);
			if(rst==3)
			{
				usDist=c1;
				ucPara=x;
				if(x>=10 && x<=99 && c1=='\r' && c2=='\n')
				{
					ucPara=x;
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
          UartSend("Good");
          break;     
			case 2:	          
          UartSend("Error2");
          break;    
			case 3:	          
          UartSend("Error3");
          break;    
    }
   ucUartFlag=0;		
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
      ulStepBegin=uwMs;
      ucStep=1;
      break;
    case 1:
      if(uwMs-ulStepBegin>760)		//760
      {				
				sWd=Wd_Read();
				usWd=(sWd/16.0+0.05)*10;	
				ulStepBegin=uwMs;
				ucStep=2;
      }
      break;
		case 2:			
				if(uwMs-ulStepBegin>5)		//100 »ò10
				{
					ucStep=0;
				}	
				break;
  }		
}

void Led_Proc(void)
{
	static u8 ucLed_Old=0;
		if(ucHour_5s)
			ucLed |=1;
		else
			ucLed &=~1;
		if(!ucMode)
			ucLed |=1<<1;
		else
			ucLed &=~(1<<1);
	
	if(ucRly_On)
	{
		if(uc100ms_Flag)
		{
			uc100ms_Flag=0;
			ucLed ^=1<<2;
		}
	}
	else
		ucLed &=~(1<<2);
	if(ucLed!=ucLed_Old)
	{
		ucLed_Old=ucLed;
		Led_Disp(ucLed);
	}
}

void Tim_Proc(void)
{
	if(!ucTim_Flag) return;
	ucTim_Flag=0;
	Rd_Time(pucTim_Now);
	if(!pucTim_Now[1] && !pucTim_Now[2])
		ucHour_5s=1;
	else if(pucTim_Now[2]>=5)
		ucHour_5s=0;
}

void Relay_Proc(void)
{
	static ucRly_On_Old=0;
	if(!ucMode)
	{
		if(usWd> ucPara*10)
		{
			//Relay(1);
			ucRly_On=1;
		}
		else
		{
			//Relay(0);
			ucRly_On=0;
		}
	}
	else
	{
		if(ucHour_5s)
		{
			//Relay(1);
			ucRly_On=1;
		}
		else
		{
			//Relay(0);
			ucRly_On=0;
		}
	}
	if(ucRly_On!=ucRly_On_Old)
	{
		if(ucRly_On)
			Relay(1);
		else
			Relay(0);
		ucRly_On_Old=ucRly_On;
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
		usDist=65535*0.017;		
	}
	else
	{
		TH0TL0=(u16)TH0;
		TH0TL0=(u16)(TH0TL0<<8) + TL0;			
		usDist=TH0TL0*0.017;
	}		
}		
