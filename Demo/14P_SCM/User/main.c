#include "main.h"

//主程序入口
void main(void)
{
    initSysTick();
	initBsp();
	
    while(1){
		taskCollect();
		taskDisplay();
		taskKey();
		taskControl();
    }
}