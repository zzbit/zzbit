#include "_config.h"

StructDownload Download;
StructStateA1 StateA1;
StructStateA2 StateA2;
StructStateB StateB;
StructStateC StateC;

StructUart_1 Uart_1;
StructFlag Flag;
StrcutRtc Rtc;
StructPt Pt;
StrcutSms Sms;
StructKey Key;
StructStateB2 StateB2;



StructPrint Print;


#if BSQ_ENABLE==1
StructPcBsq PcBsq;
#endif

#if GSM_ENABLE==1
StructGsm Gsm;
#endif

#if ((GSM_ENABLE==1)||(WIRELESS_ENABLE==1)||(RF_REC_EN==1)||(RF_KP_EN==1)||(RF_RP_EN==1)||(SEN_UART_EN==1)||(SEN_U0_PM25_EN==1)||(BSQ_RTC4_EN==1))
StructUart_0 Uart_0;
#endif

#if KEY_SET_ENABLE==1
StrcutRtc RtcTemp;
#endif

void main(void)
{
  WDTCTL = WDTPW+WDTHOLD; // Stop WDT

  McuInit();
  
  #if ((LCD_NO==7)||(LCD_NO==20))
  SHDN_L;
  //BellNn(2);//zz
  #endif
  
  BuInit();
  LcdMcuRst();
  
  
  LoggerInit();
  LoggerExtInt();
  
  AdcInit();
  
  #if WK2124_EN==1
  WK_Ini();
  #endif
  
  
  TB1_START;
  TB3_START;
  
#if  (JLY_MODEL==_1_PM25_PWM_1000)
   P5SEL&=~BIT1;
   P5DIR&=~BIT1;
   Pt.pwm_L=Pt.pwm_A=0;
   TB4_PWM_START;
#endif
   
  #if RF_KP_EN==1
     TB4_KP_START;
  #endif   
   
  #if RF_MULTI_MNJ==1
  RfDataMN();
  #endif
  
  //LcdTest();
  
  //EETest();
  //RtcTest();
  //Flag.SeeComing=1;
  //FFtest();
  //PrintTest();
  //GsmTest();
  //shr1000Test();
  
  //Pt.GprsDueOut=1;
  //Ds18b20Test();
  //LcdGsmXh(31);
  
  //PcBsq.RecNum=40;
  //Pt.GprsDueOut=40;
  

  WDT_START;

  while(1)
  {
    WDT_CLR;
    
    #if RF_KP_EN==1
    RfKpSendDeel();
    #endif


    #if WK2124_EN==1
    WK_Test();
    #endif
    
    
    #if GSM_ENABLE==1
    U0Deel();
    #endif
    
    ComDeel();


    LoggerLoop();
    
    KeyDeel();

    #if ((KEY_SET_ENABLE==1)||(PRINT_NEW_EN==1)||(CO2_GSS_TTL_EN==1)||(RF_REC_EN==1)||(RF_TXRX_470M_EN==1)||(L_LCD_BAT_EN==1))
    Loop2Deel();
    #endif


    #if RF_KP_EN==1
    if(Pt.snpt==0)
    {
       __bis_SR_register(LPM3_bits + GIE);
    }
    #else
        
            #if (((GPRS_TC_EN==1)&&(TC_115200_EN==1)) || (RF_PP_115200_EN==1))
            #else
        
                  if(Download.Downloading==0)
                  {
                      #if BAUDRATE_GSM==115200
                        __bis_SR_register(LPM0_bits + GIE);
                      #else
                        __bis_SR_register(LPM3_bits + GIE);
                      #endif
                  }
                  else
                  {
                      __bis_SR_register(LPM0_bits + GIE);
                  }
        
            #endif
    #endif

  }
}

