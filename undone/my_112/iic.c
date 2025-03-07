/*	#   I2C代码片段说明
    1. 	本文件夹中提供的驱动代码供参赛选手完成程序设计参考。
    2. 	参赛选手可以自行编写相关代码或以该代码为基础，根据所选单片机类型、运行速度和试题
        中对单片机时钟频率的要求，进行代码调试和修改。
*/

#include "iic.h"
#define DELAY_TIME 5

// 延时
static void I2C_Delay(unsigned char n)
{
    do
    {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (n--);
}

// 开启I2C通讯
void I2CStart(void)
{
    sda = 1;
    scl = 1;
    I2C_Delay(DELAY_TIME);
    sda = 0;
    I2C_Delay(DELAY_TIME);
    scl = 0;
}

// 停止I2C通讯
void I2CStop(void)
{
    sda = 0;
    scl = 1;
    I2C_Delay(DELAY_TIME);
    sda = 1;
    I2C_Delay(DELAY_TIME);
}

// 发送数据
void I2CSendByte(unsigned char byt)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        scl = 0;
        I2C_Delay(DELAY_TIME);
        if (byt & 0x80)
        {
            sda = 1;
        }
        else
        {
            sda = 0;
        }
        I2C_Delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
        I2C_Delay(DELAY_TIME);
    }

    scl = 0;
}

// 接受数据
unsigned char I2CReceiveByte(void)
{
    unsigned char da;
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
        scl = 1;
        I2C_Delay(DELAY_TIME);
        da <<= 1;
        if (sda)
            da |= 0x01;
        scl = 0;
        I2C_Delay(DELAY_TIME);
    }
    return da;
}

// 等待应答
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

// 发送应答
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

/*user code here*/

void EE_WR(u8 *pBuf, u8 addr, u8 n)
{
    I2CStart();
    I2CSendByte(0xA0); // PCF8591写寻址
    I2CWaitAck();      // 等待应答
    I2CSendByte(addr);
    I2CWaitAck();

    while (n--) // 发送n个字节
    {
        I2CSendByte(*pBuf++);
        I2CWaitAck();
    }
    I2CStop();
}
void EE_RD(u8 *pBuf, u8 addr, u8 n)
{
    I2CStart();
    I2CSendByte(0xA0); // PCF8591写寻址
    I2CWaitAck();      // 等待应答
    I2CSendByte(addr);
    I2CWaitAck();
    //I2CStop();

    I2CStart();
    I2CSendByte(0xA1);//读寻址
    I2CWaitAck();
    while (n--)
    {
        *pBuf++= I2CReceiveByte();
        // 也可I2CWaitAck(); just in case
        if (n)
        {
            I2CSendAck(0);//肯定应答
        }
        else
        {
            I2CSendAck(1);//否定应答
        }
    }
    I2CStop();
}
void PCF8591_Dac(u8 dac_Data)
{
    I2CStart();
    I2CSendByte(0x90); // PCF8591写寻址
    I2CWaitAck();
    I2CSendByte(0x43);// 控制字,0,1开启DAC通道，00四通道都是相对地电压读入,0,0AIN通道数自动递增,11AD读入AIN3
    I2CWaitAck();

    I2CSendByte(dac_Data);
    I2CWaitAck();
    I2CStop();
}
unsigned char PCF8591_ADC(void)//AD不写控制字
{
    unsigned char AD_Temp;
    I2CStart();
    I2CSendByte(0x90); // PCF8591写寻址
    I2CWaitAck();
    I2CSendByte(0x43); 
    I2CWaitAck();

    I2CStart();
    I2CSendByte(0x91);//读寻址
    I2CWaitAck();
    AD_Temp = I2CReceiveByte();
    I2CSendAck(1);//停止
    I2CStop();
    return AD_Temp;
}