#include "main.h"
#include "tim.h"
#include "iic.h"
#include "ds1302.h"
#include "onewire.h"
#include <stdio.h>
#include <string.h>

u8 ucLed=0;
//volatile u8 ucMode=0;
u8 ucState=0;  //0,1,2,3-E
u8 ucEchoMode=0;   //0,1,2
u8 ucTrigFlag=0;

u32 ulMs=0;
u16 usSec=0;
u8 uc1302_Cnt=0,uc1302_Flag=0;
u8 uc100_Cnt=0,ucADC_Flag=0,uc100_Flag=0;

u8 pucSeg_Buff[10]="123456789",pucSeg_Code[8]={0},ucSeg_Pos=0;


u8 ucAdc=0;
u16 uiFq=0;
u16 uiHd=0,uiHd_Old=0; //*10;
u16 uiHd_Max=0;
u32 ulHd_Sum=0;
float fWd;
u16 uiWd=0,uiWd_Old=0;  //why not u8? fWendu*10
u32 ulWd_Sum=0;
u16 uiWd_Max=0;
u8 ucPara=30;
u8 ucNum=0;
u8 pucTrigTime[2]={0,0};
//DS1302
u8 pucTime_Start[3]={10,20,30};
u8 pucTime_Now[3]={10,20,30};

u8 ucUartFlag=0;

void Key_Proc(void);
void Seg_Proc(void);

void Adc_Proc(void);
void Time_Proc(void);
void Led_Proc(void);
void Wd_Proc(void);
void Uart_Proc(void);
void Relay_Proc(void);
void main(void)
{
	Cls_Waishe();
	T1_Init();
	T0_Init();
	UartInit();
	//I2C
		
	Wr_Time(pucTime_Start);
	
	Seg_Tran(pucSeg_Buff,pucSeg_Code);
	//UartSend("uart_6");
	while(1)
	{
		Key_Proc();
		Seg_Proc();
		Led_Proc();
		Adc_Proc();
		Time_Proc();
		Wd_Proc();
		Uart_Proc();
		Relay_Proc();
	}
}
void Key_Proc(void)
{
	static u8 ucKey_Old=0;
	u8 ucKey_Now;
	static u32 ulKey_Press=0;
	//ucKey_Now=KBD_Read();
	ucKey_Now=KBD_Read();
	if(ucKey_Now !=ucKey_Old)
	{
		ulKey_Press=ulMs;
		if(!ucTrigFlag)
		{
			switch(ucKey_Now)
			{
				//case 0:				
				//	break;
				case 1:		//S4
					ucState=(++ucState) % 3;    //++ucState ?
					if(ucState==1) ucEchoMode=0;
					break;
				case 2:		//S5
					if(ucState==1)
					{
						ucEchoMode=(++ucEchoMode) % 3;
					}
					break;
				case 3:		//S6
					break;
				case 4:		//S7		
					break;			
				case 5:		//S8
					if(ucState==2)
					{
						ucPara=ucPara+1;
						if(ucPara>99) ucPara=99;
					}
					break;			
				case 6:		//S9
					if(ucState==2)
					{
						ucPara=ucPara-1;
					}
					break;			
			}
		}
		ucKey_Old=ucKey_Now;
	}
	if(ulKey_Press-ulMs>=2000)
	{
		switch(ucKey_Old)
		{
			case 6:
				if(ucState==1 && ucEchoMode==2)
				{
					ucNum=0;
					ulWd_Sum=0;
					uiWd_Max=0;
					uiWd=0;		//for Led Out
					uiWd_Old=0;
					ulHd_Sum=0;
					uiHd_Max=0;
					uiHd=0;
					uiHd_Old=0;
				}
				break;
		}
	}
}

