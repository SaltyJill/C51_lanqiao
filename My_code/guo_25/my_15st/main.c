#include "main.h"
#include "iic.h"
#include "driver.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// ISR
u32 T1_1MS = 0;
// TIM
u8 SEG_DP[10] = "213";
u8 SEG_CD[8] = {0};
u8 SEG_PS = 0;
u8 FLAG_SEG = 0;
u8 page = 0;
u8 page_PARA = 0;

// UART
u8 FLAG_URAT = 0;
u8 strpos = 0;
u8 strRECV[10] = {0};
u8 QUADtime = 0;
u8 FALG_TG = 0; // TG_POSI arrived flag
u8 FLAG_RC = 0; // TG_POSI received flag
// POSI
u16 POSI[2] = {0};
u16 POSI_TG[2] = {0};

// ACTION
u8 FALG_BLOCK = 0; // 0 clear roadblock 1 not yet
u8 FLAG_ACT = 0;
u8 STATUS = 0;  // 0 rest, 1 run, 2 wait
u8 PARA_R = 10; // 10* 1.0~2.0
s8 PARA_B = 0;  //-90~90cm/s
u16 SPEED = 0;  // 10*v
u16 DISTENT = 0;
// ADC FREQ
u8 FLAG_ADC = 0;
u8 LIGHT_V = 1;
u16 FREQ = 0;
// WAVE
u8 FLAG_WAVE = 0;

void UART_Fuc(void);
void ADC_Fuc(void);
void ACT_Fuc(void);
void WAVE_Fuc(void);
void LED_Fuc(void);
void DEV_Fuc(void);
void KEY_Fuc(void);
void SEG_Fuc(void);

void main(void)
{
    DEV_Cls();
    T1_Int();
    T0_Int();
    Uart_Int();
    while (1)
    {
        UART_Fuc();
        ADC_Fuc();
        DEV_Fuc();
        WAVE_Fuc();
        KEY_Fuc();
        ACT_Fuc();
        LED_Fuc();
        SEG_Fuc();
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
            if (!STATUS)
            {
                sprintf(SEG_DP, "L%3u-%3u", POSI[0], POSI[1]);
            }
            else
            {
                sprintf(SEG_DP, "L%3u-%3u", POSI_TG[0], POSI_TG[1]);
            }
            break;
        case 1:
            switch (STATUS)
            {
            case 0:
                sprintf(SEG_DP, "E2`-----");
                break;
            case 1:
                sprintf(SEG_DP, "E1`%5.1f", SPEED / 10.0);
                break;
            case 2:
                sprintf(SEG_DP, "E3`%5u", DISTENT);
                break;
            }
            break;
        case 2:
            sprintf(SEG_DP, "P`%2.1f`%3d", PARA_R / 10.0, PARA_B);
            break;
        default:
            break;
        }
    }
}

void KEY_Fuc(void)
{
    static u8 key_past;
    u8 key_now;
    key_now = KEY_Matrix();
    if (key_now != key_past)
    {
        switch (key_now)
        {
        case 4:
            if ((FLAG_RC && !STATUS) || (!FALG_BLOCK && STATUS == 2))
            {
                STATUS = 1;
            }
            else if (STATUS == 1)
            {
                STATUS = 2;
            }
            break;
        case 5:
            if (!STATUS)
            {
                POSI[0] = 0;
                POSI[1] = 0;
            }
            break;
        case 8:
            if (page == 1)
            {
                page_PARA = 0;
            }
            page = (page++) % 3;
            break;
        case 9:
            page_PARA = !page_PARA;
            break;
        case 12:
            if (page_PARA)
            {
                PARA_B = (PARA_B > 85) ? 90 : PARA_B + 5;
            }
            else
            {
                PARA_R = (PARA_R > 19) ? 20 : PARA_R + 1;
            }
            break;
        case 13:
            if (page_PARA)
            {
                PARA_B = (PARA_B < -85) ? -90 : PARA_B - 5;
            }
            else
            {
                PARA_R = (PARA_R < 11) ? 10 : PARA_R - 1;
            }
            break;
        }
        key_past = key_now;
    }
}

void DEV_Fuc(void)
{
    u8 relay;
    relay = (STATUS == 1) ? 1 : 0;
    Relay(relay);
}

void LED_Fuc(void)
{
    static u8 led_past = 0;
    static u8 flag_L1 = 0;
    static u8 cnt_L3 = 0;
    static u32 time_L = 0;
    u8 led_now = 0;
    if (T1_1MS - time_L > 100)
    {
        time_L = T1_1MS;
        flag_L1 = !flag_L1;
        if (FALG_TG)
        {
            cnt_L3++;
        }
    }
    // L1 L2
    switch (STATUS)
    {
    case 0:
        led_now &= (u8) ~(1);
        led_now &= (u8) ~(1 << 1); // L2
        break;
    case 1:
        led_now |= (u8)(1);
        if (LIGHT_V) // L2
        {
            led_now &= (u8) ~(1 << 1);
        }
        else
        {
            led_now |= (u8)(1 << 1);
        }
        break;
    case 2:
        if (flag_L1)
        {
            led_now |= (u8)(1);
        }
        else
        {
            led_now &= (u8) ~(1);
        }
        led_now &= (u8) ~(1 << 1); // L2
        break;
    }
    // L3
    if (FALG_TG)
    {
        if (cnt_L3 >= 300)
        {
            cnt_L3 = 0;
            FALG_TG = 0;
        }
        led_now |= (u8)(1 << 2);
    }
    else
    {
        led_now &= (u8) ~(1 << 2);
    }
    if (led_now != led_past)
    {
        led_past = led_now;
        LED_Disp(led_now);
    }
}

