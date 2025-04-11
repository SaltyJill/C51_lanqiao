#include "frq.h"

//
void InitFrqTimer(void)
{
	AUXR &= 0x7F;

	TMOD |= 0x04;
	TH0 = 0;
	TL0 = 0;
	TR0 = 1;
	TF0 = 0;
}

//
unsigned int CalFrq(void)
{
	unsigned int frq = 0;
	
	TR0 = 0;	//暂停计数器
	frq = ((TH0 << 8) | TL0);

	TH0 = 0;
	TL0 = 0;
	TR0 = 1;

	return frq;
}

