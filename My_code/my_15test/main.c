#include "main.h"
#include "driver.h"
#include "iic.h"
#include "onewire.h"
#include "ds1302.h"
#include <stdio.h>
// LED 参数
u8 LED_OUT = 0;
u8 FLAG_LIGHT = 0;
u8 FLAG_L1 = 0,
   FLAG_L2 = 0;
// ISR 参数
u32 T1_1ms = 0;
// SEG 参数
u8 SEG_DP[12] = "123442";
u8 SEG_CD[8] = {0};
u8 SEG_PO = 0;
u8 FLAG_SEG = 0;
u8 page = 0,
   page_tpL = 0,
   page_PARA = 0,
   page_BACK = 0;
// WAV 参数
sbit waveP10 = P1 ^ 0;
sbit waveP11 = P1 ^ 1;
s8 L_ADJ = 0;
s16 L_PARA = 100;
s16 LVAL = 0;
s16 L_MAX = 0;
// TEP 参数
u16 TEP_VAL = 24;
// ADC 参数
u8 FLAG_8591 = 0;
u8 chnal = 0;
u16 ADC_V[3] = {0};
// TIM 参数
u8 TIM_NOW[3] = {0};
u8 TIM_TRIG[3] = {0};
u8 TIM_Set[3] = {12, 59, 45};
u8 FLAG_TIM = 0;
// URT 参数
u8 Error_D = 0;
// PWM 参数
sbit pwmP34 = P3 ^ 4;
u8 WAVE_DUTY = 0;
u8 FLAG_PWM = 0;
u8 FLAG_ULT = 0;
u32 PWM_STIME = 0;
// EErom
u8 EE_D[2] = {0};
void SEG_Fuc(void);
void KEY_Fuc(void);
void TIM_Fuc(void);
void TEP_Fuc(void);
void V8591_Fuc(void);
void WAV_Fuc(void);
void LED_Fuc(void);
void URT_Fuc(void);
void DEV_Fuc(void);

