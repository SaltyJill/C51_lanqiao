/*	# 	DS1302代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/								
#include "ds1302.h"
//
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
	SDA=1;	_nop_();
	return (temp);			
}

void Wr_Time(u8 *pucTime)
{
	Write_Ds1302_Byte(0x8E,0);
	Write_Ds1302_Byte(0x80,pucTime[2]/10*16+pucTime[2] % 10);
	Write_Ds1302_Byte(0x82,pucTime[1]/10*16+pucTime[1] % 10);
	Write_Ds1302_Byte(0x84,pucTime[0]/10*16+pucTime[0] % 10);
	Write_Ds1302_Byte(0x8E,0x80);
}

void Rd_Time(u8 *pucTime)
{
	u8 Temp;
	Temp=Read_Ds1302_Byte(0x81);
	pucTime[2]=Temp/16*10+Temp % 16;
	Temp=Read_Ds1302_Byte(0x83);
	pucTime[1]=Temp/16*10+Temp % 16;
	Temp=Read_Ds1302_Byte(0x85);
	pucTime[0]=Temp/16*10+Temp % 16;
}
