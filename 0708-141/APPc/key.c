#include "_config.h"

#define MT_MIN_CHA   1
#define MT_MIN_CHB   2
#define MT_MAX_CHA   3
#define MT_MAX_CHB   4
#define MT_YEAR      5
#define MT_MONTH     6
#define MT_DAY       7
#define MT_HOUR      8
#define MT_MINUTE    9
#define MT_END       10
  
void KeyDeel(void)
{
  if(Key.Step)//有按键按下
  {
      Flag.SeeComing=0;//有按钮操作则屏蔽GSM监测
      
      #if ((LCD_NO==20)&&(G_20_ENTER_EN==0))
        Pt.BGpt=BG_TIME;//背光时间  
        BG_LED_ON;
      #endif
      
      
      #if ((POWER_TIME_ENABLE==1)&&(LCD_NO==7))
      if(Pt.sd_pt==0)//省电状态
      {
          //if(Key.Step==KEY_STEP_4)//只有Key4才能切换进入正常模式
          //{
            if(WakeUp())//刚刚唤醒
            {
              KeyDoStart();
              Key.Step=0;
              KeyIntSet();//重新开启所有按键中断
              return;
            }
          //}
      }
      else//正常模式
      {
        if(Key.Step==KEY_STEP_4)
        {
           #if DIGITAL_FILTER_EN==1
           FilterClr();//滤波计数清0
           #endif
        }
      }
      #else
      if(WakeUp())//刚刚唤醒
      {
        KeyDoStart();
        Key.Step=0;
        KeyIntSet();//重新开启所有按键中断
        return;
      }
      #endif
      
      #if MIN_MAX_AVG_ENABLE==1
      if(KeyMinMaxDoing())//正在查询最大最小值
      {
        Key.Step=0;
        KeyIntSet();//重新开启所有按键中断
        return;
      }
      #endif

      if(Key.Step==KEY_STEP_1)
      {
        Key_1_Do();
        
      }
      
      #if G_20_ENTER_EN==0
        else if(Key.Step==KEY_STEP_2)
        {
          Key_2_Do();
          #if GPS_MOUSE_EN==1
          StateC.SeeBDSing=0;
          #endif
        }
        else if(Key.Step==KEY_STEP_3)
        {
          Key_3_Do();
  
        }
        else if(Key.Step==KEY_STEP_4)
        {
          Key_4_Do();

        }
      #endif

      KeyIntSet();//重新开启所有按键中断

      Key.Step=0;
  }
}
//ON/OFF
void Key_1_Do(void)
{
#if ((LCD_NO==7)||(LCD_NO==20))
  
  #if RF_KP_EN==1
  
     if(StateA2.WorkState==WORK_STATE_RECORDING)
     {
         StateA2.WorkState=WORK_STATE_STOP;//关机
         GetRfFzFromEE();//从EE中读取Pt.Fz
         LcdFz(Pt.Fz); 
         
         
     }
     else
     {
        LcdNN(1);
        StateA2.WorkState=WORK_STATE_RECORDING;
     }
     
     StateA2ToEE();
     
  #endif
  
  
  #if ((RF_REC_EN==1)&&(RECORD_ENABLE==0)&&(RF_U1_EN==0)&&(RF_MULTI_EN==0))//变送器RF
  LcdU0U1_RF();
  #endif
  
  //Flag.SeeComing=1;//zz
  //U1SendStrPC("AAA");
  
  #if ((WIFI_ENABLE==1)||(RF_RP_EN==1)||(RF_MULTI_EN==1))
       Key.Ct=1;
       Key.DecT=DEC_TIME;
  
  #elif KEY_SET_ENABLE==1
      Key.KeyDog=KEY_DOG_TIME;
      if(Key.SetModePt>0)//设置模式 -
      {
        //切换设置想Key.Mt
        Key.Mt++;
        #if CH_NUM==1
        if(Key.Mt==MT_MIN_CHA+1)
          Key.Mt=MT_MAX_CHA;
        else if(Key.Mt==MT_MAX_CHA+1)
          Key.Mt=MT_YEAR;
        #endif
        
        Mt2Kt();
        if(Key.Mt==MT_END)//结束
        {
          KeySetEnd();
        }
      }
      else
      {
        Key.Ct=1;
        Key.DecT=DEC_TIME;
        
        if(KeySetAddrQuit())
          return;

        KeyHand();//手动启动手动停止

      }
  #else

      #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1))//模块关机
            #if GPRS_TC_EN==1
                Key.Ct=1;
                Key.DecT=DEC_TIME;
            #else

                if(Sms.GsmKeyShut==0)
                {
                  SmsKeyOff();
                }
                else
                {
                  SmsKeyOn();
                   #if RTC_UTC_EN==1
                   UtcStart();
                   #endif
                }
                SmsKeySet();
      
            #endif
      
      #else
      KeyHand();//手动启动手动停止
      #endif
  #endif 
    
    
#else//lcd08
    #if LED_WORK_ENABLE==0//非指示灯模式时，才有查看
    
      #if SHUXIAN_ENABLE==1
      if(StateA2.WorkState==WORK_STATE_RECORDING)
      {
        KeyView();
      }
      #else
      KeyView();
      #endif
    
    #endif
#endif
}
//SPEAKER
void Key_2_Do(void)
{
    #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1)&&(GPRS_6OFF==1))//模块
    if(Sms.GsmKeyShut==1)
      return;
    #endif
  
#if ((LCD_NO==7)||((LCD_NO==20)&&(G_20_ENTER_EN==0)))
  #if WIFI_ENABLE==1 
        Key.Ct=2;
        Key.DecT=DEC_TIME;
    
  #elif ((KEY_SET_ENABLE==1)&&(RF_RP_EN==0)&&(RF_REC_EN==0)&&(RF_TXRX_470M_EN==0))
  Key.KeyDog=KEY_DOG_TIME;
  if(Key.SetModePt>0)//设置模式 +
  {
    KeyAccDec(Key.Kt,0);
  }
  else
  {
    Key.Ct=2;
    if(KeySetAddrQuit())
      return;
    KeyAlarm();
  }
  #else
  
    #if ((RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1)||(L_LCD_BAT_EN==1))
        Key.Ct=2;
        Key.DecT=DEC_TIME;
    #else
      KeyAlarm();
    #endif
  
  #endif 
  
#else
  
  #if SHUXIAN_ENABLE==1
    KeyShutDown();
  #else
    KeyHand();
    #if LED_WORK_ENABLE==1//手动启动或收到停机后，立即指示
    Pt.LedPt=2;
    #endif
  #endif
#endif
}
//MAX/MIN
void Key_3_Do(void)
{
//#if (RF_MULTI_EN==0)//多机接收机，该键无用
  
    #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1)&&(GPRS_6OFF==1))//模块
    if(Sms.GsmKeyShut==1)
      return;
    #endif
  
  #if ((LCD_NO==7)||((LCD_NO==20)&&(G_20_ENTER_EN==0)))
    #if (KEY_SET_ENABLE==1)
    Key.KeyDog=KEY_DOG_TIME;
    if(Key.SetModePt>0)//设置模式 -
    {
      KeyAccDec(Key.Kt,1);
    }
    else
    {
        Key.Ct=3;
        Key.DecT=DEC_TIME;
    }
    #else
    
       #if ((PRINT_NEW_EN==1)||(RF_REC_EN==1))
          Key.Ct=3;
          Key.DecT=DEC_TIME;
       #else
          KeyView();//查看
       #endif
    #endif
  #endif
          
//#endif      
}
//打印
void Key_4_Do(void)
{
  /*
    #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1))//模块
    if(Sms.GsmKeyShut==1)
      return;
    #endif
  */
  
#if ((LCD_NO==7)||((LCD_NO==20)&&(G_20_ENTER_EN==0)))
  
  //#if ((RF_REC_EN==1)&&(RECORD_ENABLE==1)&&(RF_U1_EN==0))//记录仪RF
  //LcdU0U1_RF();
  //return;
  //#endif
  
  #if RF_MULTI_EN==1//多机接收机，该键无用
       Key.Ct=4;
       Key.DecT=DEC_TIME;
  #else

  
        #if GSM_KEY_TEST_EN==1
        SmsTestStart();
        return;
        #endif
        
        #if (KEY_SET_ENABLE==1)
        Key.KeyDog=KEY_DOG_TIME;
        Key.Ct=4;
        if(KeySetAddrQuit())
          return;
        #endif 
        
         #if ((RF_REC_EN==1)||(PRINT_NEW_EN==1)||(CO2_GSS_TTL_EN==1))
           Key.Ct=4;
           Key.DecT=DEC_TIME;
         #else
            // 打印和短信开关不能同时使用
            #if PRINT_ALL_EN==1
              Bell_Off();//关闭声光和蜂鸣
              PrintAll();
            #endif
          
        #endif
              
        
        #if (KEY_SET_ENABLE==1)
        if(Key.SetModePt>0)//设置模式,确认
        {
          //设置
          Key.Kt++;
          
          if((Key.Kt==5)||(Key.Kt==13))//同时设置通道1数据最大和最小值，完成
          {
            Uart_1.RxBuf[5]=1;
            Uart_1.RxBuf[6]=0xff&Key.AlarmExt[0];
            Uart_1.RxBuf[7]=0xff&(Key.AlarmExt[0]>>8);
            Uart_1.RxBuf[8]=0xff&Key.AlarmExt[1];
            Uart_1.RxBuf[9]=0xff&(Key.AlarmExt[1]>>8);
            if(ChALEnable(0))
            {
              Uart_1.RxBuf[10]=1;
            }
            else
            {
              Uart_1.RxBuf[10]=0;
            }
            
            alarm_buf_to_flash(&Uart_1.RxBuf[6],Uart_1.RxBuf[5]-1,1);
            
            #if CH_NUM==1
            if(Key.Kt==5)
            {
              Key.Kt=9;
              Key.Mt=MT_MAX_CHA;
            }
            else if(Key.Kt==13)
            {
                Key.Kt=17;
                Key.Mt=MT_YEAR;//开始设置rtc
				
		Rtc.SS=ReadRtcD10();//先读取当前rtc

				
                RtcTemp=Rtc;
                LcdRtcShan(20,RtcTemp.Year,0);
            }
            #elif CH_NUM==2
            if(Key.Kt==5)
              Key.Mt=MT_MIN_CHB;
            else if(Key.Kt==13)
              Key.Mt=MT_MAX_CHB;
            #endif
          }
          else if((Key.Kt==9)||(Key.Kt==17))//设置通道2数据，完成
          {
              Uart_1.RxBuf[5]=2;
              Uart_1.RxBuf[6]=0xff&Key.AlarmExt[2];
              Uart_1.RxBuf[7]=0xff&(Key.AlarmExt[2]>>8);
              Uart_1.RxBuf[8]=0xff&Key.AlarmExt[3];
              Uart_1.RxBuf[9]=0xff&(Key.AlarmExt[3]>>8);
              if(ChALEnable(1))
              {
                Uart_1.RxBuf[10]=1;
              }
              else
              {
                Uart_1.RxBuf[10]=0;
              }
              
              alarm_buf_to_flash(&Uart_1.RxBuf[6],Uart_1.RxBuf[5]-1,1);
            
              if(Key.Kt==9)
                Key.Mt=MT_MAX_CHA;
              else if(Key.Kt==17)
              {
                Key.Mt=MT_YEAR;//开始设置rtc
				
		Rtc.SS=ReadRtcD10();//先读取当前rtc

				
                RtcTemp=Rtc;
                LcdRtcShan(20,RtcTemp.Year,0);
              }
          }
          else if(Key.Kt==18)//设置月
          {
            Key.Mt=MT_MONTH;
          }
          else if(Key.Kt==19)//设置日
          {
            Key.Mt=MT_DAY;
          }
          else if(Key.Kt==20)//设置时
          {
            Key.Mt=MT_HOUR;
          }
          else if(Key.Kt==21)//设置分
          {
            Key.Mt=MT_MINUTE;
          }
          else if(Key.Kt==22)//rtc设置结束
          {
           #if ((RF_MULTI_EN==1)||(RF_RP_EN==1))
           #else
            RtcD10ToBcd(&RtcTemp);
            Rtc=RtcTemp;
            RtcSetTime();//写入
			

	    Rtc.SS=ReadRtcD10();
			
            LcdTime(Rtc.Hour,Rtc.Minute,1);
            #endif
            
            KeySetEnd();
          }
        }
        else
        {
          Key.DecT=DEC_TIME;
        }
        #endif
  
  #endif
  
#else

#endif
}
void KeyShutDown(void)
{
  #if SHUXIAN_ENABLE==1
  //关机
  if(StateA2.WorkState==WORK_STATE_RECORDING)
  {
    StateA2.WorkState=WORK_STATE_STOP;
    LcdShowOn(0);
  }
  //开机
  else
  {
    StateA1.RecordNum=0;
    StateA2.WorkState=WORK_STATE_RECORDING;
    LcdShowOn(1);
  }
  #endif
}
void KeyAlarm(void)
{
#if ((RF_MULTI_EN==1)||(RF_RP_EN==1))//无线中继器和无线转换器不进入
#else
          
  
      
      if(HaveALEnable())
      {
          ChAEnToFlash(99,0);//关闭报警
          
          #if  ALARM_NO_CN==1
          LcdNN(ALARM_OFF);
          #else
          LcdAlarmOn(0);
          #endif
          
          BELL_OFF;
          SG_OFF;
          
          #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==0))
          SmsKeyOff();
          #endif
      }
      else
      {
          #if G_20_ENTER_EN==1
            #if RF_U1_EN==1
              U1_RF_PC_X();
            #endif
          #endif
        
        
          ChAEnToFlash(99,1);//打开报警
          
          #if  ALARM_NO_CN==1
          LcdNN(ALARM_ON);
          #else
          LcdAlarmOn(1);
          #endif
          
          #if ALARM_TIME_ENABLE==1
          AlarmedClr();
          #endif
          
          //#if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==0))
          //SmsKeyOn();
          //#endif
          


      }
      
      #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==0))
      SmsKeySet();
      #endif
      
      Sms.IntervalPt=10;
      LoggerAlarmAllSeek();//立即查询
      
      #if LCD_NO==7
      LcdValue();
      #endif
      
      #if ALARM_KEY_CONTINUE_EN==1
      Pt.AlarmKC=0;//必须等按键消除
      #endif
      
      #if CMD_CTRL_EN==1
      Pt.wdkai=0;
      Pt.sdkai=0;
      #endif
      
  
#endif
}
u8 KeyMinMaxDoing(void)
{
  if(Pt.min_max_pt>0)//正在显示最大最小平均值，不进入
    return 1;
  else
    return 0;
}

