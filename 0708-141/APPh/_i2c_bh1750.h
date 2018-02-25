#ifndef __BH1750_H
#define __BH1750_H

#define   IIC3_DELAY       20//15

//..............................................
#if LCD_NO==7

      #define   SDA3             BIT6
      #define   SCL3             BIT5
      #define   I2C3_PSEL        P2SEL
      #define   I2C3_PDIR        P2DIR
      #define   I2C3_POUT        P2OUT
      #define   SDA3_PIN         P2IN
      #define   I2C3_PREN        P2REN

#elif LCD_NO==8

      #define   SDA3             BIT7
      #define   SCL3             BIT6
      #define   I2C3_PSEL        P1SEL
      #define   I2C3_PDIR        P1DIR
      #define   I2C3_POUT        P1OUT
      #define   SDA3_PIN         P1IN
      #define   I2C3_PREN        P1REN


#elif ((LCD_NO==20)&&(G_20_GZ_EN==1))
/*
      #define   SDA3             BIT7
      #define   SCL3             BIT6
      #define   I2C3_PSEL        P1SEL
      #define   I2C3_PDIR        P1DIR
      #define   I2C3_POUT        P1OUT
      #define   SDA3_PIN         P1IN
      #define   I2C3_PREN        P1REN
*/

      #define   SDA3             BIT0
      #define   SCL3             BIT1
      #define   I2C3_PSEL        P2SEL
      #define   I2C3_PDIR        P2DIR
      #define   I2C3_POUT        P2OUT
      #define   SDA3_PIN         P2IN
      #define   I2C3_PREN        P2REN



#endif

//..................................................

#if ((LCD_NO==7)||(LCD_NO==8)||((LCD_NO==20)&&(G_20_GZ_EN==1)))

  #define   SDA3_DIR_H       I2C3_PDIR|=SDA3
  #define   SDA3_DIR_L       I2C3_PDIR&=~SDA3
  
  #define   SDA3_H           I2C3_PDIR&=~SDA3;I2C3_PREN|=SDA3;I2C3_POUT|=SDA3
  #define   SDA3_L           I2C3_PDIR|=SDA3;I2C3_PREN&=~SDA3;I2C3_POUT&=~SDA3
  #define   SCL3_H           I2C3_PDIR&=~SCL3;I2C3_PREN|=SCL3;I2C3_POUT|=SCL3
  #define   SCL3_L           I2C3_PDIR|=SCL3;I2C3_PREN&=~SCL3;I2C3_POUT&=~SCL3


  #define  I2C3_PORT_INIT    I2C3_PSEL&=~(SDA3+SCL3);I2C3_PDIR&=~(SCL3+SDA3);\
                             I2C3_PREN|=SCL3+SDA3;I2C3_POUT|=SCL3+SDA3;
#else

  #define  I2C3_PORT_INIT    NOP

#endif




#if SENCOND_GZ_EN==1

    //第2个传感器
    #define   SDABH             BIT1
    #define   SCLBH             BIT0
    #define   I2CBH_PSEL        P5SEL
    #define   I2CBH_PDIR        P5DIR
    #define   I2CBH_POUT        P5OUT
    #define   SDABH_PIN         P5IN
    #define   I2CBH_PREN        P5REN
    
    //..................................................
    #define   SDABH_DIR_H       I2CBH_PDIR|=SDABH
    #define   SDABH_DIR_L       I2CBH_PDIR&=~SDABH
    
    #define   SDABH_H           I2CBH_PDIR&=~SDABH;I2CBH_PREN|=SDABH;I2CBH_POUT|=SDABH
    #define   SDABH_L           I2CBH_PDIR|=SDABH;I2CBH_PREN&=~SDABH;I2CBH_POUT&=~SDABH
    #define   SCLBH_H           I2CBH_PDIR&=~SCLBH;I2CBH_PREN|=SCLBH;I2CBH_POUT|=SCLBH
    #define   SCLBH_L           I2CBH_PDIR|=SCLBH;I2CBH_PREN&=~SCLBH;I2CBH_POUT&=~SCLBH
    
    #define  I2CBH_PORT_INIT    I2CBH_PSEL&=~(SDABH+SCLBH);I2CBH_PDIR&=~(SCLBH+SDABH);\
                                I2CBH_PREN|=SCLBH+SDABH;I2CBH_POUT|=SCLBH+SDABH;

#endif

void LxStart(void);
u16 LxRead(void);
u16 GzEnlarge(u16 cc);


#if SENCOND_GZ_EN==1
void LxBHStart(void);
u16 LxBHRead(void);
#endif


#endif
