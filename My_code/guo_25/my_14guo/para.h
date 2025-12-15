//ISR
u32 T1_1MS=0;
// SEG
u8 SEG_DP[10]="3333";
u8 SEG_CD[8]={0};
u8 SEG_PS=0;
u8 FLAG_SEG=0;

u8 page=0;
u8 page_M=0;//0 cm;1 m
u8 page_P=0;//0 D;1 T
u8 page_S=0;//0 F1;1 F2;3 F3 

u8 PARA_D=0;
u8 PARA_T=0;
//TEP
u16 TEMP//*10

//WAVE
sbit waveP10=P1^0;
sbit waveP10=P1^0;
u8 FALG_WAVE=0;
u8 FALG_Dget=0;
u8 FALG_Dout=0;
u16 DISTEMT_RC[60]={0};
u16 DISTENT=0;//cm
u16 WAVE_V=340;
s16 ADJ_V=0;

//DAC
u8 BT_V=0;//10*