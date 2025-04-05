/*	# 	单总线代码片段说明
    1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
    2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
        中对单片机时钟频率的要求，进行代码调试和修改。
*/

// 温度传感器模拟量转换
#include "onewire.h"
void Delay_OneWire(unsigned int t)
{
    unsigned char i;
    while (t--)
    {
        for (i = 0; i < 12; i++)
            ;
    }
}

// DS18B20写函数
void Write_DS18B20(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        DQ = 0;
        DQ = dat & 0x01;
        Delay_OneWire(5);
        DQ = 1;
        dat >>= 1;
    }
    Delay_OneWire(5);
}

// DS18B20读函数
unsigned char Read_DS18B20(void)
{
    unsigned char i;
    unsigned char dat;

    for (i = 0; i < 8; i++)
    {
        DQ = 0;
        dat >>= 1;
        DQ = 1;
        if (DQ)
        {
            dat |= 0x80;
        }
        Delay_OneWire(5);
    }
    return dat;
}

// 初始化DS18B20
bit init_ds18b20(void)
{
    bit initflag = 0;

    DQ = 1;
    Delay_OneWire(12);
    DQ = 0;
    Delay_OneWire(80);
    DQ = 1;
    Delay_OneWire(10);
    initflag = DQ;
    Delay_OneWire(5);

    return initflag;
}
/*user code here*/

// DS18B20启动温度转换
void DS18B20_START_CV(void)
{
    bit bRtn = 0;
    bRtn = init_ds18b20();
    Write_DS18B20(0xCC);
    Write_DS18B20(0x44);
}
// DS18B20温度读取
signed int DS18B20_RD(void)
{
    unsigned char DH, DL;
    bit bRtn = 0;
    bRtn = init_ds18b20();
    Write_DS18B20(0xCC);
    Write_DS18B20(0xBE);
    DL = Read_DS18B20();
    DH = Read_DS18B20();
    return (DH << 8)+DL;
}
