#include "main.h"
#include "driver.h"
#include "iic.h"
#include "ds1302.h"
#include "ds18b20.h"
#include <stdio.h>
#include <string.h>
/*LED参数*/
u8 LED_DP = 0;
u8 flagL4 = 0;
u8 flagL5 = 0;
u8 flagL6 = 0;
/*T1中断参数*/
u32 T1_1MS = 0;
/*ADC参数*/
u8 FLAG_ADC = 0;
u8 DARK_nLIGHT = 0;
u8 TRIG_Cnt = 0;
u8 AD_ALLOW = 0;
/*SEG参数*/
u8 SEG_DP[12] = "23232", SEG_Code[8] = {0}, SEG_POSI = 0;
u8 FLAG_SEG = 0;
u8 page = 0, BACK_page = 0;
u8 TPandRH = 0, CD_3S = 0;
u8 RH_DP = 0;
/*温度与湿度参数*/
u8 FLAG_overT = 0;
u8 FLAG_FREQ = 0;
u8 FLAG_TEP = 0;
u16 TEMP_C = 0,
    TEMP_P = 0,
    TEMP_Max = 0,
    TEMP_PARA = 300;
u16 RH = 0,
    RH_P = 0,
    RH_Max = 0;
u16 Freq = 0;
u32 TEMP_Sum = 0;
u32 RH_Sum = 0;
/*TIM参数*/
u8 FLAG_TIME = 0;
u8 TIME_NOW[3] = {0};
u8 TIME_Trig[2] = {0};
/*UART*/
u8 RETURN_DATE = 0;
u8 FLAG_URAT = 0;
void SEG_FUC(void);
void KEY_FUC(void);
void LED_FUC(void);
void ADC_FUC(void);
void TIM_FUC(void);
void TEP_FUC(void);
void FRQ_FUC(void);
void UART_FUC(void);
void main(void)
{
    u8 timeset[3] = {10, 59, 45};
    DS1302_SET(timeset);
    DEV_Cls();
    T1_Int();
    T0_Int();
    UART_Int();
    while (1)
    {
        ADC_FUC();
        KEY_FUC();
        FRQ_FUC();
        TIM_FUC();
        TEP_FUC();
        SEG_FUC();
        LED_FUC();
        UART_FUC();
    }
}
void SEG_FUC(void)
{
    static u32 TRH = 0;
    if (FLAG_SEG)
    {
        FLAG_SEG = 0;
        if (TPandRH || CD_3S || RH_DP)
        {
            if (!CD_3S)
            {
                TRH = T1_1MS;
                CD_3S = 1;
            }
            if (AD_ALLOW == 0)
            {
                sprintf(SEG_DP, "E``%2u-AA", (u16)TEMP_C / 10);
            }
            else
            {
                sprintf(SEG_DP, "E``%2u-%2u", (u16)TEMP_C / 10, (u16)RH / 10);
            }
            if (T1_1MS - TRH > 3000)
            {
                CD_3S = 0;
                RH_DP = 0;
            }
        }
        else
        {
            switch (page)
            {
            case 0:
                sprintf(SEG_DP, "%02u-%02u-%02u", (u16)TIME_NOW[0], (u16)TIME_NOW[1], (u16)TIME_NOW[2]);
                break;
            case 1:
                switch (BACK_page)
                {
                case 0:
                    if (TRIG_Cnt)
                    {
                        sprintf(SEG_DP, "C`%2u-%4.1f", (u16)TEMP_Max / 10, TEMP_Sum / 10.0 / TRIG_Cnt); // TRIG_Cnt);
                    }
                    else
                    {
                        sprintf(SEG_DP, "C```````");
                    }
                    break;
                case 1:
                    if (TRIG_Cnt)
                    {
                        sprintf(SEG_DP, "H`%2u-%4.1f", (u16)RH_Max / 10, RH_Sum / 10.0 / TRIG_Cnt); // TRIG_Cnt);
                    }
                    else
                    {
                        sprintf(SEG_DP, "H```````");
                    }
                    break;
                case 2:
                    if (TRIG_Cnt)
                    {
                        sprintf(SEG_DP, "F%02u%02u-%02u", (u16)TRIG_Cnt, (u16)TIME_Trig[0], (u16)TIME_Trig[1]);
                    }
                    else
                    {
                        sprintf(SEG_DP, "F%02u`````", (u16)TRIG_Cnt);
                    }
                    break;
                }
                break;
            case 2:
                sprintf(SEG_DP, "E`````%2u", (u16)TEMP_PARA / 10);
                break;
            }
        }
        SEG_Tran(SEG_DP, SEG_Code);
    }
}
void KEY_FUC(void)
{
    static u32 keyPt = 0;
    static u8 keyPast = 0;
    static u8 keyPflag = 0;
    u8 keyNow = 0;
    keyNow = KEY_Martix();
    if (keyPast != keyNow)
    {
        keyPt = T1_1MS;
        switch (keyNow)
        {
        case 0:
            if (keyPflag)
            {
                keyPflag = 0;
                TEMP_Sum = 0;
                TEMP_Max = 0;
                RH_Max = 0;
                RH_Sum = 0;
                TRIG_Cnt = 0;
                TIME_Trig[0] = 0;
                TIME_Trig[1] = 0;
            }
            break;
        case 4:
            page = (++page) % 3;
            break;
        case 5:
            if (page == 1)
            {
                BACK_page = (++BACK_page) % 3;
            }
            break;
        case 8:
            if (TEMP_PARA == 990)
            {
                TEMP_PARA = 0;
            }
            if (page == 2)
            {
                TEMP_PARA += 10;
            }
            break;
        case 9:
            if (TEMP_PARA == 0)
            {
                TEMP_PARA = 990;
            }
            if (page == 2)
            {
                TEMP_PARA -= 10;
            }
            break;
        }
        keyPast = keyNow;
    }
    if (T1_1MS - keyPt > 2000)
    {
        switch (keyPast)
        {
        case 9:
            if (page == 1)
            {
                keyPflag = 1;
            }
            break;
        }
    }
}
void LED_FUC(void)
{
    static u8 ledPast = 0;
    static u32 Ltime = 0;
    // L1,L2,L3
    LED_DP |= (u8)1 << page;
    LED_DP &= ((u8)1 << page) | 0xF8;
    // L4
    if (FLAG_overT)
    {
        if (T1_1MS - Ltime > 100)
        {
            Ltime = T1_1MS;
            flagL4 = !flagL4;
        }
        if (flagL4)
        {
            LED_DP |= (1 << 3);
        }
        else
        {
            LED_DP &= ~(1 << 3);
        }
    }
    else
    {
        LED_DP &= ~(1 << 3);
    }
    // L5
    if (flagL5)
    {
        flagL5 = 0;
        if (AD_ALLOW)
        {
            LED_DP &= ~(1 << 4);
        }
        else
        {
            LED_DP |= (1 << 4);
        }
    }
    // L6
    if (flagL6)
    {
        flagL6 = 0;
        if (TRIG_Cnt > 1 && RH > RH_P && TEMP_C > TEMP_P)
        {
            LED_DP |= (1 << 5);
        }
        else
        {
            LED_DP &= ~(1 << 5);
        }
        TEMP_P = TEMP_C;
        RH_P = RH;
    }
    if (ledPast != LED_DP)
    {
        ledPast = LED_DP;
        LED_Disp(LED_DP);
    }
}
void ADC_FUC(void)
{
    float fADC;
    u16 V_light;
    static u8 past_DL = 0;
    u8 ADCget;
    if (FLAG_ADC)
    {
        FLAG_ADC = 0;
        ADCget = PCF8591_ADC();
        fADC = ADCget * 5.0 / 255;
        V_light = fADC * 10;
        if (V_light < 10)
        {
            DARK_nLIGHT = 1;
            if (past_DL == 0)
            {
                RH_DP = 1;
                flagL5 = 1;
                flagL6 = 1;
                if (AD_ALLOW && CD_3S == 0)
                {
                    TPandRH = 1;
                    FLAG_TEP = 1;
                    FLAG_FREQ = 1;
                    TRIG_Cnt++;
                    TIME_Trig[0] = TIME_NOW[0];
                    TIME_Trig[1] = TIME_NOW[1];
                }
                else
                {
                    TPandRH = 0;
                }
            }
        }
        else
        {
            TPandRH = 0;
            DARK_nLIGHT = 0;
        }
        past_DL = DARK_nLIGHT;
    }
}
void TIM_FUC(void)
{
    if (FLAG_TIME)
    {
        FLAG_TIME = 0;
        DS1302_RD(TIME_NOW);
    }
}
void TEP_FUC(void)
{
    static u8 stepTP = 0;
    static u32 timeTP = 0;
    s16 tp = 0;
    switch (stepTP)
    {
    case 0:
        DS18B20_ST();
        stepTP = 1;
        timeTP = T1_1MS;
        break;
    case 1:
        if (T1_1MS - timeTP > 800)
        {
            tp = DS18B20_RD();
            TEMP_C = (tp / 16.0) * 10;
            timeTP = T1_1MS;
            stepTP = 2;
        }
        break;
    case 2:
        if (T1_1MS - timeTP > 5)
        {
            stepTP = 0;
        }
    }
    // get
    if (FLAG_TEP)
    {
        FLAG_TEP = 0;
        TEMP_Sum += TEMP_C;
        if (TEMP_C > TEMP_Max)
        {
            TEMP_Max = TEMP_C;
        }
    }
    if (TEMP_C > TEMP_PARA)
    {
        FLAG_overT = 1;
    }
    else
    {
        FLAG_overT = 0;
    }
}
void FRQ_FUC(void)
{
    if ((Freq >= 200) && (Freq <= 2000))
    {
        if (FLAG_FREQ)
        {
            FLAG_FREQ = 0;
            RH = (Freq * 2.0 / 45 + 10.0 / 9) * 10;
            if (RH > RH_Max)
            {
                RH_Max = RH;
            }
            RH_Sum = RH_Sum + RH;
        }
        AD_ALLOW = 1;
    }
    else
    {
        AD_ALLOW = 0;
        RH = 0;
    }
}
void UART_FUC(void)
{
    u8 send[12] = {0};
    switch (RETURN_DATE)
    {
    case 1:
        sprintf(send, "%u\r\n", (u16)TRIG_Cnt);
        UART_Send(send);
        break;
    case 2:
        TEMP_Sum = 0;
        TEMP_Max = 0;
        RH_Max = 0;
        RH_Sum = 0;
        TRIG_Cnt = 0;
        TIME_Trig[0] = 0;
        TIME_Trig[1] = 0;
        sprintf(send, "ALL_CLEAN\r\n");
        UART_Send(send);
        break;
    case 3:
        sprintf(send, "PARA_SET\r\n");
        UART_Send(send);
        break;
    case 4:
        sprintf(send, "PARA_ERRO\r\n");
        UART_Send(send);
        break;
    }
    RETURN_DATE = 0;
}
void UART_ISR(void) interrupt 4
{
    static u8 str[9] = {0};
    static u8 posi = 0;
    u16 tpara, match;
    u8 r, n;
    if (RI)
    {
        RI = 0;
        str[posi++] = SBUF;
        str[posi] = 0;
        if (!strcmp(str, "Num\r\n"))
        {
            RETURN_DATE = 1;
            posi = 0;
        }
        else if (!strcmp(str, "Clr\r\n"))
        {
            RETURN_DATE = 2;
            posi = 0;
        }
        else if (posi == 8)
        {
            match = sscanf(str, "Para%2d%c%c", &tpara, &r, &n);
            if (match == 3)
            {
                if (r == '\r' && n == '\n' && tpara < 100 && tpara >= 0)
                {
                    TEMP_PARA = tpara * 10;
                    RETURN_DATE = 3;
                }
                else
                {
                    RETURN_DATE = 4;
                }
            }
            posi = 0;
        }
    }
}
void T1_ISR(void) interrupt 3
{
    static u32 T1_10MS = 0, T1_100MS = 0, T1_1S = 0;
    T1_1MS++;
    if (++T1_10MS == 10)
    {
        T1_10MS = 0;
        FLAG_SEG = 1;
    }
    if (++T1_100MS == 100)
    {
        T1_100MS = 0;
        FLAG_ADC = 1;
        FLAG_TIME = 1;
    }
    if (++T1_1S == 1000)
    {
        T1_1S = 0;
        Freq = (u16)(TH0 << 8) | TL0;
        TR0 = 0;
        TL0 = 0x00;
        TH0 = 0x00;
        TR0 = 1;
    }
    SEG_Disp(SEG_Code, SEG_POSI);
    SEG_POSI = (++SEG_POSI) & 0x07;
}