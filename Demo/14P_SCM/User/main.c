#include "main.h"

//���������
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