void main(void)
{
    DEV_cls();
    T1_int();
    T0_int();
    URT_int();
    DS1302_SET(TIM_Set);
    EE_RD(EE_D, 1, 2);
    if (EE_D[0] > 60 && EE_D[0] < 200)
    {
        L_PARA = EE_D[0];
    }
    else
    {
        L_PARA = 100;
    }
    if (EE_D[1] > -30 && EE_D[1] < 30)
    {
        L_ADJ = EE_D[1];
    }
    else
    {
        EE_D[1] = 0;
        L_ADJ = EE_D[1];
    }
    while (1)
    {
        KEY_Fuc();
        TIM_Fuc();
        TEP_Fuc();
        V8591_Fuc();
        WAV_Fuc();
        SEG_Fuc();
        LED_Fuc();
        URT_Fuc();
        DEV_Fuc();
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
            if (!page_tpL)
            {
                if (LVAL > 0)
                {
                    sprintf(SEG_DP, "F%3.1f`%3u", TEP_VAL / 10.0, (u16)LVAL);
                }
                else
                {
                    sprintf(SEG_DP, "F%3.1f`LL", TEP_VAL / 10.0);
                }
            }
            else
            {
                sprintf(SEG_DP, "F%3.2f%2.1f%2.1f", ADC_V[0] / 100.0, ADC_V[1] / 100.0, ADC_V[2] / 100.0);
            }
            break;
        case 1:
            if (!page_PARA)
            {
                sprintf(SEG_DP, "P1``%4d", (u16)L_PARA);
            }
            else
            {
                sprintf(SEG_DP, "P2``%4d", (s16)L_ADJ);
            }
            break;
        case 2:
            sprintf(SEG_DP, "%02u-%02u-%02u", (u16)TIM_NOW[0], (u16)TIM_NOW[1], (u16)TIM_NOW[2]);
            break;
        case 3:
            if (!page_BACK)
                sprintf(SEG_DP, "HF```%3u", (u16)L_MAX);
            else
                sprintf(SEG_DP, "HA%2u%2u%2u", (u16)TIM_TRIG[0], (u16)TIM_TRIG[1], (u16)TIM_TRIG[2]);
            break;
        }
        SEG_tran(SEG_DP, SEG_CD);
    }
}
void KEY_Fuc(void)
{
    static u8 key_past;
    static u32 key_4t = 0;
    u8 key_now;
    key_now = KEY_martix();
    if (key_now != key_past)
    {
        switch (key_now)
        {
        case 0:
            if (key_past == 4)
            {
                key_4t = T1_1ms;
            }
            break;
        case 4:
            if (T1_1ms - key_4t < 300 && (key_4t != 0))
            {
                page = (++page) & 0x03;
                if (page == 2)
                {
                    EE_D[0] = L_PARA;
                    EE_D[1] = L_ADJ;
                    EE_WR(EE_D, 1, 2);
                }
            }
            break;
        case 5:
            switch (page)
            {
            case 0:
                page_tpL = !page_tpL;
                break;
            case 1:
                page_PARA = !page_PARA;
                break;
            case 3:
                page_BACK = !page_BACK;
                break;
            default:
                break;
            }
            break;
        case 8:
            if (page == 1)
            {
                if (page_PARA)
                {
                    L_ADJ = L_ADJ + 10;
                }
                else
                {
                    L_PARA = L_PARA + 20;
                }
            }
            if (L_PARA > 200)
            {
                L_PARA = 200;
            }
            if (L_ADJ > 30)
            {
                L_ADJ = 0;
            }
            break;
        case 9:
            if (page == 1)
            {
                if (page_PARA)
                {
                    L_ADJ = L_ADJ - 10;
                }
                else
                {
                    L_PARA = L_PARA - 20;
                }
            }
            if (L_PARA < 60)
            {
                L_PARA = 60;
            }
            if (L_ADJ < -30)
            {
                L_ADJ = 0;
            }
            break;
        default:
            break;
        }
        key_past = key_now;
    }
}
void TIM_Fuc(void)
{
    if (FLAG_TIM)
    {
        FLAG_TIM = 0;
        DS1302_RED(TIM_NOW);
    }
}
void TEP_Fuc(void)
{
    static u32 tep_time;
    static u8 tep_step = 0;
    s16 iwd;
    switch (tep_step)
    {
    case 0:
        DS18b20_CV();
        tep_time = T1_1ms;
        tep_step = 1;
        break;
    case 1:
        if (T1_1ms - tep_time > 800)
        {
            iwd = DS18b20_RD();
            TEP_VAL = (iwd / 16.0 + 0.05) * 10;
            tep_step = 2;
            tep_time = T1_1ms;
        }
        break;
    case 2:
        if (T1_1ms - tep_time > 5)
        {
            tep_step = 0;
        }
        break;
    }
}
void V8591_Fuc(void)
{
    static u8 DAC_O;
    static u8 N = 0;
    float DACget;
    u8 ADCget;
    if (FLAG_8591)
    {
        FLAG_8591 = 0;
        if (N == 2)
        {
            chnal = 3;
        }
        else
        {
            chnal = N;
        }
        ADCget = PCF8591_ADC(chnal);
        ADC_V[N] = (ADCget * 5.0 / 255 + 0.005) * 100;
        N = (++N) % 3;
    }
    if (ADC_V[1] < 100)
    {
        FLAG_LIGHT = 1;
    }
    else
    {
        FLAG_LIGHT = 0;
    }
    // DAC out
    if (LVAL > 0)
    {
        if (LVAL <= 40)
        {
            DAC_O = (1 * 255 / 5);
        }
        else if (LVAL >= L_PARA)
        {
            DAC_O = (5 * 255 / 5);
        }
        else
        {
            DACget = 4.0 * LVAL / (L_PARA - 40) - 160.0 / (L_PARA - 40) + 1;
            DAC_O = DACget * 255.0 / 5;
        }
        PCF8591_DAC(DAC_O);
    }
}
void WAV_Fuc(void)
{
    u8 i = 4;
    u16 wave_T;
    if (FLAG_ULT)
    {
        FLAG_ULT = 0;
        TR0 = 0;
        TL0 = 0xF3;
        TH0 = 0xFF;
        TF0 = 0;
        TR0 = 1;
        while (i--)
        {
            while (!TF0)
                ;
            TF0 = 0;
            waveP10 ^= 1;
        }
        TR0 = 0;
        TL0 = 0x00;
        TH0 = 0x00;
        TF0 = 0;
        TR0 = 1;
        while (!TF0 && waveP11)
            ;
        TR0 = 0;
        if (TF0)
        {
            LVAL = 255 + L_ADJ;
            TF0 = 0;
        }
        else
        {
            wave_T = (u16)TH0;
            wave_T = (u16)(wave_T << 8) + TL0;
            LVAL = wave_T * 0.017 + L_ADJ;
        }
        if (LVAL > 255)
        {
            LVAL = 255;
        }
        if (LVAL < 0)
        {
            LVAL = -10;
        }
        if (LVAL > L_MAX)
        {
            L_MAX = LVAL;
            TIM_TRIG[0] = TIM_NOW[0];
            TIM_TRIG[1] = TIM_NOW[1];
            TIM_TRIG[2] = TIM_NOW[2];
        }
    }
}
void LED_Fuc(void)
{
    static u8 led_past;
    static u32 LED_t = 0;
    // L8
    if (FLAG_LIGHT)
    {
        LED_OUT |= (u8)(1 << 7);
    }
    else
    {
        LED_OUT &= (u8) ~(1 << 7);
    }
    // Flash
    if ((T1_1ms - LED_t) > 200)
    {
        LED_t = T1_1ms;
        FLAG_L1 = !FLAG_L1;
        FLAG_L2 = !FLAG_L2;
    }
    // L1
    if (page)
    {
        LED_OUT &= (u8) ~(1);
    }
    else
    {
        if (FLAG_L1)
            LED_OUT |= (u8)1;
        else
            LED_OUT &= (u8) ~(1);
    }
    // L2
    if (LVAL < 0)
    {
        LED_OUT |= (u8)(1 << 1);
    }
    else
    {
        if (LVAL <= L_PARA)
        {
            LED_OUT &= (u8) ~(1 << 1);
        }
        else
        {
            if (FLAG_L2)
                LED_OUT |= (u8)(1 << 1);
            else
                LED_OUT &= (u8) ~(1 << 1);
        }
    }

    if (LED_OUT != led_past)
    {
        led_past = LED_OUT;
        LED_disp(LED_OUT);
    }
}

