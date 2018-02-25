#ifndef __I2C_RTC_H
#define __I2C_RTC_H


#define   SDA2             BIT4
#define   SCL2             BIT3

#define   I2C2_PSEL        P2SEL
#define   I2C2_PDIR        P2DIR
#define   I2C2_POUT        P2OUT
#define   SDA2_PIN         P2IN
#define   I2C2_PREN        P2REN

#define   SDA2_DIR_H       I2C2_PDIR|=SDA2
#define   SDA2_DIR_L       I2C2_PDIR&=~SDA2

#define   SDA2_H           I2C2_PDIR&=~SDA2;I2C2_PREN|=SDA2;I2C2_POUT|=SDA2
#define   SDA2_L           I2C2_PDIR|=SDA2;I2C2_PREN&=~SDA2;I2C2_POUT&=~SDA2
#define   SCL2_H           I2C2_PDIR&=~SCL2;I2C2_PREN|=SCL2;I2C2_POUT|=SCL2
#define   SCL2_L           I2C2_PDIR|=SCL2;I2C2_PREN&=~SCL2;I2C2_POUT&=~SCL2

#define   I2C2_PORT_INIT   I2C2_PSEL&=~(SDA2+SCL2);I2C2_PDIR&=~(SCL2+SDA2);\
                           I2C2_PREN|=SCL2+SDA2;I2C2_POUT|=SCL2+SDA2;

#define   IIC2_DELAY       10

void RtcInit(void);
void RtcReadTime(void);
void RtcSetTime(void);

u32 ReadRtcD10(void);

void RtcTest(void);

#endif
