#include "_config.h"

#if K_MAX31855_EN==1
/*
         SCK   SS      MISO    
通道一   P5.2  P5.1    P5.0    
通道二   P1.1  P1.0    P5.3    
通道三   P2.2  P4.0    P3.3    

*/
//通道1  SCK 5.2   SS 5.1-----------------------------------------------------------------
#define   SPI_CLK_BIT     BIT2
#define   SPI_CS_BIT      BIT1

#define   SPI_PSEL        P5SEL
#define   SPI_PDIR        P5DIR
#define   SPI_POUT        P5OUT
#define   SPI_PREN        P5REN

#define   SPI_CS_L        SPI_PDIR|=SPI_CS_BIT;SPI_PREN&=~SPI_CS_BIT;SPI_POUT&=~SPI_CS_BIT;
#define   SPI_CS_H        SPI_PDIR|=SPI_CS_BIT;SPI_PREN|=SPI_CS_BIT;SPI_POUT|=SPI_CS_BIT;

#define   SPI_CLK_L       SPI_PDIR|=SPI_CLK_BIT;SPI_PREN&=~SPI_CLK_BIT;SPI_POUT&=~SPI_CLK_BIT;
#define   SPI_CLK_H       SPI_PDIR|=SPI_CLK_BIT;SPI_PREN|=SPI_CLK_BIT;SPI_POUT|=SPI_CLK_BIT;

//DATA  5.0
#define   SPI_DATA_BIT    BIT0
#define   SPI_DATA_PIN    P5IN
#define   SPI_DATA_DIR    P5DIR

#define   SPI_DATA_IN     SPI_DATA_DIR&=~SPI_DATA_BIT;


//通道2  SCK 1.1  SS 1.0  ------------------------------------------------------------------
#define   SPI2_CLK_BIT     BIT1
#define   SPI2_CS_BIT      BIT0

#define   SPI2_PSEL        P1SEL
#define   SPI2_PDIR        P1DIR
#define   SPI2_POUT        P1OUT
#define   SPI2_PREN        P1REN

#define   SPI2_CS_L        SPI2_PDIR|=SPI2_CS_BIT;SPI2_PREN&=~SPI2_CS_BIT;SPI2_POUT&=~SPI2_CS_BIT;
#define   SPI2_CS_H        SPI2_PDIR|=SPI2_CS_BIT;SPI2_PREN|=SPI2_CS_BIT;SPI2_POUT|=SPI2_CS_BIT;

#define   SPI2_CLK_L       SPI2_PDIR|=SPI2_CLK_BIT;SPI2_PREN&=~SPI2_CLK_BIT;SPI2_POUT&=~SPI2_CLK_BIT;
#define   SPI2_CLK_H       SPI2_PDIR|=SPI2_CLK_BIT;SPI2_PREN|=SPI2_CLK_BIT;SPI2_POUT|=SPI2_CLK_BIT;

//DATA  5.3
#define   SPI2_DATA_BIT    BIT3
#define   SPI2_DATA_PIN    P5IN
#define   SPI2_DATA_DIR    P5DIR

#define   SPI2_DATA_IN     SPI2_DATA_DIR&=~SPI2_DATA_BIT;


//通道3  SCK 2.2  SS 4.0------------------------------------------------------------------------
#define   SPI3_CLK_BIT     BIT2
#define   SPI3_PSEL        P2SEL
#define   SPI3_PDIR        P2DIR
#define   SPI3_POUT        P2OUT
#define   SPI3_PREN        P2REN

#define   SPI3_CLK_L       SPI3_PDIR|=SPI3_CLK_BIT;SPI3_PREN&=~SPI3_CLK_BIT;SPI3_POUT&=~SPI3_CLK_BIT;
#define   SPI3_CLK_H       SPI3_PDIR|=SPI3_CLK_BIT;SPI3_PREN|=SPI3_CLK_BIT;SPI3_POUT|=SPI3_CLK_BIT;

#define   SPI3_CS_BIT      BIT0
#define   SPI3_2PSEL       P4SEL
#define   SPI3_2PDIR       P4DIR
#define   SPI3_2POUT       P4OUT
#define   SPI3_2PREN       P4REN

#define   SPI3_CS_L        SPI3_2PDIR|=SPI3_CS_BIT;SPI3_2PREN&=~SPI3_CS_BIT;SPI3_2POUT&=~SPI3_CS_BIT;
#define   SPI3_CS_H        SPI3_2PDIR|=SPI3_CS_BIT;SPI3_2PREN|=SPI3_CS_BIT;SPI3_2POUT|=SPI3_CS_BIT;

//DATA  3.3
#define   SPI3_DATA_BIT    BIT3
#define   SPI3_DATA_PIN    P3IN
#define   SPI3_DATA_DIR    P3DIR