//判断是否为抖动
u8 KeyXiaoDou(void)
{
    u8 KeyPut=0;
    
    WDT_CLR;
    TIME_KEY_STOP;//时间到，关闭定时器
    if(Key.Step==KEY_STEP_1)
    {
      if((KEY_PIN&KEY_1_BIT)==0)
         KeyPut=1;
    }
    else if(Key.Step==KEY_STEP_2)
    {
      if((KEY_PIN&KEY_2_BIT)==0)
        KeyPut=1;
    }
    else if(Key.Step==KEY_STEP_3)
    {
      if((KEY_PIN&KEY_3_BIT)==0)
        KeyPut=1;
    }
    else if(Key.Step==KEY_STEP_4)
    {
      if((KEY_PIN&KEY_4_BIT)==0)
        KeyPut=1;
    }
    
    if(KeyPut==1)
    {
      return 1;//判断为按键按下
    }
    else
    {
      Key.Step=0;
      KeyIntSet();//重新开启所有按键中断
      #if KEY_SET_ENABLE==1
      KeySetAddrQuit();
      #endif
      return 0;//判断为按键抖动
    }
}

#if ((KEY_SET_ENABLE==1)||(PRINT_NEW_EN==1)||(CO2_GSS_TTL_EN==1)||((RF_REC_EN==1)||(RF_RP_EN==1))||(RF_TXRX_470M_EN==1)||(L_LCD_BAT_EN==1))
//0.25秒循环一次
void Loop2Deel(void)
{
  if(Flag.KeyLcdLoop==1)
  {
    Flag.KeyLcdLoop=0;

    #if KEY_SET_ENABLE==1
    KeySetModeDeel();
    #endif
    KeyDecDeel();
    
    
     //中继打包转发数据
     #if ((RF_MULTI_EN==1)&&(RF_MULTI_MNJ==0))
    
     if(Pt.RpOff==0)//关机时，不处理，储存与转发
     {
      
         if(Pt.pcpt<65*4)
           Pt.pcpt++; 
         
    
         if(Pt.RecRpPt==RF_REC_RP_TIME*4-4)//zz 2s
         {
           if(StateB.Address>1)
           {
               RfReset();//无线模块软件复位
           }
         }
         
          
         if(++Pt.RecRpPt>RF_REC_RP_TIME*4)
         {
                 if(StateB.Address>1)//地址>1才有转发功能
                 {
                      //发送数据包
                      RecRpSend();
                      
                      //随机数
                      u32 tt;
                      
                      tt=TAR;
                      tt+=TBR;
                      tt+=StateC.Adc[3];
                      tt+=StateC.Adc[4];
                      tt+=StateC.Adc[5];
                      tt+=StateC.Adc[6];
                      
                      tt=tt%40;
                      
                      Pt.RecRpPt=tt;//zz  从（0-10s）重新开始  ,所以实际发送间隔是 50-60s
                      
                      MultiBufEE(1);//保存到EE
                 }
                 else
                 {
                     if((Pt.pcpt<=30*4)||(Pt.pcpt>=65*4))//30-65时等待pc指令，不进行EE储存操作
                     {
                        Pt.RecRpPt=0;
                        MultiBufEE(1);//保存到EE
                        RfReset();//无线模块软件复位
                     }
                 }
    
         }
     
     }
      
     #endif
    
    
  }
}
#endif

