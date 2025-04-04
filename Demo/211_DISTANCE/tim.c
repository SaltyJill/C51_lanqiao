sfr  TCON = 0x88;
sfr  TMOD = 0x89;
sfr  AUXR = 0x8E;
sfr  TL1  = 0x8B;
sfr  TH1  = 0x8D;
sbit TR1  = TCON^6;
sbit TF1  = TCON^7;

sfr  IE   = 0xA8;
sbit ET1  = IE^3;
sbit EA   = IE^7;

void T1_Init(void)                  	// 1����@12.000MHz
{
  AUXR &= 0xBF;                     	// ��ʱ��ʱ��12Tģʽ
  TMOD &= 0x0F;                     	// ���ö�ʱ��ģʽ
  TL1 = 0x18;                       	// ���ö�ʱ��ֵ��24��
  TH1 = 0xFC;                       	// ���ö�ʱ��ֵ��252��
  TF1 = 0;                          	// ���TF1��־
  TR1 = 1;                           	// ��ʱ��1��ʼ��ʱ
  ET1 = 1;                           	// ����T1�ж�
  EA  = 1;                           	// ����ϵͳ�ж�
}

extern unsigned int  uiTms;         	// ����ֵ
extern unsigned int  uiSec;         	// ��ֵ
extern unsigned int  uiSeg_Dly;     	// ��ʾˢ����ʱ
extern unsigned char ucSeg_Dly;     	// ��ʾ��λ��ʱ
extern unsigned char ucKey_Dly;     	// ����ˢ����ʱ
extern unsigned int  uiKey_Dly;     	// ˫���򳤰���ʱ
void T1_Proc(void) interrupt 3
{
  if (++uiTms == 1000)              	// 1sʱ�䵽
  {
    uiTms = 0;
    uiSec++;
  }
  uiSeg_Dly++;
  ucSeg_Dly++;
  ucKey_Dly++;
  uiKey_Dly++;
}

sfr  CCON   = 0xD8;                   	// PCA���ƼĴ���
sfr  CL      = 0xE9;                 	// PCA��8λ��ʱֵ
sfr  CH      = 0xF9;                 	// PCA��8λ��ʱֵ
sbit CR     = CCON^6;               	// PCA���п���λ
sbit CF     = CCON^7;               	// PCA�����־λ

sfr  P1     = 0x90;
sbit TX     = P1^0;
sbit RX     = P1^1;

unsigned char Dist_Meas(void)
{
  unsigned char ucNum = 10;

  // TX���ŷ���40KHz�����ź���������������̽ͷ
  TX = 0;
  CL = 0xf4;                          	// ����PCA��8λ��ʱֵ
  CH = 0xff;                          	// ����PCA��8λ��ʱֵ
  CR = 1;                             	// ����PCA
  while (ucNum--)
  {
    while (!CF);                      	// �ȴ�PCA��ʱʱ�䵽
    TX ^= 1;                           	// ��תTX
    CL = 0xf4;                        	// ����PCA��8λ��ʱֵ
    CH = 0xff;                        	// ����PCA��8λ��ʱֵ
    CF = 0;
  }
  CR = 0;                             	// �ر�PCA
  // ���ռ�ʱ
  CL = 0;                              	// ����PCA��8λ��ʱֵ
  CH = 0;                             	// ����PCA��8λ��ʱֵ
  CR = 1;
  while(RX && !CF);                 	// �ȴ��½���
  CR = 0;
  return ((CH<<8)+CL)*0.017;        	// ������룺340*100/1000000/2
}
