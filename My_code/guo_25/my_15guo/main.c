#include "main.h"
#include "driver.h"
#include "iic.h"
#include <stdio.h>
#include <string.h>
// ISR
u8 FLAG_WAVE = 0;
u32 T1_1ms = 0;
// SEG
u8 SEG_DP[10] = "12345",
   SEG_CD[8] = {0},
   SEG_PS = 0,
   FLAG_SEG = 0;
u8 page = 0;
// UART
u8 UART_MSG = 0; // FLAG of UART_Fuc
u8 poc = 0;
u8 uartStr[10] = {0};

// ACTION
u8 STATUS = 0; // rest,wait,busy
u16 SPEED = 0;
u16 DISTENT = 0;
u16 POSI[2] = {0};    // x,y
u16 POSI_TG[2] = {0}; // Target x,y

// PARA
u8 PARA_R = 0; //*10
u8 PARA_SL = 0;
s8 PARA_B = 0;
// ADC
u8 FLAG_ADC = 0;
u8 LIGHT = 0;
// FREQ
u16 FREQ = 0;

void SEG_Fuc(void);
void KEY_Fuc(void);
void LED_Fuc(void);
void ADC_Fuc(void);
void UART_Fuc(void);
void RELAY_Fuc(void);
void WAVE_Fuc(void);

void main(void)
{
    T1_Int();
    Uart_Int();
    while (1)
    {
        ;
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
            if (STATUS == 0)
            {
                sprintf(SEG_DP, "L%3u-%3u", (u16)POSI_TG[0], (u16)POSI_TG[1]);
            }
            else
            {
                sprintf(SEG_DP, "L%3u-%3u", (u16)POSI[0], (u16)POSI[1]);
            }
            break;
        case 1:
            switch (STATUS)
            {
            case 0:
                sprintf(SEG_DP, "E2`-----");
                break;
            case 1:
                sprintf(SEG_DP, "E3`%5u"(u16)DISTENT);
                break;
            case 2:
                sprintf(SEG_CD, "E1`%5u"(u16)SPEED);
                break;
            default:
                break;
            }
            break;
        case 2:
            if (PARA_B == 0)
            {
                sprintf(SEG_DP, "P`%2.1f`%01d" PARA_R / 10.0, PARA_B);
            }
            else
            {
                sprintf(SEG_DP, "P`%2.1f`%3d" PARA_R / 10.0, PARA_B);
            }
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
            switch (STATUS)
            {
            case 0:
                if (POSI[0] != POSI_TG[0] && POSI[1] != POSI_TG[1])
                {
                    STATUS = 2;
                }
                break;
            case 1:
                if (DISTENT > 10)
                {
                    STATUS = 2;
                }
            case 2:
                STATUS = 1;
                break;
            default:
                break;
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
            page = (++page) % 3;
            if (page == 2)
            {
                PARA_SL = 0;
            }
            break;
        case 9:
            PARA_SL = !PARA_SL;
            break;
        case 12:
            if (PARA_SL)
            {
                PARA_B = (PARA_B >= 90) ? 90 : (PARA_B + 5);
            }
            else
            {
                PARA_R = (PARA_R >= 20) ? 20 : (PARA_R + 1);
            }
            break;
        case 13:
            if (PARA_SL)
            {
                PARA_B = (PARA_B <= -90) ? -90 : (PARA_B - 5);
            }
            else
            {
                PARA_R = (PARA_R <= 10) ? 10 : (PARA_R - 1);
            }
            break;
        default:
            break;
        }
        key_past = key_now;
    }
}
void RELAY_Fuc(void)
{
    if (STATUS == 2)
    {
        Relay(1);
    }
    else
    {
        Relay(0);
    }
}
void ADC_Fuc(void)
{
    u8 ADCget;
    u16 LIGHT_V;
    float fADC;
    if (FLAG_ADC)
    {
        FLAG_ADC = 0;
        ADCget = PCF8591_ADC();
        fADC = (ADCget * 5.0 / 255) * 10;
        if (fADC > 12)
        {
            LIGHT = 1;
        }
        else
        {
            LIGHT = 0;
        }
    }
}
void WAVE_Fuc(void)
{
    u8 i = 4;
    u8 wTime = 0;
    sbit waveP10 = P1 ^ 0;
    sbit waveP11 = P1 ^ 1;
    if (FLAG_WAVE)
    {
        FLAG_WAVE = 0;
        CH = 0xFF;
        CL = 0xF3;
        CF = 0;
        CR = 1;
        while (i--)
        {
            while (!CF0)
                ;
            CF0 = 0;
            waveP10 ^= 1;
        }
        CR = 0;
        CF = 0;
        CH = 0x00;
        CL = 0x00;
        CR = 1;
        while (!CF && waveP11)
            ;
        CR = 0;
        if (CF)
        {
            CF = 0;
            DISTENT = 65536 * 0.017;
        }
        else
        {
            wTime = (u8)(CH << 4) | CL;
            DISTENT = wTime * 0.017;
        }
    }
}
void ACT_Fuc(void)
{
    
}
void LED_Fuc(void) 
{
    u8 led_now;
    static u8 led_past=0;
    static u32 timeL1=0;
    //L1
    switch (STATUS)
    {
    case 0:
        led_now&=~(1)
        break;
    
    default:
        break;
    }

    if (led_now!=led_past)
    {
        LED_Disp(led_now);
        led_past=led_now;
    }
    
}

