#include "keyboard.h"

KEYStruct key;                                   

KEYStruct* GetKeyStruct(void)                 
{
    return &key;             	
}

//清除列(列线清零，行线拉高)
static void ClsC(void)
{
	P44 = 0;
	P42 = 0;
	P32 = 1;
	P33 = 1;
}

//清除行(行线清零，列线拉高)
static void ClsR(void)
{
	P32 = 0;
	P33 = 0;
	P44 = 1;
	P42 = 1;		
}

//
void ScanKeyboard(void)
{
	static unsigned char col = 0;
	static unsigned int keycounter = 0;
	static unsigned char mid_value = 0;
	
	switch(key.status)
    {
		case 0:
			ClsR();
			if((P44 == 0)||(P42 == 0)){
				key.status = 1;
			}
			break;
		case 1:
			ClsR();
			if(P44 == 0){
				col = 1;
				key.status = 2;
			}else if(P42 == 0){
				col = 2;
				key.status = 2;
			}
			break;
		case 2:
			ClsC();
			if(P32 == 0){
				mid_value = col + 2;
				key.status = 3;
			}else if(P33 == 0){
				mid_value = col;
				key.status = 3;
			}else{
				key.status = 0;
			}
			break;
		case 3:
			ClsC();	
			if((P32 == 1)&&(P33 == 1)){
				if(keycounter < 200){
					key.value = mid_value;
				}else{
					key.value = (mid_value == 4)?('L'):(mid_value);
				}
				keycounter = 0;	
				key.status = 0;
			}else{
				if(keycounter < 200){
					++keycounter;
				}
			}	
			break;
	}
}

void InitKeyStruct(void)
{
    key.pFun = ScanKeyboard;
    key.value = 0xFF;
    key.status = 0;
}