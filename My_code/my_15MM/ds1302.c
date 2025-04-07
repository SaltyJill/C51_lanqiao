/*	# 	DS1302代码片段说明
    1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
    2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
        中对单片机时钟频率的要求，进行代码调试和修改。
*/

// DS1302时钟芯片
#include "ds1302.h"
void Write_Ds1302(unsigned char temp)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        SCK = 0;
        SDA = temp & 0x01;
        temp >>= 1;
        SCK = 1;
    }
}

// DS1302 写字节
void Write_Ds1302_Byte(unsigned char address, unsigned char dat)
{
    RST = 0;
    _nop_();
    SCK = 0;
    _nop_();
    RST = 1;
    _nop_();
    Write_Ds1302(address);
    Write_Ds1302(dat);
    RST = 0;
}

// DS1302 读字节
unsigned char Read_Ds1302_Byte(unsigned char address)
{
    unsigned char i, temp = 0x00;
    RST = 0;
    _nop_();
    SCK = 0;
    _nop_();
    RST = 1;
    _nop_();
    Write_Ds1302(address);
    for (i = 0; i < 8; i++)
    {
        SCK = 0;
        temp >>= 1;
        if (SDA)
            temp |= 0x80;
        SCK = 1;
    }
    RST = 0;
    _nop_();
    SCK = 0;
    _nop_();
    SCK = 1;
    _nop_();
    SDA = 0;
    _nop_();
    SDA = 1;
    _nop_();
    return (temp);
}
/*user code here*/
void DS1302_SET(u8 *ptime)
{
    Write_Ds1302_Byte(0x8E, 0x00);
    Write_Ds1302_Byte(0x80, ptime[2] / 10 * 16 + ptime[2] % 10);
    Write_Ds1302_Byte(0x82, ptime[1] / 10 * 16 + ptime[1] % 10);
    Write_Ds1302_Byte(0x84, ptime[0] / 10 * 16 + ptime[0] % 10);
    Write_Ds1302_Byte(0x8E, 0x80);
}
void DS1302_RED(u8 *prdTime)
{
    u8 tTime;
    tTime = Read_Ds1302_Byte(0x81);
    prdTime[2] = tTime / 16 * 10 + tTime % 16;
    tTime = Read_Ds1302_Byte(0x83);
    prdTime[1] = tTime / 16 * 10 + tTime % 16;
    tTime = Read_Ds1302_Byte(0x85);
    prdTime[0] = tTime / 16 * 10 + tTime % 16;
}