#if KEY_SET_ENABLE==1
//1秒循环一次
void Loop1sDeel(void)
{
    //#if BSQ_ENABLE==1
    //bsq_allwaysSet();
    //#endif
    
    KeyBsqAddrAcc();
    KeyDogDeel();
}
void KeyDogDeel(void)
{
  if(Key.KeyDog>0)
  {
    if(--Key.KeyDog==0)
    {
      KeySetEnd();
    }
  }
}
void KeySetStart(void)
{
   GetAlarmExt(Key.AlarmExt);//取原始值,所有通道
   Key.Mt=MT_MIN_CHA;
   LcdKeyMode(Key.Mt,0);
   Key.Kt=1;
   
   #if ((RF_MULTI_EN==1)||(RF_RP_EN==1))
   #else
   
   Rtc.SS=ReadRtcD10();
   
   LcdTime(Rtc.Hour,Rtc.Minute,1);
   LcdHourFlag();
   #endif
}

//按键设置所有参数完成后，清标志
void KeySetEnd(void)
{
  Key.Mt=0;
  Key.Kt=0;
  Key.AddrPt=0;
  Key.SetModePt=0;
  
   #if ((RF_MULTI_EN==1)||(RF_RP_EN==1))
   #else
   
   Rtc.SS=ReadRtcD10();
   
   
   LcdTime(Rtc.Hour,Rtc.Minute,1);
   LcdHourFlag();
   #endif
}
void Mt2Kt(void)
{
  if(Key.Mt==MT_MIN_CHB)
    Key.Kt=5;
  else if(Key.Mt==MT_MAX_CHA)
    Key.Kt=9;
  else if(Key.Mt==MT_MAX_CHB)
    Key.Kt=13;
  else if(Key.Mt==MT_YEAR)
  {
      Key.Kt=17;

      Rtc.SS=ReadRtcD10();
	  
      RtcTemp=Rtc;
      LcdRtcShan(20,RtcTemp.Year,0);
  }
  else if(Key.Mt==MT_MONTH)
    Key.Kt=18;
  else if(Key.Mt==MT_DAY)
    Key.Kt=19;
  else if(Key.Mt==MT_HOUR)
    Key.Kt=20;
  else if(Key.Mt==MT_MINUTE)
    Key.Kt=21;
}
u8 KeySetIsBusy(void)
{
   if(Key.AddrPt>0)
     return 1;
   if(Key.SetModePt>0)
     return 1;
   return 0;
}

//取报警设定的上下限
void GetAlarmExt(u16* buf)
{
  u8 ChNo;
  for(ChNo=0;ChNo<CH_NUM;ChNo++)
  {
    buf[ChNo*2]=*((char *)CH1_A_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_L_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo)));
    buf[ChNo*2+1]=*((char *)CH1_A_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_H_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo)));
  }
}
// 取数的符号千百十个位到vbit
void vvTovbit(u16 vv,u8* vbit)
{
  if(vv&0x8000)
  {
    vbit[4]=1;
    vv=vv&0x7fff;
  }
  else
  {
    vbit[4]=0;
  }
  vbit[3]=(vv/1000)%10;
  vbit[2]=(vv/100)%10;
  vbit[1]=(vv/10)%10;
  vbit[0]=vv%10;
}
u16 vbitTovv(u8* vbit)
{
  u16 vv;
  vv=vbit[3]*1000+vbit[2]*100+vbit[1]*10+vbit[0];
  if(vbit[4])
  {
    vv|=0x8000;
  }
  return vv;
}
//按钮操作不同状态下流程
void KeySetModeDeel(void)
{
  switch (Key.SetModePt)
  {
    case 1://进入按钮设置操作
      KeySetStart();
      Key.SetModePt=2;
      break;
    case 2://闪烁操作
      if(Key.FlagS1==1)
      {
        Key.FlagS1=0;
        LcdKeyMode(Key.Mt,0);
      }
      else
      {
        Key.FlagS1=1;
        LcdKeyMode(Key.Mt,Key.Kt);
      }
      break;
      
    default:break;
  }
}
//A 通道cha  闪Kta/ B 通道 chb 闪Ktb
void LcdChABmin(u8 cha,u8 Kta,u8 chb,u8 Ktb)
{
    #if CH_NUM==1
    LcdChAB(cha,Key.AlarmExt[cha*2],Kta);
    LcdChNoValueB(HANG_NOT_SHOW,0,0);
    #else
    LcdChAB(cha,Key.AlarmExt[cha*2],Kta);
    LcdChAB(chb,Key.AlarmExt[chb*2],Ktb);
    #endif
}
//A 通道cha / B 通道 chb
void LcdChABmax(u8 cha,u8 Kta,u8 chb,u8 Ktb)
{
    #if CH_NUM==1
    LcdChAB(cha,Key.AlarmExt[cha*2+1],Kta);
    LcdChNoValueB(HANG_NOT_SHOW,0,0);
    #else
    LcdChAB(cha,Key.AlarmExt[cha*2+1],Kta);
    LcdChAB(chb,Key.AlarmExt[chb*2+1],Ktb);
    #endif
}


