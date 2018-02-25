#ifndef __LCD_H
#define __LCD_H

#if LCD_NO==8
#define LCD_INHb_H    P4DIR|=BIT4;P4REN|=BIT4;P4OUT|=BIT4//4.4 H 片选失效 
#define LCD_INHb_L    P4DIR|=BIT4;P4REN&=~BIT4;P4OUT&=~BIT4//  L 片选有效
#define LCD_SD_H      P4DIR|=BIT5;P4REN|=BIT5;P4OUT|=BIT5//4.5
#define LCD_SD_L      P4DIR|=BIT5;P4REN&=~BIT5;P4OUT&=~BIT5
#define LCD_SCL_H     P4DIR|=BIT6;P4REN|=BIT6;P4OUT|=BIT6//4.6
#define LCD_SCL_L     P4DIR|=BIT6;P4REN&=~BIT6;P4OUT&=~BIT6
#define LCD_CSB_H     P4DIR|=BIT7;P4REN|=BIT7;P4OUT|=BIT7//4.7 H开显示
#define LCD_CSB_L     P4DIR|=BIT7;P4REN&=~BIT7;P4OUT&=~BIT7 // L关显示

#define LCD_VCTL_H    NOP//P4DIR|=BIT3;P4REN|=BIT3;P4OUT|=BIT3//4.3,H,LCD电源关
#define LCD_VCTL_L    P4DIR|=BIT3;P4REN&=~BIT3;P4OUT&=~BIT3//4.3,L,LCD电源开
#else
#define LCD_INHb_H   P4DIR|=BIT7;P4REN|=BIT7;P4OUT|=BIT7//4.7 H开显示
#define LCD_INHb_L   P4DIR|=BIT7;P4REN&=~BIT7;P4OUT&=~BIT7 // L关显示
#define LCD_SD_H     P4DIR|=BIT6;P4REN|=BIT6;P4OUT|=BIT6//4.6
#define LCD_SD_L     P4DIR|=BIT6;P4REN&=~BIT6;P4OUT&=~BIT6
#define LCD_SCL_H    P4DIR|=BIT5;P4REN|=BIT5;P4OUT|=BIT5//4.5
#define LCD_SCL_L    P4DIR|=BIT5;P4REN&=~BIT5;P4OUT&=~BIT5
#define LCD_CSB_H    P4DIR|=BIT4;P4REN|=BIT4;P4OUT|=BIT4//4.4 H 片选失效 
#define LCD_CSB_L    P4DIR|=BIT4;P4REN&=~BIT4;P4OUT&=~BIT4//  L 片选有效

#define LCD_VCTL_H    P4DIR|=BIT3;P4REN|=BIT3;P4OUT|=BIT3//4.3,H,LCD电源关
#define LCD_VCTL_L    P4DIR|=BIT3;P4REN&=~BIT3;P4OUT&=~BIT3//4.3,L,LCD电源开
#endif

void BuInit(void);
void LcdTest(void);
void LcdNull(u8 ChNo,u8 Hang);

void LcdChNoValueA(u8 ChNo,u16 Value,u8 FuhaoBit);
void LcdChNoValueB(u8 ChNo,u16 Value,u8 FuhaoBit);

void BuToLcdRam(u8 addr,u8 cc);
void LcdOFFX(u8 StopCause);
void LcdNN(u8 NN);
void LcdTime(u8 T1,u8 T2,u8 BD);
void LcdRtcTime(void);
void LcdValue(void);
void LcdChNoDeel(void);
void LcdClrUnit(void);
void LcdAlarmOn(u8 cc);
void LcdBatOn(u8 cc);
void LcdGsmXh(u8 cc);

void LcdExtremum(u16* Vbuf,u8 Ch);
void LcdShowOn(u8 cc);

void Lcd08_Off(u8 StopCause);
void Lcd08_Null(u8 ChNo);
void Lcd08_NN(u8 nn);
void Lcd08_ChValue(u8 Ch,u16 vv,u8 ff);
void Lcd08_Deel(void);
void Lcd08_Min(void);
void Lcd08_Max(void);
void Lcd08_Avg(void);
void Lcd08_Bat(u8 cc);
void Lcd08_AL(u8 cc);

void LcdMcuRst(void);

void LcdWd1Unit(void);
void LcdSd2Unit(void);
void LcdMin(void);
void LcdMax(void);
void LcdAvg(void);

void Lcd1Unit(u8 ch);
void Lcd2Unit(u8 ch);
void LcdA10(void);
void LcdLowMode(u8 cc);

void Lcd1Laba(u8 cc);
void Lcd2Laba(u8 cc);

void LcdLn(u8 cc);
void LcdChAB(u8 AB,u16 Value,u8 Kt);

void LcdRtcShan(u8 T1,u8 T2,u8 Kt);

void LcdYearFlag(void);
void LcdMonthFlag(void);
void LcdHourFlag(void);

void LcdA3dst(u16 Value,u8 cc);
void LcdB3dst(u16 Value,u8 cc);

void Lcd08_3dst(u16 vv,u8 dst);

void LcdSXCB(u8 cc);
void U0U1set(void);

void LcdClear(void);

void LcdU0U1_RF(void);

#if RF_REC_EN==1
void RfLcdChDeel(void);
#endif

void Lcd_H(u8 cc);
void Lcd_P(u8 cc);
void LcdFz(u8 NN);
void Tshan(void);

void LcdABat(void);

void Lcd_Xp(u8 cc,u8 addr,u8 ss);
void Lcd_Bc(u8 cc);

void Lcd_Bfh(void);

void LcdK1(u8 cc);
void LcdK2(u8 cc);

void LcdHHHH(void);

#if RF_YQ_SS_EN==1
//每次显示1个通道，第一行显示通道
void LcdOneChValue(u8 ch);
#endif


#if LCD_NO==20
void Lcd20_NN(u8 nn);
void Lcd20_Deel(void);
void Lcd20Time(u8 T1,u8 T2,u8 BD);
void Lcd20YearFlag(void);
void Lcd20MonthFlag(void);
void Lcd20HourFlag(void);
void Lcd20GsmXh(u8 cc);
void Lcd20Bat(void);
void Lcd08OFFX(u8 StopCause);
void Lcd20Up(u8 on);
void Lcd20H(u8 cc);

#endif


#endif


