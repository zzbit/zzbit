#include "_config.h"

#if PT_MAX31865_EN==1 //RTD PT100

/*
CLK-5.1    SI-5.0    SO-5.3   CS-P1.0
*/

#define   RTD_CLK_BIT     BIT1
#define   RTD_SI_BIT      BIT0
#define   RTD_SO_BIT      BIT3

#define   RTD_PSEL        P5SEL
#define   RTD_PDIR        P5DIR
#define   RTD_POUT        P5OUT
#define   RTD_PREN        P5REN

#define   RTD_CLK_L       RTD_PDIR|=RTD_CLK_BIT;RTD_PREN&=~RTD_CLK_BIT;RTD_POUT&=~RTD_CLK_BIT;
#define   RTD_CLK_H       RTD_PDIR|=RTD_CLK_BIT;RTD_PREN|=RTD_CLK_BIT;RTD_POUT|=RTD_CLK_BIT;

#define   RTD_SI_L        RTD_PDIR|=RTD_SI_BIT;RTD_PREN&=~RTD_SI_BIT;RTD_POUT&=~RTD_SI_BIT;
#define   RTD_SI_H        RTD_PDIR|=RTD_SI_BIT;RTD_PREN|=RTD_SI_BIT;RTD_POUT|=RTD_SI_BIT;

#define   RTD_SO_L        RTD_PDIR|=RTD_SO_BIT;RTD_PREN&=~RTD_SO_BIT;RTD_POUT&=~RTD_SO_BIT;
#define   RTD_SO_H        RTD_PDIR|=RTD_SO_BIT;RTD_PREN|=RTD_SO_BIT;RTD_POUT|=RTD_SO_BIT;


//DATA  
#define   RTD_DATA_BIT    RTD_SO_BIT
#define   RTD_DATA_PIN    P5IN
#define   RTD_DATA_DIR    P5DIR

#define   RTD_DATA_IN     RTD_DATA_DIR&=~RTD_DATA_BIT;


//CS  P1.0
#define   RTD_CS_BIT      BIT0
#define   RTD_CS_L        P1DIR|=RTD_CS_BIT;P1REN&=~RTD_CS_BIT;P1OUT&=~RTD_CS_BIT;
#define   RTD_CS_H        P1DIR|=RTD_CS_BIT;P1REN|=RTD_CS_BIT;P1OUT|=RTD_CS_BIT;

/*
//CS2  P1.1
#define   RTD_CS2_BIT     BIT1
#define   RTD_CS2_L       P1DIR|=RTD_CS2_BIT;P1REN&=~RTD_CS2_BIT;P1OUT&=~RTD_CS2_BIT;
#define   RTD_CS2_H       P1DIR|=RTD_CS2_BIT;P1REN|=RTD_CS2_BIT;P1OUT|=RTD_CS2_BIT;

//CS3  P2.2
#define   RTD_CS3_BIT     BIT2
#define   RTD_CS3_L       P2DIR|=RTD_CS3_BIT;P2REN&=~RTD_CS3_BIT;P2OUT&=~RTD_CS3_BIT;
#define   RTD_CS3_H       P2DIR|=RTD_CS3_BIT;P2REN|=RTD_CS3_BIT;P2OUT|=RTD_CS3_BIT;

//CS5  P3.3
#define   RTD_CS4_BIT     BIT3
#define   RTD_CS4_L       P3DIR|=RTD_CS4_BIT;P3REN&=~RTD_CS4_BIT;P3OUT&=~RTD_CS4_BIT;
#define   RTD_CS4_H       P3DIR|=RTD_CS4_BIT;P3REN|=RTD_CS4_BIT;P3OUT|=RTD_CS4_BIT;
*/

#define  CS_1_L     RTD_CS_L;
#define  CS_1_H     RTD_CS_H;

/*
#define  CS_1_L     RTD_CS_L;RTD_CS2_H;RTD_CS3_H;RTD_CS4_H;
#define  CS_1_H     RTD_CS_H;RTD_CS2_H;RTD_CS3_H;RTD_CS4_H;

#define  CS_2_L     RTD_CS_H;RTD_CS2_L;RTD_CS3_H;RTD_CS4_H;
#define  CS_2_H     RTD_CS_H;RTD_CS2_H;RTD_CS3_H;RTD_CS4_H;


#define  CS_3_L     RTD_CS_H;RTD_CS2_H;RTD_CS3_L;RTD_CS4_H;
#define  CS_3_H     RTD_CS_H;RTD_CS2_H;RTD_CS3_H;RTD_CS4_H;

#define  CS_4_L     RTD_CS_H;RTD_CS2_H;RTD_CS3_H;RTD_CS4_L;
#define  CS_4_H     RTD_CS_H;RTD_CS2_H;RTD_CS3_H;RTD_CS4_H;
*/

//IO初始
void rtd_io_ini(void)
{
  RTD_CLK_H;RTD_CS_H;
  //RTD_CS2_H;RTD_CS3_H;RTD_CS4_H;

}

//IO，全部置0，是为关电源后，省电
void rtd_io_all_l(void)
{
  RTD_CLK_L;RTD_SI_L;RTD_SO_L;RTD_CS_L;
  //RTD_CS2_L;RTD_CS3_L;RTD_CS4_L;

}

