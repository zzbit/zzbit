#include "_config.h"

#if FS_FX_ENABLE==1

//风向，共16个方位
const u8 FxPinBuf[16]={
  0xfe,0xfc,0xfd,0xf9,
  0xfb,0xf3,0xf7,0xe7,
  0xef,0xcf,0xdf,0x9f,
  0xbf,0x3f,0x7f,0x7e
};
/*
const u16 FxValueBuf[16]={//风向采用12钟点制
 120, 7,15, 22,
  30,37,45, 52,
  60,67,75, 82,
  90,97,105,112
};*/
const u16 FxValueBuf[16]={//风向360
  360, 22,45, 67,
  90,112,135, 157,
  180,202,225, 247,
  270,292,315,337
};


//求端口某位电平
u8 GetPinBit(u8 Pin,u8 Bit)
{
  if((Pin&Bit)==0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

//求风向第cc个脚TTL电平
u8 GetTTL(u8 cc)
{
  u8 yy;
  switch(cc)
  {
    case 0:
      yy=GetPinBit(FX_0_PIN,FX_0_BIT);
      break;
    case 1:
      yy=GetPinBit(FX_1_PIN,FX_1_BIT);
      break;
    case 2:
      yy=GetPinBit(FX_2_PIN,FX_2_BIT);
      break;
    case 3:
      yy=GetPinBit(FX_3_PIN,FX_3_BIT);
      break;
    case 4:
      yy=GetPinBit(FX_4_PIN,FX_4_BIT);
      break;
    case 5:
      yy=GetPinBit(FX_5_PIN,FX_5_BIT);
      break;
    case 6:
      yy=GetPinBit(FX_6_PIN,FX_6_BIT);
      break;
    case 7:
      yy=GetPinBit(FX_7_PIN,FX_7_BIT);
      break;
    default:break;
  }
  return yy;
}

//取风向8个脚电平
u8 GetFxPin(void)
{
  u8 j=0;
  u8 dd;
  u8 cc;
  
  dd=0;
  for(j=0;j<8;j++)
  {
    cc=GetTTL(j);
    dd|=cc<<j;
  }
  return dd;
}

u16 GetFxValue(void)
{
  u8 j;
  u8 pin;
  FX_PORT_INIT;
  pin=GetFxPin();
  

  for(j=0;j<16;j++)//查表
  {
    if(pin==(FxPinBuf[j]))
    {
      return FxValueBuf[j];
    }
  }
  //表中未查到，说明风向传感器接触有误
  //return SENIOR_NULL;
  
  return 0;
 
}

#endif