void UART_Fuc(void)
{
    static u8 Array_Send[10] = {0};
    static u8 sent = 0;
    u8 match = 0;
    u16 x, y;

    if (UART_MSG)
    {
        UART_MSG = 0;
        //?
        if (!strcmp(uartStr, "?"))
        {
            switch (STATUS)
            {
            case 0:
                Uart_Send("Idle");
                break;
            case 1:
                Uart_Send("Wait");
                break;
            case 2:
                Uart_Send("Busy");
                break;
            default:
                Uart_Send("staErr");
                break;
            }
            sent = 1;
        }
        // #
        if (!strcmp(uartStr, "#"))
        {
            sprintf(Array_Send, "(%d,%d)", POSI[0], POSI[1]);
            Uart_Send(Array_Send);
            sent = 1;
        }
        //(x,y)
        match = sscanf(uartStr, "(%d,%d)", &x, &y);
        if (match == 2 && x > 0 && y > 0 && x < 999 && x < 999)
        {
            POSI_TG[0] = x;
            POSI_TG[1] = y;
            Uart_Send("Got it");
            sent = 1;
        }
        if (sent)
        {
            sent = 0;
        }
        else
        {
            Uart_Send("Error");
        }
        poc = 0;
    }
}
void UART_ISR(void) interrupt 4
{
    if (RI)
    {
        RI = 0;
        if (poc < 10)
        {
            uartStr[poc++] = SBUF;
            uartStr[poc] = 0;
        }
        else
        {
            if (poc == 10)
            {
                UART_MSG = 1;
                poc += 1;
            }
        }
    }
}

void T1_ISR(void) interrupt 3
{
    u32 T1_1s = 0, T1_100ms = 0;
    T1_1ms++;
    if (++T1_100ms == 100)
    {
        T1_100ms = 0;
        FLAG_SEG = 1;
        FLAG_ADC = 1;
        FLAG_WAVE = 1;
    }
    if (++T1_1s == 1000)
    {
        T1_1s = 0;
        TR0 = 0;
        FREQ = (u8)(TH0 << 4) | TL0;
        TH0 = 0x00;
        TL0 = 0x00;
        TF0 = 0;
        TR0 = 1;
    }
    SEG_Disp(SEG_CD, SEG_PS);
    SEG_PS = (++SEG_PS) & 0x07;
}