void WAVE_Fuc(void)
{
    u8 n = 4;
    u16 temp_w = 0;
    sbit wave_P10 = P1 ^ 0;
    sbit wave_P11 = P1 ^ 1;
    if (FLAG_WAVE)
    {
        FLAG_WAVE = 0;
        CR = 0;
        CH = 0xF4;
        CL = 0xFF;
        CF = 0;
        CR = 1;
        while (n--)
        {
            while (!CF)
            {
                ;
            }
            CF = 0;
            wave_P10 ^= 1;
        }
        CR = 0;
        CH = 0x00;
        CL = 0x00;
        CF = 0;
        CR = 1;
        while (!CF0 && wave_P11)
        {
            ;
        }
        CR = 0;
        if (CF)
        {
            CF = 0;
            DISTENT = 65536 * 0.017;
        }
        else
        {
            temp_w = (u16)(CH << 8) | (u16)CL;
            DISTENT = temp_w * 0.017;
        }
    }
}

void ACT_Fuc(void)
{
    static float road = 0; // 10*
    static float ran = 0;
    u16 abs_x = 0, abs_y = 0;
    // run
    if (FLAG_ACT)
    {
        FLAG_ACT = 0;
        switch (STATUS)
        {
        case 0:
            // rest
            if (FLAG_RC) // distence of target between now position
            {
                FLAG_RC = 0;
                abs_x = abs(POSI[0] - POSI_TG[0]);
                abs_y = abs(POSI[1] - POSI_TG[1]);
                road = sqrt(abs_x * abs_x + abs_y * abs_y) * 10;
            }
            break;
        case 1:
            // run
            if (ran >= road)
            {
                STATUS = 0;
                ran = 0;
                SPEED = 0;
            }
            else
            {
                SPEED = (3.14 * PARA_R * FREQ) / 100.0 + PARA_B * 10.0;
                ran = ran + (SPEED * 0.0001);
            }
            // block
            if (DISTENT < 30)
            {
                FALG_BLOCK = 1;
                STATUS = 2;
            }
            break;
        case 2:
            if (DISTENT >= 30)
            {
                FALG_BLOCK = 0;
                STATUS = 1;
            }
            break;
        default:
            break;
        }
    }
}

void ADC_Fuc(void)
{
    u8 ADCget = 0;
    if (FLAG_ADC)
    {
        FLAG_ADC = 0;
        ADCget = PCF8591_ADC();
        ADCget = (ADCget * 5.0 / 255 + 0.05) * 10;
        if (ADCget >= 12)
        {
            LIGHT_V = 1;
        }
        else
        {
            LIGHT_V = 0;
        }
    }
}

void UART_Fuc(void)
{
    u8 match = 0;
    u8 status_REC[][5] = {"Idle", "Busy", "Wait"};
    u8 POSI_REC[10] = {0};
    u16 x = 0, y = 0;
    if (FLAG_URAT && QUADtime)
    {
        FLAG_URAT = 0;
        QUADtime = 0;
        if (strpos == 1)
        {
            if (strRECV[0] == '?')
            {
                Uart_Send(status_REC[STATUS]);
            }
            else if (strRECV[0] == '#')
            {
                sprintf(POSI_REC, "(%u,%u)", POSI[0], POSI[1]);
                Uart_Send(POSI_REC);
            }
        }
        else
        {
            match = sscanf(strRECV, "(%u,%u)", x, y);
            if (match == 2)
            {
                POSI_TG[0] = x;
                POSI_TG[1] = y;
                FLAG_RC = 1;
            }
            else
            {
                Uart_Send("Error");
            }
        }
    }
    strpos = 0;
}

void Uart_ISR(void) interrupted 4
{
    if (RI)
    {
        RI = 0;
        FLAG_URAT = 1;
        if (strpos < 9)
        {
            strRECV[strpos++] = SBUF;
            strRECV[strpos] = 0;
        }
    }
}

void T1_ISR(void) interrupted 3
{
    static u32 T1_2MS = 0;
    static u32 T1_100MS = 0;
    static u32 T1_1S = 0;
    T1_1MS++;
    if (++T1_100MS == 100)
    {
        T1_100MS = 0;
        FLAG_SEG = 1;
        FLAG_WAVE = 1;
        FLAG_ACT = 1;
        FLAG_ADC = 1;
    }
    if (++T1_1S == 1000)
    {
        T1_1S = 0;
        TR0 = 0;
        FREQ = (u16)TH0 << 8 | (u16)TL0;
        TH0 = 0x00;
        TL0 = 0x00;
        TR0 = 1;
    }
    if (++T1_2MS == 2)
    {
        T1_2MS = 0;
        QUADtime = 1;
    }

    SEG_Disp(SEG_CD, SEG_PS);
    SEG_PS = (SEG_PS++) & 0x07;
}