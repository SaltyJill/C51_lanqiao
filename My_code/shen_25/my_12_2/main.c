#include "main.h"
#include "driver.h"
#include "iic.h"
#include <stdio.h>
/*LED 参数*/
u8 LED_DP = 0;
u8 FLAG_LED = 1;
/*ISR 参数*/
u32 T1_1MS = 0;
/*SEG 参数*/
u8 SEG_DP[10] = "1234",
   SEG_CD[8] = {0},
   SEG_Pos = 0;
u8 FLAG_SEG = 0;
u8 page = 0;
/*Freq 参数*/
u16 FREQ = 0, FREQ_SAVE = 0;
/*ADC 参数*/
u8 FLAG_ADC = 0;
u8 chnal = 1;
u16 LIGHT = 0;         // x100
u16 RB2 = 0, RB2_SAVE; // x100

void SEG_Fuc(void);
void KEY_Fuc(void);
void ADC_Fuc(void);
void LED_Fuc(void);

void main(void)
{
    DEV_Cls();
    T1_Int();
    T0_Int();
    while (1)
    {
        KEY_Fuc();
        ADC_Fuc();
        SEG_Fuc();
        LED_Fuc();
    }
}
void SEG_Fuc(void)
{
    if (FLAG_SEG)
    {
        FLAG_SEG = 0;
        switch (page)
        {
        case 0:
            sprintf(SEG_DP, "F%7u", FREQ);
            break;
        case 1:
            sprintf(SEG_DP, "N%7u", 1000000 / FREQ);
            break;
        case 2:
            if (chnal == 1)
            {
                sprintf(SEG_DP, "U-%1d``%3.2f", chnal, LIGHT / 100.0);
            }
            else if (chnal == 3)
            {
                sprintf(SEG_DP, "U-%1d``%3.2f", chnal, RB2 / 100.0);
            }
            break;
        }
        SEG_Tran(SEG_DP, SEG_CD);
    }
}
void KEY_Fuc(void)
{
    static u32 keyT = 0;
    static u8 keyPast = 0;
    static u8 flagKey7 = 0;
    u8 keyNow = 0;
    keyNow = KEY_Independ();
    if (keyPast != keyNow)
    {
        keyT = T1_1MS;
        switch (keyNow)
        {
        case 0:
            if (flagKey7)
            {
                FLAG_LED = !FLAG_LED;
            }
            break;
        case 4:
            page = (++page) % 3;
            break;
        case 5:
            chnal = (chnal + 2) & 0x03;
            break;
        case 6:
            RB2_SAVE = RB2;
            break;
        case 7:
            FREQ_SAVE = FREQ;
            break;
        }
        keyPast = keyNow;
    }
    if (T1_1MS - keyT >= 1000)
    {
        if (keyNow == 7)
        {
            flagKey7 = 1;
        }
    }
}
void ADC_Fuc(void)
{
    u8 ADCget;
    float fADC = 0;
    if (FLAG_ADC)
    {
        FLAG_ADC = 0;
        ADCget = PCF8591_ADC(chnal);
        if (chnal % 3)
        {
            LIGHT = (ADCget * 5.0 / 255 + 0.005) * 100;
        }
        else
        {
            RB2 = (ADCget * 5.0 / 255 + 0.005) * 100;
        }
    }
}
void LED_Fuc(void)
{
    static u8 led_past = 0;
    if (FLAG_LED)
    {
        // L3,L4,L5
        LED_DP |= (1 << 2 + page);
        LED_DP &= (~(1 << 2 + page)) | 0x03;
        // L1
        if (RB2 > RB2_SAVE)
        {
            LED_DP |= (u8)1;
        }
        else
        {
            LED_DP &= (u8)~1;
        }
        // L2
        if (FREQ > FREQ_SAVE)
        {
            LED_DP |= (u8)(1 << 1);
        }
        else
        {
            LED_DP &= (u8) ~(1 << 1);
        }
        if (led_past != LED_DP)
        {
            led_past = LED_DP;
            LED_Disp(LED_DP);
        }
    }
}

void T1_ISR(void) interrupt 3
{
    static T1_100MS = 0, T1_1S = 0;
    T1_1MS++;
    if (++T1_100MS == 10)
    {
        T1_100MS = 0;
        FLAG_SEG = 1;
        FLAG_ADC = 1;
    }
    if (++T1_1S == 1000)
    {
        T1_1S = 0;
        FREQ = (u16)(TH0 << 8) + TL0;
        TR0 = 0;
        TH0 = 0x00;
        TL0 = 0x00;
        TR0 = 1;
    }
    SEG_Disp(SEG_CD, SEG_Pos);
    SEG_Pos = (++SEG_Pos) & 0x07;
}