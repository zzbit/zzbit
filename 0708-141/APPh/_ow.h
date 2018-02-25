#ifndef __OW_H
#define __OW_H

//P5.2-------------------------------------------------------
#define   OW_BIT         BIT2

#define   OW_PSEL        P5SEL
#define   OW_PDIR        P5DIR
#define   OW_POUT        P5OUT
#define   OW_PIN         P5IN
#define   OW_PREN        P5REN

#define   DQ_DDR_H       OW_PDIR|=OW_BIT
#define   DQ_DDR_L       OW_PDIR&=~OW_BIT
#define   DQ_H           DQ_DDR_L;OW_PREN|=OW_BIT;OW_POUT|=OW_BIT
#define   DQ_L           DQ_DDR_H;OW_PREN&=~OW_BIT;OW_POUT&=~OW_BIT
#define   DQ_IN          OW_PIN&OW_BIT

//P5.1-----------------------------------------------------
#define   OW2_BIT         BIT1

#define   OW2_PSEL        P5SEL
#define   OW2_PDIR        P5DIR
#define   OW2_POUT        P5OUT
#define   OW2_PIN         P5IN
#define   OW2_PREN        P5REN

#define   DQ2_DDR_H       OW2_PDIR|=OW2_BIT
#define   DQ2_DDR_L       OW2_PDIR&=~OW2_BIT
#define   DQ2_H           DQ2_DDR_L;OW2_PREN|=OW2_BIT;OW2_POUT|=OW2_BIT
#define   DQ2_L           DQ2_DDR_H;OW2_PREN&=~OW2_BIT;OW2_POUT&=~OW2_BIT
#define   DQ2_IN          OW2_PIN&OW2_BIT

//P5.0-----------------------------------------------------
#define   OW3_BIT         BIT0

#define   OW3_PSEL        P5SEL
#define   OW3_PDIR        P5DIR
#define   OW3_POUT        P5OUT
#define   OW3_PIN         P5IN
#define   OW3_PREN        P5REN

#define   DQ3_DDR_H       OW3_PDIR|=OW3_BIT
#define   DQ3_DDR_L       OW3_PDIR&=~OW3_BIT
#define   DQ3_H           DQ3_DDR_L;OW3_PREN|=OW3_BIT;OW3_POUT|=OW3_BIT
#define   DQ3_L           DQ3_DDR_H;OW3_PREN&=~OW3_BIT;OW3_POUT&=~OW3_BIT
#define   DQ3_IN          OW3_PIN&OW3_BIT

//P1.0-------------------------------------------------------
#define   OW4_BIT         BIT0

#define   OW4_PSEL        P1SEL
#define   OW4_PDIR        P1DIR
#define   OW4_POUT        P1OUT
#define   OW4_PIN         P1IN
#define   OW4_PREN        P1REN

#define   DQ4_DDR_H       OW4_PDIR|=OW4_BIT
#define   DQ4_DDR_L       OW4_PDIR&=~OW4_BIT
#define   DQ4_H           DQ4_DDR_L;OW4_PREN|=OW4_BIT;OW4_POUT|=OW4_BIT
#define   DQ4_L           DQ4_DDR_H;OW4_PREN&=~OW4_BIT;OW4_POUT&=~OW4_BIT
#define   DQ4_IN          OW4_PIN&OW4_BIT

//P1.1-------------------------------------------------------
#define   OW5_BIT         BIT1

#define   OW5_PSEL        P1SEL
#define   OW5_PDIR        P1DIR
#define   OW5_POUT        P1OUT
#define   OW5_PIN         P1IN
#define   OW5_PREN        P1REN

#define   DQ5_DDR_H       OW5_PDIR|=OW5_BIT
#define   DQ5_DDR_L       OW5_PDIR&=~OW5_BIT
#define   DQ5_H           DQ5_DDR_L;OW5_PREN|=OW5_BIT;OW5_POUT|=OW5_BIT
#define   DQ5_L           DQ5_DDR_H;OW5_PREN&=~OW5_BIT;OW5_POUT&=~OW5_BIT
#define   DQ5_IN          OW5_PIN&OW5_BIT


//P2.2-------------------------------------------------------
#define   OW6_BIT         BIT2

#define   OW6_PSEL        P2SEL
#define   OW6_PDIR        P2DIR
#define   OW6_POUT        P2OUT
#define   OW6_PIN         P2IN
#define   OW6_PREN        P2REN

#define   DQ6_DDR_H       OW6_PDIR|=OW6_BIT
#define   DQ6_DDR_L       OW6_PDIR&=~OW6_BIT
#define   DQ6_H           DQ6_DDR_L;OW6_PREN|=OW6_BIT;OW6_POUT|=OW6_BIT
#define   DQ6_L           DQ6_DDR_H;OW6_PREN&=~OW6_BIT;OW6_POUT&=~OW6_BIT
#define   DQ6_IN          OW6_PIN&OW6_BIT

//P3.3-------------------------------------------------------
#define   OW7_BIT         BIT3

#define   OW7_PSEL        P3SEL
#define   OW7_PDIR        P3DIR
#define   OW7_POUT        P3OUT
#define   OW7_PIN         P3IN
#define   OW7_PREN        P3REN

#define   DQ7_DDR_H       OW7_PDIR|=OW7_BIT
#define   DQ7_DDR_L       OW7_PDIR&=~OW7_BIT
#define   DQ7_H           DQ7_DDR_L;OW7_PREN|=OW7_BIT;OW7_POUT|=OW7_BIT
#define   DQ7_L           DQ7_DDR_H;OW7_PREN&=~OW7_BIT;OW7_POUT&=~OW7_BIT
#define   DQ7_IN          OW7_PIN&OW7_BIT

//P5.3-------------------------------------------------------
#define   OW8_BIT         BIT3

#define   OW8_PSEL        P5SEL
#define   OW8_PDIR        P5DIR
#define   OW8_POUT        P5OUT
#define   OW8_PIN         P5IN
#define   OW8_PREN        P5REN

#define   DQ8_DDR_H       OW8_PDIR|=OW8_BIT
#define   DQ8_DDR_L       OW8_PDIR&=~OW8_BIT
#define   DQ8_H           DQ8_DDR_L;OW8_PREN|=OW8_BIT;OW8_POUT|=OW8_BIT
#define   DQ8_L           DQ8_DDR_H;OW8_PREN&=~OW8_BIT;OW8_POUT&=~OW8_BIT
#define   DQ8_IN          OW8_PIN&OW8_BIT

//------------------------------------------------------

#define   OW_PORT_INI     DQ_H;DQ2_H;DQ3_H;DQ4_H;DQ5_H;DQ6_H;DQ7_H;DQ8_H 





unsigned char OWSearch(void);
unsigned char docrc8(unsigned char value);

u16 OWSearchID(void);
int OWReadIDTemp(u8* BufID);
void OWConvertStart(void);
void Ds18b20Test(void);

int GetTemp(u8 LL,u8 HH);

void OneTstart(void);
int OneTread(void);

void One2Tstart(void);
int One2Tread(void);

void One3Tstart(void);
int One3Tread(void);

void One4Tstart(void);
int One4Tread(void);

void One5Tstart(void);
int One5Tread(void);

void One6Tstart(void);
int One6Tread(void);

void One7Tstart(void);
int One7Tread(void);


void One8Tstart(void);
int One8Tread(void);


void OneChTstart(u8 ch);
int OneChTread(u8 ch);

int OneChData(u8 ch);


#endif