void Seg_Proc(void)
{
	
		if(ucTrigFlag)
		{
			if(uiHd==950 || uiHd==50)     //==,=
				sprintf(pucSeg_Buff, "E  %2u-AA", (u16)uiWd/10);
			else
				sprintf(pucSeg_Buff, "E  %2u-%2u", (u16)uiWd/10,(u16)uiHd/10);
		}
		else
		{
			switch(ucState)
			{
				case 0:
					sprintf(pucSeg_Buff, "%02u-%02u-%02u", (u16)pucTime_Now[0],(u16)pucTime_Now[1],(u16)pucTime_Now[2]);
					break;
				case 1:
					if(!ucEchoMode)
					{
						if(!ucNum)
							sprintf(pucSeg_Buff, "C       ");
						else
							sprintf(pucSeg_Buff, "C %02u-%4.1f", (u16)(uiWd_Max/10),ulWd_Sum/10.0/ucNum);
							//sprintf(pucSeg_Buff, "C %02u-%4.1f", (u16)uiWd,uiWd/10.0);
						//sprintf(pucSeg_Buff, "C %02u-%4.1f", (u16)30,30.5);
					}
					else if(ucEchoMode==1)
					{
						if(!ucNum)
							sprintf(pucSeg_Buff, "H       ");
						else
							sprintf(pucSeg_Buff, "H %02u-%4.1f", (u16)(uiHd_Max/10),ulHd_Sum/10.0/ucNum);
							//sprintf(pucSeg_Buff, "H %02u-%4.1f", (u16)uiHd,uiHd/10.0);	
						//sprintf(pucSeg_Buff, "H %02u-%4.1f", (u16)40,40.5);
					}
					else
					{
						if(!ucNum)
							sprintf(pucSeg_Buff, "F00     ");
						else
							sprintf(pucSeg_Buff, "F%02u%02u-%02u", (u16)ucNum,(u16)pucTrigTime[0],(u16)pucTrigTime[1]);
					}
					break;
				case 2:
					sprintf(pucSeg_Buff, "P     %02u", (u16)ucPara);
					break;
			}
		}
		//sprintf(pucSeg_Buff, "1234567.8");	
		//if(ucAdc <50)
			//sprintf(pucSeg_Buff, "%5.1f%4u", fWd,(u16)ucAdc);	
		//else
			//sprintf(pucSeg_Buff, "%5.1f", fWd);	
		
		//sprintf(pucSeg_Buff, "%5u%2u", uiFq,(u16)ucTrigFlag);		
		Seg_Tran(pucSeg_Buff,pucSeg_Code);	
}