void McuInit(void)
{
  volatile unsigned int i;
  
  I2C_PORT_INIT;
  WP_H;
  
  for (i = 0; i < 0xfffe; i++);
  
  //DCO.........................................................................
  #if CPU_F==8000000
  if (CALBC1_8MHZ ==0xFF || CALDCO_8MHZ == 0xFF)
  {  
    while(1);                               // If calibration constants erased , do not load, trap CPU!!                                 
  }   
  BCSCTL1 = CALBC1_8MHZ;                    // Set DCO
  DCOCTL = CALDCO_8MHZ;  
  #elif CPU_F==16000000
  if (CALBC1_16MHZ ==0xFF || CALDCO_16MHZ == 0xFF)                                     
  {
    while(1);                               // If calibration constants erased , do not load, trap CPU!!                                 
  }   
  BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
  DCOCTL = CALDCO_16MHZ;  
  #endif
  
  
  P1SEL = 0x00;
  P1OUT = 0x00;
  P1DIR = 0xff;
  
  P2SEL = 0x00;
  P2OUT = 0x00;
  P2DIR = 0xff;
  
  P3SEL = 0x00;
  P3OUT = 0x00;
  P3DIR = 0xff;
  
  P4SEL = 0x00;
  P4OUT = 0x00;
  P4DIR = 0xff;
  
  P5SEL = 0x00;
  P5OUT = 0x00;
  P5DIR = 0xff;
  
  P6SEL = 0x00;
  P6OUT = 0x00;
  P6DIR = 0xff;
  
  #if LCD_NO==7
  POW_SET_IN;
  #endif
  
  //IO.........................................................................
  KeyPortInit();
  
  #if LCD_NO==20
  P5OUT |= BIT2;
  P5DIR |= BIT2;
  #endif
  
  #if G_20_GZ_EN==1
  I2C3_PORT_INIT;
  #endif
  
  //雨量
  P2DIR&=~BIT0;
  
  //风速风向
  #if FS_FX_ENABLE==1
  P1DIR&=~(BIT0+BIT1);
  P3DIR&=~BIT3;
  P4DIR&=~BIT0;
  P5DIR&=~0x0f;//5.0,5.1,5.2,5.3
  
  #elif ((DOOR1_BJ_EN==1)||(DOOR2_BJ_EN==1))
  
    #if DOOR1_BJ_EN==1
      P5SEL&=~BIT0;   //5.0 
      P5DIR&=~BIT0;
      P5REN&=~BIT0;
    #endif
      
    #if DOOR2_BJ_EN==1
      P5SEL&=~BIT1;   //5.1
      P5DIR&=~BIT1;
      P5REN&=~BIT1;
    #endif
      
  #endif
      
      
  #if DAC_EN==1
  mcp4821_gpio_init();    
  #endif
      
  
  
  //UART........................................................................
  U1Init(BAUDRATE_SYSTEM,0);
  RS485_CTL_L;//485接收状态
  #if U0U1_ENABLE==1
  Flag.U0U1ing=0;
  #endif
  
  //GSM模块和无线模块不能同时使用
  #if ((GSM_ENABLE==1)||(SEN_UART_EN==1))
  
     #if ((GPRS_TC_EN==1)&&(TC_115200_EN==1))
       U0Init(115200,1); 
     #else
       U0Init(BAUDRATE_GSM,0);   
    #endif
  
  SVSCTL = 0x60 + PORON;// SVS POR enabled @ 2.5V  zz
  
  #elif SEN_U0_PM25_EN==1
  U0Init(2400,0);
  
  #elif (WIRELESS_ENABLE==1)
  U0Init(BAUDRATE_WIRELESS,0);
  
  #elif RF_REC_EN==1
  
    U0Init(9600,0);
    #if RF_MULTI_EN==0
      RfDataNULL();
    #endif
  
    Pt.SampPt=RF_SAMP_TIME-1;
  
  #elif (RF_RP_EN==1)
    U0Init(9600,0);
    Rf_NULL_Rp();
  #endif
  
  
  #if RF_PP_115200_EN==1
    RfU_Init(115200,1);
  #endif
  
  
}
void Delay(u16 t)
{
   while(--t>0);
}
/*
void delay_us(u16 t)
{
  u16 j;
  for(j=0;j<t;j++)
  {
     Delay(8);
  }
}

void delay_ms(u16 t)
{
  u16 j;
  for(j=0;j<t;j++)
  {
    //Delay(8000);
    delay_us(1000);
  }
}
*/
void DelayMs(u16 cc)
{
  u16 j;
  for(j=0;j<cc;j++)
  {
    delay_ms(1);
  }
}
void Delay_100MS(u16 cc)
{
  u16 j;
  for(j=0;j<cc;j++)
  {
     WDT_CLR;
     delay_ms(100);  
  }
}
void BellOne(void)
{
    BELL_ON;
    delay_ms(30);
    BELL_OFF;
}
void BellNn(u8 n)
{
  while(n--)
  {
    BELL_ON;
    delay_ms(120);
    BELL_OFF;
    if(n>0)
    {
      delay_ms(120);
    }
  }
}




