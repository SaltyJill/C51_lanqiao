#include "main.h"

unsigned char ms[2] = {0, 0};
void uiLED(void);

/**
 * @brief IAP15单片机定时器0初始化
 * @param None.
 * @return None.
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

/**
 * @brief Main
 * @return None.
 */
void main(void)
{
	
	latch(LED, 0xFF);
	latch(BUZ, 0x00);	
	Timer0Init();
	InitRTC();	
	EA = 1;		

    while(1){
		if(ms[0] == 20){
			ms[0] = 0;
			ScanKeyboard();
		}
		if(ms[1] == 100){
			ms[1] = 0;
			ReadRTC();
			LedProc();
		}
		KeyProc();
		SegProc();	
	}
}

/**
 * @brief 定时器0中断服务程序
 * @param None.
 * @return None.
*/
void isr_timer_0(void)  interrupt 1
{
	if(ms[0] < 20){	
		++ms[0];
	}
	if(ms[1] < 100){
		++ms[1];
	}	
    SegDisplay();	
}