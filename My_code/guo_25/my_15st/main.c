#include "main.h"
#include "iic.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>
//ISR
u32 T1_1MS=0;



void main(void)
{
    while (1)
    {
        ;
    }
    
}

void Uart_ISR(void) interrupted 4
{
    
}

void T1_ISR(void) interrupted 3
{
    u32 T1_100MS=0;
    T1_1MS++;
    if (++T1_100MS==100)
    {
        T1_100MS=0;
    }

} 