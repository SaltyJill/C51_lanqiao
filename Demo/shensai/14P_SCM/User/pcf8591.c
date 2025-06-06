#include "pcf8591.h"

static void Delay10Ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}

//
void InitPCF8591(void)
{
	I2CStart();
	I2CSendByte(0x90);
    I2CWaitAck();
	I2CSendByte(0x01);  //
	I2CWaitAck();
	I2CStop();
	Delay10Ms();
}

//
unsigned char GetADC(unsigned char channel)
{
	unsigned char temp;
	I2CStart();
	I2CSendByte(0x90);
    I2CWaitAck();
	I2CSendByte(0x40 | channel);  //
	I2CWaitAck();
//	Delay10Ms();
    
	I2CStart();
	I2CSendByte(0x91);
	I2CWaitAck();
	temp = I2CReceiveByte();
	I2CSendAck(1);
	I2CStop();
	
	return temp;
}

