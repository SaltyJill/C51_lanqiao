#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "stc15f2k60s2.h"
#include "intrins.h"

typedef struct{
    unsigned char status;
    unsigned char value;
    void (code *pFun)(void);
}KEYStruct;

void InitKeyStruct(void);
KEYStruct* GetKeyStruct(void);
#endif