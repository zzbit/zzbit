#include "_config.h"

#if DAC_EN==1


void mcp4821_gpio_init(void)
{
  P5SEL&=~(BIT0+BIT1+BIT2);
  P5DIR|=BIT0;
  P5DIR|=BIT1;
  P5DIR|=BIT2;
  
  P1SEL&=~(BIT0);
  P1DIR|=BIT0;
}


void mcp4821_out(u8 cs,int data)
{
  int i;
  int Bdata=0x0800;
  if(cs==0)
  {
    MCP4821_CS2(1);//Ƭѡ����
    MCP4821_CS1(0);//Ƭѡ����
  }
  else
  {
    MCP4821_CS2(0);
    MCP4821_CS1(1);
  }
  
  for(i=0;i<4;i++)//������Ϣ
  {
    if(i==3)
    {
        MCP4821_SDI(1);//����  1
    }
    else
    {
        MCP4821_SDI(0);//����  0 
    }
    delay_ms(MCP4821_DELAY);
    MCP4821_SCK(1);//ʱ��������
    delay_ms(MCP4821_DELAY);
    MCP4821_SCK(0);//ʱ��������
    delay_ms(MCP4821_DELAY);
  }
   for(i=0;i<12;i++)//12λ����
   {
      if((data&Bdata)==Bdata)
    {
       MCP4821_SDI(1);//����  
    }
    else
    {
       MCP4821_SDI(0);//����  
    }
     data=data<<1;
     delay_ms(MCP4821_DELAY);
     MCP4821_SCK(1);//ʱ��������
     delay_ms(MCP4821_DELAY);
     MCP4821_SCK(0);//ʱ��������
     delay_ms(MCP4821_DELAY);
    
  }
  
   if(cs==0)
   {
     MCP4821_CS1(1);//Ƭѡ����
     delay_ms(MCP4821_DELAY*2);
     MCP4821_CS1(0);//Ƭѡ����
   }
  else
  {
     MCP4821_CS2(1);//Ƭѡ����
     delay_ms(MCP4821_DELAY*2);
     MCP4821_CS2(0);//Ƭѡ����
  }

}

#define DI4   598
#define DI20  2993

u16 Dout(u8 ChNo)
{
  float FT;
  int RL,RH,VV;
  u16 AA;
  
  
  //ʵʱ����
  VV=StateC.Value[ChNo];
  
  if(VV==SENIOR_NULL)
  {
       AA=DI4;
  }
  else
  {
      //��������
      RL=*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x7f);
      if(((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x80)==0x80)
      {
        RL=RL*(-1);
      }
    
      //��������
      RH=*((char*)CH1_V_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char*)CH1_V_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x7f);
      if(((*((char*)CH1_V_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x80)==0x80)
      {
        RH=RH*(-1);
      }
    
    
      if(0x01&(StateC.FuhaoBit>>ChNo))
      {
          VV=(-1)*VV;
      }

      FT=(float)(VV-RL)*(DI20-DI4);
      FT=FT/(RH-RL)+DI4;
      
      AA=(u16)FT;
  }
  return AA;
}


void DacOutDeel(void)
{
    if(StateA2.WorkState==WORK_STATE_RECORDING)
    {
      
        //StateC.Value[0]=300;
        mcp4821_out(0,Dout(0));
        
        //StateC.Value[1]=500;
        mcp4821_out(1,Dout(1));
    }
}



#endif

