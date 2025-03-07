#include "main.h"
#include "time.h"
#include "iic.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

u8 led_out = 0;       // LED 输出
u8 led1_Flag = 0;     // LED_1 点亮允许
u8 led3_cnt;          // LED_3 连续无效按键数
u32 breakin_Time = 0; // 穿越时间

u8 seg_Disp[9] = "12 34567"; // 数码管显示的状态，显示码，位置
u8 seg_Code[8] = {0};
u8 seg_Add = 0;

volatile u32 ISR_1Ms = 0; // 全局每1ms中断一次的时钟变量，可供time.c调用
u32 Time_10Ms = 0;        // 10ms分频

u8 Menu = 0;          // 三个菜单
u8 nouse_keyNum = 0;  // 无效按键个数
s16 volta_Base = 250; // 参考电压，单位为0.01V

u8 break_num;      // 穿越参考电压次数
u8 ADC_Flag = 0;   // ADC工作状态
u16 ADC_100Ms = 0; // ADC转换每100ms进行一次
u16 ADC;           // ADC转换结果

u8 EE_save;

void key_Fuc(void);
void seg_Update(void);
void ADC_Fuc(void);
void LED_Fuc(void);

void main(void)
{
    outer_cls();
    T1_Init();
    EE_RD(&EE_save, 0, 1);
    if (EE_save > 50)
    {
        EE_save = 0;
    }
    // volta_Base = EE_save * 10;
    // EE_WR(&EE_save, 0, 1);
    while (1)
    {
        key_Fuc();
        seg_Update();
        ADC_Fuc();
        LED_Fuc();
    }
}

void T1_TSR() interrupt 3 // 1ms 12T
{
    ISR_1Ms++;
    /*数码管显示*/
    Seg_Disp(seg_Code, seg_Add);
    if (++seg_Add == 8)
    {
        seg_Add = 0;
    }
    /*分频10ms*/
    if (ISR_1Ms % 10 == 0)
    {
        Time_10Ms++;
    }
    if (++ADC_100Ms == 100)
    {
        ADC_100Ms = 0;
        ADC_Flag = 1;
    }
}

void key_Fuc(void)
{
    static u8 keyPast = 0;
    u8 keyNow;
    keyNow = key_MatrixRead();
    if (keyNow != keyPast)
    {
        switch (keyNow)
        {
        case 0:
            break;
        case 9:
            Menu = (++Menu) % 3;
            if (Menu == 2)
            {
                EE_save = volta_Base / 10;
                EE_WR(&EE_save, 0, 1);
            }
            nouse_keyNum = 0;
            break;
        case 10:
            if (Menu == 2)
            {
                break_num = 0;
                nouse_keyNum = 0;
            }
            break;
        case 13:
            if (Menu == 1)
            {
                volta_Base += 50;
                nouse_keyNum = 0;
            }
            else if (volta_Base > 500)
            {
                volta_Base = 0;
                nouse_keyNum = 0;
            }
            break;
        case 14:
            if (Menu == 1)
            {
                volta_Base -= 50;
                nouse_keyNum = 0;
            }
            else if (volta_Base < 0)
            {
                volta_Base = 500;
                nouse_keyNum = 0;
            }
            break;
        default:
            break;
        }
        keyPast = keyNow;
    }
}

void seg_Update(void)
{
    static u32 Seg_10ms = 0;
    if (Seg_10ms != Time_10Ms)
    {
        Seg_10ms = Time_10Ms;
        if (Menu == 0)
        {
            sprintf(seg_Disp, "U    %4.2f", ADC / 100.0);
        }
        else if (Menu == 1)
        {
            sprintf(seg_Disp, "P    %4.2f", volta_Base / 100.0);
        }
        else
        {
            sprintf(seg_Disp, "N    %2u", break_num);
        }
        Seg_trans(seg_Disp, seg_Code);
    }
}

void ADC_Fuc(void)
{
    u8 ucADC;
    float fADC;
    static u16 ADC_past = 0;
    if (!ADC_Flag)
    {
        return;
    }
    ADC_Flag = 0;
    ucADC = PCF8591_ADC();
    fADC = ucADC * 5.0 / 255; // 转换为数字量
    ADC = fADC * 100 + 0.5;   // 扩大100倍并进行四舍五入
    if (ADC_past > volta_Base && ADC <= volta_Base)
    {
        break_num++;
    }
    if (ADC_past >= volta_Base && ADC < volta_Base)
    {
        led1_Flag = 1;
        breakin_Time = ISR_1Ms;
    }
    if (ADC >= volta_Base)
    {
        led1_Flag = 0;
    }
    ADC_past = ADC;
}

void LED_Fuc(void)
{
    static u32 LED_10ms = 0;
    if (LED_10ms != Time_10Ms)
    {
        LED_10ms = Time_10Ms;
        /*LED1*/
        if (led1_Flag && (LED_10ms * 10 - breakin_Time) >= 5000)
        {
            led_out |= (u8)1;
        }
        else
        {
            led_out &= ~(u8)1;
        }
        /*LED2*/
        if (break_num % 2 == 0)
        {
            led_out |= (u8)1 << 1; 
        }
        else
        {
            led_out &= ~((u8)1 << 1);
        }
        /*LED3*/
        if (led3_cnt == 3)
        {
            led_out |= (u8)1 << 2;
            led3_cnt = 0;
        }
        else
        {
            led_out &= ~((u8)1 << 2);
        }
    }
    led_dis(led_out);
}