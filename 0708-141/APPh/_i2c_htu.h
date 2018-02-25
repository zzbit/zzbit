#ifndef __HTU_H
#define __HTU_H


/********************************************************************************************/
/* INCLUDES																					*/
/********************************************************************************************/

#define   IIC_HTU_DELAY       20//15


//G_20_ENTER_EN - 4.0  4.2  
//..............................................
//P5.0 SDA   P5.1  SCL

#if G_20_ENTER_EN==1

  #define   SDA_HTU             BIT0
  #define   SCL_HTU             BIT2
  #define   I2C_HTU_PSEL        P4SEL
  #define   I2C_HTU_PDIR        P4DIR
  #define   I2C_HTU_POUT        P4OUT
  #define   SDA_HTU_PIN         P4IN
  #define   I2C_HTU_PREN        P4REN

#else

  #define   SDA_HTU             BIT0
  #define   SCL_HTU             BIT1
  #define   I2C_HTU_PSEL        P5SEL
  #define   I2C_HTU_PDIR        P5DIR
  #define   I2C_HTU_POUT        P5OUT
  #define   SDA_HTU_PIN         P5IN
  #define   I2C_HTU_PREN        P5REN

#endif

//..................................................
#define   SDA_HTU_DIR_H       I2C_HTU_PDIR|=SDA_HTU
#define   SDA_HTU_DIR_L       I2C_HTU_PDIR&=~SDA_HTU


#define   SDA_HTU_H           I2C_HTU_PDIR|=SDA_HTU;I2C_HTU_PREN|=SDA_HTU;I2C_HTU_POUT|=SDA_HTU
#define   SDA_HTU_L           I2C_HTU_PDIR|=SDA_HTU;I2C_HTU_PREN&=~SDA_HTU;I2C_HTU_POUT&=~SDA_HTU
#define   SCL_HTU_H           I2C_HTU_PDIR|=SCL_HTU;I2C_HTU_PREN|=SCL_HTU;I2C_HTU_POUT|=SCL_HTU
#define   SCL_HTU_L           I2C_HTU_PDIR|=SCL_HTU;I2C_HTU_PREN&=~SCL_HTU;I2C_HTU_POUT&=~SCL_HTU

#define   I2C_HTU_PORT_INIT   I2C_HTU_PSEL&=~(SDA_HTU+SCL_HTU);I2C_HTU_PDIR|=(SCL_HTU+SDA_HTU);\
                              I2C_HTU_PREN|=SCL_HTU+SDA_HTU;I2C_HTU_POUT|=SCL_HTU+SDA_HTU;

/*
//SDA  P3.3   SCL  2.2
#define   SDA_HTU             BIT3
#define   I2C_HTU_PSEL        P3SEL
#define   I2C_HTU_PDIR        P3DIR
#define   I2C_HTU_POUT        P3OUT
#define   I2C_HTU_PREN        P3REN
#define   SDA_HTU_PIN         P3IN

#define   SCL_HTU             BIT2
#define   SCL_SEL             P2SEL
#define   SCL_PDIR            P2DIR
#define   SCL_PREN            P2REN
#define   SCL_POUT            P2OUT

//..................................................
#define   SDA_HTU_DIR_H       I2C_HTU_PDIR|=SDA_HTU
#define   SDA_HTU_DIR_L       I2C_HTU_PDIR&=~SDA_HTU


#define   SDA_HTU_H           I2C_HTU_PDIR|=SDA_HTU;I2C_HTU_PREN|=SDA_HTU;I2C_HTU_POUT|=SDA_HTU
#define   SDA_HTU_L           I2C_HTU_PDIR|=SDA_HTU;I2C_HTU_PREN&=~SDA_HTU;I2C_HTU_POUT&=~SDA_HTU

#define   SCL_HTU_H           SCL_PDIR|=SCL_HTU;SCL_PREN|=SCL_HTU;SCL_POUT|=SCL_HTU
#define   SCL_HTU_L           SCL_PDIR|=SCL_HTU;SCL_PREN&=~SCL_HTU;SCL_POUT&=~SCL_HTU

#define   I2C_HTU_PORT_INIT   I2C_HTU_PSEL&=~(SDA_HTU);I2C_HTU_PDIR|=(SDA_HTU);\
                              I2C_HTU_PREN|=SDA_HTU;I2C_HTU_POUT|=SDA_HTU;\
                              SCL_SEL&=~(SCL_HTU);SCL_PDIR|=(SCL_HTU);\
                              SCL_PREN|=SCL_HTU;SCL_POUT|=SCL_HTU;

*/

u16 HtuMeasure(u8 cc,u8 fh);

u16 GetHIH613X(int* outbuf);

u16 GetSHT30(int* out);

#endif