//按钮设置时，不同状态的显示
void LcdKeyMode(u8 Mt,u8 Kt)
{
  switch(Mt)
  {
    case MT_MIN_CHA://显示min chA
      LcdChABmin(0,Kt,1,0);
      LcdMin();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
      break;
  case MT_MIN_CHB://显示min chB
      LcdChABmin(0,0,1,Kt);
      LcdMin();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_MAX_CHA://显示max chA
      LcdChABmax(0,Kt,1,0);
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
      break;
  case MT_MAX_CHB://显示max chB
      LcdChABmax(0,0,1,Kt);
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
      break;
  case MT_YEAR:// 显示年
      LcdChABmax(0,0,1,0);
      LcdRtcShan(20,RtcTemp.Year,Kt);
      LcdYearFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_MONTH://显示月
      LcdChABmax(0,0,1,0);
      LcdRtcShan(RtcTemp.Month,RtcTemp.Day,Kt);
      LcdMonthFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_DAY://显示日
      LcdChABmax(0,0,1,0);
      LcdRtcShan(RtcTemp.Month,RtcTemp.Day,Kt);
      LcdMonthFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_HOUR://显示时
      LcdChABmax(0,0,1,0);
      LcdRtcShan(RtcTemp.Hour,RtcTemp.Minute,Kt);
      LcdHourFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_MINUTE://显示分
      LcdChABmax(0,0,1,0);
      LcdRtcShan(RtcTemp.Hour,RtcTemp.Minute,Kt);
      LcdHourFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  default:break;
  }
}
//Dec 0 +  1 -
u16 KeySetOne(u8 Kt,u8 Dec,u16 vch)
{
    u8 vbit[5];
    u16 cc;
    cc=vch;
    
    vvTovbit(vch,vbit);
    
    if((Kt==1)||(Kt==5)||(Kt==9)||(Kt==13))//千位或正负号
    {
          if(Dec==0)//+1
          {
            if(vbit[4]==0)
            {
              if(++vbit[3]>9)
              {
                vbit[4]=1;//设定为负数
                vbit[3]=0;
              }
            }
            else
            {
              vbit[4]=0;//设定为正数0
              vbit[3]=0;
            }
          }
          else//-1
          {
            if(vbit[4]==0)
            {
              if(--vbit[3]>9)
              {
                vbit[4]=1;//设定为负数
                vbit[3]=0;
              }
            }
            else
            {
              vbit[4]=0;//设定为正数9
              vbit[3]=9;
            }
          }
    }
    else if((Kt==2)||(Kt==6)||(Kt==10)||(Kt==14))//百位
    {
          if(Dec==0)
          {
            if(++vbit[2]>9)vbit[2]=0;
          }
          else
          {
            if(--vbit[2]>9)vbit[2]=9;
          }
    }
    else if((Kt==3)||(Kt==7)||(Kt==11)||(Kt==15))//十位
    {
          if(Dec==0)
          {
            if(++vbit[1]>9)vbit[1]=0;
          }
          else
          {
            if(--vbit[1]>9)vbit[1]=9;
          }
    }
    else if((Kt==4)||(Kt==8)||(Kt==12)||(Kt==16))// 个位
    {
          if(Dec==0)
          {
            if(++vbit[0]>9)vbit[0]=0;
          }
          else
          {
            if(--vbit[0]>9)vbit[0]=9;
          }
    }
    cc=vbitTovv(vbit);
    return cc;
}

//按钮+-操作
void KeyAccDec(u8 Kt,u8 Dec)
{
  if(Kt<5)
  {
    Key.AlarmExt[0]=KeySetOne(Kt,Dec,Key.AlarmExt[0]);
  }
  else if(Kt<9)
  {
    Key.AlarmExt[2]=KeySetOne(Kt,Dec,Key.AlarmExt[2]);
  }
  else if(Kt<13)
  {
    Key.AlarmExt[1]=KeySetOne(Kt,Dec,Key.AlarmExt[1]);
  }
  else if(Kt<17)
  {
    Key.AlarmExt[3]=KeySetOne(Kt,Dec,Key.AlarmExt[3]);
  }
  //开始设置rtc
  else if(Kt==17)//设置年
  {
    if(Dec==1)
    {
        RtcTemp.Year--;
        if(RtcTemp.Year==255)
        {
          RtcTemp.Year=0;
        }
    }
    else
    {
        RtcTemp.Year++;
        if(RtcTemp.Year==100)
        {
          RtcTemp.Year=0;
        }
    }
  }
  else if(Kt==18)//设置月
  {
    if(Dec==1)
    {
        RtcTemp.Month--;
        if(RtcTemp.Month==0)
        {
          RtcTemp.Month=12;
        }
    }
    else
    {
        RtcTemp.Month++;
        if(RtcTemp.Month>=13)
        {
          RtcTemp.Month=1;
        }
    }
  }
  else if(Kt==19)//设置 日
  {
    if(Dec==1)
    {
        RtcTemp.Day--;
        if(RtcTemp.Day==0)
        {
          RtcTemp.Day=31;
        }
    }
    else
    {
        RtcTemp.Day++;
        if(RtcTemp.Day>=32)
        {
          RtcTemp.Day=1;
        }
    }
  }
  else if(Kt==20)//设置 时
  {
    if(Dec==1)
    {
        RtcTemp.Hour--;
        if(RtcTemp.Hour==255)
        {
          RtcTemp.Hour=23;
        }
    }
    else
    {
        RtcTemp.Hour++;
        if(RtcTemp.Hour>=24)
        {
          RtcTemp.Hour=0;
        }
    }
  }
  else if(Kt==21)//设置 分
  {
    if(Dec==1)
    {
        RtcTemp.Minute--;
        if(RtcTemp.Minute==255)
        {
          RtcTemp.Minute=59;
        }
    }
    else
    {
        RtcTemp.Minute++;
        if(RtcTemp.Minute>=60)
        {
          RtcTemp.Minute=0;
        }
    }
  }
  LcdKeyMode(Key.Mt,0);
}

