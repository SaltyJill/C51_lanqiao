#include "task.h"

unsigned char cntr_light = 0;
unsigned char cntr_rtc = 0;
unsigned int  cntr_tmp = 0;
unsigned int  cntr_frq = 0;
unsigned char cntr_key = 0;
unsigned char cntr_dsp = 0;
unsigned char cntr_led = 0;
unsigned int  cntr_3s = 0;

unsigned int frq = 0;	//频率
unsigned int hum = 0;	//湿度
unsigned int temp = 0;	//温度
unsigned int mid_temp = 0;	
unsigned int adc_light = 0;	//环境光

unsigned char ui = 0;		//切换界面
unsigned char ui_ec = 0;	//回显子界面
unsigned char mid_ui = 0;	//

unsigned int sum_temp = 0;
unsigned int sum_hum = 0;
unsigned int max_temp = 0;
unsigned int ave_temp = 0;
unsigned int max_hum = 0;
unsigned int ave_hum = 0;
unsigned int hour = 0;
unsigned int min = 0;

unsigned int count = 0;

unsigned char para_t = 30;	//默认温度参数:23℃

bit f_col = 0;

//1ms中断一次@12MHz
void initSysTick(void)
{
    AUXR |= 0x04;
    T2L = 0x20;
	T2H = 0xD1;
	AUXR |= 0x10;
    IE2 |= 0x04;
	EA = 1;
}


//初始化板级外设
void initBsp(void)
{
	BUZ(0x00);
	LED(0xFF);
	InitDsp();
	initRTCStruct();
	InitPCF8591();	
	InitKeyStruct();
	InitFrqTimer();
	getTemperature();
	GetADC(1);
}

//定时器中断服务函数
void isrSysTick(void)    interrupt 12
{
	GetDisplayStruct()->SegDisplay();

	if(cntr_light < 200){
		++cntr_light;
	}
	
	if(cntr_tmp < 200){
		++cntr_tmp;
	}
	
	if(cntr_rtc < 100){
		++cntr_rtc;
	}
	
	if(cntr_frq < 1000){
		++cntr_frq;
	}
	
	if(cntr_led < 100){
		++cntr_led;
	}
	
	if(cntr_dsp < 50){
		++cntr_dsp;
	}
		
	if(cntr_key < 10){
		++cntr_key;
	}
	
	if(cntr_3s > 0){
		--cntr_3s;
	}
}

//
void taskControl(void)
{
	static unsigned char led = 0xff;
	static unsigned char old_light = 0;
	static unsigned int old_temp = 0;
	static unsigned char old_hum = 0;
	
	RTC *rtc = getRTCStruct();
	
	//100ms 刷新
	if(cntr_led < 100)
		return;
	cntr_led = 0;
	
	if(ui == 0){
		led &= ~1;
		led |= 2;
		led |= 4;
	}else if(ui == 1){
		led &= ~2;
		led |= 1;
		led |= 4;
	}else if(ui == 'T'){
		led &= ~4;
		led |= 1;
		led |= 2;
	}else{
		led |= 1;
		led |= 2;
		led |= 4;
	}
	
	if(temp > para_t*10){
		led ^= (1<<3);
	}else{
		led |= (1<<3);
	}
	
	if(hum == 'S'){
		led &= ~(1<<4);
	}else{
		led |= (1<<4);
	}
	
	/* 启动采集 */
	if((adc_light < 30)&&(old_light > 30)){
		if(cntr_3s == 0){
			f_col = 1;
			mid_ui = ui;
			ui = 'T';
			cntr_3s = 3000;
		}
	}
	/* 返回界面 */
	if((cntr_3s == 0)&&(ui == 'T')){
		ui = mid_ui;
	}

	old_light = adc_light;
	
	/* 温/湿度最大值及平均值计算 */
	if(f_col == 1){
		f_col = 0;
		
		old_temp = temp;
		old_hum = hum;
		
		temp = mid_temp;
		
		/* 频率转湿度 */
		if((frq < 200)||(frq > 2000)){
			hum = 'S';
		}else{
			/* hum = 2frq/45 + 10/9 */
			hum = (unsigned int)((2*frq + 50)/45.0);
			count++;
		}
		
		if(hum != 'S'){
			hour = (rtc->rtc[0]*10) + (rtc->rtc[1]);
			min  = (rtc->rtc[2]*10) + (rtc->rtc[3]);
			/* 温度最大值 */
			if(temp > max_temp){
				max_temp = temp;
			}
			/* 温度平均值 */
			sum_temp = (count-1)*ave_temp + temp;
			ave_temp = sum_temp/count;
			
			/* 湿度最大值 */
			if(hum > max_hum){
				max_hum = hum;
			}
			/* 湿度平均值 */
			sum_hum = (count-1)*ave_hum + hum*10;
			ave_hum = sum_hum/(count);
		}
	}
	
	/* L6点亮 */
	if(count >= 2){
		if((old_temp < temp)&&(old_hum < hum)){
			led &= ~(1<<5);
		}else{
			led |= (1<<5);
		}
	}

	LED(led);
}


//
void taskCollect(void)
{
	/* 采集温度数据  */
	if(cntr_tmp == 200){
		cntr_tmp = 0;
		mid_temp  = (unsigned int)(getTemperature() * 10);
	}
	
	/* 采集环境光数据  */
	if(cntr_light == 200){
		cntr_light = 0;
		adc_light = GetADC(1);
	}
	
	/* 采集RTC数据  */
	if(cntr_rtc == 100){
		cntr_rtc = 0;
		getRTCStruct()->Read();
	}	
	
	/* 采集频率数据 */
	if(cntr_frq == 1000){
		cntr_frq = 0;
		frq = CalFrq();
	}
		
    if(cntr_key == 10){
		cntr_key = 0;
        GetKeyStruct()->pFun();
    }		
}

