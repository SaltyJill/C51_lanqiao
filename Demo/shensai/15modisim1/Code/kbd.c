#include "kbd.h"

unsigned char key = 0;

/**
 * @brief Get the Key Value object
 * @param None.
 * @return key, 返回键值 
 */
unsigned char GetKeyValue(void){
	return key;
}

/**
 * @brief 清除键值
 * @param None.
 * @return None.
 */
void ClearKeyValue(void)
{
	key = 255;
}

/**
 * @brief 清除键盘列线
 * @param None.
 * @return None.
 */
static void ClsC(void)
{
	P3 = 0x0F;
	P42 = 0;
	P44 = 0;
}

/**
 * @brief 清除键盘行线
 * @param None.
 * @return None.
 */
static void ClsR(void)
{
	P3 = 0xF0;
	P42 = 1;
	P44 = 1;		
}

/**
 * @brief 读取键盘行列线状态
 * @param None.
 * @return port, 键盘行列状态
 */
static unsigned char ReadKeyPort(void)
{
	unsigned char port = 0;

	port = (P3 & 0x3F);

	(P42)?(port |= 0x40):(port &= 0xBF);
	(P44)?(port |= 0x80):(port &= 0x7F);

	return port;
}

/**
 * @brief 键盘扫描函数
 * @param None.
 * @return None.
 */
void ScanKeyboard(void)
{
	static unsigned char col = 0;
	static unsigned char status = 0;
	
    switch(status)
    {
        case 0: 
			ClsR();
			if(ReadKeyPort() != 0xF0){
				status = 1;
			}
            break;
        case 1:
			ClsR();
			switch(ReadKeyPort()){
				case 0xE0:
					col = 4;
					status = 2;
					break;
				case 0xD0:
					col = 3;
					status = 2;
					break;
				case 0xB0:
					col = 2;
					status = 2;
					break;
				case 0x70:
					col = 1;
					status = 2;
					break;
				default:
					status = 0;	
					break;
			}
			break;
        case 2:
			ClsC();
			switch(ReadKeyPort()){
				case 0x0E:
					key = col - 1;
					status = 3;
					break;
				case 0x0D:
					key = col + 3;
					status = 3;
					break;
				case 0x0B:
					key = col + 7;
					status = 3;
					break;
				case 0x07:
					key = col + 11;
					status = 3;
					break;
				default:
					status = 0;	//抖动
					break;
			}

			if(key > 15) key = 255;
            break; 
		case 3:
			ClsC();
			if(ReadKeyPort() == 0x0F){
				status = 0;
			}
			break; 
    }
}
