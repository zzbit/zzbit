#include "_config.h"


#if LCD_NO==20//--------------------------------------------

// 1 2 3 4 5 数字
  #define a1  BIT4
  #define b1  BIT0
  #define c1  BIT2
  #define d1  BIT7
  #define e1  BIT6
  #define f1  BIT5
  #define g1  BIT1


  const char digit1[] =
  {
    a1+b1+c1+d1+e1+f1,                      //  "0"
    b1+c1,                                  //  "1"
    a1+b1+d1+e1+g1,                         //  "2"
    a1+b1+c1+d1+g1,                         //  "3"
    b1+c1+f1+g1,                            //  "4"
    a1+c1+d1+f1+g1,                         //  "5"
    a1+c1+d1+e1+f1+g1,                      //  "6"
    a1+b1+c1,                               //  "7"
    a1+b1+c1+d1+e1+f1+g1,                   //  "8"
    a1+b1+c1+d1+f1+g1,                      //  "9"
  };


// 6 7 8 9 数字
  #define a2  BIT3
  #define b2  BIT6
  #define c2  BIT4
  #define d2  BIT0
  #define e2  BIT1
  #define f2  BIT2
  #define g2  BIT5

  const char digit2[] =
  {
    a2+b2+c2+d2+e2+f2,                      //  "0"
    b2+c2,                                  //  "1"
    a2+b2+d2+e2+g2,                         //  "2"
    a2+b2+c2+d2+g2,                         //  "3"
    b2+c2+f2+g2,                            //  "4"
    a2+c2+d2+f2+g2,                         //  "5"
    a2+c2+d2+e2+f2+g2,                      //  "6"
    a2+b2+c2,                               //  "7"
    a2+b2+c2+d2+e2+f2+g2,                   //  "8"
    a2+b2+c2+d2+f2+g2,                      //  "9"
  };

#else//-----------------------------------------------------------------

  #define a1  BIT7
  #define b1  BIT6
  #define c1  BIT5
  #define d1  BIT4
  #define e1  BIT3
  #define f1  BIT2
  #define g1  BIT1


  const char digit1[] =
  {
    a1+b1+c1+d1+e1+f1,                      //  "0"
    b1+c1,                                  //  "1"
    a1+b1+d1+e1+g1,                         //  "2"
    a1+b1+c1+d1+g1,                         //  "3"
    b1+c1+f1+g1,                            //  "4"
    a1+c1+d1+f1+g1,                         //  "5"
    a1+c1+d1+e1+f1+g1,                      //  "6"
    a1+b1+c1,                               //  "7"
    a1+b1+c1+d1+e1+f1+g1,                   //  "8"
    a1+b1+c1+d1+f1+g1,                      //  "9"
  };

#endif

//总计35*4=140段，17*8=136，最后4段独立开来
u8 LcdRam[18];
void BuSendByte(u8 Byte)
{
  u8 j;
  for(j=0;j<8;j++)
  {
    NOP;NOP;NOP;NOP;
    LCD_SCL_L;
    NOP;NOP;NOP;NOP;
    if(Byte&0x80)
    {
      LCD_SD_H;
    }
    else
    {
      LCD_SD_L;
    }
    Byte<<=1;
    NOP;NOP;NOP;NOP;
    LCD_SCL_H;
  }
}
void LcdClear(void)//清屏
{
  u8 j;
  delay_ms(10);
  LCD_INHb_H;
  LCD_CSB_H;
  delay_us(10);

  LCD_CSB_L;
  BuSendByte(0xea);
  BuSendByte(0xC0);
  LCD_CSB_H;

  LCD_CSB_L;
  BuSendByte(0xBF);//DISSET
  BuSendByte(0xF0);//BLKSET
  BuSendByte(0xFC);//APCTL
  BuSendByte(0xC8);//display on,1/3bias
  LCD_CSB_H;  

  LCD_CSB_L;
  BuSendByte(0xBF);
  BuSendByte(0xF0);
  BuSendByte(0xFC);
  BuSendByte(0xC8);
  BuSendByte(0x00);//ADSET
  
  for(j=0;j<17;j++)//总计35段，最后字节为S34和S0
  {
    LcdRam[j]=0x00;
    BuSendByte(LcdRam[j]);
  }
  BuSendByte(0x00);
  LCD_CSB_H;
}
void LcdOneAddr(u8 Addr,u8 cc)
{
  LCD_CSB_H;
  delay_ms(1);
  
  LCD_CSB_L;
  //ICSET MSB
  if(Addr<=0x1f)
  {
    BuSendByte(0xE8);
  }
  else
  {
    BuSendByte(0xEC);
  }
  //地址低5位
  BuSendByte(Addr&0x1f);
  BuSendByte(cc);
  LCD_CSB_H;
}
void LcdShowOn(u8 cc)
{
  if(cc==1)
  {
    LCD_INHb_H;
  }
  else
  {
    LCD_INHb_L;
    LCD_VCTL_H;
  }
}
void LcdA10(void)
{
    LcdRam[4]=0;
    LcdRam[3]=0;
    LcdRam[2]=0;
    LcdRam[1]=digit1[1];
    LcdRam[0]=digit1[0];
}

void LcdNull(u8 ChNo,u8 Hang)
{
  u8 ch2same=0;
  if(((*((char*)CH1_T_ADDR+0*ONE_CH_CONFIG_BYTES))==(*((char*)CH1_T_ADDR+1*ONE_CH_CONFIG_BYTES)))&&(CH_NUM==2))//2通道相同设备，显示通道号
    ch2same=1;
  
  if(ChNo>CH_X_NOT_SHOW)
    ChNo-=CH_X_NOT_SHOW;//得到真实通道号
  
  
  if(Hang==0)
  {
    if((CH_NUM>2)||(ch2same==1))
      LcdRam[4]=digit1[ChNo];
    else
      LcdRam[4]=0x00;
    LcdRam[3]=a1+b1+c1+e1+f1;
    LcdRam[2]=b1+c1+d1+e1+f1;
    LcdRam[1]=e1+d1+f1;
    LcdRam[0]=e1+d1+f1;
  }
  else
  {
    if((CH_NUM>2)||(ch2same==1))
      LcdRam[9]=digit1[ChNo];
    else
      LcdRam[9]=0;
    LcdRam[8]=a1+b1+c1+e1+f1;
    LcdRam[7]=b1+c1+d1+e1+f1;
    LcdRam[6]=e1+d1+f1;
    LcdRam[5]=e1+d1+f1;
  }
}

void LcdClrUnit(void)
{
  
  LcdRam[2]&=~BIT0;//L9
  LcdRam[3]&=~BIT0;//RH
  
  LcdRam[7]&=~BIT0;//L11
  LcdRam[8]&=~BIT0;//L12
  
  LcdRam[15]&=~BIT0;//mm
  LcdRam[15]&=~BIT1;//LX
  LcdRam[15]&=~BIT2;//kW
  LcdRam[15]&=~BIT3;//O
  LcdRam[15]&=~BIT4;//L13
  LcdRam[15]&=~BIT5;//L14
  LcdRam[15]&=~BIT6;//A
  LcdRam[15]&=~BIT7;//L10
  
  LcdRam[16]&=~BIT0;//ppm
  LcdRam[16]&=~BIT1;//m/s
  LcdRam[16]&=~BIT2;//pa
  LcdRam[16]&=~BIT4;//V
  LcdRam[16]&=~BIT5;//L7
  LcdRam[16]&=~BIT6;//L8
  
  LcdRam[14]&=~BIT4;//showAVG
  LcdRam[14]&=~BIT0;//clrMAX
  LcdRam[14]&=~BIT1;//clrMIN
}
//Kt不为0时不显示当前位,只显示10进制rtc
void LcdRtcShan(u8 T1,u8 T2,u8 Kt)
{
  if((Kt==18)||(Kt==20))
  {
    LcdRam[10]=0;
    LcdRam[11]=0;
  }
  else
  {
      if(T1>9)
      {
        LcdRam[10]=digit1[T1/10];//11
        LcdRam[11]=digit1[T1%10];//12      
      }
      else
      {
        LcdRam[10]=digit1[0];
        LcdRam[11]=digit1[T1];
      }
  }
  
  if((Kt==17)||(Kt==19)||(Kt==21))
  {
    LcdRam[12]=0;
    LcdRam[13]=0;
  }
  else
  {
      if(T2>9)
      {
        LcdRam[12]=digit1[T2/10];//11
        LcdRam[13]=digit1[T2%10];//12      
      }
      else
      {
        LcdRam[12]=digit1[0];
        LcdRam[13]=digit1[T2];
      }
  }
  
  LcdRam[10]|=BIT0;
  LcdRam[13]|=BIT0;
}

//Kt不为0时不显示当前位,,AB=0 行1  否则行2
void LcdChAB(u8 AB,u16 Value,u8 Kt)
{
            u8 temp;
            u16 vv;
            u8 FuhaoBit;
            
            FuhaoBit=0;
            if(Value&0x8000)
            {
              FuhaoBit=1;
            }
            vv=Value&0x7fff;
  
            //----------------------------------
            if(AB==0)
            {
              LcdRam[4]=0;
            }
            else
            {
              LcdRam[9]=0;
            }
         
            //------------------------------------
            if((Kt==1)||(Kt==5)||(Kt==9)||(Kt==13))
            {
              temp=0x00;
            }
            else
            {
                if(FuhaoBit)//符号
                {
                    temp=g1;
                }
                else
                {
                  if(Value>999)
                  {
                    temp=digit1[(vv/1000)%10];
                  }
                  else
                  {
                    temp=digit1[0];
                  }
                }
            }
            if(AB==0)
            {
              LcdRam[3]=temp;
            }
            else
            {
              LcdRam[8]=temp;
            }
            //--------------------------------
            if((Kt==2)||(Kt==6)||(Kt==10)||(Kt==14))
            {
              temp=0x00;
            }
            else
            {
                if(Value>99)
                {
                  temp=digit1[(vv/100)%10];
                }
                else
                {
                  temp=digit1[0];
                }
            }
            if(AB==0)
            {
              LcdRam[2]=temp;
            }
            else
            {
              LcdRam[7]=temp;
            }
            //----------------------------------------
            
            
            if((Kt==3)||(Kt==7)||(Kt==11)||(Kt==15))
            {
               temp=0;
            }
            else
            {
                if(vv>9)
                {
                  temp=digit1[(vv/10)%10];
                }
                else
                {
                  temp=digit1[0];
                }
            }
            if(AB==0)
            {
              LcdRam[1]=temp;
            }
            else
            {
              LcdRam[6]=temp;
            }
            //----------------------------------------
            
            
            if((Kt==4)||(Kt==8)||(Kt==12)||(Kt==16))
            {
              temp=0;
            }
            else
            {
              temp=digit1[vv%10];
            }
            if(AB==0)
            {
              LcdRam[0]=temp;
              LcdRam[0]|=BIT0;//1位小数BIT0
            }
            else
            {
              LcdRam[5]=temp;
              LcdRam[5]|=BIT0;//1位小数BIT0
            }
}