//设置地址
void KeySetAddr(void)
{
   I2C_EE_BufferWrite(&Key.AddrPt,ADDRESS_ADDR,1,0);//设置地址
   I2C_EE_BufferRead(&StateB.Address,ADDRESS_ADDR,1,0);
   LcdNN(StateB.Address);
   Key.AddrPt=0;//退出设置地址模式
}
//设置地址后退出
u8 KeySetAddrQuit(void)
{

  #if BSQ_ENABLE==1
  if(Key.AddrPt>0)
  {
    #if RF_KP_EN==1
       if(StateA2.WorkState==WORK_STATE_RECORDING)//开机状态下，设置地址
       {
           KeySetAddr();
       }
       else//关机状态下，设置频率
       {
           VDC33_ON;
           delay_ms(50);
                             
           Pt.Fz=Key.AddrPt;
           LcdFz(Pt.Fz); 
                        
           SetRfFzToEE();
           GetRfFzFromEE();//验证是否正确
           RfSetF(Pt.Fz);
           
           RF_IN_SLP;
       }    
       
    #else
                    
      KeySetAddr();
      
    #endif
    
    Key.AddrPt=0;
    Key.DecT=0;

    return 1;
  }
  #endif
  return 0;
}
void KeyBsqAddrAcc(void)
{
  if(Key.AddrPt>0)
  {
    if(Key.DecT==0)
    {
      if(Key.FlagX1==1)//第一个不变
      {
        Key.FlagX1=0;
      }
      else
      {
        #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))//中继器地址最大9
          Key.AddrPt++;
          
          if(Key.AddrPt>9)
          {
            Key.AddrPt=0;
            LcdNN(Key.AddrPt);
            LcdRpSt();
          }
          else
          {
            LcdNN(Key.AddrPt);
          }
        #else
           Key.AddrPt++;
           LcdNN(Key.AddrPt);
        #endif
        
      }
    }
  }
}


#endif

#if ((KEY_SET_ENABLE==1)||(PRINT_NEW_EN==1)||(CO2_GSS_TTL_EN==1)||(RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1)\
  ||(L_LCD_BAT_EN==1))

#if ((RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))
void KeyXzAcc(void)
{
  if(Pt.Xz>0)
  {
      if(Pt.Foo==1)
      {
        Pt.Foo=0;
        Pt.Xz++;
      }
      else
      {
        Pt.Foo=1;
        
      }
      
      if(Pt.Xz>30)
        Pt.Xz=0;//zz
      LcdNN(Pt.Xz);
  }
}
#endif

