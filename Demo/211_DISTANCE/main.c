// ʹ�ó���ǰ����J13����ΪIOģʽ��2-3�Ŷ̽ӣ�
#include "tim.h"
#include "seg.h"
#include <stdio.h>
#include "key.h"

unsigned int  uiTms;                	// ����ֵ
unsigned int  uiSec;                	// ��ֵ
unsigned char ucState;              	// ϵͳ״̬
unsigned char ucDist;               	// ����ֵ

void Led_Proc(void);
void Seg_Proc(void);
void Key_Proc(void);
// ������
void main(void)
{
  Close_Peripheral();
  T1_Init();
	
  while (1)
  {
    Seg_Proc();
    Key_Proc();
    Led_Proc();
  }
}

unsigned char pucSeg_Char[12];      	// ��ʾ�ַ�
unsigned char pucSeg_Code[8];       	// ��ʾ����
unsigned char ucSeg_Pos;            	// ��ʾλ��
unsigned int  uiSeg_Dly;            	// ��ʾˢ����ʱ
unsigned char ucSeg_Dly;            	// ��ʾ��λ��ʱ
// ע�⣺sprintf()�����ַ���������ӡ�\0��������pucSeg_Buf[]�ĳ�������ӦΪ9��
// ����ַ����а���С���㣬pucSeg_Buf[]�ĳ�������ӦΪ10��
void Seg_Proc(void)
{
  if (uiSeg_Dly > 300)              	// 300msˢ��1��
  {
    uiSeg_Dly = 0;

    switch (ucState & 0xf0)
    {
      case 0:                        	// ��ʾT1ʱ��
        sprintf(pucSeg_Char, "1   %04u", (int)uiSec);
        break;
      case 0x10:                     	// ��ʾ����ֵ
        ucDist = Dist_Meas();
        sprintf(pucSeg_Char, "2   %04u", (int)ucDist);
    }
    Seg_Tran(pucSeg_Char, pucSeg_Code);
  }
  if (ucSeg_Dly >= 2)               	// 2ms��λ1��
  {
    ucSeg_Dly = 0;

    Seg_Disp(pucSeg_Code[ucSeg_Pos], ucSeg_Pos);
    ucSeg_Pos = ++ucSeg_Pos & 7;    	// �����ѭ����ʾ
  }
}

unsigned char ucKey_Old;            	// �ɼ�ֵ
unsigned char ucKey_Dly;            	// ����ˢ����ʱ
unsigned int  uiKey_Dly=500;        	// ˫���򳤰���ʱ
void Key_Proc(void)
{
  unsigned char ucKey_Dn;           	// ���¼�ֵ
  unsigned char ucKey_Up;           	// �ɿ���ֵ

  if(ucKey_Dly < 10)                	// ��ʱ10ms����
    return;
  ucKey_Dly = 0;

  ucKey_Dn = Key_Read();            	// ��ֵ��ȡ
  if (ucKey_Dn != ucKey_Old)        	// ��ֵ�仯
  {
    ucKey_Up = ucKey_Old;           	// �����ֵ
    ucKey_Old = ucKey_Dn;
  } else {
    ucKey_Up = 0;                   	// �����ֵ
    ucKey_Dn = 0;
  }

  if (ucKey_Dn == 4)                	// S4����
  {
    ucState ^= 0x10;                	// �޸���״̬��L5��ת��
    if (uiKey_Dly < 500)            	// S4˫��
      ucState ^= 2;                 	// �޸Ĵ�״̬��L2��ת��
    uiKey_Dly = 0;                  	// ��ʼ��ʱ
  }

  if (ucKey_Up == 4)                	// S4�ɿ�
      ucState ^= 1;                 	// �޸Ĵ�״̬��L1��ת��
  if ((ucKey_Old == 4) && (uiKey_Dly > 1000))
  {                                 	// S4����
    ucState ^= 4;                   	// �޸Ĵ�״̬��L3��ת��
    uiKey_Dly = 0;                  	// ���¼�ʱ
  }
}

void Led_Proc(void)
{
  Led_Disp(ucState);
}