#if BAT_LCD_EN==1
void LcdABat(void)
{
  u8 cc;
  cc=GetBatt();
  if(cc>99)
    cc=99;
  
  LcdRam[4]=digit1[(cc/10)%10];
}
#endif

void LcdChNoValueA(u8 ChNo,u16 Value,u8 FuhaoBit)//FuhaoBit不为0则为负数
{
  u8 over=0;
  u8 j;
  
  if(KeyDoing())
    return;
  if(Flag.RstFirstSample==0)
    return;
  if(Value==HAND_STOP)
    return;
  
  
  if(ChNo==HANG_NOT_SHOW)
  {
      for(j=0;j<5;j++)
      {
        LcdRam[j]=0x00;
      }
  }
  else if((Value==SENIOR_NULL)||(Value==(0x7fff&SENIOR_NULL)))
  {
      LcdNull(ChNo,0);
      #if BAT_LCD_EN==1
      LcdABat();
      #endif
  }
  else if(Value==HAND_STOP)
  {
      LcdOFFX(OFF_HAND);
  }
  else
  {
          if(ChNo>CH_X_NOT_SHOW)//通道号不显示
          {
            LcdRam[4]=0x00;
            ChNo-=CH_X_NOT_SHOW;//得到真实通道号
          }
          else
          {
            LcdRam[4]=digit1[ChNo];
          }
          
          #if BAT_LCD_EN==1
          LcdABat();
          #endif
          
   
          Value=Value&0x7fff;
          if(Value>9999)
          {
            Value=Value/10;
            over=1;
          }
          
          //数值显示
          
          #if U_x10_ENABLE==1//显示放大10倍
          u8 cc=*((char*)CH1_U_ADDR+(ChNo-1)*ONE_CH_CONFIG_BYTES);
          if((cc==_U_x10ppm)||(cc==_U_x10kpa)||(cc==_U_x10umol)||(cc==_U_x10mv)||(cc==_U_x10mm)||(cc==_U_x10o))
          {
            over=1;
          }
          #endif
          
          if(FuhaoBit)//符号
          {
            LcdRam[3]=g1;
            if(Value>999)//负数时，处理
            {
              Value=Value/10;
              over=1;
            }
          }
          else
          {
            if(Value>999)
            {
              LcdRam[3]=digit1[(Value/1000)%10];
            }
            else
            {
              LcdRam[3]=0;
            }
          }
      
          if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*(ChNo-1))==_S_1)
          {
            if(over==1)
            {
              LcdA3dst(Value,0);
            }
            else
            {
              LcdA3dst(Value,1);
            }
          }
          #if OPT_EN==0
          else if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*(ChNo-1))==_S_2)
          {
              if(over==1)
              {
                LcdA3dst(Value,1);
              }
              else
              {
                LcdA3dst(Value,2);
              }
          }
          
          else if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*(ChNo-1))==_S_0)
          {
            LcdA3dst(Value,0);
          }
          #endif
  }
}
void LcdChNoValueB(u8 ChNo,u16 Value,u8 FuhaoBit)//FuhaoBit不为0则为负数
{
  u8 j;
  u8 over=0;
  
  if(KeyDoing())
    return;
  if(Flag.RstFirstSample==0)
    return;
  if(Value==HAND_STOP)
    return;
  
  if(ChNo==HANG_NOT_SHOW)
  {
    for(j=5;j<10;j++)
    {
      LcdRam[j]=0x00;
    }
  }
  else if((Value==SENIOR_NULL)||(Value==(0x7fff&SENIOR_NULL)))
  {
      LcdNull(ChNo,1);
  }
  else if(Value==HAND_STOP)
  {
      LcdOFFX(OFF_HAND);
  }
  else
  {
      if(ChNo>CH_X_NOT_SHOW)//通道号不显示
      {
        LcdRam[9]=0x00;
        ChNo-=CH_X_NOT_SHOW;//真实通道号
      }
      else 
      {
        LcdRam[9]=digit1[ChNo];
      }

      Value=Value&0x7fff;
      if(Value>9999)//最大显示9999
      {
        Value=Value/10;
        over=1;//溢出过，显示时，小数位必须去掉1位
      }

      #if U_x10_ENABLE==1//显示放大10倍
      u8 cc=*((char*)CH1_U_ADDR+(ChNo-1)*ONE_CH_CONFIG_BYTES);
      if((cc==_U_x10ppm)||(cc==_U_x10kpa)||(cc==_U_x10umol)||(cc==_U_x10mv)||(cc==_U_x10mm)||(cc==_U_x10o))
      {
        over=1;
      }
      #endif
    

      //符号
      if(FuhaoBit)
      {
        LcdRam[8]=g1;
        if(Value>999)//负数时，处理
        {
          Value=Value/10;
          over=1;
        }
      }
      else 
      {
        if(Value>999)
        {
          LcdRam[8]=digit1[(Value/1000)%10];
        }
        else  
        {
          LcdRam[8]=0x00;
        }
      }
      //数值

      if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*(ChNo-1))==_S_1)
      {
        if(over==1)
        {
          LcdB3dst(Value,0);
        }
        else
        {
          LcdB3dst(Value,1);
        }
      }
      #if OPT_EN==0
      else if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*(ChNo-1))==_S_2)
      {
        if(over==1)
        {
          LcdB3dst(Value,1);
        }
        else
        {
          LcdB3dst(Value,2);
        }
      }
      
      else if(*((char*)CH1_S_ADDR+ONE_CH_CONFIG_BYTES*(ChNo-1))==_S_0)
      {
        LcdB3dst(Value,0);
      }
      #endif
  }
  
      #if ((SIM68_EN==1)&&(SIM68_SLP_EN==1))
      if(StateC.GpsSlping==1)//休眠时
      {
        Lcd_Bc(1);
      }
      else
      {
        Lcd_Bc(0);
      }
      #endif
  
}
//第一行显示，后3位数字显示
void LcdA3dst(u16 Value,u8 cc)
{
   if(cc==1)//1位小数
   {
            if(Value>99)
            {
              LcdRam[2]=digit1[(Value/100)%10];
            }
            else
            {
              LcdRam[2]=0;
            }
            if(Value>9)
            {
              LcdRam[1]=digit1[(Value/10)%10];
            }
            else
            {
              //#if U_x10_ENABLE==1
              //LcdRam[1]=0;
              //#else
              LcdRam[1]=digit1[0];
              //#endif
            }
            LcdRam[0]=digit1[Value%10];
            
            if(Flag.AdcMd==1)
            {
              LcdRam[0]&=~BIT0;
            }
            else
            {
              //#if U_x10_ENABLE==1
              //LcdRam[0]&=~BIT0;
              //#else
              LcdRam[0]|=BIT0;//1位小数BIT0
              //#endif
            }
   }
   #if ((OPT_EN==0)||(RF_YQ_SS_EN==1))
   else if(cc==2)
   {
  
            if(Value>99)
            {
              LcdRam[2]=digit1[(Value/100)%10];
            }
            else
            {
              LcdRam[2]=digit1[0];
            }
            if(Value>9)
            {
              LcdRam[1]=digit1[(Value/10)%10];
            }
            else
            {
              LcdRam[1]=digit1[0];
            }
            LcdRam[0]=digit1[Value%10];
            
            if(Flag.AdcMd==1)
            {
              LcdRam[0]&=~BIT0;
            }
            else
            {
              //#if U_x10_ENABLE==1
              //LcdRam[1]&=~BIT0;
              //#else
              LcdRam[1]|=BIT0;//2位小数BIT0
              //#endif
            }
   }
   else
   {
            if(Value>99)
            {
              LcdRam[2]=digit1[(Value/100)%10];
            }
            else
            {
              LcdRam[2]=0;
            }
            if(Value>9)
            {
              LcdRam[1]=digit1[(Value/10)%10];
            }
            else
            {
              LcdRam[1]=0;
            }
            LcdRam[0]=digit1[Value%10];

   }
   #endif
}
//第二行显示，后3位数字显示
void LcdB3dst(u16 Value,u8 cc)
{
  if(cc==1)//1位小数
  {
        if(Value>99)
        {
          LcdRam[7]=digit1[(Value/100)%10];
        }
        else  
        {
          LcdRam[7]=0;
        }
        if(Value>9)
        {
          LcdRam[6]=digit1[(Value/10)%10];
        }
        else 
        {
          LcdRam[6]=digit1[0];//0.0
        }
        LcdRam[5]=digit1[Value%10];
        
        //小数点
        if(Flag.AdcMd==1)
        {
          LcdRam[5]&=~BIT0;
        }
        else
        {
          LcdRam[5]|=BIT0;
        }
  }
  #if OPT_EN==0
  else if(cc==2)//2位小数
  {
          if(Value>99)
          {
            LcdRam[7]=digit1[(Value/100)%10];
          }
          else 
          {
            LcdRam[7]=digit1[0];
          }
          if(Value>9)
          {
            LcdRam[6]=digit1[(Value/10)%10];//0.00
          }
          else 
          {
            LcdRam[6]=digit1[0];
          }
          LcdRam[5]=digit1[Value%10];
          
          //小数点
          if(Flag.AdcMd==1)
          {
            LcdRam[6]&=~BIT0;//2位小数
          }
          else
          {
            LcdRam[6]|=BIT0;
          }
  }
  else//整数数
  {
  
        if(Value>99)
        {
          LcdRam[7]=digit1[(Value/100)%10];
        }
        else 
        {
          LcdRam[7]=0;
        }
        if(Value>9)
        {
          LcdRam[6]=digit1[(Value/10)%10];
        }
        else 
        {
          LcdRam[6]=0;
        }
        LcdRam[5]=digit1[Value%10];//0
  }
  #endif
  
  #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
  LcdRam[8]=0;
  #endif
}

