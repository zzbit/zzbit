#include "_config.h"
//180225 work ok en 3 4 zz cc dd ee bit
void AdcInit(void)
{
    u8 ref[8],j;
    for(j=0;j<8;j++)
    {
      ref[j]=*((char*)CH1_R_ADDR+j*ONE_CH_CONFIG_BYTES);
      #if BAT_AD_ENABLE==1
      if(j==7)
        ref[j]=_R_2V5;//电池基准电压2.5
      #endif
      
      
      if(ref[j]==_R_AVCC)
      {
        ref[j]=SREF_0;
      }
      else if(ref[j]==_R_2V5)
      {
        ref[j]=SREF_1;
      }
      else if(ref[j]==_R_EX)
      {
        ref[j]=SREF_2;
      }
    }
    
    ADC_PORT_INIT;
    
    ADC12CTL0=0;
    ADC12CTL0|=ADC12ON+MSC+SHT0_10+SHT1_10;// Turn on ADC12, set sampling time
    ADC12CTL0|=REF2_5V+REFON;//2.5V ref
    ADC12CTL1=SHP+CONSEQ_1;// Use sampling timer, single sequ
    
    ADC12MCTL0=INCH_0+ref[0];
    ADC12MCTL1=INCH_1+ref[1];
    ADC12MCTL2=INCH_2+ref[2];
    ADC12MCTL3=INCH_3+ref[3];
    ADC12MCTL4=INCH_4+ref[4];
    ADC12MCTL5=INCH_5+ref[5];
    ADC12MCTL6=INCH_6+ref[6];
    ADC12MCTL7=INCH_7+ref[7]+EOS;

    ADC12IE=BIT7;
    ADC_OFF;
    ADC_REF_OFF;
}

void AdcToValue(u16 Adc,u8 ChNo)
{
  ulonglong Zhengshu,Xiaoshu,ValueOut;//最大值 18446744073709551615
  u16 i,ValueLow,ValueHigh;
  vu8 FuhaoLow,FuhaoHigh;
  u8 Cpt;
  StructCurve Curve;
  NOP;
  NOP;
  NOP;
  CurveFlashToBuf(&Curve,ChNo);
  
  //量程下限
  ValueLow=*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x7f);
  if(((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x80)==0x80)
  {
    FuhaoLow=1;
  }
  else
  {
    FuhaoLow=0;
  }
  //量程上限
  ValueHigh=*((char*)CH1_V_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char*)CH1_V_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x7f);
  if(((*((char*)CH1_V_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo+1))&0x80)==0x80)
  {
    FuhaoHigh=1;
  }
  else
  {
    FuhaoHigh=0;
  }
  
  
  WDT_CLR;
  Cpt=0;
  while((StateC.Adc[ChNo]>Curve.Point[Cpt])&&(Cpt<Curve.PointNum))Cpt++;
  if(Cpt==0)
  {
    StateC.Value[ChNo]=ValueLow;
    if(FuhaoLow==1)
    {
      StateC.FuhaoBit|=1<<ChNo;
    }
    else
    {
      StateC.FuhaoBit&=~(1<<ChNo);
    }
  }
  else if(Cpt>=Curve.PointNum)
  {
    StateC.Value[ChNo]=ValueHigh;
    if(FuhaoHigh==1)
    {
      StateC.FuhaoBit|=1<<ChNo;
    }
    else 
    {
      StateC.FuhaoBit&=~(1<<ChNo);
    }
  }
  else
  {
      #if OPT_EN==0
      if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*ChNo)==_S_2)//2位小数
      {
        ValueLow=ValueLow/10;
        ValueHigh=ValueHigh/10;
      }
      #endif
    
      Zhengshu=0;
      for(i=0;i<Cpt-1;i++)
      {
        Zhengshu+=10*Curve.Space[i];
      }
      if(FuhaoLow==0)
      {
        StateC.FuhaoBit&=~(1<<ChNo);
        Zhengshu+=ValueLow;
      }
      else
      {
        if(Zhengshu>=ValueLow)
        {
          StateC.FuhaoBit&=~(1<<ChNo);
          Zhengshu=Zhengshu-ValueLow;
        }
        else
        {
          StateC.FuhaoBit|=1<<ChNo;
          Zhengshu=ValueLow-Zhengshu;
        }
      }
      Zhengshu=Zhengshu*1000;
      
      Xiaoshu=StateC.Adc[ChNo]-Curve.Point[Cpt-1];
      Xiaoshu=Xiaoshu*Curve.Space[Cpt-1]*10*1000;
  
      Xiaoshu=Xiaoshu/(Curve.Point[Cpt]-Curve.Point[Cpt-1]);
  
      if((StateC.FuhaoBit>>ChNo)&0x01)
      {
        ValueOut=Zhengshu-Xiaoshu;
      }
      else 
      {
        ValueOut=Zhengshu+Xiaoshu;
      }
      if(ValueOut==0)
      {
        StateC.FuhaoBit&=~(1<<ChNo);
      }
      
      //小数位格式处理
      if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*ChNo)==_S_1)//1位小数
      {
        if((ValueOut/100)%10>4)
        {
          ValueOut+=1000;
        }
        ValueOut=ValueOut/1000;
      }
      
      #if OPT_EN==0
      else if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*ChNo)==_S_2)//2位小数
      {
        if((ValueOut/10)%10>4)
        {
          ValueOut+=100;
        }
        ValueOut=ValueOut/100;
      }

      else if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*ChNo)==_S_0)//0位小数
      {
        if((ValueOut/1000)%10>4)
        {
          ValueOut+=10000;
        }
        ValueOut=ValueOut/10000;
      }
      #endif
      
      
      StateC.Value[ChNo]=(u16)ValueOut;
      NOP;
      NOP;
      NOP;
  }
}


