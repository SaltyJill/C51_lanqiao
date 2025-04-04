#include "main.h"
#include "iic.h"
#include "driver.h"
#include <stdio.h>
/*LED参数*/
u8 led_DISP = 0;
u8 flag_L1 = 0;
/*定时器参数*/
u32 ISR_T1 = 0;
/*ADC参数*/
u8 flag_ADC = 0;
u16 voltage = 0;   // 100倍扩大
u16 volt_para = 0; // 10倍扩大
/*数码管参数*/
u8 flag_SEG = 0;
u8 seg_DP[12] = "213332";
u8 seg_CD[8] = {0};
u8 segPS = 0;
u8 page = 0;
u8 breakIn_count = 0;
/*键盘参数*/
u8 key_noUse = 0;
/*EErom参数*/
u16 EE_date = 0;
void led_Fuc(void);
void seg_Fuc(void);
void adc_Fuc(void);
void key_Fuc(void);

void main(void)
{
    dev_Cls();
    T1_int();
    E2_RD(&EE_date, 0, 1);
    if (EE_date > 50 || (EE_date % 5) != 0)
    {
        EE_date = 25;
    }
    volt_para = EE_date;
    while (1)
    {
        key_Fuc();
        adc_Fuc();
        seg_Fuc();
        led_Fuc();
    }
}
void led_Fuc(void)
{
    static u32 low_time = 0;
    static u8 led_past;
    // L1
    if (flag_L1)
    {
        low_time = ISR_T1;
        flag_L1 = 0;
    }
    else if ((voltage <= volt_para*10) && (ISR_T1 - low_time > 5000))
    {
        led_DISP |= 1;
    }
    else
    {
        led_DISP &= ~1;
    }
    // L2
    if (breakIn_count % 2)
    {
        led_DISP |= (1 << 1);
    }
    else
    {
        led_DISP &= ~(1 << 1);
    }
    // L3
    if (key_noUse >= 3)
    {
        led_DISP |= (1 << 2);
    }
    else
    {
        led_DISP &= ~(1 << 2);
    }
    if (led_DISP != led_past)
    {
        led_past = led_DISP;
        led_Disp(led_DISP);
    }
}
void seg_Fuc(void)
{
    if (flag_SEG)
    {
        flag_SEG = 0;
        switch (page)
        {
        case 0:
            sprintf(seg_DP, "U````%3.2f", voltage / 100.0);
            break;
        case 1:
            sprintf(seg_DP, "P````%3.2f", volt_para / 10.0);
            break;
        case 2:
            sprintf(seg_DP, "N`````%2u", (u16)breakIn_count);
            break;
        }
    }
    seg_Tran(seg_DP, seg_CD);
}
void adc_Fuc(void)
{
    float fADC;
    static u16 volt_past;
    u8 ADCget;
    if (flag_ADC)
    {
        flag_ADC = 0;
        ADCget = PCF8591_ADC();
        fADC = ADCget * 5.0 / 255;
        voltage = (fADC + 0.005) * 100; // 手动四舍五入
        if ((volt_past >= volt_para * 10) && (voltage < volt_para * 10))
        {
            breakIn_count++;
            flag_L1 = 1;
        }
        volt_past = voltage;
    }
}
void key_Fuc(void)
{
    static u8 key_past = 0;
    u8 key_RD;
    key_RD = key_Martix();
    if (key_RD != key_past)
    {
        key_past = key_RD;
        switch (key_RD)
        {
        case 0:
            break;
        case 12:
            page = (++page) % 3;
            key_noUse = 0;
            if (page == 2)
            {
                E2_WR(&volt_para, 0, 1);
            }
            break;
        case 13:
            if (page == 2)
            {
                breakIn_count = 0;
                key_noUse = 0;
            }
            else
            {
                key_noUse++;
            }
            break;
        case 16:
            if (page == 1)
            {
                key_noUse = 0;
                if (volt_para > 45)
                {
                    volt_para = 0;
                }
                else
                {
                    volt_para += 5;
                }
            }
            else
            {
                key_noUse++;
            }
            break;
        case 17:
            if (page == 1)
            {
                key_noUse = 0;
                if (volt_para == 0)
                {
                    volt_para = 50;
                }
                else
                {
                    volt_para -= 5;
                }
            }
            else
            {
                key_noUse++;
            }
            break;
        default:
            break;
        }
    }
}
void T1_ISR(void) interrupt 3
{
    static u32 ISR_10MS = 0, ISR_100MS = 0;
    ISR_T1++;
    if (++ISR_10MS == 10)
    {
        ISR_10MS = 0;
        flag_SEG = 1;
    }
    if (++ISR_100MS == 100)
    {
        ISR_100MS = 0;
        flag_ADC = 1;
    }
    seg_Disp(seg_CD, segPS);
    if (++segPS == 8)
    {
        segPS = 0;
    }
}