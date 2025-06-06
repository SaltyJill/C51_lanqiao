#include "ui.h"

int input_val = 0xFFFFF;
unsigned char num_index = 0;
unsigned char input_time[2];
bit up = 0;
unsigned char ui = 0;
bit input = 0;


/**
 * @brief RTC时钟显示界面
 * @param rtc ,指向时钟数据的指针
 * @return None.
 */
static void uiRTC(unsigned char * rtc)
{	
	unsigned char *pSEG = GetSegData();
	
	pSEG[0] = rtc[0];
	pSEG[1] = rtc[1];
	pSEG[2] = 15;
	pSEG[3] = rtc[2];
	pSEG[4] = rtc[3];
	pSEG[5] = 15;
	pSEG[6] = rtc[4];
	pSEG[7] = rtc[5];
}

/**
 * @brief 时间记录界面数据处理
 * @param None.
 * @return None.
 */
static void uiRCD(void)
{
	unsigned char *pSEG = GetSegData();

	pSEG[0] =16;
	pSEG[1] =10;
	pSEG[2] =10;
	if(input){
		pSEG[3] = input_time[0]/10;
		pSEG[4] = input_time[0]%10;
		pSEG[5] = 15;
		pSEG[6] = input_time[1]/10;
		pSEG[7] = input_time[1]%10;
	}else{
		pSEG[3] = 10;
		pSEG[4] = 10;
		pSEG[5] = 10;
		pSEG[6] = 10;
		pSEG[7] = 10;	
	}
}

/**
 * @brief 输入数值界面处理函数
 * @param num ,当前输入结果
 * @return None.
 */
static void uiNUMBER(unsigned int num)
{
	unsigned char *pSEG = GetSegData();
	
	pSEG[0] = 12;
	pSEG[1] = 10;
	pSEG[2] = 10;
	pSEG[3] = 10;
	
	if(num == 0xFFFF){
		pSEG[4] = 10;
		pSEG[5] = 10;
		pSEG[6] = 10;
		pSEG[7] = 10;
	}else{
		pSEG[4] = (num > 999)?(num/1000):(10);
		pSEG[5] = (num > 99)?(num%1000/100):(10);
		pSEG[6] = (num > 9)?(num%100/10):(10);
		pSEG[7] = num%10;
	}
}

/**
 * @brief 数码管显示处理函数
 * @param None.
 * @return None.
 */
void SegProc(void)
{
	unsigned char *pRTC = GetRTC();
	
	if(ui == 0){
		uiRTC(pRTC);
	}else if(ui == 1){
		uiNUMBER(input_val);
	}else{
		uiRCD();
	}
}

/**
 * @brief 键值转码
 *        将键盘值转换为0-9数字
 * @param val ,键盘值
 * @return 键盘值对应的数字
 */
unsigned char KeyValTrans(unsigned char val)
{
	if      (val == 4)	return 0;
	else if (val == 5)	return 1;
	else if (val == 6)	return 2;
	else if (val == 7)	return 3;
	else if (val == 9)	return 4;
	else if (val == 10)	return 5;
	else if (val == 11)	return 6;
	else if (val == 13)	return 7;
	else if (val == 14)	return 8;
	else if (val == 15)	return 9;
	else				return 255;
}

/**
 * @brief 键盘输入处理程序
 * @param None.
 * @return None.
 */
void KeyProc(void)
{
	unsigned char number = 0;
	static int input_val_old = 0;
	unsigned char *pRTC = GetRTC();
	
	if(GetKeyValue() == 12){ //S4
		if(++ui == 3){
			ui = 0;
		}
		ClearKeyValue();
	}
	if(ui == 1){
		input = 0;
		number = KeyValTrans(GetKeyValue());
		if(number <= 10){
			if(input_val == 0xFFFF){
				input_val = 0;
				input_time[0] = pRTC[0]*10 + pRTC[1];
				input_time[1] = pRTC[2]*10 + pRTC[3];
			}
			//
			if(input_val <= 999){
				input_val = input_val * 10 + number;
			}
		}else{
			if(GetKeyValue() == 8){
				input_val = 0xFFFF;
			}
		}
		ClearKeyValue();
	}
	if(ui == 2){
		if(input_val != 0xFFFF){
			up = (input_val > input_val_old)?(1):(0);
			input = 1;
			input_val_old = input_val;
			input_val = 0xFFFF;
			write_eeprom(0, input_time[0]);
			write_eeprom(1, input_time[1]);
			write_eeprom(2, (unsigned char)(input_val_old >> 8));
			write_eeprom(3, (unsigned char)(input_val_old & 0x00FF));
			
		}		
	}
}

/**
 * @brief 指示灯功能处理函数（LED1-LED4）
 * @param None.
 * @return None.
 */
void LedProc(void)
{
	unsigned char led = 0xff;
	
	led = ~(1<<ui);
	if(up){
		led &= ~(1 << 3);
	}else{
		led |= (1 << 3);
	}
	latch(LED, led);
}