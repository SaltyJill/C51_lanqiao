#ifndef __MAIN_H
#define __MAIN_H

#include "stc15f2k60s2.h"
#include "seg.h"
#include "kbd.h"
#include "ds1302.h"
#include "iic.h"
#include "intrins.h"
#include "ui.h"

#define LED	4
#define BUZ	5
#define COM	6
#define SEG	7

#define latch(X, Y)  {P0 = Y; P2 = ((P2 & 0x1F) | (X << 5)); P2 &= 0x1F;}


#endif