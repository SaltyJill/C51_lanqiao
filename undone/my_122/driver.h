#ifndef __DRIVER_H
#define __DRIVER_H
#include "main.h"
void led_Disp(u8 led_out);
void outside_Cls(void);
void segTrans(u8 *pDisp, u8 *pCode);
void segDisp(u8 *pCode, u8 Adds);
unsigned char key_IndividualRead(void);

#endif