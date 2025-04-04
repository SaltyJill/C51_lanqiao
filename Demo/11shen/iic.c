/*	#   I2C代码片段说明
	1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
	2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
		中对单片机时钟频率的要求，进行代码调试和修改。
*/

#include "iic.h"
#define DELAY_TIME	5

//
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();
        _nop_();_nop_();_nop_();_nop_();_nop_();		
    }
    while(n--);      	
}

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 0;
	I2C_Delay(DELAY_TIME);
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
	I2C_Delay(DELAY_TIME);
    sda = 1;
	I2C_Delay(DELAY_TIME);
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		I2C_Delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
		I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
		I2C_Delay(DELAY_TIME);
    }
	
    scl = 0;  
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		I2C_Delay(DELAY_TIME);
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		I2C_Delay(DELAY_TIME);
	}
	return da;    
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
	I2C_Delay(DELAY_TIME);
    ackbit = sda; 
    scl = 0;
	I2C_Delay(DELAY_TIME);
	
	return ackbit;
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
	I2C_Delay(DELAY_TIME);
    scl = 1;
	I2C_Delay(DELAY_TIME);
    scl = 0; 
	sda = 1;
	I2C_Delay(DELAY_TIME);
}

void EE_Write(u8* pucBuf, u8 addr, u8 num)
{ 
   I2CStart();
   I2CSendByte(0xa0);
   I2CWaitAck();
   I2CSendByte(addr);
   I2CWaitAck();
   while(num--)
   {
      I2CSendByte(*pucBuf++);
      I2CWaitAck();
      //I2C_Delay(200);
   }
   I2CStop();
   //operate_delay(20);
}
void EE_Read(u8* pucBuf, u8 addr,u8 num)
{
   I2CStart();
   I2CSendByte(0xa0);
   I2CWaitAck();
   I2CSendByte(addr);
   I2CWaitAck();
   I2CStart();
   I2CSendByte(0xa1);
   I2CWaitAck();
   while(num--)
      {
         *pucBuf++ = I2CReceiveByte();
         if(num) I2CSendAck(0);
             else I2CSendAck(1);
      }
      I2CStop();
}
unsigned char PCF8591_Adc(void)
{
 unsigned char temp;
 I2CStart();
 I2CSendByte(0x90);
 I2CWaitAck();
// 允许 DAC，43--AIN3 3,40--AIN0
 I2CSendByte(0x43); 
 I2CWaitAck();
 I2CStart();
 I2CSendByte(0x91);
 I2CWaitAck();
 temp = I2CReceiveByte();
 I2CSendAck(1);
 I2CStop();

 return temp;
}
void PCF8591_Dac(unsigned char dat)
{
 I2CStart();
 I2CSendByte(0x90);
 I2CWaitAck();
 // 允许 DAC，43--AIN3 3,40----AIN0
 I2CSendByte(0x43); 
 I2CWaitAck();
 I2CSendByte(dat); 
 I2CWaitAck();
 I2CStop();
}