//显示Gsm信号强度 cc,0-31
void LcdGsmXh(u8 cc)
{
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
 
  //if(Flag.SampPt==1)//采样忙  zz
  //  return;
      
      
      
#if LCD_NO==20
   Lcd20GsmXh(cc);
#else
    
  if(cc<1)//0格 0
  {
    LcdRam[14]|=BIT3;
    LcdRam[14]&=~BIT2;
    LcdRam[14]&=~BIT6;
    LcdRam[14]&=~BIT5;
  }
  else if(cc<10)//1格 1-9
  {
    LcdRam[14]|=BIT3;
    LcdRam[14]|=BIT2;
    LcdRam[14]&=~BIT6;
    LcdRam[14]&=~BIT5;
  }
  else if(cc<20)//2格 10-19
  {
    LcdRam[14]|=BIT3;
    LcdRam[14]|=BIT2;
    LcdRam[14]|=BIT6;
    LcdRam[14]&=~BIT5;
  }
  else if(cc<32)//满格 20-31
  {
    LcdRam[14]|=BIT3;
    LcdRam[14]|=BIT2;
    LcdRam[14]|=BIT6;
    LcdRam[14]|=BIT5;
  }
  else if(cc==100)//不显示信号标志
  {
    LcdRam[14]&=~BIT3;
    LcdRam[14]&=~BIT2;
    LcdRam[14]&=~BIT6;
    LcdRam[14]&=~BIT5;
  }
  BuToLcdRam(14,1);
  
#endif
  
}
void LcdOFFX(u8 StopCause)
{
  
   #if GPRS_TC_EN==1
   if(Pt.PrintThisDueout>0)//打印的时候不进入
     return;
   #endif
   
   #if PRINT_FZ_EN==1
   if(Pt.tout>0)
     return;
   #endif
   

    if(KeyDoing())//zz
     return;

  
  
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
  
  if(Flag.SampPt==1)//采样忙
    return;
  
  #if MIN_MAX_AVG_ENABLE==1
  if(Pt.min_max_pt>0)
    return;
  #endif  
  
#if LCD_NO==8
  for(u8 j=0;j<6;j++)
    LcdRam[j]=0;
  Lcd08_Off(StopCause);
  BuToLcdRam(0,6);
  
#elif LCD_NO==20
  Lcd08OFFX(StopCause);
#else
  
  u8 j;
  for(j=0;j<5;j++)
    LcdRam[j]=0;
  LcdRam[9]=0;
  
  #if BSQ_ENABLE==1
  LcdRam[8]=digit1[StopCause];
  #else
  if((StopCause==OFF_PC)||((StopCause==OFF_HAND)&&(StateB.StartMode!=START_MODE_HAND))) LcdRam[8]=0;
  else LcdRam[8]=digit1[StopCause];//
  #endif
  
  LcdRam[7]=a1+b1+c1+d1+e1+f1;//O
  LcdRam[6]=a1+e1+f1+g1;//F
  LcdRam[5]=a1+e1+f1+g1;//F
  LcdRam[13]|=BIT0;
  LcdRam[14]&=~BIT3;//信号
  LcdRam[14]&=~BIT2;
  LcdRam[14]&=~BIT6;
  LcdRam[14]&=~BIT5;
  LcdRam[14]&=~BIT0;//MAX
  LcdRam[14]&=~BIT1;//MIN
  LcdRam[14]&=~BIT4;//AVG
  LcdRam[14]&=~BIT7;//电池
  LcdRam[16]&=~BIT7;//报警
  LcdRam[16]&=~BIT3; 
  LcdRam[4]&=~BIT0;//喇叭
  LcdRam[9]&=~BIT0;
  
  LcdRam[17]=0;

  LcdClrUnit();
  BuToLcdRam(0,18);
#endif
  

  
}

//显示Cn
void LcdLn(u8 cc)
{
#if MIN_MAX_AVG_ENABLE==1
  if(Pt.min_max_pt>0)
    return;
#endif
#if LCD_NO==7
  u8 j;
  for(j=0;j<5;j++)
    LcdRam[j]=0;
  LcdRam[9]=0;
  LcdRam[8]=0;
  LcdRam[7]=a1+e1+f1+g1;
  LcdRam[6]=digit1[(cc/10)%10];
  LcdRam[5]=digit1[cc%10];
  LcdRam[13]|=BIT0;
  LcdClrUnit();
  BuToLcdRam(0,17);
#endif
}
void LcdFz(u8 NN)
{
#if MIN_MAX_AVG_ENABLE==1
  if(Pt.min_max_pt>0)
    return;
#endif  

  u8 j;
  for(j=0;j<5;j++)
    LcdRam[j]=0;
  LcdRam[9]=0;
  
#if RF_CC_EN==1
  #if  ((RF_PP_115200_EN==1)||(RF_KP_EN==1))
    LcdRam[8]=a1+b1+e1+f1+g1;//PP
    LcdRam[7]=a1+b1+e1+f1+g1;
  #else
    LcdRam[8]=a1+e1+f1+d1;//CC
    LcdRam[7]=a1+e1+f1+d1;
  #endif
#else
  LcdRam[8]=a1+e1+f1+g1;
  LcdRam[7]=a1+e1+f1+g1;
#endif
  LcdRam[6]=digit1[(NN/10)%10];
  LcdRam[5]=digit1[NN%10];
  LcdRam[13]|=BIT0;
  LcdClrUnit();
  BuToLcdRam(0,17);

}
void LcdNN(u8 NN)
{
#if MIN_MAX_AVG_ENABLE==1
  if(Pt.min_max_pt>0)
    return;
#endif  
#if LCD_NO==8
  Lcd08_NN(NN);
  BuToLcdRam(0,6);
#elif LCD_NO==20
  Lcd20_NN(NN);
  
#else
  u8 j;
  for(j=0;j<5;j++)
    LcdRam[j]=0;
  LcdRam[9]=0;
  LcdRam[8]=0;
  if(((NN/100)%10)==0)
  {
    LcdRam[7]=0;
  }
  else
  {
    LcdRam[7]=digit1[(NN/100)%10];
  }
  LcdRam[6]=digit1[(NN/10)%10];
  LcdRam[5]=digit1[NN%10];
  LcdRam[13]|=BIT0;
  LcdClrUnit();
  BuToLcdRam(0,17);
#endif
}
void LcdHHHH(void)
{
#if MIN_MAX_AVG_ENABLE==1
  if(Pt.min_max_pt>0)
    return;
#endif  

  u8 j;
  for(j=0;j<5;j++)
    LcdRam[j]=0;
  LcdRam[9]=0;

  LcdRam[8]=b1+c1+e1+f1+g1;//HHHH
  LcdRam[7]=b1+c1+e1+f1+g1;
  LcdRam[6]=b1+c1+e1+f1+g1;
  LcdRam[5]=b1+c1+e1+f1+g1;
  LcdRam[13]|=BIT0;
  LcdClrUnit();
  BuToLcdRam(0,17);

}

void LcdTime(u8 T1,u8 T2,u8 BD)
{
#if LCD_NO==20
   Lcd20Time(T1,T2,BD);
#else
  
  
  if(T1>9){
    if(BD==0)//BCD
    {
      LcdRam[10]=digit1[T1/16];//11
      LcdRam[11]=digit1[T1%16];//12
    }
    else//D10
    {
      LcdRam[10]=digit1[T1/10];//11
      LcdRam[11]=digit1[T1%10];//12      
    }
  }
  else{
    LcdRam[10]=digit1[0];
    LcdRam[11]=digit1[T1];
  }
  if(T2>9){
    if(BD==0)
    {
      LcdRam[12]=digit1[T2/16];//11
      LcdRam[13]=digit1[T2%16];//12
    }
    else
    {
      LcdRam[12]=digit1[T2/10];//11
      LcdRam[13]=digit1[T2%10];//12      
    }
  }
  else{
    LcdRam[12]=digit1[0];
    LcdRam[13]=digit1[T2];
  }
#endif
}
void LcdYearFlag(void)
{
#if LCD_NO==20
  Lcd20YearFlag();
#else  
  LcdRam[11]&=~BIT0;
  LcdRam[12]&=~BIT0;
#endif
}
void LcdMonthFlag(void)
{
#if LCD_NO==20
  Lcd20MonthFlag();
#else  
  LcdRam[11]&=~BIT0;
  LcdRam[12]|=BIT0;
#endif
}
void LcdHourFlag(void)
{
#if LCD_NO==20
  Lcd20HourFlag();
#else  
  LcdRam[11]|=BIT0;
  LcdRam[12]|=BIT0;
#endif
}

