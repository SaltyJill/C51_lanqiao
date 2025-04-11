#include "dsp.h"

DisplayStruct dsp;

								/* 0,1,2,3,4,5,6,7,8,9,熄灭,-,C,H,F,P,E,A */                                                                              
code unsigned char segCode[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,0xc6,0x89,0x8e,0x8c,0x86,0x88};

//
DisplayStruct* GetDisplayStruct(void)
{
    return &dsp;
}

//
void SegDisplay(void)
{
    SEG(0xFF);
    COM(1 << dsp.com);

    if(dsp.com != dsp.dot){
        SEG(segCode[dsp.buf[dsp.com]]);
    }else{
        SEG(segCode[dsp.buf[dsp.com]] & 0x7F);
    }

    if(++dsp.com >= 8){
        dsp.com = 0;
    }
}

//
void InitDsp(void)
{
    dsp.buf[0] = 10;
    dsp.buf[1] = 10;
    dsp.buf[2] = 10;
    dsp.buf[3] = 10;
    dsp.buf[4] = 10;
    dsp.buf[5] = 10;
    dsp.buf[6] = 10;
    dsp.buf[7] = 10;
    dsp.dot = 255;
	dsp.SegDisplay = SegDisplay;
}