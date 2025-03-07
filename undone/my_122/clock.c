#include "clock.h"
// 12T 1ms
void T1_clock(void)
{
    TH1 = 0xFC;
    TL1 = 0x18;
    TMOD &= 0x0F;
    AUXR &= ~(1 << 6);
    TF1 = 0;
    EA = 1 ;
	ET1 = 1;
    TR1 = 1;
}
void T0_count(void)
{
    TH0 = 0x00;
    TL0 = 0x00;
    TMOD = (TMOD & 0xF0) | 0x04;
    AUXR &= ~(1 << 7);
    TF0 = 0;
    ET0 = 0;
    TR0 = 1;
}