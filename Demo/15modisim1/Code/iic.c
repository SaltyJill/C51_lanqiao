#include "iic.h"

#define DELAY_TIME 5

/** ����I2C����ʱ���ߺ������� */


/**
* @brief I2C������һЩ��Ҫ����ʱ
*
* @param[in] i - ��ʱʱ�����.
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
* @brief ����I2C������������.
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
* @brief ����I2C����ֹͣ����
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
* @brief I2C����һ���ֽڵ�����
*
* @param[in] byt - �����͵��ֽ�
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
* @brief �ȴ�Ӧ��
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
* @brief ��д����������һЩ��Ҫ����ʱ
*
* @param[in] i - ָ����ʱʱ��
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
* @brief ��AT24C02(add)��д������val
*
* @param[in] add - AT24C02�洢��ַ
* @param[in] val - ��д��AT24C02��Ӧ��ַ������
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