//
void taskKey(void)
{
	//按键部分处理
	switch(GetKeyStruct()->value){
		/* S4 */
		case 1:
			if(ui != 'T'){
				if(++ui == 3){
					ui = 0;
				}
			}
			ui_ec = 0;
			GetKeyStruct()->value = 255;
			break;
		/* S5 */
		case 3:
			if(ui == 1){
				if(++ui_ec == 3){
					ui_ec = 0;
				}
			}
			GetKeyStruct()->value = 255;
			break;
		/* S8 */
		case 2:
			if(ui == 2){
				if(para_t < 99){
					++para_t;
				}
			}
			GetKeyStruct()->value = 255;
			break;
		/* S9 */
		case 4:
			if(ui == 2){
				if(para_t >0){
					--para_t;
				}
			}
			GetKeyStruct()->value = 255;		
			break;
		/* 长按键 */
		case 'L':
			if(ui_ec == 2){
				count = 0;
				hour = 0;
				min = 0;
				temp = 0;
				hum = 0;
				max_temp = 0;
				ave_temp = 0;
				max_hum = 0;
				ave_hum = 0;
			}
			GetKeyStruct()->value = 255;
			break;
		default:
			GetKeyStruct()->value = 255;			
			break;
	}
}

//显示处理
void taskDisplay(void)
{
	
	DisplayStruct* seg = GetDisplayStruct();
	RTC *rtc = getRTCStruct();
	
	//显示部分处理
	if(cntr_dsp < 50){
		return;
	}	
	cntr_dsp = 0;
	
	switch(ui){
		case 0:
			seg->dot = 255;
			seg->buf[0] = rtc->rtc[0];
			seg->buf[1] = rtc->rtc[1];
			seg->buf[2] = 11;
			seg->buf[3] = rtc->rtc[2];
			seg->buf[4] = rtc->rtc[3];
			seg->buf[5] = 11;
			seg->buf[6] = rtc->rtc[4];
			seg->buf[7] = rtc->rtc[5];		
			break;
		case 1:
			if(count != 0){
				/* 回显界面 */
				switch(ui_ec){
					/* 温度回显 */
					case 0:
						seg->dot = 6;
						seg->buf[0] = 12;
						seg->buf[1] = 10;
						seg->buf[2] = max_temp/100;
						seg->buf[3] = max_temp%100/10;
						seg->buf[4] = 11;
						seg->buf[5] = ave_temp/100;
						seg->buf[6] = ave_temp%100/10;
						seg->buf[7] = ave_temp%10;
						break;
					/* 湿度回显 */
					case 1:
						seg->dot = 6;
						seg->buf[0] = 13;
						seg->buf[1] = 10;
						seg->buf[2] = max_hum/10;
						seg->buf[3] = max_hum%10;
						seg->buf[4] = 11;
						seg->buf[5] = ave_hum/100;
						seg->buf[6] = ave_hum%100/10;
						seg->buf[7] = ave_hum%10;
						break;
					/* 时间回显 */
					case 2:
						seg->dot = 255;
						seg->buf[0] = 14;
						seg->buf[1] = count/10;
						seg->buf[2] = count%10;
						seg->buf[3] = hour/10;
						seg->buf[4] = hour%10;
						seg->buf[5] = 11;
						seg->buf[6] = min/10;
						seg->buf[7] = min%10;
						break;
				}		
				break;
			}else{
				/* 回显界面 */
				switch(ui_ec){
					/* 温度回显 */
					case 0:
						seg->dot = 255;
						seg->buf[0] = 12;
						seg->buf[1] = 10;
						seg->buf[2] = 10;
						seg->buf[3] = 10;
						seg->buf[4] = 10;
						seg->buf[5] = 10;
						seg->buf[6] = 10;
						seg->buf[7] = 10;
						break;
					/* 湿度回显 */
					case 1:
						seg->dot = 255;
						seg->buf[0] = 13;
						seg->buf[1] = 10;
						seg->buf[2] = 10;
						seg->buf[3] = 10;
						seg->buf[4] = 10;
						seg->buf[5] = 10;
						seg->buf[6] = 10;
						seg->buf[7] = 10;
						break;
					/* 时间回显 */
					case 2:
						seg->dot = 255;
						seg->buf[0] = 14;
						seg->buf[1] = count/10;
						seg->buf[2] = count%10;
						seg->buf[3] = 10;
						seg->buf[4] = 10;
						seg->buf[5] = 10;
						seg->buf[6] = 10;
						seg->buf[7] = 10;
						break;
				}		
			}
			break;
		/* 参数界面 */
		case 2:
			seg->dot = 255;
			seg->buf[0] = 15;
			seg->buf[1] = 10;
			seg->buf[2] = 10;
			seg->buf[3] = 10;
			seg->buf[4] = 10;
			seg->buf[5] = 10;
			seg->buf[6] = para_t/10;
			seg->buf[7] = para_t%10;
			break;
		/* 温湿度界面 */
		case 'T':
			seg->dot = 255;
			seg->buf[0] = 16;
			seg->buf[1] = 10;
			seg->buf[2] = 10;
			seg->buf[3] = temp/100;
			seg->buf[4] = temp%100/10;
			seg->buf[5] = 11;
			if(hum == 'S'){
				seg->buf[6] = 17;
				seg->buf[7] = 17;
			}else{
				seg->buf[6] = hum/10;
				seg->buf[7] = hum%10;
			}
			break;
	}	
}