//CLK  0-ACLK  1-SMCLK
void U0Init(u32 Baudrate,u8 UART_CLK)
{
  P3SEL|=BIT4+BIT5;                             // P3.4,5 = USCI_A0 TXD/RXD
  P3DIR|=BIT4;
  P3DIR&=~BIT5;  

  if(Baudrate==9600)
  {

      UCA0CTL1 |= UCSSEL0;  // CLK = ACLK =32768
      UCA0CTL1 &=~ UCSSEL1;                  
      UCA0BR0 = 0X03;                           
      UCA0BR1 = 0X00;                              //
      UCA0MCTL = UCBRS_3 ;                      // Modulation UCBRSx = 1
      UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
      
  }
  

  else if(Baudrate==115200)
  {
      UCA0CTL1 |= UCSSEL0;  // CLK = SMCLK
      UCA0CTL1 |= UCSSEL1;                  
      UCA0BR0 = 0X45;                           
      UCA0BR1 = 0X00;                        
      UCA0MCTL = UCBRS_4 ;                    
      UCA0CTL1 &= ~UCSWRST;                  
      
  }

  
  #if OPT_EN==0
  else if(Baudrate==2400)
  {
      UCA0CTL1 |= UCSSEL0;  // CLK = ACLK =32768
      UCA0CTL1 &=~ UCSSEL1;                  
      UCA0BR0 = 0X0d;                           
      UCA0BR1 = 0X00;                              //
      UCA0MCTL = 0x0a ;                      // Modulation UCBRSx = 1
      UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
   
  }

  #endif
  
  #if RF_KP_EN==1
    IE2 &=~ UCA0RXIE;
  #else
    IE2 |= UCA0RXIE;                        // Enable USCI_A1 RX interrupt
  #endif
  

}

//CLK  0-ACLK  1-SMCLK
void U1Init(u32 Baudrate,u8 UART_CLK)
{
  P3SEL|=BIT6+BIT7;
  P3DIR|=BIT6;
  P3DIR&=~BIT7;
  
  if(Baudrate==9600)
  {

              UCA1CTL1 |= UCSSEL0; 
              UCA1CTL1 &=~ UCSSEL1; 
                //UCA1CTL1 |= UCSSEL_1;                     // CLK = ACLK =32768
              UCA1BR0 = 0X03;                           
              UCA1BR1 = 0X00;                              //
              UCA1MCTL = UCBRS_3 ;                      // Modulation UCBRSx = 1
              UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
              UC1IE |= UCA1RXIE;                        // Enable USCI_A1 RX interrupt

  }
  #if ((GPRS_TC_EN==0)||(RF_PP_115200_EN==1)||(USB_ENABLE==1))
  else if(Baudrate==115200)
  {
          #if CPU_F==8000000
          UCA1CTL1 |= UCSSEL0; 
          UCA1CTL1 |= UCSSEL1; 
          UCA1BR0 = 0X45;                            // 8M/9600
          UCA1BR1 = 0X00;                              //
          UCA1MCTL = UCBRS_4 ;                      // Modulation UCBRSx = 1
          UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
          UC1IE |= UCA1RXIE;                        // Enable USCI_A1 RX interrupt
          #elif CPU_F==16000000
          UCA1CTL1 |= UCSSEL0; 
          UCA1CTL1 |= UCSSEL1; 
          UCA1BR0 = 0X8a;                            // 16M/9600
          UCA1BR1 = 0X00;                              //
          UCA1MCTL = UCBRS_7 ;                      // Modulation UCBRSx = 6
          UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
          UC1IE |= UCA1RXIE;                        // Enable USCI_A1 RX interrupt  
          #endif
  }
  #endif
  
  UART_USB_TXD_CLR;//一定要有
}
u8 U0SendByte(u8 Byte)
{
  WDT_CLR;
  UCA0TXBUF = Byte;                           // TX -> RXed character
  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  IFG2&=~UCA0TXIFG;
  return Byte;
}