void LcdRtcTime(void)
{
#if ((LCD_NO==7)||(LCD_NO==20))

    #if KEY_SET_ENABLE==1
    if(Key.Kt>=17)
      return;
    #endif
    
    if(Flag.SampPt==1)//采样忙
      return;

    Pt.RtcPt++;
    if(Pt.RtcPt>59)Pt.RtcPt=0;
    if((Pt.RtcPt==2)||(Pt.RtcPt==5)||(Pt.RtcPt==12)||(Pt.RtcPt==17)||(Pt.RtcPt==22)||(Pt.RtcPt==27)||(Pt.RtcPt==32)||(Pt.RtcPt==35)||(Pt.RtcPt==42)||(Pt.RtcPt==47)||(Pt.RtcPt==52)||(Pt.RtcPt==57))//5秒读一次时间
    {
      Rtc.SS=ReadRtcD10();
	  
      LcdTime(Rtc.Hour,Rtc.Minute,1);
      LcdHourFlag();
    }
    else if((Pt.RtcPt==3)||(Pt.RtcPt==33))
    { 
      LcdTime(20,Rtc.Year,1);//30秒显示一次日期
      LcdYearFlag();
    }
    else if((Pt.RtcPt==4)||(Pt.RtcPt==34))
    {
      LcdTime(Rtc.Month,Rtc.Day,1);
      LcdMonthFlag();
    }
    else if(!((Pt.RtcPt>=3&&Pt.RtcPt<5)||(Pt.RtcPt>=33&&Pt.RtcPt<35)))
    {
      if(Pt.RtcPt%2==0)
      {LcdHourFlag();}
      else
      {LcdYearFlag();}
    }
    
    #if LCD_NO==7
    
        LcdRam[10]|=BIT0;
        LcdRam[13]|=BIT0;
        
        BuToLcdRam(10,4);
    
    #elif LCD_NO==20
        
        LcdRam[8]|=BIT7;//时钟标志
        //LcdRam[1]|=BIT3;//横杠
        
        BuToLcdRam(5,4);
    #endif
    
#endif
}
void Tshan(void)
{
    #if LCD_NO==7
    
      LcdRam[10]&=~BIT0;
      BuToLcdRam(10,1);
      delay_ms(60);
      LcdRam[10]|=BIT0;
      BuToLcdRam(10,1);
    
    #elif LCD_NO==20
      
      LcdRam[8]&=~BIT7;
      BuToLcdRam(8,1);
      delay_ms(60);
      LcdRam[8]|=BIT7;//时钟标志
      BuToLcdRam(8,1);
      
    #endif
  
  

}
void BuToLcdRam(u8 addr,u8 cc)
{
  u8 j;
  for(j=addr;j<addr+cc;j++)
  {
    LcdOneAddr(j*2,LcdRam[j]);
  }
}
void BuInit(void)
{
  LCD_VCTL_L;
  
  delay_ms(30);
  LcdClear();
  
  LCD_INHb_H;
  LCD_CSB_H;
  delay_us(10);

  LCD_CSB_L;
  BuSendByte(0xea);
  BuSendByte(0xC0);
  LCD_CSB_H;

  LCD_CSB_L;
  BuSendByte(0xBF);//DISSET
  BuSendByte(0xF0);//BLKSET
  BuSendByte(0xFC);//APCTL
  BuSendByte(0xC8);//display on,1/3bias
  LCD_CSB_H;
  
  LCD_CSB_L;
  BuSendByte(0xBF);
  BuSendByte(0xF0);
  BuSendByte(0xFC);
  BuSendByte(0xC8);
  
  LCD_CSB_H;
}
void LcdMcuRst(void)
{
#if LCD_NO==8
  LcdNN(MCU_RESET);
  delay_ms(1000);
#elif LCD_NO==7
  LcdRam[13]|=BIT0;//lcd07显示横杠提示复位
  BuToLcdRam(13,1); 
#endif
}

//报警开、报警关
void LcdAlarmOn(u8 cc)
{
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
  
  if(Flag.SampPt==1)//采样忙
    return;
  
#if LCD_NO==7
  
      #if  ALARM_NO_CN==1
        
        LcdRam[16]&=~BIT7;
        LcdRam[16]&=~BIT3;
        BuToLcdRam(16,1);
        
      #else
        
        if(cc==1)
        {
          LcdRam[16]|=BIT7;
          LcdRam[16]&=~BIT3;
        }
        else
        {
          LcdRam[16]&=~BIT7;
          LcdRam[16]|=BIT3;
        }
      
        BuToLcdRam(16,1);
      #endif
        
#elif LCD_NO==20
        
        if(cc==1)
        {
            Lcd20H(1);
        }
        else
        {
            Lcd20H(0);
        }
        
  
#endif
}

void LcdLowMode(u8 cc)
{
  #if ((LCD_NO==7)&&((POWER_TIME_ENABLE==1)&&(SD_ENABLE==1))||((GPRS_ENABLE==1)&&(BAT_LCD_EN==1)))
     
    if(cc==1)
    {
      LcdRam[17]|=BIT4;//省电模式
      LcdRam[17]&=~BIT5;
      //LcdRam[17]&=~BIT6;
      //LcdRam[17]&=~BIT7;
    }
    else
    {
      LcdRam[17]&=~BIT4;
      LcdRam[17]|=BIT5;//正常模式
      //LcdRam[17]&=~BIT6;
      //LcdRam[17]&=~BIT7;
    }
    LcdRam[17]=0xf0&LcdRam[17];
    LcdRam[17]|=0x0f&(LcdRam[0]>>4);//LcdRam[17]最后高四与LcdRam[0]高四段组成8位
    
    BuToLcdRam(17,1);
 #endif
}
void LcdBatOn(u8 cc)
{
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
  
  if(Flag.SampPt==1)//采样忙
    return;
  
#if LCD_NO==8
  Lcd08_Bat(cc);
  BuToLcdRam(0,1);
#elif LCD_NO==7
  if(cc==1)
  {
    LcdRam[14]|=BIT7;
  }
  else
  {
    LcdRam[14]&=~BIT7;
  } 
  BuToLcdRam(14,1);
#endif
}
//第一行  显示 k 开门状态
void LcdK1(u8 cc)
{
  if(cc==1)
  {
    LcdRam[16]|=BIT6;//第一行 K   L8
  }
  else
  {
    LcdRam[16]&=~BIT6;//第一行 K   L8
  }
}
//第二行  显示 k 开门状态
void LcdK2(u8 cc)
{
  if(cc==1)
  {
    LcdRam[15]|=BIT5;//第一行 K   L14
  }
  else
  {
    LcdRam[15]&=~BIT5;//第一行 K   L14
  }
}


void LcdMin(void)
{
  LcdRam[14]&=~BIT4;//ClrAVG
  LcdRam[14]&=~BIT0;//ClrMAX;
  LcdRam[14]|=BIT1;//ShowMIN  
}
void LcdMax(void)
{
  LcdRam[14]&=~BIT4;//ClrAVG
  LcdRam[14]|=BIT0;//showMAX;
  LcdRam[14]&=~BIT1;//clrMIN  
}
void LcdAvg(void)
{
  LcdRam[14]|=BIT4;//showAVG
  LcdRam[14]&=~BIT0;//clrMAX;
  LcdRam[14]&=~BIT1;//clrMIN  
}
void LcdSXCB(u8 cc)//上限超标
{
  if(cc)
  {LcdRam[17]|=BIT6;}
  else
  {LcdRam[17]&=~BIT6;}
  
  LcdRam[17]=0xf0&LcdRam[17];
  LcdRam[17]|=0x0f&(LcdRam[0]>>4);//LcdRam[17]最后高四与LcdRam[0]高四段组成8位
  
  BuToLcdRam(17,1);
}
void U0U1set(void)
{
#if U0U1_ENABLE==1
   if(Flag.U0U1ing==0)
   {
      Flag.U0U1ing=1;
   }
   else
   {
      Flag.U0U1ing=0;
   }
   LcdSXCB(Flag.U0U1ing);
#endif
}

//无线卡片主机进入透传模式
void LcdU0U1_RF(void)
{
#if U0U1_ENABLE==1
   if(Flag.U0U1ing==0)
   {
      Flag.U0U1ing=1;
      LcdOFFX(OFF_RF_U0U1);
   }
   else
   {
      Flag.U0U1ing=0;
      
      LcdNN(1);

     //#if RF_MULTI_EN==1
     //Pt.Spt=9;
     //#endif
      
      
   } 
#endif
}


//判断第一行对应通道超标喇叭是否显示
void Lcd1Laba(u8 cc)
{
  if(cc)
  {LcdRam[4]|=BIT0;}
  else
  {LcdRam[4]&=~BIT0;}
}
void Lcd2Laba(u8 cc)
{
  if(cc)
  {LcdRam[9]|=BIT0;}
  else
  {LcdRam[9]&=~BIT0;}
}