void Led_Proc(void)
{
	static u8 ucLed_Old=0;
	if(ucTrigFlag)
	{
		ucLed &=~7;
		ucLed |=1<<2;
	}
	else
	{
		ucLed &=~7;
		if(!ucState)
		{
			ucLed |=1<<0;
		}
		else if(ucState==1)
		{
			ucLed |=1<<1;
		}
	}
	if(uiWd>(ucPara*10))  				//L4
		if(uc100_Flag)
		{
			uc100_Flag=0;
			ucLed ^=1<<3;
		}
	else
		ucLed &=~(1<<3);
	if(uiHd>=100 && uiHd<=900)		//L5
		ucLed &=~(1<<4);
	else		
		ucLed |=1<<4;
	if(ucNum>1)
	{
		if(uiWd>uiWd_Old && uiHd>uiHd_Old)
			ucLed |=1<<5;
		else
			ucLed &=~(1<<5);
	}
	else
		ucLed &=~(1<<5);
	if(ucLed!=ucLed_Old)
	{
		ucLed_Old=ucLed;
		Led_Disp(ucLed);
	}
}
void Relay_Proc(void)
{
	static u8 ucRly_On_Old=0;
	u8 ucRly_On;
	if(ucAdc<50)
		ucRly_On=1;
	else
		ucRly_On=0;
	if(ucRly_On!=ucRly_On_Old)
	{
		if(ucRly_On)
			Relay(1);
		else
			Relay(0);
		ucRly_On_Old=ucRly_On;
	}		
}
void ISR_T1() interrupt 3		//1ms
{
	//static u8 ucMsCnt=0;
	ulMs++;
	
	if(++uc1302_Cnt==10)		//
	{
		uc1302_Cnt=0;
		uc1302_Flag=1;
	}
	if(++uc100_Cnt==100)		//
	{
		uc100_Cnt=0;
		ucADC_Flag=1;
		uc100_Flag=1;
	}
	/* method 2
	if(ulMs % 10==0)
		ucSeg_Flag=1;*/
	//if(ulMs % 10==0)   //method 3
	//	ulMs10++;	
	/*
	if(ulMs % 100 ==0)
	{
		ulMs100++;
	}*/
		
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
	if(ulMs % 1000==0)
	{
		usSec++;
		//ucLed ^=1<<7;  //Led8
		//Led_Disp(ucLed);
		//P34 Freq in Capture
		
		uiFq=(u16)(TH0<<8)+TL0;
		TR0=0;
		TH0=0;
		TL0=0;
		TR0=1;   
			
		//P34 Freq in Capture
		
	}
	Seg_Disp(pucSeg_Code,ucSeg_Pos);
	if(++ucSeg_Pos==8) ucSeg_Pos=0;		
}
void ISR_T0(void) interrupt 1     //100us
{
	static u8 uc100Cnt=0;
	u8 pucDuty[4]={2,3,5,8};	//based 100us,pulse width
  //PWM  target:4
	/*	
	if(++uc100Cnt==5)
	{
		uc100Cnt=0;
		P34 ^=1;
	}*/
	//PWM  target:5
	/*	
	if(++uc100Cnt==10) uc100Cnt=0;
	if(!uc100Cnt)
		P34=1;
	else if(uc100Cnt==4)
		P34=0;*/
	//PWM target:6
	/*
	if(++uc100Cnt==10) uc100Cnt=0;
	if(!uc100Cnt)
		P34=1;
	else if(uc100Cnt==pucDuty[ucMode])
		P34=0;*/
	//PWM target:7
	/*if(ucRun)
	{
		if(++uc100Cnt==10) uc100Cnt=0;
		if(!uc100Cnt)
			P34=1;
		else if(uc100Cnt==pucDuty[ucMode])
			P34=0;
	}
	else
		P34=0;
	*/
}
void Time_Proc(void)
{
	if(!uc1302_Flag) return;
	uc1302_Flag=0;
	Rd_Time(pucTime_Now);	
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
void Adc_Proc(void)
{
	static u8 pucArr[5]={0},ucArrPos=0,ucAdcTemp;
	static u8 ucBool_Old=0;
	static u32 ulTrigBegin=0;
	u8 ucBool;
	if(ucTrigFlag)
	{
		if(ulMs-ulTrigBegin>=3000)
			ucTrigFlag=0;
	}
	if(ucADC_Flag)
	{
		ucADC_Flag=0;
		//ucAdc=PCF8591_Adc();
		ucAdcTemp=PCF8591_Adc();
		pucArr[ucArrPos++]=ucAdcTemp;
		//if(ucArrPos==5) ucAdc=MaoPao_Filter(pucArr,5);
		if(ucArrPos==5) 
		{
			ucArrPos=0;
			ucAdc=ComPare_Filter(pucArr,5); 
			if(ucAdc>=50)
			{
				ucBool=1;
				ucLed |=1<<7;		//debug
			}
			else
			{
				ucBool=0;
				ucLed &=~(1<<7);	//debug
			}
			Led_Disp(ucLed);		//debug
			if(!ucTrigFlag)
			{
				if(ucBool_Old && !ucBool  )
				{
					ucTrigFlag=1;
					ulTrigBegin=ulMs;
					uiWd_Old=uiWd;
					uiWd=fWd*10;			
					if(uiFq>=200 && uiFq<=2000)
					{
							uiHd_Old=uiHd;
							uiHd=(10+(uiFq-200)*80.0/1800)*10;	//valid
							if(uiHd>uiHd_Max) uiHd_Max=uiHd;
							ulHd_Sum +=uiHd;
							if(uiWd>uiWd_Max) uiWd_Max=uiWd;
							ulWd_Sum +=uiWd;
							ucNum++;
							pucTrigTime[0]=pucTime_Now[0];
							pucTrigTime[1]=pucTime_Now[1];
					}
					else if(uiFq>2000)
					{
							uiHd=950;    //no valid
					}
					else
					{
							uiHd=50;			//no valid
					}
				}
			}		
			ucBool_Old=ucBool;
		}
	}
}
void ISR_Uart1(void) interrupt 4 
{              
	static u8 pucRecv[9]={0};
	static u8 ucPos=0;
	u8 c1,c2;
	int rst;
	u16 x;
	if(RI)
	{
		RI=0;
		pucRecv[ucPos++]=SBUF;
		pucRecv[ucPos]=0;
		if(!strcmp(pucRecv,"Num\r\n"))
		{
			ucUartFlag=1;
			ucPos=0;
		}
		else if(!strcmp(pucRecv,"Clr\r\n"))
		{
			ucNum=0;
			ulWd_Sum=0;
			uiWd_Max=0;
			uiWd=0;		//for Led Out
			uiWd_Old=0;
			ulHd_Sum=0;
			uiHd_Max=0;
			uiHd=0;
			uiHd_Old=0;
			
			ucUartFlag=2;
			ucPos=0;
		}
		else if(ucPos==8)   
		{
			rst=sscanf(pucRecv,"Para%2d%c%c",&x,&c1,&c2);
			if(rst==3)
			{				
				if(x>0 && c1=='\r' && c2=='\n')
				{
					ucPara=x;
					ucUartFlag=2;
				}
				else
					ucUartFlag=3;				
			}
			else
				ucUartFlag=3;
			ucPos=0;
		}
	}
}
void Uart_Proc(void)
{
   u8 pucSend[8]={0};
   switch(ucUartFlag)
   {
      case 1:	
          sprintf(pucSend,"%u\r\n",(u16)ucNum);
          UartSend(pucSend);
          break;
      case 2:
          sprintf(pucSend,"OK\r\n");
          UartSend(pucSend);
          break;
      case 3:
          sprintf(pucSend,"ERR\r\n");
          UartSend(pucSend);
          break;
    }
   ucUartFlag=0;		
}