//异或码计算方法：
//不管是 7D 或 7E, 始终按原始数据进行校验。
//数据发送：如果是7D 或 7E ，发送的时候要转换后再发送：
//0x7D-> 7D 01
//0x7E-> 7D 02

u8 U0_SF_SendByte(u8 Byte)
{
  u8 bbc=0;
  WDT_CLR;
  
  if(Byte==0x7D)
  {
      UCA0TXBUF = 0x7D;                           // TX -> RXed character
      while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
      IFG2&=~UCA0TXIFG;
      
      UCA0TXBUF = 0x01;                           // TX -> RXed character
      while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
      IFG2&=~UCA0TXIFG;

  }
  else if(Byte==0x7E)
  {
      UCA0TXBUF = 0x7D;                           // TX -> RXed character
      while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
      IFG2&=~UCA0TXIFG;

      UCA0TXBUF = 0x02;                           // TX -> RXed character
      while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
      IFG2&=~UCA0TXIFG;
  }
  else
  {
      UCA0TXBUF = Byte;                           // TX -> RXed character
      while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
      IFG2&=~UCA0TXIFG;
  }
  
  bbc^=Byte;

  return bbc;
}
u8 U0SendBuf(u8* BufIn,u16 Len)
{
  u8 add=0;
  for(u16 i=0;i<Len;i++)
    add+=U0SendByte(BufIn[i]);
  return add;
}
void U0SendString(u8* string)
{
  while(*string)
  {
    U0SendByte(*string++);
  }
}
void U0SendValue(u32 cc)
{
  if(cc>=1000000)
    U0SendByte((cc/1000000)%10+0x30);   
  if(cc>=100000)
    U0SendByte((cc/100000)%10+0x30);   
  if(cc>=10000)
    U0SendByte((cc/10000)%10+0x30);  
  if(cc>=1000)   
    U0SendByte((cc/1000)%10+0x30);
  if(cc>=100)
    U0SendByte((cc/100)%10+0x30);
  if(cc>=10)
    U0SendByte((cc/10)%10+0x30);
  U0SendByte(cc%10+0x30);
}
u8 U1SendByte(u8 Byte)
{
  WDT_CLR;

#if WIRELESS_ENABLE==1
  if(Uart_0.FlagFinish==1)//从U0接收数据，则，向U0发出数据
  {
    U0SendByte(Byte);
    return Byte;
  }
  else
  {
    UCA1TXBUF = Byte;
    while (!(UC1IFG&UCA1TXIFG));         // USCI_A1 TX buffer ready?
    UC1IFG&=~UCA1TXIFG;
    return Byte;
  }
    
#else
  UCA1TXBUF = Byte;
  while (!(UC1IFG&UCA1TXIFG));         // USCI_A1 TX buffer ready?
  UC1IFG&=~UCA1TXIFG;
  return Byte;
#endif
}
u8 U1SendBuf(u8* BufIn,u16 Len)
{
  u8 add=0;
  for(u16 i=0;i<Len;i++)
  {
    add+=U1SendByte(BufIn[i]);
  }
  return add;
}
void U1SendString(u8 *string)
{
    while(*string)
    {
      U1SendByte(*string++);
    }
}
void U1Send6v(u32 cc)
{
    U1SendByte((cc/100000)%10+0x30); 
    U1SendByte((cc/10000)%10+0x30); 
    U1SendByte((cc/1000)%10+0x30);
    U1SendByte((cc/100)%10+0x30);
    U1SendByte((cc/10)%10+0x30);
    U1SendByte(cc%10+0x30);
}
void U1SendValue(u32 cc)
{
  if(cc>=1000000)
    U1SendByte((cc/1000000)%10+0x30);   
  if(cc>=100000)
    U1SendByte((cc/100000)%10+0x30);   
  if(cc>=10000)
    U1SendByte((cc/10000)%10+0x30);  
  if(cc>=1000)   
    U1SendByte((cc/1000)%10+0x30);
  if(cc>=100)
    U1SendByte((cc/100)%10+0x30);
  if(cc>=10)
    U1SendByte((cc/10)%10+0x30);
  U1SendByte(cc%10+0x30);
}
void U1SendStrPC(u8 *ss)
{
  if(1==Flag.SeeComing)
  {
    ComSendH();
    while(*ss)
    {
      U1SendByte(*ss++);
    }
    ComSendL();
  }
}
void U0_RecErr(void)
{
   WDT_CLR;
   
   TIME_U0_STOP;
   Uart_0.FlagHeadOk=0;
   Uart_0.FlagFinish=0;
   Uart_0.RecPt=0;
   
   

}
void U1_RecErr(void)
{
   TIME_U1_STOP;
   Uart_1.FlagHeadOk=0;
   Uart_1.FlagFinish=0;
   Uart_1.RecPt=0;
   #if MODBUS_ENABLE==1
   Flag.Modbus=0;
   #endif
   
   Flag.Gps=0;
   
   #if SEN_MODBUS_485_EN==1
   StateC.sen_refg=0;
   #endif
   
    #if BSQ_YIDA_EN==1
    Flag.BsqYIDA=0;
    
   
    #endif
   
}

