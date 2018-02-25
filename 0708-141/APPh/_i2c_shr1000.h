#ifndef __SHR1000_H
#define __SHR1000_H

#define   IIC4_DELAY       20//15

//..............................................


#if JLY_MODEL==_8_NTC_HIH_CO2TTL5000_GZ200K_QY_NH3_H2S_CH4
// P5.1  P5.2
    #define   SDA4             BIT2
    #define   SCL4             BIT1
    #define   I2C4_PSEL        P5SEL
    #define   I2C4_PDIR        P5DIR
    #define   I2C4_POUT        P5OUT
    #define   SDA4_PIN         P5IN
    #define   I2C4_PREN        P5REN


#else

    #define   SDA4             BIT6
    #define   SCL4             BIT5
    #define   I2C4_PSEL        P2SEL
    #define   I2C4_PDIR        P2DIR
    #define   I2C4_POUT        P2OUT
    #define   SDA4_PIN         P2IN
    #define   I2C4_PREN        P2REN
    
    /*
    #define   SDA4             BIT1
    #define   SCL4             BIT0
    #define   I2C4_PSEL        P4SEL
    #define   I2C4_PDIR        P4DIR
    #define   I2C4_POUT        P4OUT
    #define   SDA4_PIN         P4IN
    #define   I2C4_PREN        P4REN
    */
    
    /*
    #define   SDA4             BIT2
    #define   SCL4             BIT1
    #define   I2C4_PSEL        P5SEL
    #define   I2C4_PDIR        P5DIR
    #define   I2C4_POUT        P5OUT
    #define   SDA4_PIN         P5IN
    #define   I2C4_PREN        P5REN
    */

#endif
    
    //..................................................
    #define   SDA4_DIR_H       I2C4_PDIR|=SDA4
    #define   SDA4_DIR_L       I2C4_PDIR&=~SDA4
    
    #define   SDA4_H           I2C4_PDIR|=SDA4;I2C4_PREN|=SDA4;I2C4_POUT|=SDA4
    #define   SDA4_L           I2C4_PDIR|=SDA4;I2C4_PREN&=~SDA4;I2C4_POUT&=~SDA4
    #define   SCL4_H           I2C4_PDIR|=SCL4;I2C4_PREN|=SCL4;I2C4_POUT|=SCL4
    #define   SCL4_L           I2C4_PDIR|=SCL4;I2C4_PREN&=~SCL4;I2C4_POUT&=~SCL4
    
    #define   I2C4_PORT_INIT   I2C4_PSEL&=~(SDA4+SCL4);I2C4_PDIR|=(SCL4+SDA4);\
                               I2C4_PREN|=SCL4+SDA4;I2C4_POUT|=SCL4+SDA4;





void shr1000Write(void);
u16 shr1000ReadPressure(void);
u16 shr1000ReadTemp(void);


#endif
