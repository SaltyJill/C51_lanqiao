#include "main.h"
#include "driver.h"
#include "iic.h"
#include <stdio.h>
/*定时器参数*/
u32 ISR_1MS;
/*数码管参数*/
u8 seg_DISP[10] = "12345";
u8 seg_CODE[8] = {0};
u8 seg_POSI = 0;
u8 flag_SEGM = 0;
u8 page = 0;
u8 begin_nEND = 0;
/*ADC参数*/
u8 RH_para = 50;
u8 RH = 0;
u32 sec_1S = 3;
/*LED参数*/
u8 led_DISP = 0;
u8 flag_ADC = 0;
/*继电器参数*/
u8 flag_RELAY = 0;

void LED_FUC(void);
void SEG_FUC(void);
void KEYB_FUC(void);
void ADC_FUC(void);
void RELAY_FUC(void);

void main(void)
{
    Other_Cls();
    T1_int();
    while (1)
    {
        KEYB_FUC();
        ADC_FUC();
        LED_FUC();
        SEG_FUC();
        RELAY_FUC();
    }
}
void LED_FUC(void)
{
    static u8 led_past;
    switch (page)
    {
    case 0:
        led_DISP |= 1;
        led_DISP &= 0xF9;
        break;
    case 1:
        led_DISP |= (1 << 1);
        led_DISP &= 0xFA;
        break;
    case 2:
        led_DISP |= (1 << 2);
        led_DISP &= 0xFC;
    default:
        break;
    }
    if (begin_nEND)
    {
        led_DISP |= (1 << 7);
    }
    else
    {
        led_DISP &= ~(1 << 7);
    }
    if (led_past != led_DISP)
    {
        led_past = led_DISP;
        Led_Out(led_DISP);
    }
}
void SEG_FUC(void)
{
    if (flag_SEGM)
    {
        flag_SEGM = 0;
        switch (page)
        {
        case 0:
            sprintf(seg_DISP, "C`````%02u", (u16)RH);
            break;
        case 1:
            sprintf(seg_DISP, "E`````%02u", (u16)RH_para);
            break;
        case 2:
            sprintf(seg_DISP, "H`````%2u", (u16)sec_1S);
            break;
        }
    }
    Seg_TR(seg_DISP, seg_CODE);
}
void KEYB_FUC(void)
{
    static u8 KEY_PAST;
    u8 KEY_NOW;
    KEY_NOW = Key_MARTIX();
    if (KEY_PAST != KEY_NOW)
    {
        switch (KEY_NOW)
        {
        case 4:
            page = (++page) % 3;
            break;
        case 5:
            begin_nEND = ~begin_nEND;
            break;
        case 8:
            if (page == 1)
            {
                RH_para -= 5;
            }
            else if (page == 2)
            {
                if (--sec_1S < 1)
                {
                    sec_1S = 10;
                }
            }
            if (RH_para < 30)
            {
                RH_para = 90;
            }
            break;
        case 9:
            if (page == 1)
            {
                RH_para += 5;
            }
            else if (page == 2)
            {
                if (++sec_1S > 10)
                {
                    sec_1S = 1;
                }
            }
            if (RH_para > 90)
            {
                RH_para = 30;
            }
            break;
        }
        KEY_PAST = KEY_NOW;
    }
}
void ADC_FUC(void)
{
    u8 ucADC;
    static u8 rh_past;
    if (flag_ADC)
    {
        flag_ADC = 0;
        ucADC = PCF8591_ADC();
        RH = (80 / 153.0) * ucADC - (50 / 3.0);
        if (RH < 10)
        {
            RH = 10;
        }
        else if (RH > 90)
        {
            RH = 90;
        }
        if (rh_past > RH_para && RH < RH_para)
        {
            flag_RELAY = 1;
        }
        rh_past = RH;
    }
}
void RELAY_FUC(void)
{
    static u32 relay_T;
    if (begin_nEND && flag_RELAY)
    {
        Relay(1);
        relay_T = ISR_1MS;
        flag_RELAY = 0;
    }
    else if (ISR_1MS - relay_T > (sec_1S * 1000))
    {
        Relay(0);
    }
}
void T1_ISR(void) interrupt 3
{
    static u32 ISR_10MS = 0, ISR_100MS = 0;
    ISR_1MS++;
    if (++ISR_10MS == 10)
    {
        ISR_10MS = 0;
        flag_SEGM = 1;
    }
    if (++ISR_100MS == 100)
    {
        ISR_100MS = 0;
        flag_ADC = 1;
    }
    Seg_DP(seg_CODE, seg_POSI);
    if (++seg_POSI == 8)
    {
        seg_POSI = 0;
    }
}