void ComSendH(void)
{
#if WIRELESS_ENABLE==1//无线不考虑功耗
  if(Uart_0.FlagFinish==1)
  {
    //从U0发送
  }
  else
  {
     UART_USB_TXD_SET;
     RS485_CTL_H;
     SHDN_H;
  }
    
#else
  
   UART_USB_TXD_SET;
   RS485_CTL_H;
   SHDN_H;
#endif
}
void ComSendL(void)
{
#if WIRELESS_ENABLE==1//无线不考虑功耗
  if(Uart_0.FlagFinish==1)
  {
    //从U0发送
  }
  else
  {
     UART_USB_TXD_CLR;
     RS485_CTL_L;
     SHDN_L;
  }
#else
  
   UART_USB_TXD_CLR;
   RS485_CTL_L;
   SHDN_L;
#endif
}

void ClrBuf(u8* buf,u16 len)
{
  while(len--)
  {
    *buf++=0x00;
  }
  
}

void WirelessEnd(void)
{
   #if WIRELESS_ENABLE==1//无线不考虑功耗
   //SHDN_H;//必须置高
   Uart_0.FlagFinish=0;
   LED_OFF;
   #endif
}
u8 u8ToS3(u8 in,u8* ss)
{
  u8 i=0;
  
  if(in>99)
  {
    ss[i++]=(in/100)%10+0x30;
  }
  if(in>9)
  {
    ss[i++]=(in/10)%10+0x30;
  }
  ss[i++]=in%10+0x30;
  return i;
}

