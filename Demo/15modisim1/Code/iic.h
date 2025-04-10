/** 
* @file         i2c.h 
* @brief        51��Ƭ��ͨ��I0ģ��I2C����
* @author       GhpZhu 
* @date     2012/8/21 
* @version  A001 
* @par Copyright (c):  
*       All User 
* @par History:          
*   version: author, date, desc\n 
*/

#ifndef __IIC_H
#define __IIC_H

#include "main.h"

sbit scl = P2^0;
sbit sda = P2^1;

void i2c_delay(unsigned char i);
void i2c_start(void);
void i2c_stop(void);

void i2c_sendbyte(unsigned char byt);
unsigned char i2c_waitack(void);
unsigned char i2c_receivebyte(void);
void i2c_sendack(unsigned char ackbit);

void write_eeprom(unsigned char add,unsigned char val);
unsigned char read_eeprom(unsigned char add);

void init_pcf8591(void);
unsigned char adc_pcf8591(void);
void dac_pcf8591(unsigned char dat);

#endif 