// LED 参数
u8 LED_OUT = 0;
u8 FLAG_LIGHT = 0;
u8 FLAG_L1 = 0,
   FLAG_L2 = 0;
// ISR 参数
u32 T1_1ms = 0;
// SEG 参数
u8 SEG_DP[12] = "123442";
u8 SEG_CD[8] = {0};
u8 SEG_PO = 0;
u8 FLAG_SEG = 0;
u8 page = 0,
   page_tpL = 0,
   page_PARA = 0,
   page_BACK = 0;
// WAV 参数
sbit waveP10 = P1 ^ 0;
sbit waveP11 = P1 ^ 1;
sbit waveP34 = P3 ^ 4;
u8 FLAG_WAVE = 0;
u8 FLAG_ULT = 0;
u8 WAVE_DUTY = 0;
s8 L_ADJ = 0;
s16 L_PARA = 100;
s16 LVAL = 0;
s16 L_MAX = 0;
// TEP 参数
u16 TEP_VAL = 24;
// ADC 参数
u8 FLAG_8591 = 0;
u8 chnal=0;
u16 ADC_V[3]={0};
// TIM 参数
u8 TIM_NOW[3] = {0};
u8 TIM_TRIG[3] = {0};
u8 TIM_Set[3] = {12, 59, 45};
u8 FLAG_TIM = 0;
// URT 参数
u8 Error_D = 0;
u8 FLAG_URT=0;
// EErom
u8 EE_D[2] = {0};