void KeyDecDeel(void)
{
  if(Key.DecT>0)
  {
    Key.DecT--;

    #if WIFI_ENABLE==1
        if(Key.Ct==1)//key1，OFF
        {
            if((KEY_PIN&KEY_1_BIT)==0)//没有释放
            {
                if(Key.DecT==0)
                {
                    if((KEY_PIN&KEY_2_BIT)==0)//zz, 2个键同时按住
                    {
                      Pt.ReloadPt=4;
                      P4OUT|=BIT2;//拉低，等待4秒后，自动释放
                      LcdNN(WIFI_RELOAD);
                      Delay_100MS(4);
                      
                      Pt.WifiPt=0;
                    
                    }
                 }
            }
            else//已经释放
            {
                if(Key.DecT>4)
                {
                    Key.DecT=0;
                     if(Pt.Fg60FF==0)
                        Pt.Fg60FF=1;
                     else
                        Pt.Fg60FF=0;
                 
                     OFF6toOFF(Pt.Fg60FF);
                     OFF6KeySet();
                     
                      #if RF_REC_EN==1
                      RfDataNULL();//初始NULL
                      Pt.SampPt=RF_SAMP_TIME-1;
                      #endif
    
                }
            }
            
            return;

        }
        
        if(Key.Ct==2)//key2， SPK
        {
            if((KEY_PIN&KEY_2_BIT)==0)//没有释放
            {
                if(Key.DecT==0)
                {
                    if((KEY_PIN&KEY_1_BIT)==0)//zz, 2个键同时按住
                    {
                      Pt.ReloadPt=4;
                      P4OUT|=BIT2;//拉低，等待4秒后，自动释放
                      LcdNN(WIFI_RELOAD);
                      Delay_100MS(4);
                      
                      Pt.WifiPt=0;
                    
                    }
                 }
            }
            else//已经释放
            {
                if(Key.DecT>4)
                {
                  Key.DecT=0;
                  
                  KeyAlarm();//报警开关
    
                }
            }
            return;
        }
        
        
    #endif

   #if ((((PRINT_NEW_EN==1)||(RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))&&(CO2_GSS_TTL_EN==0))||(L_LCD_BAT_EN==1))
    
    #if GPRS_TC_EN==1
    if(Key.Ct==1)//key1，OFF
    {
        if((KEY_PIN&KEY_1_BIT)==0)//没有释放
        {
            if(Key.DecT==0)
            {
                if(Sms.GsmKeyShut==0)//开机时，关机
                {
                    SmsKeyOff();
                    SmsKeySet();
                    
                    
                    Pt.PrintThisDueout=0;//关机,则退出打印 zz
                    
                    #if ON_OFF_BJ_EN==1
                    //保存关机标记和时间
                    Rtc.SB=ReadRtcD10();
                    PCBSQ_RecOne((u8)GJ_BJ);//记录关机标记
                    #endif
                    
                    #if RF_U1_EN==1
                    U1ToRf();//打印完成后，切换到RF
                    #endif


                }
            }
        }
        else//已经释放
        {
            if(Key.DecT>4)
            {
                Key.DecT=0;
                
                if(Sms.GsmKeyShut==1)//关机时，开机
                {
                    SmsKeyOn();
                    SmsKeySet();
                    
                    #if RTC_UTC_EN==1
                    UtcStart();
                    #endif
                    

                    #if ON_OFF_BJ_EN==1
                    //保存开机标记和时间
                    Rtc.SB=ReadRtcD10();
                    PCBSQ_RecOne((u8)KJ_BJ);//记录开机标记
                      
                    PcBsq.PCwaitTT=8;//zz
                    #endif

                }
                
                #if G_20_ENTER_EN==1
                else//开机时为切换报警开关
                {
                  
                    KeyAlarm();
                  
                }
                #endif

            }
        }
    }

    #endif
    

    
    #if ((RECORD_ENABLE==0)&&(GPRS_ENABLE==0)&&(WIFI_ENABLE==0))
    if(Key.Ct==1)//key1，OFF,,设置地址键
    {
        if((KEY_PIN&KEY_1_BIT)==0)//没有释放
        {
            if(Key.DecT==0)
            {
                  Key.FlagX1=1;
                  Key.AddrPt=1;
                  LcdNN(Key.AddrPt);//从地址1开始，设置完成后，变为0，表示退出设置状态
            }
        }
        else//已经释放
        {
            if(Key.DecT>4)
            {
                Key.DecT=0;
                if(KeySetAddrQuit())//设置地址
                {
                    //设置地址完成后
                    #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
                    LcdRpSt();
                    #endif
                }
                else
                {
                    #if RF_MULTI_EN==0
                  
                      if(Pt.RpOff==0)
                      {
                          //中继器关闭
                          Pt.RpOff=1;
                          
                          
                          #if RF_MULTI_EN==1
                          RfDataNULL();
                          
                            #if U0U1_ENABLE==1
                            Flag.U0U1ing=1;
                            LcdOFFX(OFF_RF_U0U1);
                            #else
                            LcdOFFX(6);
                            #endif
                          
                          #else
                            
                          IE2 &= ~UCA0RXIE;//zz
                          LcdOFFX(OFF_BSQ_GPRS_HAND_STOP);//60FF
                          
                          #endif
                          
                          
                          #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
                          Lcd_Xp(0,0,0);
                          #endif        
                          
                          //需进入透传模式，故不能断电
                          //VDC33_OFF;
                          //U0_TXD_CLR;//省电
                        
                      }
                      else
                      {
                          //开启
                          Pt.RpOff=0;
                          IE2 |= UCA0RXIE;
                          
                          #if RF_MULTI_EN==1
                            #if U0U1_ENABLE==1
                            Flag.U0U1ing=0;
                            #endif
                          LcdNN(1);
                          Pt.Spt=9;//立即读连接数
                          
                          #if RF_MULTI_MNJ==1
                          RfDataMN();
                          #endif
                          
                          
                          #endif
                
                          U0_TXD_SET;
                          
                          VDC33_ON;
                          U1_TO_RF;
                          RF_IN_REC;
                          
                          #if (RF_RP_EN==1)
                             Pt.LinkN=0;
                             Pt.ScanPt=0;
                             Rf_NULL_Rp();
                             LcdRpSt();
                             
                          #elif (RF_MULTI_EN==1)
                             
                             Pt.Spt=9;
                            
                          #endif
                        
                       }
                      
                   #endif
    
                }


            }
            
            

            
        }
    }
    
    
    #endif
    
    
    #if G_20_ENTER_EN==0
    
    if(Key.Ct==2)//key2， SPK
    {
        if((KEY_PIN&KEY_2_BIT)==0)//没有释放
        {
            #if L_LCD_BAT_EN==1
            if(Key.DecT==0)
            {
               //查看电量
              #if BAT_AD_ENABLE==1
              LcdNN(GetBatt());
              Lcd_Bfh();
              
              Delay_100MS(4);
              #endif
            }
          
            #elif WIFI_ENABLE==1
            if(Key.DecT==0)
            {
                if((KEY_PIN&KEY_1_BIT)==0)//zz, 2个键同时按住
                {
                      Pt.ReloadPt=4;
                      P4OUT|=BIT2;//拉低，等待4秒后，自动释放
                      LcdNN(WIFI_RELOAD);
                      Delay_100MS(4);
                      
                      Pt.WifiPt=0;
                    
                }
            }
            #endif
        }
        else//已经释放
        {
            if(Key.DecT>4)
            {
              Key.DecT=0;
              
              KeyAlarm();//报警开关

            }
        }
    }
    if(Key.Ct==3)//key3
    {
        if((KEY_PIN&KEY_3_BIT)==0)//没有释放
        {
          if(Key.DecT==0)
          {
            if((KEY_PIN&KEY_4_BIT)==0)//key4没有释放
            {
              
                #if LCD_NO==7
                PrintKeyClr();
                #endif

              
            }
            else//key4没按下，而key3长按,设置频率
            {   
                #if LCD_NO==20//20为清空数据
                PrintKeyClr();
                #endif
              
              
                #if ((RF_REC_EN==1)||(RF_RP_EN==1))
                Pt.KeyApt=3;//2秒后再次检测按键引脚
                #endif
            }
          }
        }
        else//已经释放
        {
          if(Key.DecT>4)
          {
              #if ((RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))
              if(Pt.Xz>0)
              {
                  //设置频率
                  Pt.Fz=Pt.Xz;
                  SetRfFzToEE();//Pt.Fz保存到EE
                  GetRfFzFromEE();//从EE中读取Pt.Fz
                  
                  LcdFz(Pt.Fz);
                  RfSetF(Pt.Fz);//设置到RF
                  delay_ms(100);
                  RF_IN_REC;
                  
                  Pt.Xz=0;//设置结束
                  
                  #if RF_REC_EN==1
                  RfDataNULL();//初始NULL
                  Pt.SampPt=RF_SAMP_TIME-1;
                  #endif
                  
                  #if (RF_RP_EN==1)
                  Pt.LinkN=0;
                  Pt.ScanPt=0;
                  Rf_NULL_Rp();
                  #endif
                  
                  //显示至少持续1秒
                  delay_ms(500);
                  WDT_CLR;
                  delay_ms(500);
                  WDT_CLR;
                  
                  
                  #if RF_U1_EN==1
                    U1ToRf();
                  #endif
                  
                  
                  #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
                  LcdRpSt();
                  #endif

              }
              else
              #endif  
              {
            
                #if RF_U1_EN==1
                  U1_RF_PC_X();//U1-RF-PC切换
                #else
                  KeyView();//查看               
                #endif
              }

              Key.DecT=0;
          }
        }
    }
    
    
    if(Key.Ct==4)//key4
    {
        if((KEY_PIN&KEY_4_BIT)==0)//没有释放
        {
          if(Key.DecT==0)
          {
            if((KEY_PIN&KEY_3_BIT)==0)//key3没有释放
            {
              
                #if LCD_NO==7
                PrintKeyClr();
                #endif


            }
            else
            {
              //RF同步配对
              #if ((RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))
                  RfReset();//复位 
                  
                  GetRfFzFromEE();//验证是否正确  zz
                  LcdFz(Pt.Fz);//显示当前频率
                  
                  #if RF_RP_EN==0
                    delay_ms(500);
                    WDT_CLR;
                    delay_ms(500);
                    WDT_CLR;
  
                    SendSnToSenior();//Rf发送同步指令
                    delay_ms(200);
                    WDT_CLR;
                    delay_ms(300);
                    WDT_CLR;
                    SendSnToSenior();//Rf发送同步指令
                  #endif
                    
               #endif
            }
          }
        }
        else//已经释放
        {
          if(Key.DecT>4)
          {
              #if PRINT_NEW_EN==1
                  Bell_Off();//关闭声光和蜂鸣
                  
                  #if (((RS232_ENABLE==1)&&(RRINT_NEW_N_EN==1))||(L_REALOUT_PRINT_EN==1))//打印
                  
                  
                      #if PRINT_FZ_EN==1
                  
                          if(Pt.prt_x<=PRT_CT)
                          {
                            Pt.prt_x++;
                            if(Pt.prt_x>PRT_CT)
                              Pt.prt_x=1;
                          }

                          
                          LcdNN(Pt.prt_x);
                          Pt.tout=TOUT_TIME;
                        
                      #else
                        
                          
                          #if L_REALOUT_PRINT_EN==1
                          StateC.sending=1; 
                          #endif  
                          
                          Print_Begin();//开始
                          
                          #if L_REALOUT_PRINT_EN==1
                          StateC.sending=0;
                          #endif  
                          
                          
                      #endif
    
                          
                  #endif
              #endif
              Key.DecT=0;
          }
        }
    }
    #endif
    
    //PRINT_NEW_EN==1
  #elif CO2_GSS_TTL_EN==1
    if(Key.Ct==3)//key3
    {
        if((KEY_PIN&KEY_3_BIT)==0)//没有释放
        {
          if(Key.DecT==0)
          {
            if((KEY_PIN&KEY_4_BIT)==0)//key4没有释放
            {
             
            }
          }
        }
        else//已经释放
        {
          if(Key.DecT>4)
          {
              #if RF_U1_EN==1
                U1_RF_PC_X();//U1-RF-PC切换
              #else
                KeyView();//查看               
              #endif

              Key.DecT=0;
          }
        }
    }
    
    if(Key.Ct==4)//key4，第一个键LOG
    {
        if((KEY_PIN&KEY_4_BIT)==0)//没有释放
        {
          if(Key.DecT==0)
          {
            #if SEN_U1_EN==1
    
                 U1_TO_RF;//必须切换到传感器
                 delay_ms(10);   
                 
                  #if TTL_ZERO_X10==1
                    U1SendString("X 42\r\n");
                    delay_ms(60);  
                    U1SendString("X 42\r\n");
                  #else
                    U1SendString("X 420\r\n");
                    delay_ms(60);  
                    U1SendString("X 420\r\n");
                  #endif

            #else
                  #if TTL_ZERO_X10==1
                    U0SendString("X 42\r\n");
                  #else
                    U0SendString("X 420\r\n");
                  #endif              
            #endif
                
              LcdNN(CO2_SET_0);
              delay_ms(50);
              WDT_CLR;
              delay_ms(50);
              
              
                  #if DIGITAL_FILTER_EN==1
                  for(u8 j=0;j<CH_NUM;j++)
                    FilterChClr(j);//对应通道数字滤波变量清0
                  #endif
              
          }
        }
        else//已经释放
        {
          if(Key.DecT>4)
          {
            Key.DecT=0;
          }
        }
    }


    
   #elif KEY_SET_ENABLE==1
    if(Key.Ct==1)//key1,第四个键，电源
    {
        if((KEY_PIN&KEY_1_BIT)==0)//一直没有释放
        {
          if(Key.DecT==0)
          {
              #if ((U0U1_ENABLE==1)&&(RECORD_ENABLE==0))
              U0U1set();
              #endif
              
              #if WIFI_ENABLE==1//wifi reload
              Pt.ReloadPt=4;
              P4OUT|=BIT2;//拉低，等待4秒后，自动释放
              LcdNN(WIFI_RELOAD);
              #endif
          }
        }
        else
        {
           if(Key.DecT>4)
           {
               Key.DecT=0;
             
               #if WIFI_ENABLE==1//开关机
               if(Pt.Fg60FF==0)
                  Pt.Fg60FF=1;
               else
                  Pt.Fg60FF=0;
           
               OFF6toOFF(Pt.Fg60FF);
               OFF6KeySet();
               #endif
           }
        }
    }
    
    if(Key.Ct==3)//key3,第二个键MAX
    {
        if((KEY_PIN&KEY_3_BIT)==0)//没有释放
        {
          if(Key.DecT==0)
          {
            #if BSQ_ENABLE==1
            Key.FlagX1=1;
            Key.AddrPt=1;
            LcdNN(Key.AddrPt);//从地址1开始，设置完成后，变为0，表示退出设置状态
            #endif
          }
        }
        else//已经释放
        {
          if(Key.DecT>4)
          {

                  if(KeySetAddrQuit())//设置地址
                  {
                    //设置地址完成后
                    #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
                    LcdRpSt();
                    #endif
                  }
                  else
                  { 
                     KeyView();//查看
                     Key.DecT=0;
                  }

          }
        }
    }
    
    
    if(Key.Ct==4)//key4，第一个键LOG
    {
        if((KEY_PIN&KEY_4_BIT)==0)//没有释放
        {
          if(Key.DecT==0)
          {
              #if RF_KP_EN==1
                          
              #else
            
                  #if ((U0U1_ENABLE==1)&&(RECORD_ENABLE==1))
                  U0U1set();
                  #else
                    #if ((RF_MULTI_EN==1)||(RF_RP_EN==1))//无线中继器和无线转换器不进入
                    #else
                    Key.SetModePt=1;//进入设置模式 
                    #endif
                  #endif
                    
              #endif
          }
        }
        else//已经释放
        {
          if(Key.DecT>4)
          {
            #if BSQ_ENABLE==1
            
              #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
            
              #elif RF_KP_EN==1
            
                if(StateA2.WorkState==WORK_STATE_RECORDING)
                {
                   LcdNN(StateB.Address);
                   
                   IE2 &=~ UCA0RXIE;
                }
                else
                {
                   LcdHHHH();
                   
                   Pt.snpt=HHHH_TIME;
                   
                   VDC33_ON;
                   RF_IN_REC;//接收状态
                   
                   RfU_Init(115200,1);
                   IE2 |= UCA0RXIE;
                }
            
            
              #else
                LcdNN(StateB.Address);
              #endif
            
            #endif
            Key.DecT=0;
          }
        }
    }
    #endif
  }
}
#endif


