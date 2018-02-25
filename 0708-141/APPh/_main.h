#ifndef __MAIN_H
#define __MAIN_H

void Delay_100MS(u16 cc);
void Delay(u16 t);
void McuInit(void);
void U0Init(u32 Baudrate,u8 UART_CLK);
void U1Init(u32 Baudrate,u8 UART_CLK);
u8 U1SendByte(u8 Byte);

u8 CheckBuf(u8* BufTmpA,u8* BufB,u16 len);
void BellNn(u8 n);
void BellOne(void);

void U0_RecErr(void);
void U1SendValue(u32 cc);
void U1SendString(u8 *string);
void U1SendStrPC(u8 *ss);


u8 U0_SF_SendByte(u8 Byte);

u8 U0SendByte(u8 Byte);
void U0SendString(u8* string);
void U0SendValue(u32 cc);
u8 U0SendBuf(u8* BufIn,u16 Len);
u8 U1SendBuf(u8* BufIn,u16 Len);
void ComSendH(void);
void ComSendL(void);

u16 get_min(u16 cc,u16 dd);
u16 get_max(u16 cc,u16 dd);
long get_mum(long cc,u16 dd);
u16 Get45(u32 cc);
u16 get_mid_value(u16* buf,u8 nn);

u16 get_avg(long mum,u32 nn);
long u16_to_long(u16 dd);

u8 u8ToS3(u8 in,u8* ss);
u8 u16ToS5(u16 in,u8* ss);

int DewpointCal(float T,float RH);

void ComSendL(void);
void WirelessEnd(void);

u16 GetXpt(u8* BufIn,u16 StartPt,u16 EndPt,u8 CC,u16 x);

u8 CheckNum(u8 cc);

void U1Send6v(u32 cc);

void U1_RecErr(void);

void DelayMs(u16 cc);

u8 CK_Buf_Num(u8* buf,u8 len);


u8 GetPosS(u8* BufIn,u8 StartPt,u8 EndPt,u8* SS);
u8 CompBuf(u8* BufA,u8 LenA,u8* BufB,u8 LenB);

void Get_u8buf_formU32(u8* buf,u32 tt);
float getFt_from_ieee(u8* buf);

u8 GetPos15N(u8* BufIn,u8 StartPt,u8 EndPt);
u8 GetPos20NC(u8* BufIn,u8 StartPt,u8 EndPt);

void ClrBuf(u8* buf,u16 len);

/*
void delay_us(u16 cc);
void delay_ms(u16 cc);
*/
/*
void delay_us(u16 t);
void delay_ms(u16 t);
*/
#endif
