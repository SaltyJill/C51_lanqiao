#ifndef __DSP_H
#define __DSP_H


#include "main.h"

typedef struct {
    unsigned char buf[8];
    unsigned char com;
    unsigned char dot;
	void(code *SegDisplay)(void);
}DisplayStruct;

void InitDsp(void);
DisplayStruct* GetDisplayStruct(void);


#endif