//CS,高低选择 ，cc=0,L  cc=1,H
void CS_X(u8 ch,u8 cc)
{
  if(cc==0)
  {
      if(ch==0)
      {
        CS_1_L;
      }
      /*
      else if(ch==1)
      {
        CS_2_L;
      }
      else if(ch==2)
      {
        CS_3_L;
      }
      else if(ch==3)
      {
        CS_4_L;
      }*/
  }
  else
  {
      if(ch==0)
      {
        CS_1_H;
      }
      /*
      else if(ch==1)
      {
        CS_2_H;
      }
      else if(ch==2)
      {
        CS_3_H;
      }
      else if(ch==3)
      {
        CS_4_H;
      }*/
  }
}

//读地址addr , len个字节
void rtd_read(u8 addr,u8* out,u8 len,u8 ch)
{
  u8 j,i;
  u8 u8bit;
  

  CS_X(ch,0);
  RTD_CLK_H;
  
  //寄存器地址
  RTD_SO_H;
  for(j=0;j<8;j++)//读取全部32位数据
  {
     NOP;NOP;NOP;NOP;NOP;NOP;
     RTD_CLK_L;
    
     u8bit=(addr<<j)&0x80;
     if(u8bit)
     {
       RTD_SI_H;
     }
     else
     {
       RTD_SI_L;
     }
     NOP;NOP;NOP;NOP;NOP;NOP;
     RTD_CLK_H;
  }
  
  
  //读len个字节
  RTD_DATA_IN;
  
  for(i=0;i<len;i++)
  {
      u8bit=0;
      for(j=0;j<8;j++)//读取全部32位数据
      {
        NOP;NOP;NOP;NOP;NOP;NOP;
         RTD_CLK_L;
         u8bit=u8bit<<1;
         if(RTD_DATA_PIN&RTD_DATA_BIT)
         {
           u8bit|=0x01;
         }
         NOP;NOP;NOP;NOP;NOP;NOP;
         RTD_CLK_H;
      }
      out[i]=u8bit;
  }
  
  //结束
  RTD_CLK_H;
  CS_X(ch,1);
  
}

//写地址,一个字节
void rtd_write(u8 addr,u8* in,u8 len,u8 ch)
{
  u8 i,j;
  u8 u8bit;
  
  CS_X(ch,0);
  RTD_CLK_H;
  
  //寄存器地址
  RTD_SO_H;
  for(j=0;j<8;j++)
  {
    NOP;NOP;NOP;NOP;NOP;NOP;
     RTD_CLK_L;
     u8bit=(addr<<j)&0x80;
     if(u8bit)
     {
       RTD_SI_H;
     }
     else
     {
       RTD_SI_L;
     }
     NOP;NOP;NOP;NOP;NOP;NOP;
     RTD_CLK_H;
  }
  
  
  //len个字节
  for(i=0;i<len;i++)
  {
      for(j=0;j<8;j++)
      {
         NOP;NOP;NOP;NOP;NOP;NOP;
         RTD_CLK_L;
         
         u8bit=(in[i]<<j)&0x80;
         if(u8bit)
         {
           RTD_SI_H;
         }
         else
         {
           RTD_SI_L;
         }
         NOP;NOP;NOP;NOP;NOP;NOP;
         RTD_CLK_H; 
      }
  }
  
  //结束
  RTD_CLK_H;
  CS_X(ch,1);
}


void max31865_ini(u8 ch)
{
   u8 buf[4];
  
  //0xD1,  Auto Mode, 3-Wire, 50Hz
  //0x91,Manual Mode, 3-Wire, 50Hz
  #define MAX31865_CONFIG   0X91

   //ini-------------------------------
   buf[0]=MAX31865_CONFIG;
   rtd_write(0x80,buf,1,ch);

   delay_ms(1);
   
   buf[0]=0xff;
   buf[1]=0xff;
   buf[2]=0x00;
   buf[3]=0x00;
   rtd_write(0x83,&buf[0],4,ch);
}

//开始转换 
void max31865_start(u8 ch)
{
  u8 buf[1];
  buf[0]=MAX31865_CONFIG|0x20;
  rtd_write(0x80,buf,1,ch);
}

//读温度-----------------------------
int max31865_temp(u8 ch)
{
    #define R_REF           400//PT100是400、PT1000是4000
  
    int vv;
    u8 buf[2];
    u8 RTD_H,RTD_L;
    float AD_Value;                //store the value of RTD MSB and RTD LSB register
    float RTD_Resistor;            //RTD resistor value based on RTD_MSB and RTD_LSB register
    float Corrected_Temperature;   //Temperature after correction
  
     rtd_read(0x01,buf,2,ch);
     RTD_H=buf[0];
     RTD_L=buf[1];
     
      AD_Value=((((u16)RTD_H<<8)|RTD_L)&0xFFFE)>>1;
      
      //550-24370  300-17371  -200-1517
      if((AD_Value>=1517)&&(AD_Value<=17371))//AD值不能超表中范围300  zz
      {
          RTD_Resistor=((AD_Value*R_REF)/32768.0);
                
          if(RTD_Resistor>=(R_REF/4))//正温
          {
              Corrected_Temperature=((sqrt(pow(0.0039083,2)+4*0.0000005775*(1-(RTD_Resistor/R_REF)*4))-0.0039083)/(2*(0-0.0000005775)));
          }
          else//负温
          {
             Corrected_Temperature=0-241.96+2.2163*RTD_Resistor+0.0028541*pow(RTD_Resistor,2)-0.000009912*pow(RTD_Resistor,3)+0.000000017052*pow(RTD_Resistor,4);
          }
          vv=(int)(Corrected_Temperature*10);//带1位小数
      }
      else
      {
          vv=SENIOR_NULL;
      }
       
      return vv;
}






#endif
