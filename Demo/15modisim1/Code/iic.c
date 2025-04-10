#include "iic.h"

#define DELAY_TIME 5

/** 定义I2C总线时钟线和数据线 */


/**
* @brief I2C总线中一些必要的延时
*
* @param[in] i - 延时时间调整.
* @return none
*/
void i2c_delay(unsigned char i)
{
    do
    {
        _nop_();
    }
    while(i--);        
}

/**
* @brief 产生I2C总线启动条件.
*
* @param[in] none
* @param[out] none
* @return none
*/
void i2c_start(void)
{
    sda = 1;
    scl = 1;
    i2c_delay(DELAY_TIME);
    sda = 0;
    i2c_delay(DELAY_TIME);
    scl = 0;    
}

/**
* @brief 产生I2C总线停止条件
*
* @param[in] none
* @param[out] none.
* @return none
*/
void i2c_stop(void)
{
    sda = 0;
    scl = 1;
    i2c_delay(DELAY_TIME);
    sda = 1;
    i2c_delay(DELAY_TIME);       
}

/**
* @brief I2C发送一个字节的数据
*
* @param[in] byt - 待发送的字节
* @return none
*/
void i2c_sendbyte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
        i2c_delay(DELAY_TIME);
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
        i2c_delay(DELAY_TIME);
        scl = 1;
        byt <<= 1;
        i2c_delay(DELAY_TIME);
    }
	
    scl = 0;  
}

/**
* @brief 等待应答
*
* @param[in] none
* @param[out] none
* @return none
*/
unsigned char i2c_waitack(void)
{
	unsigned char ackbit;
	
    scl = 1;
    i2c_delay(DELAY_TIME);
    ackbit = sda; //while(sda);  //wait ack
    scl = 0;
    i2c_delay(DELAY_TIME);
	
	return ackbit;
}

/**
* @brief 读写操作过程中一些必要的延时
*
* @param[in] i - 指定延时时间
* @return - none
*/
void operate_delay(unsigned char t)
{
	unsigned char i;
	
	while(t--){
		for(i=0; i<112; i++);
	}
}

/**
* @brief 向AT24C02(add)中写入数据val
*
* @param[in] add - AT24C02存储地址
* @param[in] val - 待写入AT24C02相应地址的数据
* @return - none
*/
void write_eeprom(unsigned char add,unsigned char val)
{
    i2c_start();
    i2c_sendbyte(0xa0);
    i2c_waitack();
    i2c_sendbyte(add);
    i2c_waitack();
    i2c_sendbyte(val);
    i2c_waitack();
    i2c_stop();
	operate_delay(10);
}