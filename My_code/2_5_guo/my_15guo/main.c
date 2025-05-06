#include "main.h"
#include "driver.h"
#include "iic.h"
#include <stdio.h>
#include <string.h>
// ISR
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

// OTHER
u8 STATUS = 0;     // rest,wait,busy
u16 POSI[2] = {0}; // x,y

void SEG_Fuc(void);
void KEY_Fuc(void);
void LED_Fuc(void);
void ADC_Fuc(void);
void UART_Fuc(void);

void main(void)
{
    T1_Int();
    Uart_Int();
    while (1)
    {
        ;
    }
}
void SEG_Fuc(void) {}
void KEY_Fuc(void) {}
void LED_Fuc(void) {}
void ADC_Fuc(void) {}
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
            POSI[0] = x;
            POSI[1] = y;
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
    u32 T1_10ms = 0, T1_100ms = 0;
    if (++T1_100ms == 100)
    {
        T1_100ms = 0;
        FLAG_SEG = 1;
    }
    SEG_Disp(SEG_CD, SEG_PS);
    SEG_PS = (++SEG_PS) & 0x07;
}