u8 u16ToS5(u16 in,u8* ss)
{
  u8 i=0;
  
  if(in>9999)
  {
    ss[i++]=(in/10000)%10+0x30;
  }
  if(in>999)
  {
    ss[i++]=(in/1000)%10+0x30;
  }
  if(in>99)
  {
    ss[i++]=(in/100)%10+0x30;
  }
  if(in>9)
  {
    ss[i++]=(in/10)%10+0x30;
  }
  ss[i++]=in%10+0x30;
  return i;
}

//1060 106 
//cc四舍五入，返回值为cc/10
u16 Get45(u32 cc)
{
  u16 aa;
  u32 dd;
  
  dd=cc;
  if((dd%10)>4)
    dd+=10;
  dd=dd/10;
  aa=(u16)dd;
  return aa;
}
long u16_to_long(u16 dd)
{
  long ss;
  
  if(dd&0x8000)//负
  {
    dd=dd&0x7fff;
    ss=dd;
    ss=ss*(-1);
  }
  else
  {
    ss=dd;
  }
  return ss;
}
long get_mum(long cc,u16 dd)
{
  u8 fd=0;
  long mm;
  
  if((cc==SENIOR_NULL)||(cc==HAND_STOP))
    return dd;
  if((dd==SENIOR_NULL)||(dd==HAND_STOP))
    return cc;
  
  if(dd&0x8000)
  {
    fd=1;
    dd=dd&0x7fff;
  }
  if(fd==1)
    mm=cc-dd;
  else
    mm=cc+dd;
  return mm;
}
u8 get_add(u8* buf,u16 len)
{
  u8 add=0;
  u16 j=0;
  
  for(j=0;j<len;j++)
    add+=buf[j];
  return add;
  
}
u16 get_avg(long mum,u32 nn)
{
  long ss;
  u32 tt;
  u16 xx;
  
  ss=mum*10;
  if(ss<0)
  {
    ss=ss*(-1);
    tt=ss/nn;
    xx=Get45(tt);
    xx=xx|0x8000;
  }
  else
  {
    tt=ss/nn;
    xx=Get45(tt);
  }
  return xx;
}
u16 get_min(u16 cc,u16 dd)
{
  u8 fc=0,fd=0;
  u16 aa=0;
  
  if((cc==SENIOR_NULL)||(cc==HAND_STOP))
    return dd;
  if((dd==SENIOR_NULL)||(dd==HAND_STOP))
    return cc;
  
#if ((DOOR1_BJ_EN==1)||(DOOR2_BJ_EN==1))
  if((cc>=DOOR1_OPEN)&&(cc<=DD_BJ))
    return dd;
  if((dd>=DOOR1_OPEN)&&(dd<=DD_BJ))
    return cc;
#endif
  
  
  if(cc&0x8000)
  {
    fc=1;
    cc=cc&0x7fff;
  }
  if(dd&0x8000)
  {
    fd=1;
    dd=dd&0x7fff;
  }
  
  if((fc==1)&&(fd==1))
  {
    aa=MAX(cc,dd);
    aa=aa|0x8000;
  }
  else if((fc==0)&&(fd==0))
  {
    aa=MIN(cc,dd);
  }
  else if((fc==0)&&(fd==1))
  {
    aa=dd;
    aa=aa|0x8000;
  }
  else if((fc==1)&&(fd==0))
  {
    aa=cc;
    aa=aa|0x8000;
  }
  return aa;
}
u16 get_max(u16 cc,u16 dd)
{
  u8 fc=0,fd=0;
  u16 aa=0;

  if((cc==SENIOR_NULL)||(cc==HAND_STOP))
    return dd;
  if((dd==SENIOR_NULL)||(dd==HAND_STOP))
    return cc;
  
#if ((DOOR1_BJ_EN==1)||(DOOR2_BJ_EN==1))
  if((cc>=DOOR1_OPEN)&&(cc<=DD_BJ))
    return dd;
  if((dd>=DOOR1_OPEN)&&(dd<=DD_BJ))
    return cc;
#endif
  
  if(cc&0x8000)
  {
    fc=1;
    cc=cc&0x7fff;
  }
  if(dd&0x8000)
  {
    fd=1;
    dd=dd&0x7fff;
  }
  
  if((fc==1)&&(fd==1))
  {
    aa=MIN(cc,dd);
    aa=aa|0x8000;
  }
  else if((fc==0)&&(fd==0))
  {
    aa=MAX(cc,dd);
  }
  else if((fc==0)&&(fd==1))
  {
    aa=cc;
  }
  else if((fc==1)&&(fd==0))
  {
    aa=dd;
  }
  return aa;
}
//取中值
u32 GetMidCC(u16* buf,u8 nn)
{
  u32 sum;
  if((nn%2)==0)//偶数，求中间2个值的均值
  {
    sum=(buf[(nn-1)/2]+buf[(nn-1)/2+1])/2;
  }
  else//奇数，求中间值
  {
    sum=buf[(nn-1)/2];
  }
  return sum;
}
//求buf去掉头尾共cc个值后的平均值,前提：nn必须大于cc
u32 GetMidAvgCC(u16* buf,u8 nn,u8 cc)
{
  u16 j;
  u32 sum=0;
  for(j=cc/2;j<nn-cc/2;j++)
  {
    sum+=buf[j];
  }
  sum=sum/(nn-cc);
  return sum;
}
//冒泡排法,再取中值 或 平均值
//能有效克服因偶然因素引起的波动干扰  
//对温度、液位的变化缓慢的被测参数有良好的滤波效果
//本程序只对AD值进行滤波，所以数据类型可以全部为u16型，没有负数
u16 get_mid_value(u16* buf,u8 nn)
{
  u8 i,j;
  u16 temp;
  u32 sum;
  
  for (j=0;j<nn-1;j++)
  {   
    for (i=0;i<nn-1-j;i++)
    {   
      if(buf[i]>buf[i+1])
      {   
       temp = buf[i];
       buf[i] = buf[i+1];
       buf[i+1] = temp;
      }
    }
  }
  
  if(nn<FILET_NUM)
    sum=GetMidCC(buf,nn);//取中值
  else
  {
    sum=GetMidAvgCC(buf,nn,FILET_NUM-FILET_MID);//中值与均值综合，只保留中间的FILET_X个求平均值
  }
  
  temp=(u16)sum;
  return temp;
}

