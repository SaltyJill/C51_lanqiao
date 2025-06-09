#include "main.h"
#include "tim.h"
#include "iic.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

u8 ucLed = 0;
u8 ucWs = 0;
u8 ucState = 0; // 0,1,2,3
u8 ucMode = 0;  // 0,1,2
u8 ucRB = 0;    // 0-R
u32 ulMs = 0;
u16 us1000_Cnt = 0;
u8 uc100_Cnt = 0, ucWave_Flag = 0, ucAdc_Flag = 0, ucLed_Flag = 0;
u8 ucMs_Flag = 0;
u8 pucSeg_Buff[10] = "123456789", pucSeg_Code[8] = {0}, ucSeg_Pos = 0;

u16 uiFq = 0;
u16 uiDist;
u8 ucAdc;

sbit Tx_Wave = P1 ^ 0;
sbit Rx_Wave = P1 ^ 1;

u8 pucRecv[10] = {0}; //>=9,
u8 ucPos = 0, ucUart_Flag = 0, ucUart_Cnt = 0;

float curP[2] = {0, 0};
u16 tgtP[2] = {0, 0};
float x_cos, y_sin;

float V;
u8 R = 10; //*10
s8 B1 = 0;

u8 uc3s_Flag = 0;
u32 uwMs_Get = 0;
void Key_Proc(void);
void Seg_Proc(void);

void ADC_Proc(void);

void Led_Proc(void);
void Waishe_Proc(void);
void Wave_Proc(void);
void UART_Proc(void);
void V_Proc(void);
void Busy_Proc(void);

