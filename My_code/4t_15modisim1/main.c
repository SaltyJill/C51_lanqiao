#include "main.h"
#include "driver.h"
#include "ds1302.h"
#include "iic.h"
#include <stdio.h>
// LED
u8 LED_DP = 0;
u8 FLAG_L4 = 0;
// T1 ISR
u32 T1_1MS = 0;
// SEG
u8 SEG_DP[10] = "2134";
u8 SEG_CD[8] = {0};
u8 SEG_PS = 0;
u8 page = 0;
u8 FLAG_SAVE = 0;
u8 FLAG_SEG = 0;
// EE
u8 EE_DATE[5] = {0};
u16 DATE = 0;
// TIM
u8 TIME_NOW[3] = {0};
u8 TIME_SAVE[2] = {0};
u8 FLAG_TIM = 0;
//KEY
u8 cnt=0;

unsigned char Char_Hex(u8 c);
void EE_Fuc(void);
void SEG_Fuc(void);
void KEY_Fuc(void);
void TIM_Fuc(void);
void LED_Fuc(void);

void main(void)
{
    u8 timeSET[3] = {12, 59, 45};
    DS1302_SET(timeSET);
    DEV_Cls();
    T1_Int();
    while (1)
    {
      KEY_Fuc();
			TIM_Fuc();
			SEG_Fuc();
			LED_Fuc();
			EE_Fuc();
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
            sprintf(SEG_DP,"%02u-%02u-%02u", (u16)TIME_NOW[0], (u16)TIME_NOW[1], (u16)TIME_NOW[2]);
            break;
        case 1:
						if(cnt)
						{
							sprintf(SEG_DP, "C```%4u",DATE);
						}
						else
						{
							sprintf(SEG_DP, "C```````");
						}
            break;
        case 2:
            sprintf(SEG_DP, "E``%02u-%02u", (u16)TIME_SAVE[0], (u16)TIME_SAVE[1]);
            break;
        }
        SEG_Tran(SEG_DP, SEG_CD);
    }
}
void KEY_Fuc(void)
{
    static u8 key_past = 0;
    u8 key_now;
    key_now = KEY_Martix();
    if (key_past != key_now)
    {
        switch (key_now)
        {
        case 0:
            break;
        case 4:
            page = (++page) % 3;
            if (page == 1)
            {
              DATE = 0;
							cnt=0;
            }
            if (page == 2)
            {
                FLAG_SAVE = 1;
            }
            break;
        case 5:
            if (page == 1)
            {
              DATE = 0;
							TIME_SAVE[0]=0;
							TIME_SAVE[1]=0;
							cnt=0;
            }
            break;
            // num
        case 6:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 0;
								cnt++;
            }
            break;
        case 10:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 1;
								cnt++;
            }
            break;
        case 14:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 2;
								cnt++;
            }
            break;
        case 18:
            if (page == 1 && cnt <=3)
            {
               DATE = DATE * 10 + 3;
							cnt++;
            }
            break;
        case 9:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 4;
							cnt++;
            }
            break;
        case 13:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 5;
							cnt++;
            }
            break;
        case 17:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 6;
							cnt++;
            }
            break;
        case 8:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 7;
							cnt++;
            }
            break;
        case 12:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 8;
							cnt++;
            }
            break;
        case 16:
            if (page == 1 && cnt <=3)
            {
                DATE = DATE * 10 + 9;
							cnt++;
            }
            break;
        }
        key_past = key_now;
    }
}
unsigned char Char_Hex(u8 c)
{
		u8 vc=0;
    if (c >= '0' && c <= '9')
    {
        vc=c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        vc=c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        vc=c - 'a' + 10;
    }
		return vc;
}
void TIM_Fuc(void)
{
    if (FLAG_TIM)
    {
        FLAG_TIM = 0;
        DS1302_RED(TIME_NOW);
    }
}
void EE_Fuc(void)
{
    u8 tempE;
    static u16 date_past = 0;
    if (FLAG_SAVE)
    {
        FLAG_SAVE = 0;
        TIME_SAVE[0] = TIME_NOW[0];
        TIME_SAVE[1] = TIME_NOW[1];
				if(date_past)
				{
					FLAG_L4 = (DATE > date_past) ? (1) : (0);
				}
        date_past = DATE;
        sprintf(EE_DATE, "%04x", DATE);
        tempE = TIME_SAVE[0];
				E2_WR(&tempE,0,1);
        tempE = TIME_SAVE[1];
				E2_WR(&tempE, 1, 1);
        tempE = (Char_Hex(EE_DATE[0]) << 4) | Char_Hex(EE_DATE[1]);
				E2_WR(&tempE, 2, 1);
        tempE = (Char_Hex(EE_DATE[2]) << 4) | Char_Hex(EE_DATE[3]);
        E2_WR(&tempE, 3, 1);
    }
}
void LED_Fuc(void)
{
    static led_past = 0;
    // L1,L2,L3
    LED_DP |= (u8)(1 << page);
    LED_DP &= (u8)(1 << page) | 0xF8;
    // L4
    if (FLAG_L4)
    {
        LED_DP |= (u8)(1 << 3);
    }
    else
    {
        LED_DP &= (u8) ~(1 << 3);
    }
    if (led_past != LED_DP)
    {
        LED_Disp(LED_DP);
        led_past = LED_DP;
    }
}
void T1_ISR(void) interrupt 3
{
    static u32 T1_100MS = 0;
    T1_1MS++;
    if (++T1_100MS == 100)
    {
        T1_100MS = 0;
        FLAG_SEG = 1;
        FLAG_TIM = 1;
    }
    SEG_Disp(SEG_CD, SEG_PS);
    SEG_PS = (++SEG_PS) & 0x07;
}