#ifndef __DAC_H
#define __DAC_H


#define MCP4821_CS1(a);    if(a==1)  P5OUT|=BIT2; else P5OUT&=~BIT2; 
#define MCP4821_CS2(a);    if(a==1)  P5OUT|=BIT1; else P5OUT&=~BIT1;
#define MCP4821_SCK(a);    if(a==1)  P5OUT|=BIT0; else P5OUT&=~BIT0;
#define MCP4821_SDI(a);    if(a==1)  P1OUT|=BIT0; else P1OUT&=~BIT0;

#define MCP4821_DELAY  1

void mcp4821_gpio_init(void);
void mcp4821_out(u8 cs,int data);
void DacOutDeel(void);


#endif

