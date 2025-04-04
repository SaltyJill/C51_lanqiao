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

// 启动通讯
void I2CStart(void)
{
    sda = 1;
    scl = 1;
    I2C_Delay(DELAY_TIME);
    sda = 0;
    I2C_Delay(DELAY_TIME);
    scl = 0;
}

// 停止通讯
void I2CStop(void)
{
    sda = 0;
    scl = 1;
    I2C_Delay(DELAY_TIME);
    sda = 1;
    I2C_Delay(DELAY_TIME);
}

// 发送字节
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
void E2_RD(u8 *pDataER, u8 add, u8 nbyte)
{
    I2CStart();
    I2CSendByte(0xA0);
    I2CWaitAck();
    I2CSendByte(add);
    I2CWaitAck();
    I2CStart();
    I2CSendByte(0xA1);
    I2CWaitAck();
    while (nbyte--)
    {
        *pDataER++ = I2CReceiveByte();
        if (nbyte)
        {
            I2CSendAck(0);
        }
        else
        {
            I2CSendAck(1);
        }
    }
    I2CStop();
}
void E2_WR(u8 *pDataEW, u8 add, u8 nbyte)
{
    I2CStart();
    I2CSendByte(0xA0);
    I2CWaitAck();
    I2CSendByte(add);
    I2CWaitAck();
    while (nbyte--)
    {
        I2CSendByte(*pDataEW++);
        I2CWaitAck();
    }
    I2CStop();
}
unsigned char PCF8591_ADC(void)
{
    u8 tADC;
    I2CStart();
    I2CSendByte(0x90);
    I2CWaitAck();
    I2CSendByte(0x03);
    I2CWaitAck();
    I2CStart();
    I2CSendByte(0x91);
    tADC = I2CReceiveByte();
    I2CSendAck(1);
    I2CStop();
    return tADC;
}
// void PCF8591_DAC(u8 DACout)
// {
//     I2CStart();
//     I2CSendByte(0x90);
//     I2CWaitAck();
//     I2CSendByte(0x43);
//     I2CWaitAck();
//     I2CSendByte(DACout);
//     I2CWaitAck();
//     I2CStop();
// }