#define   SPI3_DATA_IN     SPI3_DATA_DIR&=~SPI3_DATA_BIT;


//IO初始
void k_io_ini(void)
{
  SPI_CLK_H;SPI_CS_H;SPI2_CLK_H;SPI2_CS_H;SPI3_CLK_H;SPI3_CS_H;

}

//IO，全部置0，是为关电源后，省电
void k_io_all_l(void)
{
  SPI_CLK_L;SPI_CS_L;SPI2_CLK_L;SPI2_CS_L;SPI3_CLK_L;SPI3_CS_L;

}

u32 SpiRead(void)
{
  u8 j;
  u32 u32bit;
  
  SPI_CS_L;
  
  SPI_DATA_IN;
  SPI_CLK_L;
  
  u32bit=0;
  for(j=0;j<32;j++)//读取全部32位数据
  {
     NOP;NOP;NOP;
     SPI_CLK_H;
     u32bit=u32bit<<1;
     if(SPI_DATA_PIN&SPI_DATA_BIT)
     {
       u32bit|=0x01;
     }
     SPI_CLK_L;
     NOP;NOP;NOP;
  }
  SPI_CLK_L;
  SPI_CS_H;
  
  return u32bit;
}

u32 Spi2Read(void)
{
  u8 j;
  u32 u32bit;
  
  SPI2_CS_L;
  
  SPI2_DATA_IN;
  SPI2_CLK_L;
  
  u32bit=0;
  for(j=0;j<32;j++)//读取全部32位数据
  {
     NOP;NOP;NOP;
     SPI2_CLK_H;
     u32bit=u32bit<<1;
     if(SPI2_DATA_PIN&SPI2_DATA_BIT)
     {
       u32bit|=0x01;
     }
     SPI2_CLK_L;
     NOP;NOP;NOP;
  }
  SPI2_CLK_L;
  SPI2_CS_H;
  
  return u32bit;
}
u32 Spi3Read(void)
{
  u8 j;
  u32 u32bit;
  
  SPI3_CS_L;
  
  SPI3_DATA_IN;
  SPI3_CLK_L;
  
  u32bit=0;
  for(j=0;j<32;j++)//读取全部32位数据
  {
     NOP;NOP;NOP;
     SPI3_CLK_H;
     u32bit=u32bit<<1;
     if(SPI3_DATA_PIN&SPI3_DATA_BIT)
     {
       u32bit|=0x01;
     }
     SPI3_CLK_L;
     NOP;NOP;NOP;
  }
  SPI3_CLK_L;
  SPI3_CS_H;
  
  return u32bit;
}

void Get_K_Temp(int* out,u8 ch)
{
  
    u32 u32bit;
    u8 buff[4];
    int data;
    float ft;

    if(ch==0)
    {
       u32bit=SpiRead();
    }
    else if(ch==1)
    {
      u32bit=Spi2Read();
    }
    else if(ch==2)
    {
      u32bit=Spi3Read();
    }
  
    buff[0]=0xff&(u32bit>>24);
    buff[1]=0xff&(u32bit>>16);
    buff[2]=0xff&(u32bit>>8);
    buff[3]=0xff&(u32bit>>0);
   
    if(((buff[1] & 0x01) == 0x01)||((buff[1] & 0x02) == 0x02)||((buff[3] & 0x08) == 0x08)) //判断是否有错误存在
    {
          if((buff[3] & 0x04) == 0x04)//短路到VCC
          {
            NOP;
          }
          if((buff[3] & 0x02) == 0x02)//短路到GND
          {
            NOP;
          }
          if((buff[3] & 0x01) == 0x01)//开路
          {
            NOP;
          }
          
          out[0]=SENIOR_NULL;
          out[1]=SENIOR_NULL;
    }
    else
    {
        //热端温度
        if((buff[0] & 0x80) == 0x80) //符号位1,负温度
        {
            data = ((0x3fff - (((buff[0] << 8) + buff[1]) >> 2)) + 1);
            ft = (0 - (data * 0.25)); //计算热电偶温度
        }
        else
        { 
            data = (((buff[0] << 8) + buff[1]) >> 2);
            ft = (data * 0.25); //正热电偶
        }
        out[0]=(int)(ft*10.0);
          
        
        //冷端温度
        if((buff[2] & 0x80) == 0x80)
        {
            data = ((0xfff - (((buff[2] << 8) + buff[3]) >> 4)) + 1);
            ft = (0 - (data * 0.0625));
        }
        else
        {
            data = (((buff[2] << 8) + buff[3]) >> 4);
            ft = (data * 0.0625);   
  
        }
        
        out[1]=(int)(ft*10.0);

    }
}

#endif


