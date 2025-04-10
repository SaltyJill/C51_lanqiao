#include "sys.h"

/*>
  */
void Timer0Init(void)		
{
	AUXR |= 0x80;
	TMOD &= 0xF0;
	TL0 = 0x40;	
	TH0 = 0xA2;	
	TF0 = 0;
	TR0 = 1;

    ET0 = 1;
}

/*>
  */
void BoardInit(void)
{
	latch(LED, 0xFF);
	latch(BUZ, 0x00);
	Timer0Init();
	
	InitRTC();	
	EA = 1;
}