void DEV_Fuc(void)
{
    if (FLAG_LIGHT)
    {
        if (LVAL > L_PARA)
        {
            DEV_out(1, 1);
        }
        else
        {
            DEV_out(1, 0);
        }
        if (LVAL < 0)
        {
            DEV_out(0, 1);
        }
        else
        {
            DEV_out(0, 0);
        }
    }
}
void URT_Fuc(void)
{
    // message
    if (Error_D)
    {
        switch (Error_D)
        {
        case 1:
            URT_send("Error_D");
            break;
        case 2:
            URT_send("Error_L");
            break;
        case 3:
            URT_send("OK");
            break;
        }
        Error_D = 0;
    }
}
void Uart_ISR(void) interrupt 4
{
    static u8 urat[16] = {0};
    static u8 poc = 0;
    int sc;
    u8 r, n;
    s16 urdate1, urdate2;
    if (RI)
    {
        RI = 0;
        urat[poc++] = SBUF;
        urat[poc] = 0;
        if (poc == 7)
        {
            sc = sscanf(urat, "PWM%2d%c%c", &urdate1, &r, &n);
            if (sc == 3)
            {
                if (r == '\r' && n == '\n' && urdate1 > 0 && urdate1 % 10 == 0)
                {
                    FLAG_PWM = 1;
                    PWM_STIME = T1_1ms;
                    WAVE_DUTY = urdate1 / 10;
                    poc = 0;
                    Error_D = 3;
                }
                else
                {
                    Error_D = 1;
                    poc = 0;
                }
            }
        }
        else if (poc == 15)
        {
            sc = sscanf(urat, "P1:%3d,P2:%3d%c%c", &urdate1, &urdate2, &r, &n);
            if (sc == 4)
            {
                if (r == '\r' && n == '\n' && urdate1 >= 60 && urdate1 <= 200 && urdate2 <= 30 && urdate2 >= -30)
                {
                    L_PARA = urdate1;
                    L_ADJ = (s8)urdate2;
                    poc = 0;
                    Error_D = 3;
                }
                else
                {
                    Error_D = 1;
                    poc = 0;
                }
            }
        }
        else if (poc > 15)
        {
            Error_D = 2;
            poc = 0;
        }
    }
}
void T1_ISR(void) interrupt 3
{
    static u8 PWM_cnt = 0;
    static u32 T1_100ms = 0;
    T1_1ms++;
    if (++T1_100ms == 100)
    {
        T1_100ms = 0;
        FLAG_SEG = 1;
        FLAG_8591 = 1;
        FLAG_TIM = 1;
        FLAG_ULT = 1;
    }
    if (FLAG_PWM)
    {
        if (T1_1ms - PWM_STIME < 5000)
        {
            if (++PWM_cnt == 10)
            {
                PWM_cnt = 0;
                pwmP34 = 0;
            }
            else if (PWM_cnt == WAVE_DUTY)
            {
                pwmP34 = 1;
            }
        }
        else
        {
            FLAG_PWM = 0;
        }
    }
    SEG_disp(SEG_CD, SEG_PO);
    SEG_PO = (++SEG_PO) & 0x07;
}