void Lcd1Unit(u8 ch)
{
  if(ch==UNIT_NOT_SHOW)
  {
    {LcdRam[16]&=~BIT5;LcdRam[2]&=~BIT0;LcdRam[3]&=~BIT0;}
  }
  else
  {
    if(_U_OC==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//C
    {LcdRam[16]|=BIT5;}
    else if(_U_RH==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//%RH
    {LcdRam[2]|=BIT0;LcdRam[3]|=BIT0;}
    else if(_U_bf==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//%
    {LcdRam[2]|=BIT0;}
    else if((_U_ppm==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))||(_U_x10ppm==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES)))//10ppm，LCD显示为ppm整数
    {LcdRam[16]|=BIT0;}
    else if(_U_v==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//v
    {LcdRam[16]|=BIT4;}
    else if(_U_m_s==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//m/s
    {LcdRam[16]|=BIT1;}
    else if((_U_kpa==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))\
      ||(_U_x10kpa==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES)))//没有小数点，显示为kpa
    {LcdRam[16]|=BIT2+BIT6;}
    
    #if (JLY_MODEL==_3_DL_10A)//3路都是A，而A 在第二行，则一直显示在第二行
        else if(_U_a==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//A
        {LcdRam[15]|=BIT6;}
    #endif

  }
  LcdRam[13]|=BIT0;//横杠
}
void Lcd2Unit(u8 ch)
{    
  if(ch==UNIT_NOT_SHOW)
  {
    {LcdRam[15]&=~BIT7;LcdRam[7]&=~BIT0;LcdRam[8]&=~BIT0;LcdRam[15]&=~BIT4;}
  }
  else
  {
    if(_U_OC==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//c
    {LcdRam[15]|=BIT7;}
    else if(_U_RH==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//%RH
    {LcdRam[7]|=BIT0;LcdRam[8]|=BIT0;LcdRam[15]|=BIT4;}
    else if(_U_bf==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//%
    {LcdRam[7]|=BIT0;}
    else if(_U_klx==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//KLX
    {LcdRam[15]|=BIT1+BIT5;}
    else if((_U_mm==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))||(_U_x10mm==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES)))//mm  10mm
    {LcdRam[15]|=BIT0;}
    else if((_U_o==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))||(_U_x10o==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES)))//o  10o
    {LcdRam[15]|=BIT3;}
    else if(_U_kw==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//KW
    {LcdRam[15]|=BIT2;}
    else if(_U_a==*((char*)CH1_U_ADDR+ch*ONE_CH_CONFIG_BYTES))//A
    {LcdRam[15]|=BIT6;}

  }
  LcdRam[13]|=BIT0;//横杠
}

void Lcd_Bfh(void)
{
  LcdRam[7]|=BIT0;
  BuToLcdRam(0,17);
}


//...............---------------............lcd08...........----------------------..................
//--------------------------------------------------------------------------------------------------
void Lcd08_Off(u8 StopCause)
{
  if(KeyDoing())
    return;
  
  LcdRam[0]=0;
  if((StopCause==OFF_PC)||((StopCause==OFF_HAND)&&(StateB.StartMode!=START_MODE_HAND)))
  {
    LcdRam[1]=0;
  }
  else LcdRam[1]=digit1[StopCause];//
  LcdRam[2]=a1+b1+c1+d1+e1+f1;//O
  LcdRam[3]=a1+e1+f1+g1;//F
  LcdRam[4]=a1+e1+f1+g1;//F
}
void Lcd08_Null(u8 ChNo)
{
    if(CH_NUM>1)
      LcdRam[0]=digit1[ChNo];
    else
      LcdRam[0]=0x00;
    LcdRam[1]=a1+b1+c1+e1+f1;
    LcdRam[2]=b1+c1+d1+e1+f1;
    LcdRam[3]=e1+d1+f1;
    LcdRam[4]=e1+d1+f1;
}
void Lcd08_NN(u8 nn)
{
    LcdRam[0]=0;
    LcdRam[1]=0;
    LcdRam[2]=0;
    LcdRam[3]=digit1[nn/10];
    LcdRam[4]=digit1[nn%10];
    LcdRam[5]=0;
}
void Lcd08_Min(void)
{
  LcdRam[1]|=BIT0;//MIN
}
void Lcd08_Max(void)
{
  LcdRam[2]|=BIT0;//MAX
}
void Lcd08_Avg(void)
{
  LcdRam[5]|=BIT4;//AVG
}
//cc=1,显示，cc=0,不显示
void Lcd08_AL(u8 cc)
{
  if(cc==1)
    LcdRam[5]|=BIT0;
  else
    LcdRam[5]&=~BIT0;
}
void Lcd08_Bat(u8 cc)
{
  if(cc==1)
    LcdRam[0]|=BIT0;//BAT
  else
    LcdRam[0]&=~BIT0;//BAT
}
void Lcd08_ClrUnit(void)
{
  LcdRam[5]&=~0xf7;
}
//通道号,Ch,从1开始显示，如果Ch>CH_X_NOT_SHOW,不显示通道号
void Lcd08_ChValue(u8 Ch,u16 vv,u8 ff)
{
  u8 over=0;
  
  if(KeyDoing())
    return;
  if(Flag.RstFirstSample==0)
    return;
  if(vv==HAND_STOP)
    return;

  
  if(Ch>CH_X_NOT_SHOW)//通道号不显示
  {
    LcdRam[0]&=~0xfe;
    Ch-=CH_X_NOT_SHOW;//真实通道号
  }
  else//通道号显示
  {
    LcdRam[0]&=~0xfe;
    LcdRam[0]|=0xfe&digit1[Ch];//BIT0为BATT
  }

  if((vv==SENIOR_NULL)||(vv==(0x7fff&SENIOR_NULL)))//NULL显示
  {
    Lcd08_Null(Ch);
  }
  else if(vv==HAND_STOP)
  {
    Lcd08_Off(OFF_HAND);
  }
  else
  {
    if(ff)//负号
    {
      LcdRam[1]=g1;
    }
    else
    {
      if(vv>9999)//显示最大显示9999，超出后，小数位退一位
      {
        vv=vv/10;
        over=1;
      }
      
      if(vv>999)//千位
        LcdRam[1]=digit1[(vv/1000)%10];
      else
        LcdRam[1]=0;
    }
    if(_S_1==*((char*)CH1_S_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES)) 
    {
      if(over==1)
      {
        Lcd08_3dst(vv,0);
      }
      else
      {
        Lcd08_3dst(vv,1);
      }
    }
    else if(_S_2==*((char*)CH1_S_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES)) 
    {
      if(over==1)
      {
        Lcd08_3dst(vv,1);
      }
      else
      {
        Lcd08_3dst(vv,2);
      }
    }
    
  }
  //单位显示...
  Lcd08_ClrUnit();
  if(_U_OC==*((char*)CH1_U_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES))
  {
    LcdRam[5]|=BIT7;//C
  }
  else if(_U_RH==*((char*)CH1_U_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES))
  {
    LcdRam[5]|=BIT6|BIT2;//%RH
  }
}
void Lcd08_3dst(u16 vv,u8 dst)
{
  if(dst==1)
  {
      if(vv>99)//百位
        LcdRam[2]=digit1[(vv/100)%10];
      else
        LcdRam[2]=0;
      if(vv>9)//十位
        LcdRam[3]=digit1[(vv/10)%10];
      else
        LcdRam[3]=digit1[0];
      LcdRam[4]=digit1[vv%10];//个位
      
      LcdRam[3]&=~BIT0;
      LcdRam[4]|=BIT0;//2P，一位小数
  }
  else if(dst==2)
  {
      if(vv>99)//百位
        LcdRam[2]=digit1[(vv/100)%10];
      else
        LcdRam[2]=digit1[0];
      if(vv>9)//十位
        LcdRam[3]=digit1[(vv/10)%10];
      else
        LcdRam[3]=digit1[0];
      LcdRam[4]=digit1[vv%10];//个位
      
      LcdRam[3]|=BIT0;
      LcdRam[4]&=~BIT0;
  }
  else
  {
      if(vv>99)//百位
        LcdRam[2]=digit1[(vv/100)%10];
      else
        LcdRam[2]=0;
      if(vv>9)//十位
        LcdRam[3]=digit1[(vv/10)%10];
      else
        LcdRam[3]=0;
      LcdRam[4]=digit1[vv%10];//个位
  }
}
void Lcd08_Deel(void)
{
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
    
    if(Flag.SampPt==1)//采样忙
      return;
    
#if MIN_MAX_AVG_ENABLE==1
  if(Pt.min_max_pt>0)
    return;
#endif
  

  if(CH_NUM==1)//不显示通道号
  {
    Lcd08_ChValue(CH_X_NOT_SHOW+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
  }
  else//显示通道号
  {
    Lcd08_ChValue(Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
  }
  Lcd08_AL(SeeChAlarm(Pt.LcdPt));//是否超标
  BuToLcdRam(0,6);
  if(CH_NUM>1)
  {
    Pt.LcdPt++;
    if(Pt.LcdPt==CH_NUM)
      Pt.LcdPt=0;
  }
}

#if RF_YQ_SS_EN==1
//每次显示1个通道，第一行显示通道
void LcdOneChValue(u8 ch)
{

      LcdClrUnit();
      
      LcdA3dst(ch+1,0);//第一行显示通道号
      
      LcdChNoValueB(CH_X_NOT_SHOW+1,Pt.RfData[ch][0].value,0x8000&Pt.RfData[ch][0].value);
      Lcd2Laba(SeeChAlarm(1));
      Lcd2Unit(1);

      BuToLcdRam(0,17);
      LcdAlarmEn();

   
}
#endif

//-------------------------------------------------------------------------------
void LcdValue(void)
{
  #if  (CH_NUM<=2)
  
  
   #if GPRS_TC_EN==1
   if(Pt.PrintThisDueout>0)
     return;
   #endif
   
   #if PRINT_FZ_EN==1
   if(Pt.tout>0)
     return;
   #endif
  
  
   #if ((SIM68_EN==1)&&(RF_U1_EN==1))//U1 切换到 PC时处理
      if(Pt.U1toPC==1)
        return;
   #endif
  
  
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
    
   if(Flag.SampPt==1)//采样忙
     return;
    
   #if MIN_MAX_AVG_ENABLE==1
   if(Pt.min_max_pt>0)
     return;
   #endif
   
   
   #if KEY_SET_ENABLE==1
   if(KeySetIsBusy())
     return;
   #endif
   
   #if LCD_X_ENABLE==1
   if(Pt.LcdxPt!=0)
     return;
   #endif
   
  if(KeyDoing())
    return;
  if(Flag.RstFirstSample==0)
    return;
   
   #if BSQ_ENABLE==0
   if(StateA2.WorkState==WORK_STATE_STOP)//停机状态
   {
     return;
   }
   else
   {
     if((StateB.StartMode==START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND)&&(Flag.HandOff==1))//假停
     {
       return;
     }
   }
   #endif
  
    
    #if LCD_NO==8
       #if LED_WORK_ENABLE==0
       if(CH_NUM==1)
       {
         Lcd08_Deel();
       }
       #endif
       return;
       
    #else
        if(2>=CH_NUM)
        {
          if(Flag.AdcMd==1)//曲线调整模式
          {
            if(CH_NUM==1)
            {
              LcdChNoValueA(CH_X_NOT_SHOW+1,StateC.real_adc[0],0);
            }
            else//2
            {
              LcdChNoValueA(CH_X_NOT_SHOW+1,StateC.real_adc[0],0);
              LcdChNoValueB(CH_X_NOT_SHOW+2,StateC.real_adc[1],0);
            }
      
            LcdClrUnit();
            
            LcdRam[13]|=BIT0;
            LcdRam[14]&=~(BIT0|BIT1|BIT4);
            BuToLcdRam(0,17);
            return;
          }
    
          LcdClrUnit();
          
          
          if((CH_NUM==1)//通道数=1，单位只在第二行的情况
             &&((_U_mm==*((char*)CH1_U_ADDR))||(_U_x10mm==*((char*)CH1_U_ADDR))\
               ||(_U_klx==*((char*)CH1_U_ADDR))))
          {
              LcdChNoValueA(HANG_NOT_SHOW,0,0);
              LcdChNoValueB(CH_X_NOT_SHOW+1,StateC.Value[0],StateC.FuhaoBit&BIT0);
              Lcd2Laba(SeeChAlarm(0));
              Lcd2Unit(0);//通道1显示想第2行
          }
          else//标准情况
          {
              u8 ch2same=0;
              if(((*((char*)CH1_T_ADDR+0*ONE_CH_CONFIG_BYTES))==(*((char*)CH1_T_ADDR+1*ONE_CH_CONFIG_BYTES)))&&(CH_NUM==2))//2通道相同设备，显示通道号
                ch2same=1;
            
              if(ch2same==1)
                LcdChNoValueA(1,StateC.Value[0],StateC.FuhaoBit&BIT0);
              else
                LcdChNoValueA(CH_X_NOT_SHOW+1,StateC.Value[0],StateC.FuhaoBit&BIT0);
              
              if(CH_NUM>1)
              {
                if(ch2same==1)
                  LcdChNoValueB(2,StateC.Value[1],StateC.FuhaoBit&BIT1);
                else              
                  LcdChNoValueB(CH_X_NOT_SHOW+2,StateC.Value[1],StateC.FuhaoBit&BIT1);
                
                Lcd1Laba(SeeChAlarm(0));
                Lcd1Unit(0);
                Lcd2Laba(SeeChAlarm(1));
                Lcd2Unit(1);
              }
              else
              {
                #if JLY_MODEL==_1_SHR_30_120_kpa
                if(StateC.Value[0]==SENIOR_NULL)
                {
                  LcdChNoValueB(HANG_NOT_SHOW,0,0);
                }
                else
                {
                  LcdB3dst(StateC.Value[0]%100,2);//2位小数显示在B行
                  LcdRam[7]=0;
                }
                #else
                LcdChNoValueB(HANG_NOT_SHOW,0,0);
                #endif
                
                Lcd1Laba(SeeChAlarm(0));
                Lcd1Unit(0);
              }
          }
          
          //开门标记显示 k
          #if DOOR1_BJ_EN==1
          //if(Pt.LcdPt==0)//第一路开关门信号
          {
            if(Pt.Door1Fg==1)
            {
              LcdK1(1);
            }
            else
            {
              LcdK1(0);
            }
          }
          #endif
          
          
          #if SIM68_EN==1
                
                //LcdChNoValueB(CH_X_NOT_SHOW+2,StateC.SatelliteN,0);//zz
                //LcdChNoValueB(CH_X_NOT_SHOW+2,StateC.Vkmh,0);//zz
          #endif
          
                BuToLcdRam(0,17);
                
                LcdAlarmEn();
                LcdMode();
          
             }
             
             #if LCD_X_ENABLE==1
             Pt.LcdxPt=LCD_X_TIME;
             #endif
          #endif
       
    #endif
   


}

//多路显示
void LcdChNoDeel(void)
{
   #if GPRS_TC_EN==1
   if(Pt.PrintThisDueout>0)//打印的时候不进入
     return;
   #endif
   
   #if PRINT_FZ_EN==1
   if(Pt.tout>0)
     return;
   #endif
  
  
   #if ((SIM68_EN==1)&&(RF_U1_EN==1))//U1 切换到 PC时处理
      if(Pt.U1toPC==1)
        return;
   #endif
  
   #if MIN_MAX_AVG_ENABLE==1
   if(Pt.min_max_pt>0)
     return;
   #endif
  
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
    
   #if KEY_SET_ENABLE==1
   if(KeySetIsBusy())
     return;
   #endif
    
  if(KeyDoing())
    return;
  if(Flag.RstFirstSample==0)
    return;
    
   #if BSQ_ENABLE==0
   if(StateA2.WorkState==WORK_STATE_STOP)//停机状态
   {
     return;
   }
   else
   {
      if((StateB.StartMode==START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND)&&(Flag.HandOff==1))//假停
      {
        return;
      }
   }
   #endif
    
   if(Flag.SampPt==1)//采样忙
    return;
   
   
   #if OPT_AD_EN==0
   if(Flag.AdcMd==1)//曲线调整模式
   {
       LcdClrUnit();
        
      //第一行...............................................................
      #if CH_PAIR_ENABLE==1
      LcdChNoValueA((Pt.LcdPt+2)/2,StateC.real_adc[Pt.LcdPt],0);
      #else
      LcdChNoValueA(Pt.LcdPt+1,StateC.real_adc[Pt.LcdPt],0);
      #endif
    
      if(++Pt.LcdPt>=CH_NUM)
      {
        Pt.LcdPt=0;
        LcdChNoValueB(HANG_NOT_SHOW,0,0);
        {LcdRam[15]&=~BIT7;LcdRam[7]&=~BIT0;LcdRam[8]&=~BIT0;LcdRam[15]&=~BIT0;}//清第二行单位
        BuToLcdRam(0,17);
        return;
      }
      //第二行...............................................................
      #if CH_PAIR_ENABLE==1
      LcdChNoValueB((Pt.LcdPt+1)/2,StateC.real_adc[Pt.LcdPt],0);
      #else
      LcdChNoValueB(Pt.LcdPt+1,StateC.real_adc[Pt.LcdPt],0);
      #endif
      
      if(++Pt.LcdPt>=CH_NUM)
      {
        Pt.LcdPt=0;
        BuToLcdRam(0,17);
        return;
      }
      BuToLcdRam(0,17);
        
   }
   else
   #endif
   {
       LcdClrUnit();
        
      //第一行...............................................................
      #if CH_PAIR_ENABLE==1
      LcdChNoValueA((Pt.LcdPt+2)/2,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
      #else
      LcdChNoValueA(Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
      #endif
      Lcd1Laba(SeeChAlarm(Pt.LcdPt));
      Lcd1Unit(Pt.LcdPt);
      
      //开门标记显示 k
      #if DOOR1_BJ_EN==1
      if(Pt.LcdPt==0)//第一路开关门信号
      {
        if(Pt.Door1Fg==1)
        {
          LcdK1(1);
        }
        else
        {
          LcdK1(0);
        }
      }
      #endif
      
    
      if(++Pt.LcdPt>=CH_NUM)
      {
        Pt.LcdPt=0;
        LcdChNoValueB(HANG_NOT_SHOW,0,0);
        {LcdRam[15]&=~BIT7;LcdRam[7]&=~BIT0;LcdRam[8]&=~BIT0;LcdRam[15]&=~BIT0;}//清第二行单位
        
        #if (JLY_MODEL==_3_WD_SD_QY)
            if(StateC.Value[2]==SENIOR_NULL)
            {
              LcdChNoValueB(HANG_NOT_SHOW,0,0);
            }
            else
            {
              LcdB3dst(StateC.Value[2]%100,2);//2位小数显示在B行
              LcdRam[7]=0;
            }
        #endif
        
        BuToLcdRam(0,17);
        return;
      }
      //第二行...............................................................
      #if CH_PAIR_ENABLE==1
      LcdChNoValueB((Pt.LcdPt+1)/2,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
      #else
      LcdChNoValueB(Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
      #endif
      Lcd2Laba(SeeChAlarm(Pt.LcdPt));
      Lcd2Unit(Pt.LcdPt);
      
      #if DOOR2_BJ_EN==1
      if(Pt.LcdPt==1)//第二路开关门信号
      {
        if(Pt.Door2Fg==1)
        {
          LcdK2(1);
        }
        else
        {
          LcdK2(0);
        }
      }
      #endif
      
      
      if(++Pt.LcdPt>=CH_NUM)
      {
        Pt.LcdPt=0;
        BuToLcdRam(0,17);
        return;
      }
      BuToLcdRam(0,17);
      
      LcdAlarmEn();
      LcdMode();
   }
}

#if MIN_MAX_AVG_ENABLE==1
//极值显示
void LcdExtremum(u16* Vbuf,u8 Ch)
{
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
           return;//从关屏模式，不进行显示操作
      }
    #else
      #if ((SD_ENABLE==1)&&(LCD_NO==8))
      if(Pt.sd_pt==0)
         return;//从关屏模式，不进行显示操作
      #endif
    #endif
  

   #if KEY_SET_ENABLE==1
   if(KeySetIsBusy())
     return;
   #endif
    
    if(Flag.SampPt==1)//采样忙
     return;
    
    if(StateA1.RecordNum==0)
    {
      for(u8 j=0;j<CH_NUM;j++)
        Vbuf[j]=SENIOR_NULL;
    }
    
     #if LCD_NO==8
         if(CH_NUM==1)
           Lcd08_ChValue(CH_X_NOT_SHOW+1,0x7fff&Vbuf[Ch],(0x01&(Vbuf[Ch]>>15)));
         else
           Lcd08_ChValue(Ch+1,0x7fff&Vbuf[Ch],(0x01&(Vbuf[Ch]>>15)));
     #else
    
          LcdClrUnit();
          #if CH_PAIR_ENABLE==1
          LcdChNoValueA(Ch+1,Vbuf[Ch*2],(0x01&(Vbuf[Ch*2]>>15)));
          LcdChNoValueB(Ch+1,Vbuf[Ch*2+1],(0x01&(Vbuf[Ch*2+1]>>15)));
          Lcd1Unit(Ch*2);
          Lcd2Unit(Ch*2+1);

          #else
          if(CH_NUM<=2)
          {
            LcdChNoValueA(CH_X_NOT_SHOW+1,Vbuf[Ch],(0x01&(Vbuf[Ch]>>15)));
            if(CH_NUM>1)
            {
              LcdChNoValueB(CH_X_NOT_SHOW+2,Vbuf[Ch+1],(0x01&(Vbuf[Ch+1]>>15)));
              Lcd1Unit(Ch);
              Lcd2Unit(Ch+1);
            }
            else
            {
              #if JLY_MODEL==_1_SHR_30_120_kpa
              if(Vbuf[Ch]==SENIOR_NULL)
              {
                LcdChNoValueB(HANG_NOT_SHOW,0,0);
              }
              else
              {
                LcdB3dst(Vbuf[Ch]%100,2);
                LcdRam[7]=0;
              }
              #else
              LcdChNoValueB(HANG_NOT_SHOW,0,0);
              #endif
              
              Lcd1Unit(Ch);
            }
          }
          else//CH_NUM>2
          {
            LcdChNoValueA(2*Ch+1,Vbuf[2*Ch],(0x01&(Vbuf[2*Ch]>>15)));
            Lcd1Unit(2*Ch);
            if(2*Ch+2<=CH_NUM)
            {
              LcdChNoValueB(2*Ch+2,Vbuf[2*Ch+1],(0x01&(Vbuf[2*Ch+1]>>15)));
              Lcd2Unit(2*Ch+1);
            }
            else
            {
              LcdChNoValueB(HANG_NOT_SHOW,0,0);
              
              #if (JLY_MODEL==_3_WD_SD_QY)
                  if(Vbuf[2*Ch]==SENIOR_NULL)
                  {
                    LcdChNoValueB(HANG_NOT_SHOW,0,0);
                  }
                  else
                  {
                    LcdB3dst(Vbuf[2*Ch]%100,2);//2位小数显示在B行
                    LcdRam[7]=0;
                  }
              #endif
                  
              
              Lcd2Unit(UNIT_NOT_SHOW);
            }
          }

          #endif

    #endif
}

#endif

#if RF_REC_EN==1
void RfLcdChDeel(void)
{
    #if WIFI_ENABLE==1
    if(Pt.Fg60FF==1)//关机模式
      return;
    #endif
      
    if(KeyDoing())
      return;
    if(Flag.RstFirstSample==0)
      return;
    
    if(Pt.Xz>0)
      return;
    
    
#if RF_YQ_SS_EN==1
    LcdOneChValue(Pt.LcdPt);
    
    if(++Pt.LcdPt>=RF_SEN_MAX)
      Pt.LcdPt=0;
#else
  
  
      LcdClrUnit();
      
      #if CH_NUM<=2
        LcdChNoValueA(CH_X_NOT_SHOW+Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
      #else
        
        #if CH_PAIR_ENABLE==1
            LcdChNoValueA((Pt.LcdPt+2)/2,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));    
        #else
            LcdChNoValueA(Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));    
        #endif
        
      #endif
      Lcd1Laba(SeeChAlarm(Pt.LcdPt));
      Lcd1Unit(Pt.LcdPt);
      
      if(++Pt.LcdPt>=CH_NUM)
      {
        Pt.LcdPt=0;
        LcdChNoValueB(HANG_NOT_SHOW,0,0);
        {LcdRam[15]&=~BIT7;LcdRam[7]&=~BIT0;LcdRam[8]&=~BIT0;LcdRam[15]&=~BIT0;}//清第二行单位
        
        BuToLcdRam(0,17);
        return;
      }
      
      #if CH_NUM<=2
        LcdChNoValueB(CH_X_NOT_SHOW+Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
      #else
        #if CH_PAIR_ENABLE==1
            LcdChNoValueB((Pt.LcdPt+1)/2,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
        #else
            LcdChNoValueB(Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
        #endif
      #endif
      Lcd2Laba(SeeChAlarm(Pt.LcdPt));
      Lcd2Unit(Pt.LcdPt);
      
      if(++Pt.LcdPt>=CH_NUM)
      {
        Pt.LcdPt=0;
        BuToLcdRam(0,17);
        return;
      }
      BuToLcdRam(0,17);
      
      LcdAlarmEn();
      
#endif
      
}

#endif

//在时钟数字这里显示收到的地址,ss=0为不显示
void Lcd_Xp(u8 cc,u8 addr,u8 ss)
{
  if(ss==0)
  {
      LcdRam[10]=0;
      LcdRam[11]=0;
      LcdRam[12]=0;
      LcdRam[13]=0;
  }
  else
  {
      LcdRam[10]=digit1[cc%10];
      
      if(addr>99)
        LcdRam[11]=digit1[(addr/100)%10];
      else
        LcdRam[11]=0;
      
     
      if(addr>9)
        LcdRam[12]=digit1[(addr/10)%10];
      else
        LcdRam[12]=0;
      
      LcdRam[13]=digit1[addr%10];
  }
  
  
  LcdRam[13]|=BIT0;
  
  BuToLcdRam(10,4);
}

void Lcd_P(u8 cc)
{
  LcdRam[1]=a1+b1+e1+f1+g1;//P
  LcdRam[0]=digit1[cc%10];
  BuToLcdRam(0,17);
}
void Lcd_H(u8 cc)
{
  LcdRam[1]=b1+c1+e1+f1+g1;//H
  LcdRam[0]=digit1[cc%10];
  BuToLcdRam(0,17);
}
//第二行，通道号位置
void Lcd_Bc(u8 cc)
{
  if(cc==1)
    LcdRam[9]=a1+d1+e1+f1;//c
  else
    LcdRam[9]=0;
  BuToLcdRam(9,1);
}


//LCD20==========================================================================
#if LCD_NO==20
void Lcd20_Off(u8 StopCause)
{
  if(KeyDoing())
    return;
  
  LcdRam[0]=0;
  if((StopCause==OFF_PC)||((StopCause==OFF_HAND)&&(StateB.StartMode!=START_MODE_HAND)))
  {
    LcdRam[1]=0;
  }
  else LcdRam[1]=digit1[StopCause];//
  LcdRam[2]=a1+b1+c1+d1+e1+f1;//O
  LcdRam[3]=a1+e1+f1+g1;//F
  LcdRam[4]=a1+e1+f1+g1;//F
}
void Lcd20_Null(u8 ChNo)
{
    if(CH_NUM>1)
      LcdRam[0]=digit1[ChNo];
    else
      LcdRam[0]=0x00;
    LcdRam[1]=a1+b1+c1+e1+f1;
    LcdRam[2]=b1+c1+d1+e1+f1;
    LcdRam[3]=e1+d1+f1;
    LcdRam[4]=e1+d1+f1;
}
void Lcd20_NN(u8 nn)
{
    u16 j;
    
    for(j=0;j<5;j++)
      LcdRam[j]=0;
    
    LcdRam[3]=digit1[nn/10];
    LcdRam[4]=digit1[nn%10];
    
    LcdRam[1]|=BIT3;//横杠

    BuToLcdRam(0,5);

}
void Lcd20_AL(u8 cc)//喇叭
{
  if(cc==1)
    LcdRam[0]|=BIT3;
  else
    LcdRam[0]&=~BIT3;
}

void Lcd20_3dst(u16 vv,u8 dst)
{
  if(dst==1)
  {
      if(vv>99)//百位
        LcdRam[2]=digit1[(vv/100)%10];
      else
        LcdRam[2]=0;
      if(vv>9)//十位
        LcdRam[3]=digit1[(vv/10)%10];
      else
        LcdRam[3]=digit1[0];
      LcdRam[4]=digit1[vv%10];//个位
      
      LcdRam[2]&=~BIT3;
      LcdRam[3]|=BIT3;//2P，一位小数
  }
  else if(dst==2)
  {
      if(vv>99)//百位
        LcdRam[2]=digit1[(vv/100)%10];
      else
        LcdRam[2]=digit1[0];
      if(vv>9)//十位
        LcdRam[3]=digit1[(vv/10)%10];
      else
        LcdRam[3]=digit1[0];
      LcdRam[4]=digit1[vv%10];//个位
      
      LcdRam[2]|=BIT3;
      LcdRam[3]&=~BIT3;
  }
  else
  {
      if(vv>99)//百位
        LcdRam[2]=digit1[(vv/100)%10];
      else
        LcdRam[2]=0;
      if(vv>9)//十位
        LcdRam[3]=digit1[(vv/10)%10];
      else
        LcdRam[3]=0;
      LcdRam[4]=digit1[vv%10];//个位
  }
}


//通道号,Ch,从1开始显示，如果Ch>CH_X_NOT_SHOW,不显示通道号
void Lcd20_ChValue(u8 Ch,u16 vv,u8 ff)
{
  u8 over=0;

  if(KeyDoing())
    return;
  if(Flag.RstFirstSample==0)
    return;
  if(vv==HAND_STOP)
    return;

  
  if(Ch>CH_X_NOT_SHOW)//通道号不显示
  {
    LcdRam[0]=0;
    Ch-=CH_X_NOT_SHOW;//真实通道号
  }
  else//通道号显示
  {
    LcdRam[0]=digit1[Ch];
  }

  if((vv==SENIOR_NULL)||(vv==(0x7fff&SENIOR_NULL)))//NULL显示
  {
    Lcd20_Null(Ch);
  }
  else if(vv==HAND_STOP)
  {
    Lcd20_Off(OFF_HAND);
  }
  else
  {
    if(ff)//负号
    {
      LcdRam[1]=g1;
    }
    else
    {
      if(vv>9999)//显示最大显示9999，超出后，小数位退一位
      {
        vv=vv/10;
        over=1;
      }
      
      if(vv>999)//千位
        LcdRam[1]=digit1[(vv/1000)%10];
      else
        LcdRam[1]=0;
    }
    if(_S_1==*((char*)CH1_S_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES)) 
    {
      if(over==1)
      {
        Lcd20_3dst(vv,0);
      }
      else
      {
        Lcd20_3dst(vv,1);
      }
    }
    else if(_S_2==*((char*)CH1_S_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES)) 
    {
      if(over==1)
      {
        Lcd20_3dst(vv,1);
      }
      else
      {
        Lcd20_3dst(vv,2);
      }
    }
    
  }
  //单位显示...
  LcdRam[4]&=~BIT3;//先清所有单位
  LcdRam[5]&=~BIT7;
  
  //具体单位
  if(_U_OC==*((char*)CH1_U_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES))
  {
    LcdRam[4]|=BIT3;//C
  }
  else if(_U_RH==*((char*)CH1_U_ADDR+(Ch-1)*ONE_CH_CONFIG_BYTES))
  {
    LcdRam[5]|=BIT7;//%
  }
}


void Lcd20_Deel(void)
{
   #if GPRS_TC_EN==1
   if(Pt.PrintThisDueout>0)//打印的时候不进入
     return;
   #endif
   
   #if PRINT_FZ_EN==1
   if(Pt.tout>0)
     return;
   #endif
  
  
   #if ((SIM68_EN==1)&&(RF_U1_EN==1))//U1 切换到 PC时处理
      if(Pt.U1toPC==1)
        return;
   #endif
  
   #if MIN_MAX_AVG_ENABLE==1
   if(Pt.min_max_pt>0)
     return;
   #endif
  
   #if KEY_SET_ENABLE==1
   if(KeySetIsBusy())
     return;
   #endif
    
  if(KeyDoing())
    return;
  
  if(Flag.RstFirstSample==0)
    return;
    
 
  if(Flag.SampPt==1)//采样忙
    return;
  
  
   #if LCD_X_ENABLE==1
   if(Pt.LcdxPt!=0)
     return;
   #endif
    

  if(CH_NUM<=2)//不显示通道号
  {
    Lcd20_ChValue(CH_X_NOT_SHOW+Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
    
    if(HaveALEnable())
    {
      Lcd20H(1);
    }
    else
    {
      Lcd20H(0);
    }
    
  }
  else//显示通道号
  {
    Lcd20_ChValue(Pt.LcdPt+1,StateC.Value[Pt.LcdPt],StateC.FuhaoBit&(1<<Pt.LcdPt));
  }
  

  Lcd20_AL(SeeChAlarm(Pt.LcdPt));//是否超标
  
  LcdRam[1]|=BIT3;//横杠
  BuToLcdRam(0,6);
  
  
  #if BAT_LCD_EN==1
  Lcd20Bat();
  #endif
  
  
  
  
  if(CH_NUM>1)
  {
    Pt.LcdPt++;
    if(Pt.LcdPt==CH_NUM)
      Pt.LcdPt=0;
  }
  else
  {
    Pt.LcdPt=0;//zz
  }
  
   #if LCD_X_ENABLE==1
   Pt.LcdxPt=LCD_X_TIME;
   #endif
  
}

void Lcd20Time(u8 T1,u8 T2,u8 BD)
{
  LcdRam[8]&=~0X7F;// 清除 除BIT7的其他所有段
  LcdRam[7]&=~0X7F;
  LcdRam[6]&=~0X7F;
  LcdRam[5]&=~0X7F;
  
  
  if(T1>9){
    if(BD==0)//BCD
    {
      LcdRam[8]|=digit2[T1/16];//11
      LcdRam[7]|=digit2[T1%16];//12
    }
    else//D10
    {
      LcdRam[8]|=digit2[T1/10];//11
      LcdRam[7]|=digit2[T1%10];//12      
    }
  }
  else{
    LcdRam[8]|=digit2[0];
    LcdRam[7]|=digit2[T1];
  }
  if(T2>9){
    if(BD==0)
    {
      LcdRam[6]|=digit2[T2/16];//11
      LcdRam[5]|=digit2[T2%16];//12
    }
    else
    {
      LcdRam[6]|=digit2[T2/10];//11
      LcdRam[5]|=digit2[T2%10];//12
    }
  }
  else{
    LcdRam[6]|=digit2[0];
    LcdRam[5]|=digit2[T2];
  }
}
void Lcd20YearFlag(void)
{
  LcdRam[6]&=~BIT7;
  LcdRam[7]&=~BIT7;
  
}
void Lcd20MonthFlag(void)
{
  LcdRam[6]&=~BIT7;
  LcdRam[7]|=BIT7;
  
}
void Lcd20HourFlag(void)
{
  LcdRam[7]|=BIT7;
  LcdRam[6]|=BIT7;
}

//显示Gsm信号强度 cc,0-31
void Lcd20GsmXh(u8 cc)
{
  LcdRam[9]&=~(BIT3+BIT5+BIT6+BIT7);
  
  if(cc<1)//0格 0
  {
    LcdRam[9]|=BIT3;
    LcdRam[9]&=~BIT7;
    LcdRam[9]&=~BIT6;
    LcdRam[9]&=~BIT5;
  }
  else if(cc<13)//1格 1-9
  {
    LcdRam[9]|=BIT3;
    LcdRam[9]|=BIT7;
    LcdRam[9]&=~BIT6;
    LcdRam[9]&=~BIT5;
  }
  else if(cc<20)//2格 10-19
  {
    LcdRam[9]|=BIT3;
    LcdRam[9]|=BIT7;
    LcdRam[9]|=BIT6;
    LcdRam[9]&=~BIT5;
  }
  else if(cc<32)//满格 20-31
  {
    LcdRam[9]|=BIT3;
    LcdRam[9]|=BIT7;
    LcdRam[9]|=BIT6;
    LcdRam[9]|=BIT5;
  }
  else if(cc==100)//不显示信号标志
  {
    LcdRam[9]&=~BIT3;
    LcdRam[9]&=~BIT7;
    LcdRam[9]&=~BIT6;
    LcdRam[9]&=~BIT5;
  }
  BuToLcdRam(9,1);
}

void Lcd20Bat(void)
{
  u8 cc;
  cc=GetBatt();
  if(cc>99)
    cc=99;
  

  LcdRam[9]&=~(BIT0+BIT1+BIT2);
  LcdRam[10]&=~(BIT4+BIT5+BIT6);
  
  LcdRam[9]|=BIT2;//电池框
  

  if(cc<=9)
  {
    LcdRam[9]&=~BIT0;
    LcdRam[9]&=~BIT1;
    LcdRam[10]&=~BIT6;
    LcdRam[10]&=~BIT5;
    LcdRam[10]&=~BIT4;
  }
  else if(cc<=19)//1格
  {
    LcdRam[9]&=~BIT0;
    LcdRam[9]&=~BIT1;
    LcdRam[10]&=~BIT6;
    LcdRam[10]&=~BIT5;
    LcdRam[10]|=BIT4;
  }
  else if(cc<=39)//2
  {
    LcdRam[9]&=~BIT0;
    LcdRam[9]&=~BIT1;
    LcdRam[10]&=~BIT6;
    LcdRam[10]|=BIT5;
    LcdRam[10]|=BIT4;
  }
  else if(cc<=59)//3
  {
    LcdRam[9]&=~BIT0;
    LcdRam[9]&=~BIT1;
    LcdRam[10]|=BIT6;
    LcdRam[10]|=BIT5;
    LcdRam[10]|=BIT4;
  }
  else if(cc<=79)//4
  {
    LcdRam[9]&=~BIT0;
    LcdRam[9]|=BIT1;
    LcdRam[10]|=BIT6;
    LcdRam[10]|=BIT5;
    LcdRam[10]|=BIT4;
  }
  else//5
  {
    LcdRam[9]|=BIT0;
    LcdRam[9]|=BIT1;
    LcdRam[10]|=BIT6;
    LcdRam[10]|=BIT5;
    LcdRam[10]|=BIT4;
  }

  
  BuToLcdRam(9,2);

}


void Lcd08OFFX(u8 StopCause)
{
  u16 j;
  for(j=0;j<5;j++)
    LcdRam[j]=0;
  LcdRam[9]=0;
  LcdRam[10]=0;

  
  #if BSQ_ENABLE==1
  LcdRam[1]=digit1[StopCause];
  #else
  if((StopCause==OFF_PC)||((StopCause==OFF_HAND)&&(StateB.StartMode!=START_MODE_HAND))) LcdRam[8]=0;
  else LcdRam[1]=digit1[StopCause];//
  #endif
  
  LcdRam[2]=a1+b1+c1+d1+e1+f1;//O
  LcdRam[3]=a1+e1+f1+g1;//F
  LcdRam[4]=a1+e1+f1+g1;//F
  
  LcdRam[1]|=BIT3;//横杠

  LcdRam[4]&=~BIT3;//先清所有单位
  LcdRam[5]&=~BIT7;
  

  BuToLcdRam(0,11);
}

//补传标志
void Lcd20Up(u8 on)
{
  if(on)
  {
    LcdRam[9]|=BIT4;
  }
  else
  {
    LcdRam[9]&=~BIT4;
  }
  
  BuToLcdRam(9,1);
    
}

//左上小数字显示H,代表报警开
void Lcd20H(u8 cc)
{
   if(cc==1)
   {
     LcdRam[0]=b1+c1+e1+f1+g1;  
   }
   else
   {
     LcdRam[0]=0;
   }
   BuToLcdRam(0,1);
}

#endif



void LcdTest(void)
{
  //LcdTime(88,88,1);
  //BuToLcdRam(10,4);
  
  #if LCD_NO==20
  //Lcd20_ChValue(CH_X_NOT_SHOW+1,249,0);
  //BuToLcdRam(0,6);
  
  Lcd20_Deel();
  
  #elif LCD_NO==8
  //08---------------------------------
  Lcd08_ChValue(CH_X_NOT_SHOW+1,249,0);
  Lcd08_Min();
  Lcd08_Max();
  Lcd08_Avg();
  Lcd08_Bat(1);
  Lcd08_AL(1);
  
  Lcd08_Off(OFF_PC);
  
  BuToLcdRam(0,6);
  
  //return;
  
  LcdRam[0]=digit1[0];
  LcdRam[1]=digit1[1];
  LcdRam[2]=digit1[2];
  LcdRam[3]=digit1[3];
  LcdRam[4]=digit1[4];
  Lcd08_ChValue(1,249,1);
  BuToLcdRam(0,6);
  Lcd08_ChValue(2,578,0);
  BuToLcdRam(0,6);
  
  #else
  
  //07----------------------------------
  u16 j;
  for(j=0;j<18;j++)//总计35段，最后字节为S34和S0
  {
    LcdRam[j]=0xff;
  }
  BuToLcdRam(0,18);
  /*
  for(j=0;j<17;j++)//总计35段，最后字节为S34和S0
  {
    LcdRam[j]=0;
  }
  
  LcdNull(7,0);
  BuToLcdRam(0,5);
  
  LcdNull(3,1);
  BuToLcdRam(5,5);
  
  LcdChNoValueA(1,155,0);
  BuToLcdRam(0,5);
  
  LcdChNoValueB(2,568,0);
  BuToLcdRam(5,5);
  
  //LcdOFFX(3);
  //BuToLcdRam(0,10);
  //BuToLcdRam(13,1);
  
  LcdTime(0x12,0x34,0);
  BuToLcdRam(10,4);
  LcdTime(11,32,1);
  BuToLcdRam(10,4);
  
  LcdRtcTime();
  BuToLcdRam(10,4);
  */
  #endif
  
  
}