void main(void)
{
//
		u8 Str[]="SHU";
		u8 *Str0="Hellow";
		u8 Str1[]="Hellow";
	  strcpy(Str,Str0,1);
//	
    Cls_Waishe();
    T1_Init();
    T0_Init();
    UartInit();

    UartSend("hello");
    // Seg_Tran(pucSeg_Buff,pucSeg_Code);
    while (1)
    {
        Key_Proc();
        Wave_Proc();
        ADC_Proc();
        UART_Proc();
        V_Proc();
        Busy_Proc();
        Seg_Proc();
        Led_Proc();
        Waishe_Proc();
    }
}
void Key_Proc(void)
{
    static u8 ucKey_Old = 0;
    static u8 ucKey_Vld = 0;
    u8 ucKey_Now;
    static u32 uwMs_Key = 0;
    static u8 ucKeyPress1s = 0;
    float fX, fY, fL;
    u16 ui_curP[2];
    // u8 pucTemp[20]={0};
    ucKey_Now = KBD_Read();
    if (ucKey_Now != ucKey_Old)
    {
        uwMs_Key = ulMs;
        switch (ucKey_Now)
        {
        case 4:
            if (!ucMode)
            {
                ui_curP[0] = curP[0];
                ui_curP[1] = curP[1];
                if (ui_curP[0] != tgtP[0] && ui_curP[1] != tgtP[1])
                {
                    fX = tgtP[0] - ui_curP[0];
                    fY = tgtP[1] - ui_curP[1];
                    fL = sqrt(fX * fX + fY * fY);
                    x_cos = fX / fL;
                    y_sin = fY / fL;
                    ucMode = 1;
                    ucMs_Flag = 0;
                }
            }
            else if (ucMode == 1)
            {
                ucMode = 2;
            }
            else
            {
                if (uiDist > 30)
                    ucMode = 1;
            }
            break;
        case 5:
            if (!ucMode)
            {
                curP[0] = 0;
                curP[1] = 0;
            }
            break;
        case 8: // S8
            ucState = (++ucState) % 3;
            if (ucState == 2)
                ucRB = 0;
            break;
        case 9: // S9
            if (ucState == 2)
                ucRB = !ucRB;
            break;
        case 12: // S12
            if (ucState == 2)
            {
                if (!ucRB)
                {
                    R = R + 1;
                    if (R > 20)
                        R = 10;
                }
                else
                {
                    B1 = B1 + 5;
                    if (B1 > 90)
                        B1 = -90;
                }
            }
            break;
        case 13: // S13
            if (ucState == 2)
            {
                if (!ucRB)
                {
                    R = R - 1;
                    if (R < 10)
                        R = 20;
                }
                else
                {
                    B1 = B1 - 5;
                    if (B1 < -90)
                        B1 = 90;
                }
            }
            break;
        }
        ucKey_Old = ucKey_Now;
    }
    if (ulMs - uwMs_Key >= 800)
    {
        switch (ucKey_Old)
        {
        case 5: // S8

            break;
        case 6: // S9

            break;
        }
    }
}
void V_Proc(void)
{
    if (ucMode == 1)
    {
        V = 0.314 * R * uiFq / 100 + B1;
    }
    else
        V = 0;
}
void Busy_Proc(void)
{
    u16 uiX, uiY;
    if (ucMode == 1)
    {
        if (ucMs_Flag)
        {
            ucMs_Flag = 0;
            curP[0] = curP[0] + x_cos * V * 0.001; // 1ms
            curP[1] = curP[1] + y_sin * V * 0.001;
            uiX = 10 * fabs(tgtP[0] - curP[0]) + 0.5; //*10
            uiY = 10 * fabs(tgtP[1] - curP[1]) + 0.5; //*10

            if (uiX <= 1 && uiY <= 1) //<=0.1,<=0.1
            {
                curP[0] = tgtP[0];
                curP[1] = tgtP[1];
                uwMs_Get = ulMs;
                uc3s_Flag = 1;
                ucMode = 0;
            }
        }
        if (uiDist <= 30)
            ucMode = 2;
    }
}
void Seg_Proc(void)
{
    switch (ucState)
    {
    case 0: // 改了一下要求，运行时显示当前坐标
        if (!ucMode)
            sprintf(pucSeg_Buff, "L%3u-%3u", (u16)(tgtP[0]), (u16)(tgtP[1]));
        // sprintf(pucSeg_Buff, "L%3u-%3u", (u16)(curP[0]),(u16)(curP[1]));
        else
            // sprintf(pucSeg_Buff, "L%3u-%3u", (u16)(tgtP[0]),(u16)(tgtP[1]));
            sprintf(pucSeg_Buff, "L%3u-%3u", (u16)(curP[0]), (u16)(curP[1]));
        break;
    case 1:
        if (!ucMode)
            sprintf(pucSeg_Buff, "E2 -----");
        else if (ucMode == 1)
            sprintf(pucSeg_Buff, "E1%4.1f", V);
        else
            sprintf(pucSeg_Buff, "E3%6u", (u16)(uiDist));
        break;
    case 2:
        sprintf(pucSeg_Buff, "P %3.1f %3d", R / 10.0, (s16)B1);
        break;
    }
    // sprintf(pucSeg_Buff, "%3u",(u16)ucAdc);
    // sprintf(pucSeg_Buff, "1234567.8");
    // if(ucAdc <50)
    // sprintf(pucSeg_Buff, "%5.1f%4u", fWd,(u16)ucAdc);
    // else
    // sprintf(pucSeg_Buff, "%5.1f", fWd);

    // sprintf(pucSeg_Buff, "%5u%2u", uiFq,(u16)ucTrigFlag);
    Seg_Tran(pucSeg_Buff, pucSeg_Code);
}
void Led_Proc(void)
{
    u8 ucLed_Old;
    static u8 ucIndex = 7;
    ucLed_Old = ucLed;
    if (!ucMode)
    {
        ucLed &= ~(1 << 0);
        ucLed &= ~(1 << 1);
    }
    else if (ucMode == 1)
    {
        ucLed |= 1 << 0;
        if (ucAdc >= 93) // 1.2v
            ucLed &= ~(1 << 1);
        else
            ucLed |= 1 << 1;
    }
    else
    {
        if (ucLed_Flag)
        {
            ucLed_Flag = 0;
            ucLed ^= 1 << 0;
        }
        ucLed &= ~(1 << 1);
    }
    if (uc3s_Flag)
    {
        ucLed |= 1 << 2;
        if (ulMs - uwMs_Get >= 3000)
            uc3s_Flag = 0;
    }
    else
        ucLed &= ~(1 << 2);
    if (ucLed != ucLed_Old)
    {
        Led_Disp(ucLed);
    }
}

