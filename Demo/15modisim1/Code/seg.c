#include "seg.h"

/* 供阳极数码管段码表 */
code unsigned char tab[] = {0xC0,	/* 0 */
							0xf9,	/* 1 */
							0xa4,	/* 2 */
							0xb0,	/* 3 */
							0x99,	/* 4 */
							0x92,	/* 5 */
							0x82,	/* 6 */
							0xf8,	/* 7 */
							0x80,	/* 8 */
							0x90,	/* 9 */
							0xff,	/*OFF*/
							0x8C,	/* P */
							0xC6,	/* C */
							0xC8,	/* N */
							0x88,	/* A */
							0xBF,	/* - */
							0x86,   /* E */
};

unsigned char dspbuf[8] = {10,10,10,10,10,10,10,10};
unsigned char dot_pos = 0xff;	//

/**
 * @brief Get the Seg Data object
 * @param None.
 * @return dspbuf, 指向数码管数据缓冲区的指针.
 */
unsigned char* GetSegData(void){
	return dspbuf;
}


/**
 * @brief 数码管扫描程序
 * @param None.
 * @return None.
 */
void SegDisplay(void)
{
    static unsigned char dspcom = 0;

	latch(SEG, 0xFF);
	latch(COM, 1<< dspcom);
	
	if(dspcom == dot_pos){
		latch(SEG, tab[dspbuf[dspcom]] & 0x7f);
	}else{
		latch(SEG, tab[dspbuf[dspcom]]);
	}
    if(++dspcom == 8){
        dspcom = 0;
    }
}