void KeyTimeDe(void)
{
  if(Key.KeyTimePt>0)
  {
    Key.KeyTimePt--;

    #if RECORD_ENABLE==1
    if((Key.KeyTimePt==0)&&(Flag.HandJustStart==1))//手动启动时，延时几秒才启动
    {
       Flag.HandJustStart=0;
       HandStart();
    }
    #endif
  }
}
void KeyDoStart(void)
{
  Key.KeyTimePt=KEY_TIME;
}
u8 KeyDoing(void)//正在进行按键操作，此期间屏蔽采样
{
  if(Key.KeyTimePt>0)
    return 1;
  else
    return 0;
}
//按键中断处理
void KeyIsrDo(void)
{
  WDT_CLR;
  
  if((KEY_IFG&KEY_1_BIT)==KEY_1_BIT)
  {
    if((KEY_PIN&KEY_1_BIT)==0)
      Key.Step=KEY_STEP_1;
  }
  
#if G_20_ENTER_EN==0
  if((KEY_IFG&KEY_2_BIT)==KEY_2_BIT)
  {
    if((KEY_PIN&KEY_2_BIT)==0)
      Key.Step=KEY_STEP_2;
  }
  if((KEY_IFG&KEY_3_BIT)==KEY_3_BIT)
  {
    if((KEY_PIN&KEY_3_BIT)==0)
      Key.Step=KEY_STEP_3;
  }
  if((KEY_IFG&KEY_4_BIT)==KEY_4_BIT)
  {
    if((KEY_PIN&KEY_4_BIT)==0)
      Key.Step=KEY_STEP_4;
  }
#endif
  
  KEY_IFG=0;
  if(Key.Step)//有键按下
  {
    KeyIntClr();//处理按键时屏蔽所有按键中断
    TIME_KEY_START;//开启定时器，等待延时，消抖
  }
}
void KeyPortInit(void)
{
  SET_KEY_1_INT;
  SET_KEY_2_INT;
  SET_KEY_3_INT;
  SET_KEY_4_INT;
  KEY_IFG=0;
}
void KeyIntSet(void)
{
  SET_KEY_1_INT;
  SET_KEY_2_INT;
  SET_KEY_3_INT;
  SET_KEY_4_INT;
  KEY_IFG=0;
  
}
void KeyIntClr(void)
{
  CLR_KEY_1_INT;
  CLR_KEY_2_INT;
  CLR_KEY_3_INT;
  CLR_KEY_4_INT;
}

#if RF_U1_EN==1

void U1ToRf(void)
{
      Pt.U1toPC=0;//进入无线传感器接收
      U1_TO_RF;
      
      VDC33_ON;
      
      #if RF_REC_EN==1
      RF_IN_REC;
      #endif
      
      
      #if RF_PP_115200_EN==1
      RfU_Init(115200,1);
      #endif
}
//U1-RF-PC 切换
void U1_RF_PC_X(void)
{
   if(Pt.U1toPC==0)
   {
      Pt.U1toPC=1;//进入PC接收
      U1_TO_PC;
      LcdNN(U1TOPC);
      
      #if GPS_MOUSE_EN==1
      StateC.SeeBDSing=0;
      #endif
      
            #if RF_PP_115200_EN==1
              RfU_Init(9600,1);
            #endif
                  
      #if RF_YQ_EN==1
          //连PC模式，传感器会显示NULL
          Flag.RstFirstSample=1;
          for(u8 j=0;j<CH_NUM;j++)
          {
            StateC.Value[j]=SENIOR_NULL;
          }
      #endif
      
         
   }
   else
   {
      Pt.U1toPC=0;//进入无线传感器接收
      U1ToRf();
      LcdNN(HAND_START_SAMPLE);
      
      #if SIM68_EN==1
          U1Init(115200,1);
          delay_ms(200);
          U1SendString("PSIMIPR,W,9600\r\n");
          delay_ms(200);
          U1Init(9600,0);
      #endif
   }
}
#endif

