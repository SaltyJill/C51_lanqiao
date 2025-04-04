#include "main.h"
#include "driver.h"
#include "onewire.h"
#include "ds1302.h"
#include "stdio.h"
// 全局参数如下
/*LED参数*/
u8 LED_OUT = 0;
u8 FLAG_l3 = 0;
/*SEG参数*/
u8 SEG_DP[10] = "123450"; // 8个显示+一位小数点+终止符
u8 SEG_CD[8] = {0};
u8 SEG_PS = 0;
u8 FLAG_seg = 0;
u8 WIN_MD = 0; // 0,1,2三个菜单
u8 JOB_MD = 0; // 0,1两个工作模式
/*定时器参数*/
u32 T_1MS = 0;
/*DS1302时钟参数*/
u8 FLAG_tim = 0;
u8 SECOND_DP = 0;
u8 TIME_NOW[3] = {0};
u8 FLAG_hour = 0;
/*DS18B20温度传感器*/
u16 TEMP_PARA = 23;
u16 TEMP_C = 23.5;
/*继电器参数*/
u8 FLAG_relay = 0;
// 任务如下
void SEG_FUC(void);
void KEY_FUC(void);
void TIM_FUC(void);
void LED_FUC(void);
void TEP_FUC(void);
void RELAY_FUC(void);

void main(void)
{
    u8 time_ST[3] = {10, 59, 50}; // DS1302起始时间
    Other_CLS();
    T1_INT();
    DS1302_SET(time_ST);
    while (1)
    {
        SEG_FUC();
        KEY_FUC();
        TIM_FUC();
        RELAY_FUC();
        LED_FUC();
        TEP_FUC();
    }
}
// 函数如下
void SEG_FUC(void)
{
    if (FLAG_seg)
    {
        FLAG_seg = 0;
        switch (WIN_MD)
        {
        case 0:
            sprintf(SEG_DP, "U1```%4.1f", TEMP_C/10.0);
            break;
        case 1:
            if (SECOND_DP)
            {
                sprintf(SEG_DP, "%02u-%02u-%02u", (u16)TIME_NOW[0], (u16)TIME_NOW[1], (u16)TIME_NOW[2]);
            }
            else
            {
                sprintf(SEG_DP, "U2`%02u-%02u", (u16)TIME_NOW[0], (u16)TIME_NOW[1]);
            }
            break;
        case 2:
            sprintf(SEG_DP, "U3````%02u", (u16)TEMP_PARA);
            break;
        default:
            sprintf(SEG_DP, "ERROR");
            break;
        }
    }
    Seg_TR(SEG_DP, SEG_CD);
}
void LED_FUC(void)
{
    static u8 led_past = 0;
    static u32 t_L1 = 0;
    static u32 t_L3 = 0;
    // L1
    if (FLAG_hour)
    {
        t_L1 = T_1MS;
        LED_OUT |= 1;
        FLAG_hour = 0;
    }
    else if (T_1MS - t_L1 > 5000)
    {
        LED_OUT &= ~1;
    }
    // L2
    if (!JOB_MD)
    {
        LED_OUT |= (1 << 1);
    }
    else
    {
        LED_OUT &= ~(1 << 1);
    }
    // L3
    if (FLAG_relay)
    {
        if (T_1MS - t_L3 > 100)
        {
            t_L3 = T_1MS;
            FLAG_l3 = ~FLAG_l3;
        }
        if (FLAG_l3)
        {
            LED_OUT |= (1 << 2);
        }
        else
        {
            LED_OUT &= ~(1 << 2);
        }
    }
    else
    {
        LED_OUT &= ~(1 << 2);
    }
    if (led_past != LED_OUT)
    {
        led_past = LED_OUT;
        Led_DP(LED_OUT);
    }
}
void KEY_FUC(void)
{
    static u8 key_past;
    u8 key_new;
    key_new = Key_MARTIX();
    if (key_new != key_past)
    {
        key_past = key_new;
        switch (key_new)
        {
        case 0:
            SECOND_DP = 0;
            break;
        case 12:
            WIN_MD = (++WIN_MD) % 3;
            break;
        case 13:
            JOB_MD = (++JOB_MD) % 2;
            break;
        case 16:
            if (WIN_MD == 2)
            {
                TEMP_PARA++;
                if (TEMP_PARA > 99)
                {
                    TEMP_PARA = 10;
                }
            }
            break;
        case 17:
            if (WIN_MD == 2)
            {
                TEMP_PARA--;
                if (TEMP_PARA < 10)
                {
                    TEMP_PARA = 99;
                }
            }
            if (WIN_MD == 1)
            {
                SECOND_DP = 1;
            }
            break;
        }
    }
}
void TIM_FUC(void)
{
    if (FLAG_tim)
    {
        FLAG_tim = 0;
        DS1302_RD(TIME_NOW);
        if (!TIME_NOW[1] && !TIME_NOW[2])
        {
            FLAG_hour = 1;
        }
    }
}
void TEP_FUC(void)
{
    static u8 Step_byTEP = 0;
    static u32 tep_time = 0;
    s16 temp_RD;
    switch (Step_byTEP)
    {
    case 0:
        DS18B20_START_CV();
        tep_time = T_1MS;
        Step_byTEP = 1;
        break;
    case 1:
        if (T_1MS - tep_time > 800)
        {
            temp_RD = DS18B20_RD();
            TEMP_C = (temp_RD / 16 + 0.05) * 10;
            tep_time = T_1MS;
            Step_byTEP = 2;
        }
        break;
    case 2:
        if (T_1MS - tep_time > 5)
        {
            Step_byTEP = 0;
        }
        break;
    }
}
void RELAY_FUC(void)
{
    static u32 relay_time = 0;
    static u8 flag_past = 0;
    if (!JOB_MD)
    {
        if (TEMP_C > TEMP_PARA*10)
        {
            FLAG_relay = 1;
        }
        else
        {
            FLAG_relay = 0;
        }
    }
    else
    {

        if (FLAG_hour)
        {
            FLAG_relay = 1;
            relay_time = T_1MS;
        }
        else if (T_1MS - relay_time > 5000)
        {
            FLAG_relay = 0;
        }
    }
    if (flag_past != FLAG_relay)
    {
        if (FLAG_relay)
        {
            Relay(1);
        }
        else
        {
            Relay(0);
        }
    }
}
void T1_ISR(void) interrupt 3
{
    static u8 T_10MS = 0, T_100MS = 0;
    T_1MS++;
    if (++T_10MS == 10)
    {
        T_10MS = 0;
        FLAG_seg = 1;
    }
    if (++T_100MS == 100)
    {
        T_100MS = 0;
        FLAG_tim = 1;
    }
    Seg_DP(SEG_CD, SEG_PS);
    if (++SEG_PS == 8)
    {
        SEG_PS = 0;
    }
}