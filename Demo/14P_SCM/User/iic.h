#ifndef __IIC_H
#define __IIC_H

#include "stc15f2k60s2.h"
#include "intrins.h"

sbit scl = P2^0;
sbit sda = P2^1;

typedef enum{
    ACK = 0,
    NOACK
}ACK_Signal;

void I2CStop();
void I2CStart();
void I2CSendByte(unsigned char byt);
void I2CSendAck(unsigned char ackbit);
unsigned char I2CWaitAck(void);
unsigned char I2CReceiveByte(void);

#endif