void Waishe_Proc(void)
{
    u8 ucWs_Old;
    ucWs_Old = ucWs;
    if (ucMode == 1)
        ucWs |= 1 << 4;
    else
        ucWs &= ~(1 << 4);

    if (ucWs != ucWs_Old)
    {
        Out_Waishe(ucWs);
    }
}
void ISR_T1() interrupt 3 // 1ms
{
    ulMs++;
    //}
    ucMs_Flag = 1;
    if (++uc100_Cnt == 100) //
    {
        uc100_Cnt = 0;
        ucAdc_Flag = 1;
        ucWave_Flag = 1;
        ucLed_Flag = 1;
    }

    if (++us1000_Cnt == 1000)
    {
        us1000_Cnt = 0;
        uiFq = (u16)(TH0 << 8) + TL0;
        TR0 = 0;
        TH0 = 0;
        TL0 = 0;
        TR0 = 1;
    }
    if (ucUart_Flag)
        ucUart_Cnt++;
    Seg_Disp(pucSeg_Code, ucSeg_Pos);
    if (++ucSeg_Pos == 8)
        ucSeg_Pos = 0;
}
void ADC_Proc(void)
{
    if (!ucAdc_Flag)
        return;
    ucAdc_Flag = 0;
    ucAdc = PCF8591_Read(1);
}
void Wave_Proc(void)
{
    u8 i = 4; // or i=6
    u16 CHCL = 0;
    if (!ucWave_Flag)
        return;
    ucWave_Flag = 0;
    Tx_Wave = 0;
    CL = 0xf4; // 设置PCA低8位定时值
    CH = 0xff; // 设置PCA高8位定时值
    CF = 0;
    CR = 1; // 启动PCA
    while (i--)
    {
        while (!CF)
            ; // 等待PCA定时时间到
        CF = 0;
        CL = 0xf4;    // 重置PCA低8位定时值
        CH = 0xff;    // 重置PCA高8位定时值
        Tx_Wave ^= 1; // 翻转
    }
    CR = 0; // 关闭PCA
    // 接收计时
    CL = 0; // 设置PCA低8位定时值
    CH = 0; // 设置PCA高8位定时值
    CF = 0;
    CR = 1;
    while (Rx_Wave && !CF)
        ; // 等待下降沿
    CR = 0;
    if (CF)
    {
        CF = 0;
        uiDist = 65535 * 0.017; // ok
    }
    else
    {
        CHCL = (u16)(CH << 8) + CL;
        uiDist = CHCL * 0.017; // ok
    }
}
void ISR_Uart1(void) interrupt 4
{
    if (RI)
    {
        RI = 0;
        ucUart_Cnt = 0;
        ucUart_Flag = 1;
        if (ucPos <= 8) // protect
        {
            pucRecv[ucPos++] = SBUF;
            pucRecv[ucPos] = 0;
        }
    }
}
void UART_Proc(void)
{
    u16 uiX, uiY;
    int rst;
    // u8 modStr[][4]={"Idle","Busy","Wait"};    //err
    u8 modStr[][5] = {"Idle", "Busy", "Wait"};
    u16 tmpP[2];
    u8 tmpStr[10] = {0};
    if (ucUart_Flag && ucUart_Cnt >= 2) // 2ms
    {
        ucUart_Flag = 0;
        ucUart_Cnt = 0;
        if (ucPos == 1 && pucRecv[0] == '?')
        {
            UartSend(modStr[ucMode]);
        }
        else if (ucPos == 1 && pucRecv[0] == '#')
        {
            tmpP[0] = curP[0], tmpP[1] = curP[1];
            sprintf(tmpStr, "(%d,%d)", tmpP[0], tmpP[1]);
            UartSend(tmpStr);
        }
        else
        {
            rst = sscanf(pucRecv, "(%d,%d)", &uiX, &uiY);
            if (rst == 2)
            {
                tgtP[0] = uiX, tgtP[1] = uiY;
            }
            else
            {
                UartSend("Error");
            }
        }
        ucPos = 0;
        pucRecv[ucPos] = '\0';
    }
}