//获取Bufin中第x个字符CC的位置
u16 GetXpt(u8* BufIn,u16 StartPt,u16 EndPt,u8 CC,u16 x)
{
  u16 i,j;
  i=0;
  for(j=StartPt;j<EndPt;j++)
  {
    WDT_CLR;
    if(BufIn[j]==CC)
    {
      i++;
      if(i==x)
      {
        return j;
      }
    }
  }
  return 10000;
}
//检测cc是否是数字
u8 CheckNum(u8 cc)
{
  int aa;
  aa=cc-0x30;
  if((aa>=0)&&(aa<=9))
    return 1;
  else
    return 0;
}
//检测cc是否是数字+字母
u8 CheckNC(u8 cc)
{
  int aa;
  aa=cc;
  if(((aa>='0')&&(aa<='9'))||((aa>='A')&&(aa<='Z'))||((aa>='a')&&(aa<='z')))
    return 1;
  else
    return 0;
}

//检测数组中全是数字
u8 CK_Buf_Num(u8* buf,u8 len)
{
  u8 j;
  for(j=0;j<len;j++)
  {
     if(CheckNum(buf[j])==0)
       return 0;
  }
  return 1;
}

//检测数组中全是数字+字母
u8 CK_Buf_NC(u8* buf,u8 len)
{
  u8 j;
  for(j=0;j<len;j++)
  {
     if(CheckNC(buf[j])==0)
       return 0;
  }
  return 1;
}
//比较两个数组，相等返回1，不等返回0--------------------------------------------------
u8 CheckBuf(u8* BufTmpA,u8* BufB,u16 len)
{
  while(len--)
  {
    if(*BufTmpA!=*BufB)
      return 0;
    BufTmpA++;
    BufB++;
  }
  return 1;
}
//2个字符
u8 GetPosS(u8* BufIn,u8 StartPt,u8 EndPt,u8* SS)
{
  u8 j;
  for(j=StartPt;j<EndPt;j++)
  {
    WDT_CLR;
    if(BufIn[j]==SS[0])
    {
      if(BufIn[j+1]==SS[1])
        return j;
    }
  }
  return 255;
}
//寻找15个数字
u8 GetPos15N(u8* BufIn,u8 StartPt,u8 EndPt)
{
  u8 j;

  for(j=StartPt;j<EndPt;j++)
  {
     if(CK_Buf_Num(&BufIn[j],15))
       return j;
    
  }
  return 255;
}
//寻找20个数字+字母
u8 GetPos20NC(u8* BufIn,u8 StartPt,u8 EndPt)
{
  u8 j;

  for(j=StartPt;j<EndPt;j++)
  {
     if(CK_Buf_NC(&BufIn[j],20))
       return j;
    
  }
  return 255;
}
//判断数组BufA是否包含BufB,是，返回1，否，返回0 ,BufA不能有2个连续字符与BufB重合
u8 CompBuf(u8* BufA,u8 LenA,u8* BufB,u8 LenB)
{
  u8 j,Pos;
  Pos=GetPosS(BufA,0,LenA,BufB);
  if(Pos==255) return 0;
  else
  {
    for(j=0;j<LenB;j++)
    {
      WDT_CLR;
      if(BufA[Pos+j]!=BufB[j])
        return 0;
    }
    return 1;
  }
}

