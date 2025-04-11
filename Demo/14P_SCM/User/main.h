#ifndef __MAIN_H
#define __MAIN_H

#include "stc15f2k60s2.h"
#include "task.h"
#include "dsp.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "keyboard.h"
#include "pcf8591.h"
#include "frq.h"

#define COM(X)  {P0 = X; P2 = (P2 & 0x1F | 0xC0);  P2 &= 0x1F;}
#define SEG(X)  {P0 = X; P2 = (P2 & 0x1F | 0xE0);  P2 &= 0x1F;}
#define BUZ(X)  {P0 = X; P2 = (P2 & 0x1F | 0xA0);  P2 &= 0x1F;}
#define LED(X)  {P0 = X; P2 = (P2 & 0x1F | 0x80);  P2 &= 0x1F;}

#endif