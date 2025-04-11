#include "iic.h"

//
void I2CStart(void)
{
    sda = 1;
    scl = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    sda = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    scl = 0;    
}

//
void I2CStop(void)
{
    sda = 0;
    scl = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    sda = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

//
void I2CSendByte(unsigned char byt)
{
    unsigned char i;
	
    for(i=0; i<8; i++){
        scl = 0;
		_nop_();
	    _nop_();
	    _nop_();
	    _nop_();
    	_nop_();
        _nop_();
        if(byt & 0x80){
            sda = 1;
        }
        else{
            sda = 0;
        }
        _nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
        scl = 1;
        byt <<= 1;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
        _nop_();
    }
	
    scl = 0;  
}

//
unsigned char I2CWaitAck(void)
{
	unsigned char ackbit;
	
    scl = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    ackbit = sda; 
    scl = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
	
	return ackbit;
}

//
unsigned char I2CReceiveByte(void)
{
	unsigned char da;
	unsigned char i;
	for(i=0;i<8;i++){   
		scl = 1;
		_nop_();
	    _nop_();
		_nop_();
		_nop_();
		_nop_();
        _nop_();
		da <<= 1;
		if(sda) 
			da |= 0x01;
		scl = 0;
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
        _nop_();
	}
	return da;    
}

//
void I2CSendAck(unsigned char ackbit)
{
    scl = 0;
    sda = ackbit; 
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    scl = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    scl = 0; 
	sda = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}