void Get_u8buf_formU32(u8* buf,u32 tt)
{
    buf[0]=0xff&(tt>>24);
    buf[1]=0xff&(tt>>16);
    buf[2]=0xff&(tt>>8);
    buf[3]=0xff&(tt>>0);
}


//IEEE754标准数据格式 与 十进制 利用union类型转换
//   buf[0]=0x3f; 高
//   buf[1]=0x65;
//   buf[2]=0x84;
//   buf[3]=0xaa;
float getFt_from_ieee(u8* buf)
{
   IEEE   ieee;
   
   ieee.buf[3]=buf[0];
   ieee.buf[2]=buf[1];
   ieee.buf[1]=buf[2];
   ieee.buf[0]=buf[3];
   
   return ieee.ft;
}


//求露点Td，(已知温度T,湿度RH)
//使用条件： 0℃ < T < 60℃, 1% < RH < 100%, 0℃ < Td < 50℃
//公式： Td=(237.7*m)/(17.27-m)  其中m=(17.27*T)/(237.7+T)+ln(RH/100)
int DewpointCal(float T,float RH)
{
  u8 fh;
  int Td;
  float m;
  m=RH;
  m=m/100.0;
  m=log(m);
  m=(17.27*T)/(237.7+T)+m;
  m=(237.7*m)/(17.27-m);
  m=m*100;
  //保留1位小数
  Td=(int)m;
  fh=0;
  if(Td<0)
  {
    fh=1;
    Td=Td*(-1);
  }
  if((Td%10)>4)
    Td+=10;
  Td=Td/10;
  if(fh==1)
  {
    Td=Td*(-1)+6;//露点<0时，+0.6度
  }
  return Td;//返回值为实际值的10倍
}
/*
void delay_us(u16 cc)
{
  u16 j;
  for(j=0;j<cc;j++)
  {
    NOP;NOP;NOP;NOP;NOP;
  }
}

void delay_ms(u16 cc)
{
  u16 j;
  for(j=0;j<cc;j++)
  {
    delay_us(1000);
  }
}
*/
