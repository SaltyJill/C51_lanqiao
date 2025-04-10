#include "ds1302.h"

//DS1302时间寄存器读写地址
code unsigned char w_rtc_address[7]={0x80,0x82,0x84,0x86,0x88,0x8a,0x8c}; 
code unsigned char r_rtc_address[7]={0x81,0x83,0x85,0x87,0x89,0x8b,0x8d}; 
unsigned char rtc[6];

unsigned char *GetRTC(void){
	return rtc;
}

/**
 * @brief 向DSD1302写一个字节数据
 * @param temp ,写入数据
 * @return None.
 */
void Write_Ds1302_Byte(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK=0;
		SDA=temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}  

/**
 * @brief 向DS1302的指定寄存器写入数据
 * @param address ,寄存器地址
 * @param dat ,写入数据
 */
void Write_Ds1302( unsigned char address,unsigned char dat )     
{
 	RST=0;
	_nop_();
 	SCK=0;
	_nop_();
 	RST=1;	
   	_nop_();  
 	Write_Ds1302_Byte(address);	
 	Write_Ds1302_Byte(dat);		
 	RST=0; 
}

/**
 * @brief 从DS1302的指定地址读出数据
 * @param address ,寄存器地址
 * @return 寄存器的值
 */
unsigned char Read_Ds1302 ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;
	_nop_();
 	SCK=0;
	_nop_();
 	RST=1;
	_nop_();
 	Write_Ds1302_Byte(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;
	_nop_();
 	RST=0;
	SCK=0;
	_nop_();
	SCK=1;
	_nop_();
	SDA=0;
	_nop_();
	SDA=1;
	_nop_();
	return (temp);			
}

/**
 * @brief 读取DS1302时钟芯片的时钟寄存器
 * @param rtc_buf ,保存时钟数据的缓冲区
 * @return None.
 */
void ReadRTC(void)
{
 	unsigned char i, *p;
	unsigned char tmp[3];
	
	//地址传递
 	p = (unsigned char *)r_rtc_address; 	
 	
 	for(i=0;i<3;i++){
	  	tmp[i]=Read_Ds1302(*p);
	  	p++;
 	}
	
	rtc[0] = (tmp[2] >> 4);
	rtc[1] = (tmp[2] & 0x0F);

	rtc[2] = (tmp[1] >> 4);
	rtc[3] = (tmp[1] & 0x0F);

	rtc[4] = (tmp[0] >> 4);
	rtc[5] = (tmp[0] & 0x0F);
}

/**
 * @brief 初始化时钟芯片
 * @param None.
 * @return None.
 */
void InitRTC()
{
	unsigned char tmp_val = 0;
	tmp_val = Read_Ds1302(0x81);
	Write_Ds1302(0x80, tmp_val & 0x7F); 
}
