#ifndef __FSFX_H
#define __FSFX_H

//·çÏò---------------------------------------------------------------
#define FX_0_PIN   P5IN//5.0
#define FX_0_BIT   BIT0
#define FX_1_PIN   P5IN//5.1
#define FX_1_BIT   BIT1
#define FX_2_PIN   P5IN//5.2
#define FX_2_BIT   BIT2
#define FX_3_PIN   P5IN//5.3
#define FX_3_BIT   BIT3
#define FX_4_PIN   P1IN//1.0
#define FX_4_BIT   BIT0
#define FX_5_PIN   P1IN//1.1
#define FX_5_BIT   BIT1
#define FX_6_PIN   P3IN//3.3
#define FX_6_BIT   BIT3
#define FX_7_PIN   P4IN//4.0
#define FX_7_BIT   BIT0

#define FX_PORT_INIT \
        P5SEL&=~0x0f;\
        P5REN&=~0x0f;\
        P5OUT|=0x0f;\
        P5DIR&=~0x0f;\
        P1SEL&=~(BIT0+BIT1);\
        P1REN&=~(BIT0+BIT1);\
        P1OUT&=~(BIT0+BIT1);\
        P1DIR&=~(BIT0+BIT1);\
        P3SEL&=~BIT3;\
        P3REN&=~BIT3;\
        P3OUT&=~BIT3;\
        P3DIR&=~BIT3;\
        P4SEL&=~BIT0;\
        P4REN&=~BIT0;\
        P4OUT&=~BIT0;\
        P4DIR&=~BIT0;

#define FS_BIT         BIT2
#define FS_PIN         P2IN
#define FS_IFG         P2IFG
#define FS_INIT_SET    P2SEL&=~FS_BIT;P2DIR&=~FS_BIT;P2IES|=FS_BIT;P2IFG&=~FS_BIT;P2IE|=FS_BIT;
#define FS_INIT_CLR    P2IE&=~FS_BIT;





u16 GetFxValue(void);

#endif