void KeyView(void)
{
  #if MIN_MAX_AVG_ENABLE==1
    #if BSQ_ENABLE==1//变送器时，不显示03,显示F03
    LcdLn(LOOK_MAX_MIN);
    #else
    LcdNN(LOOK_MAX_MIN);
    #endif
    Pt.min_max_pt=7;//13;
    Pt.ChPt=0;//从通道开始
    
  #else 
      //记录仪，同时带GSM和RF, 第二键切换08
      #if RF_U1_EN==1
        U1_RF_PC_X();
      #endif
    
    
  #endif
  Key.KeyTimePt=0;
}
void KeyHand(void)
{
 
    #if BSQ_ENABLE==0//变送器则屏蔽该功能
   
    if(!((StateB.StartMode==START_MODE_HAND)||(StateB.StopMode==STOP_MODE_HAND)))
      return;
    
    if(StateA2.WorkState==WORK_STATE_RECORDING)//记录状态
    {
      if((StateB.StartMode==START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND))//工作模式为手动启动手动停止
      {
         if(Flag.HandOff==0)//记录状态
         {
            #if LCD08_SD_SHIFT_EN==1
                if(Flag.LCD08_Sd_en==1)
                {
                      if(StateB.LcdRefreshMode==LCD_IMMEDIATE)//关屏时，不能停机
                      {
                        Flag.HandOff=1;//手动停机
                        LcdOFFX(OFF_HAND);
                      }
                }
                else
                {
                      Flag.HandOff=1;//手动停机
                      LcdOFFX(OFF_HAND);        
                }
            #else
                    #if ((SD_ENABLE==1)&&(LCD_NO==8))
                    if(StateB.LcdRefreshMode==LCD_IMMEDIATE)//关屏时，不能停机
                    {
                      Flag.HandOff=1;//手动停机
                      LcdOFFX(OFF_HAND);
                    }
                    #else
                    Flag.HandOff=1;//手动停机
                    LcdOFFX(OFF_HAND);
                    #endif
            #endif

         }
         else
         {
            Flag.HandOff=0;//手动启动
            Flag.StartSample=START_SAMPLE_TIME;
            LcdNN(HAND_START_SAMPLE);//手动启动成功 提示
            
            #if RF_REC_EN==1
              RfDataNULL();//初始NULL
              Pt.SampPt=RF_SAMP_TIME-1;
            #endif
            
            #if ONLY_JS_ENABLE==1//单独计数
                SET_YL1_INT;
                StateC.Value[0]=Pt.RainPt=0;
                Flag.RstFirstSample=1;//单一雨量时，一直置1
                LcdValue();
            #endif
            
            #if POWER_TIME_ENABLE==1
            if(PowShutReady())
            {
              PowTimeClr();
            }
            #endif
         }
         SaveHandFlagToEE();//保存手动工作状态

      }
      else if((StateB.StartMode!=START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND))//非手动启动，是手动停机
      {
         LoggerStop();
         StateA2.StopCause=OFF_HAND;
         StateA2ToEE();

      }
    }
    else if((StateB.StartMode==START_MODE_HAND)&&(StateA2.StopCause==OFF_HAND))//停机状态
    {
       LcdNN(HAND_START_SAMPLE);//手动启动成功 提示
       Flag.HandJustStart=1;
       KeyDoStart();
       
       #if POWER_TIME_ENABLE==1
       if(PowShutReady())
       {
         PowTimeClr();
       }
       #endif
    }
    #endif
}
void HandStart(void)//手动启动记录
{
   StateA1.RecordNum=0;
   StateA1.CurrentBufPt=0;
   StateA1ToEE();
   StateA2.CurrentSavePage=0;
   StateA2.RecordOverNum=0;
   StateA2.WorkState=WORK_STATE_RECORDING;
   StateA2.StopCause=OFF_NOT;
   //Pt.RecordPt=0;
       
   RtcReadTime();
   StateA2.StartTime[0]=Rtc.Year;
   StateA2.StartTime[1]=Rtc.Month;
   StateA2.StartTime[2]=Rtc.Day;
   StateA2.StartTime[3]=Rtc.Hour;
   StateA2.StartTime[4]=Rtc.Minute;
   StateA2.StartTime[5]=Rtc.Second;
   RtcBcdToD10(&Rtc);
   StateA2ToEE();
              
   Flag.HandOff=0;
   SaveHandFlagToEE();//保存手动工作状态
   
   Flag.StartSample=START_SAMPLE_TIME;
   
   RestartClr();
}

//返回1，从关屏状态唤醒，返回0，不是唤醒操作
u8 WakeUp(void)
{
    #if LCD08_SD_SHIFT_EN==1
      if(Flag.LCD08_Sd_en==1)
      {
        if(Pt.sd_pt==0)
        {
           #if POWER_TIME_ENABLE==1
           PowTimeClr();
           #endif
           
           #if DIGITAL_FILTER_EN==1
           FilterClr();//从关屏唤醒时，滤波计数清0
           #endif
          
           Pt.min_max_pt=0;
           //KeyDoStart();//zz
           Pt.sd_pt=SD_TIME;//省电模式正常模式自动切换
           LcdShowOn(1);
           LcdLowMode(0);
           
           #if BSQ_ENABLE==1
           LcdLn(LCD_WAKE);
           #else
           LcdNN(LCD_WAKE);
           #endif
           
		   
	   Rtc.SS=ReadRtcD10();//更新RTC时间
           
    
           return 1;//从关屏模式退出时，不进行其他操作
        }
        Pt.sd_pt=SD_TIME;
      }
    #else
        #if SD_ENABLE==1//此处没有07或08 之分
        if(Pt.sd_pt==0)
        {
           #if POWER_TIME_ENABLE==1
           PowTimeClr();
           #endif
           
           #if DIGITAL_FILTER_EN==1
           FilterClr();//从关屏唤醒时，滤波计数清0
           #endif
          
           Pt.min_max_pt=0;
           //KeyDoStart();//zz
           Pt.sd_pt=SD_TIME;//省电模式正常模式自动切换
           LcdShowOn(1);
           LcdLowMode(0);
           
           #if BSQ_ENABLE==1
           LcdLn(LCD_WAKE);
           #else
           LcdNN(LCD_WAKE);
           #endif
           
		   
	   Rtc.SS=ReadRtcD10();//更新RTC时间
           
    
           return 1;//从关屏模式退出时，不进行其他操作
        }
        Pt.sd_pt=SD_TIME;
        #endif
    #endif

    return 0;
}

#if WIFI_ENABLE==1//开关机
void OFF6toOFF(u8 cc)
{
  if(cc==1)
  {
     StateA2.WorkState=WORK_STATE_STOP;
     POWER_CTL_ALL_OFF;
     LcdOFFX(OFF_BSQ_GPRS_HAND_STOP);//60FF
  }
  else
  {
      Pt.RtcPt=0;
      Pt.RecordPt=0;
      Pt.RealSamplePt=0;
      StateA2.WorkState=WORK_STATE_RECORDING;
      #if DIGITAL_FILTER_EN==1
      FilterClr();
      #endif
      LcdShowOn(1);//开屏
      LcdNN(StateB.Address);//显示地址
  }
}
void OFF6KeySet(void)
{
  u8 Buf[1];
  Buf[0]=Pt.Fg60FF;
  I2C_EE_BufferWrite(Buf,OFF6_KEY_ADDR,1,0);
}
void OFF6KeyRead(void)
{
  u8 Buf[1];
  I2C_EE_BufferRead(Buf,OFF6_KEY_ADDR,1,0);
  Pt.Fg60FF=Buf[0];
}
#endif
