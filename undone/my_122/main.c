#include "main.h"
#include "driver.h"
#include "clock.h"
#include "I2C.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

u8 led_out = 0;

u8 segDisp[10] = "12345678";
u8 segCode[8] = {0};
u8 segAdds = 0;

volatile u32 ISR_1Ms = 0;
u32 ISR_10Ms = 0;
u32 ISR_100Ms = 0;
u32 ISR_1S = 0;

void main(void)
{
    outside_Cls();
    T1_clock();
    T0_count();
    while (1)
    {
    }
}

void T1_ISR() interrupt 3
{
    ISR_1Ms++;
    if (ISR_1Ms % 10 == 0)
    {
        ISR_10Ms++;
    }
    if (ISR_1Ms % 100 == 0)
    {
        ISR_100Ms++;
    }
    if (ISR_1Ms % 1000 == 0)
    {
        ISR_1S++;
    }
    segDisp(segCode, segAdds);
    if (segAdds == 8)
    {
        segAdds = 0;
    }
}
void led_fuc()
{
}
void key_fuc(){
    
}
void seg_fuc()
{
}
void adc_fuc(){

}