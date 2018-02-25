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
  if(Key.Step)//�а�������
  {
      Flag.SeeComing=0;//�а�ť����������GSM���
      
      #if ((LCD_NO==20)&&(G_20_ENTER_EN==0))
        Pt.BGpt=BG_TIME;//����ʱ��  
        BG_LED_ON;
      #endif
      
      
      #if ((POWER_TIME_ENABLE==1)&&(LCD_NO==7))
      if(Pt.sd_pt==0)//ʡ��״̬
      {
          //if(Key.Step==KEY_STEP_4)//ֻ��Key4�����л���������ģʽ
          //{
            if(WakeUp())//�ոջ���
            {
              KeyDoStart();
              Key.Step=0;
              KeyIntSet();//���¿������а����ж�
              return;
            }
          //}
      }
      else//����ģʽ
      {
        if(Key.Step==KEY_STEP_4)
        {
           #if DIGITAL_FILTER_EN==1
           FilterClr();//�˲�������0
           #endif
        }
      }
      #else
      if(WakeUp())//�ոջ���
      {
        KeyDoStart();
        Key.Step=0;
        KeyIntSet();//���¿������а����ж�
        return;
      }
      #endif
      
      #if MIN_MAX_AVG_ENABLE==1
      if(KeyMinMaxDoing())//���ڲ�ѯ�����Сֵ
      {
        Key.Step=0;
        KeyIntSet();//���¿������а����ж�
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

      KeyIntSet();//���¿������а����ж�

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
         StateA2.WorkState=WORK_STATE_STOP;//�ػ�
         GetRfFzFromEE();//��EE�ж�ȡPt.Fz
         LcdFz(Pt.Fz); 
         
         
     }
     else
     {
        LcdNN(1);
        StateA2.WorkState=WORK_STATE_RECORDING;
     }
     
     StateA2ToEE();
     
  #endif
  
  
  #if ((RF_REC_EN==1)&&(RECORD_ENABLE==0)&&(RF_U1_EN==0)&&(RF_MULTI_EN==0))//������RF
  LcdU0U1_RF();
  #endif
  
  //Flag.SeeComing=1;//zz
  //U1SendStrPC("AAA");
  
  #if ((WIFI_ENABLE==1)||(RF_RP_EN==1)||(RF_MULTI_EN==1))
       Key.Ct=1;
       Key.DecT=DEC_TIME;
  
  #elif KEY_SET_ENABLE==1
      Key.KeyDog=KEY_DOG_TIME;
      if(Key.SetModePt>0)//����ģʽ -
      {
        //�л�������Key.Mt
        Key.Mt++;
        #if CH_NUM==1
        if(Key.Mt==MT_MIN_CHA+1)
          Key.Mt=MT_MAX_CHA;
        else if(Key.Mt==MT_MAX_CHA+1)
          Key.Mt=MT_YEAR;
        #endif
        
        Mt2Kt();
        if(Key.Mt==MT_END)//����
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

        KeyHand();//�ֶ������ֶ�ֹͣ

      }
  #else

      #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1))//ģ��ػ�
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
      KeyHand();//�ֶ������ֶ�ֹͣ
      #endif
  #endif 
    
    
#else//lcd08
    #if LED_WORK_ENABLE==0//��ָʾ��ģʽʱ�����в鿴
    
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
    #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1)&&(GPRS_6OFF==1))//ģ��
    if(Sms.GsmKeyShut==1)
      return;
    #endif
  
#if ((LCD_NO==7)||((LCD_NO==20)&&(G_20_ENTER_EN==0)))
  #if WIFI_ENABLE==1 
        Key.Ct=2;
        Key.DecT=DEC_TIME;
    
  #elif ((KEY_SET_ENABLE==1)&&(RF_RP_EN==0)&&(RF_REC_EN==0)&&(RF_TXRX_470M_EN==0))
  Key.KeyDog=KEY_DOG_TIME;
  if(Key.SetModePt>0)//����ģʽ +
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
    #if LED_WORK_ENABLE==1//�ֶ��������յ�ͣ��������ָʾ
    Pt.LedPt=2;
    #endif
  #endif
#endif
}
//MAX/MIN
void Key_3_Do(void)
{
//#if (RF_MULTI_EN==0)//������ջ����ü�����
  
    #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1)&&(GPRS_6OFF==1))//ģ��
    if(Sms.GsmKeyShut==1)
      return;
    #endif
  
  #if ((LCD_NO==7)||((LCD_NO==20)&&(G_20_ENTER_EN==0)))
    #if (KEY_SET_ENABLE==1)
    Key.KeyDog=KEY_DOG_TIME;
    if(Key.SetModePt>0)//����ģʽ -
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
          KeyView();//�鿴
       #endif
    #endif
  #endif
          
//#endif      
}
//��ӡ
void Key_4_Do(void)
{
  /*
    #if ((GSM_KEY_SHUT_EN==1)&&(GPRS_ENABLE==1))//ģ��
    if(Sms.GsmKeyShut==1)
      return;
    #endif
  */
  
#if ((LCD_NO==7)||((LCD_NO==20)&&(G_20_ENTER_EN==0)))
  
  //#if ((RF_REC_EN==1)&&(RECORD_ENABLE==1)&&(RF_U1_EN==0))//��¼��RF
  //LcdU0U1_RF();
  //return;
  //#endif
  
  #if RF_MULTI_EN==1//������ջ����ü�����
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
            // ��ӡ�Ͷ��ſ��ز���ͬʱʹ��
            #if PRINT_ALL_EN==1
              Bell_Off();//�ر�����ͷ���
              PrintAll();
            #endif
          
        #endif
              
        
        #if (KEY_SET_ENABLE==1)
        if(Key.SetModePt>0)//����ģʽ,ȷ��
        {
          //����
          Key.Kt++;
          
          if((Key.Kt==5)||(Key.Kt==13))//ͬʱ����ͨ��1����������Сֵ�����
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
                Key.Mt=MT_YEAR;//��ʼ����rtc
				
		Rtc.SS=ReadRtcD10();//�ȶ�ȡ��ǰrtc

				
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
          else if((Key.Kt==9)||(Key.Kt==17))//����ͨ��2���ݣ����
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
                Key.Mt=MT_YEAR;//��ʼ����rtc
				
		Rtc.SS=ReadRtcD10();//�ȶ�ȡ��ǰrtc

				
                RtcTemp=Rtc;
                LcdRtcShan(20,RtcTemp.Year,0);
              }
          }
          else if(Key.Kt==18)//������
          {
            Key.Mt=MT_MONTH;
          }
          else if(Key.Kt==19)//������
          {
            Key.Mt=MT_DAY;
          }
          else if(Key.Kt==20)//����ʱ
          {
            Key.Mt=MT_HOUR;
          }
          else if(Key.Kt==21)//���÷�
          {
            Key.Mt=MT_MINUTE;
          }
          else if(Key.Kt==22)//rtc���ý���
          {
           #if ((RF_MULTI_EN==1)||(RF_RP_EN==1))
           #else
            RtcD10ToBcd(&RtcTemp);
            Rtc=RtcTemp;
            RtcSetTime();//д��
			

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
  //�ػ�
  if(StateA2.WorkState==WORK_STATE_RECORDING)
  {
    StateA2.WorkState=WORK_STATE_STOP;
    LcdShowOn(0);
  }
  //����
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
#if ((RF_MULTI_EN==1)||(RF_RP_EN==1))//�����м���������ת����������
#else
          
  
      
      if(HaveALEnable())
      {
          ChAEnToFlash(99,0);//�رձ���
          
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
        
        
          ChAEnToFlash(99,1);//�򿪱���
          
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
      LoggerAlarmAllSeek();//������ѯ
      
      #if LCD_NO==7
      LcdValue();
      #endif
      
      #if ALARM_KEY_CONTINUE_EN==1
      Pt.AlarmKC=0;//����Ȱ�������
      #endif
      
      #if CMD_CTRL_EN==1
      Pt.wdkai=0;
      Pt.sdkai=0;
      #endif
      
  
#endif
}
u8 KeyMinMaxDoing(void)
{
  if(Pt.min_max_pt>0)//������ʾ�����Сƽ��ֵ��������
    return 1;
  else
    return 0;
}

//�ж��Ƿ�Ϊ����
u8 KeyXiaoDou(void)
{
    u8 KeyPut=0;
    
    WDT_CLR;
    TIME_KEY_STOP;//ʱ�䵽���رն�ʱ��
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
      return 1;//�ж�Ϊ��������
    }
    else
    {
      Key.Step=0;
      KeyIntSet();//���¿������а����ж�
      #if KEY_SET_ENABLE==1
      KeySetAddrQuit();
      #endif
      return 0;//�ж�Ϊ��������
    }
}

#if ((KEY_SET_ENABLE==1)||(PRINT_NEW_EN==1)||(CO2_GSS_TTL_EN==1)||((RF_REC_EN==1)||(RF_RP_EN==1))||(RF_TXRX_470M_EN==1)||(L_LCD_BAT_EN==1))
//0.25��ѭ��һ��
void Loop2Deel(void)
{
  if(Flag.KeyLcdLoop==1)
  {
    Flag.KeyLcdLoop=0;

    #if KEY_SET_ENABLE==1
    KeySetModeDeel();
    #endif
    KeyDecDeel();
    
    
     //�м̴��ת������
     #if ((RF_MULTI_EN==1)&&(RF_MULTI_MNJ==0))
    
     if(Pt.RpOff==0)//�ػ�ʱ��������������ת��
     {
      
         if(Pt.pcpt<65*4)
           Pt.pcpt++; 
         
    
         if(Pt.RecRpPt==RF_REC_RP_TIME*4-4)//zz 2s
         {
           if(StateB.Address>1)
           {
               RfReset();//����ģ�������λ
           }
         }
         
          
         if(++Pt.RecRpPt>RF_REC_RP_TIME*4)
         {
                 if(StateB.Address>1)//��ַ>1����ת������
                 {
                      //�������ݰ�
                      RecRpSend();
                      
                      //�����
                      u32 tt;
                      
                      tt=TAR;
                      tt+=TBR;
                      tt+=StateC.Adc[3];
                      tt+=StateC.Adc[4];
                      tt+=StateC.Adc[5];
                      tt+=StateC.Adc[6];
                      
                      tt=tt%40;
                      
                      Pt.RecRpPt=tt;//zz  �ӣ�0-10s�����¿�ʼ  ,����ʵ�ʷ��ͼ���� 50-60s
                      
                      MultiBufEE(1);//���浽EE
                 }
                 else
                 {
                     if((Pt.pcpt<=30*4)||(Pt.pcpt>=65*4))//30-65ʱ�ȴ�pcָ�������EE�������
                     {
                        Pt.RecRpPt=0;
                        MultiBufEE(1);//���浽EE
                        RfReset();//����ģ�������λ
                     }
                 }
    
         }
     
     }
      
     #endif
    
    
  }
}
#endif

#if KEY_SET_ENABLE==1
//1��ѭ��һ��
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
   GetAlarmExt(Key.AlarmExt);//ȡԭʼֵ,����ͨ��
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

//�����������в�����ɺ����־
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

//ȡ�����趨��������
void GetAlarmExt(u16* buf)
{
  u8 ChNo;
  for(ChNo=0;ChNo<CH_NUM;ChNo++)
  {
    buf[ChNo*2]=*((char *)CH1_A_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_L_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo)));
    buf[ChNo*2+1]=*((char *)CH1_A_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_H_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo)));
  }
}
// ȡ���ķ���ǧ��ʮ��λ��vbit
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
//��ť������ͬ״̬������
void KeySetModeDeel(void)
{
  switch (Key.SetModePt)
  {
    case 1://���밴ť���ò���
      KeySetStart();
      Key.SetModePt=2;
      break;
    case 2://��˸����
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
//A ͨ��cha  ��Kta/ B ͨ�� chb ��Ktb
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
//A ͨ��cha / B ͨ�� chb
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


//��ť����ʱ����ͬ״̬����ʾ
void LcdKeyMode(u8 Mt,u8 Kt)
{
  switch(Mt)
  {
    case MT_MIN_CHA://��ʾmin chA
      LcdChABmin(0,Kt,1,0);
      LcdMin();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
      break;
  case MT_MIN_CHB://��ʾmin chB
      LcdChABmin(0,0,1,Kt);
      LcdMin();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_MAX_CHA://��ʾmax chA
      LcdChABmax(0,Kt,1,0);
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
      break;
  case MT_MAX_CHB://��ʾmax chB
      LcdChABmax(0,0,1,Kt);
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
      break;
  case MT_YEAR:// ��ʾ��
      LcdChABmax(0,0,1,0);
      LcdRtcShan(20,RtcTemp.Year,Kt);
      LcdYearFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_MONTH://��ʾ��
      LcdChABmax(0,0,1,0);
      LcdRtcShan(RtcTemp.Month,RtcTemp.Day,Kt);
      LcdMonthFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_DAY://��ʾ��
      LcdChABmax(0,0,1,0);
      LcdRtcShan(RtcTemp.Month,RtcTemp.Day,Kt);
      LcdMonthFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_HOUR://��ʾʱ
      LcdChABmax(0,0,1,0);
      LcdRtcShan(RtcTemp.Hour,RtcTemp.Minute,Kt);
      LcdHourFlag();
      LcdMax();
      Lcd1Unit(0);
      Lcd2Unit(1);
      BuToLcdRam(0,17);
    break;
  case MT_MINUTE://��ʾ��
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
    
    if((Kt==1)||(Kt==5)||(Kt==9)||(Kt==13))//ǧλ��������
    {
          if(Dec==0)//+1
          {
            if(vbit[4]==0)
            {
              if(++vbit[3]>9)
              {
                vbit[4]=1;//�趨Ϊ����
                vbit[3]=0;
              }
            }
            else
            {
              vbit[4]=0;//�趨Ϊ����0
              vbit[3]=0;
            }
          }
          else//-1
          {
            if(vbit[4]==0)
            {
              if(--vbit[3]>9)
              {
                vbit[4]=1;//�趨Ϊ����
                vbit[3]=0;
              }
            }
            else
            {
              vbit[4]=0;//�趨Ϊ����9
              vbit[3]=9;
            }
          }
    }
    else if((Kt==2)||(Kt==6)||(Kt==10)||(Kt==14))//��λ
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
    else if((Kt==3)||(Kt==7)||(Kt==11)||(Kt==15))//ʮλ
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
    else if((Kt==4)||(Kt==8)||(Kt==12)||(Kt==16))// ��λ
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

//��ť+-����
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
  //��ʼ����rtc
  else if(Kt==17)//������
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
  else if(Kt==18)//������
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
  else if(Kt==19)//���� ��
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
  else if(Kt==20)//���� ʱ
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
  else if(Kt==21)//���� ��
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

//���õ�ַ
void KeySetAddr(void)
{
   I2C_EE_BufferWrite(&Key.AddrPt,ADDRESS_ADDR,1,0);//���õ�ַ
   I2C_EE_BufferRead(&StateB.Address,ADDRESS_ADDR,1,0);
   LcdNN(StateB.Address);
   Key.AddrPt=0;//�˳����õ�ַģʽ
}
//���õ�ַ���˳�
u8 KeySetAddrQuit(void)
{

  #if BSQ_ENABLE==1
  if(Key.AddrPt>0)
  {
    #if RF_KP_EN==1
       if(StateA2.WorkState==WORK_STATE_RECORDING)//����״̬�£����õ�ַ
       {
           KeySetAddr();
       }
       else//�ػ�״̬�£�����Ƶ��
       {
           VDC33_ON;
           delay_ms(50);
                             
           Pt.Fz=Key.AddrPt;
           LcdFz(Pt.Fz); 
                        
           SetRfFzToEE();
           GetRfFzFromEE();//��֤�Ƿ���ȷ
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
      if(Key.FlagX1==1)//��һ������
      {
        Key.FlagX1=0;
      }
      else
      {
        #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))//�м�����ַ���9
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
        if(Key.Ct==1)//key1��OFF
        {
            if((KEY_PIN&KEY_1_BIT)==0)//û���ͷ�
            {
                if(Key.DecT==0)
                {
                    if((KEY_PIN&KEY_2_BIT)==0)//zz, 2����ͬʱ��ס
                    {
                      Pt.ReloadPt=4;
                      P4OUT|=BIT2;//���ͣ��ȴ�4����Զ��ͷ�
                      LcdNN(WIFI_RELOAD);
                      Delay_100MS(4);
                      
                      Pt.WifiPt=0;
                    
                    }
                 }
            }
            else//�Ѿ��ͷ�
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
                      RfDataNULL();//��ʼNULL
                      Pt.SampPt=RF_SAMP_TIME-1;
                      #endif
    
                }
            }
            
            return;

        }
        
        if(Key.Ct==2)//key2�� SPK
        {
            if((KEY_PIN&KEY_2_BIT)==0)//û���ͷ�
            {
                if(Key.DecT==0)
                {
                    if((KEY_PIN&KEY_1_BIT)==0)//zz, 2����ͬʱ��ס
                    {
                      Pt.ReloadPt=4;
                      P4OUT|=BIT2;//���ͣ��ȴ�4����Զ��ͷ�
                      LcdNN(WIFI_RELOAD);
                      Delay_100MS(4);
                      
                      Pt.WifiPt=0;
                    
                    }
                 }
            }
            else//�Ѿ��ͷ�
            {
                if(Key.DecT>4)
                {
                  Key.DecT=0;
                  
                  KeyAlarm();//��������
    
                }
            }
            return;
        }
        
        
    #endif

   #if ((((PRINT_NEW_EN==1)||(RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))&&(CO2_GSS_TTL_EN==0))||(L_LCD_BAT_EN==1))
    
    #if GPRS_TC_EN==1
    if(Key.Ct==1)//key1��OFF
    {
        if((KEY_PIN&KEY_1_BIT)==0)//û���ͷ�
        {
            if(Key.DecT==0)
            {
                if(Sms.GsmKeyShut==0)//����ʱ���ػ�
                {
                    SmsKeyOff();
                    SmsKeySet();
                    
                    
                    Pt.PrintThisDueout=0;//�ػ�,���˳���ӡ zz
                    
                    #if ON_OFF_BJ_EN==1
                    //����ػ���Ǻ�ʱ��
                    Rtc.SB=ReadRtcD10();
                    PCBSQ_RecOne((u8)GJ_BJ);//��¼�ػ����
                    #endif
                    
                    #if RF_U1_EN==1
                    U1ToRf();//��ӡ��ɺ��л���RF
                    #endif


                }
            }
        }
        else//�Ѿ��ͷ�
        {
            if(Key.DecT>4)
            {
                Key.DecT=0;
                
                if(Sms.GsmKeyShut==1)//�ػ�ʱ������
                {
                    SmsKeyOn();
                    SmsKeySet();
                    
                    #if RTC_UTC_EN==1
                    UtcStart();
                    #endif
                    

                    #if ON_OFF_BJ_EN==1
                    //���濪����Ǻ�ʱ��
                    Rtc.SB=ReadRtcD10();
                    PCBSQ_RecOne((u8)KJ_BJ);//��¼�������
                      
                    PcBsq.PCwaitTT=8;//zz
                    #endif

                }
                
                #if G_20_ENTER_EN==1
                else//����ʱΪ�л���������
                {
                  
                    KeyAlarm();
                  
                }
                #endif

            }
        }
    }

    #endif
    

    
    #if ((RECORD_ENABLE==0)&&(GPRS_ENABLE==0)&&(WIFI_ENABLE==0))
    if(Key.Ct==1)//key1��OFF,,���õ�ַ��
    {
        if((KEY_PIN&KEY_1_BIT)==0)//û���ͷ�
        {
            if(Key.DecT==0)
            {
                  Key.FlagX1=1;
                  Key.AddrPt=1;
                  LcdNN(Key.AddrPt);//�ӵ�ַ1��ʼ��������ɺ󣬱�Ϊ0����ʾ�˳�����״̬
            }
        }
        else//�Ѿ��ͷ�
        {
            if(Key.DecT>4)
            {
                Key.DecT=0;
                if(KeySetAddrQuit())//���õ�ַ
                {
                    //���õ�ַ��ɺ�
                    #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
                    LcdRpSt();
                    #endif
                }
                else
                {
                    #if RF_MULTI_EN==0
                  
                      if(Pt.RpOff==0)
                      {
                          //�м����ر�
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
                          
                          //�����͸��ģʽ���ʲ��ܶϵ�
                          //VDC33_OFF;
                          //U0_TXD_CLR;//ʡ��
                        
                      }
                      else
                      {
                          //����
                          Pt.RpOff=0;
                          IE2 |= UCA0RXIE;
                          
                          #if RF_MULTI_EN==1
                            #if U0U1_ENABLE==1
                            Flag.U0U1ing=0;
                            #endif
                          LcdNN(1);
                          Pt.Spt=9;//������������
                          
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
    
    if(Key.Ct==2)//key2�� SPK
    {
        if((KEY_PIN&KEY_2_BIT)==0)//û���ͷ�
        {
            #if L_LCD_BAT_EN==1
            if(Key.DecT==0)
            {
               //�鿴����
              #if BAT_AD_ENABLE==1
              LcdNN(GetBatt());
              Lcd_Bfh();
              
              Delay_100MS(4);
              #endif
            }
          
            #elif WIFI_ENABLE==1
            if(Key.DecT==0)
            {
                if((KEY_PIN&KEY_1_BIT)==0)//zz, 2����ͬʱ��ס
                {
                      Pt.ReloadPt=4;
                      P4OUT|=BIT2;//���ͣ��ȴ�4����Զ��ͷ�
                      LcdNN(WIFI_RELOAD);
                      Delay_100MS(4);
                      
                      Pt.WifiPt=0;
                    
                }
            }
            #endif
        }
        else//�Ѿ��ͷ�
        {
            if(Key.DecT>4)
            {
              Key.DecT=0;
              
              KeyAlarm();//��������

            }
        }
    }
    if(Key.Ct==3)//key3
    {
        if((KEY_PIN&KEY_3_BIT)==0)//û���ͷ�
        {
          if(Key.DecT==0)
          {
            if((KEY_PIN&KEY_4_BIT)==0)//key4û���ͷ�
            {
              
                #if LCD_NO==7
                PrintKeyClr();
                #endif

              
            }
            else//key4û���£���key3����,����Ƶ��
            {   
                #if LCD_NO==20//20Ϊ�������
                PrintKeyClr();
                #endif
              
              
                #if ((RF_REC_EN==1)||(RF_RP_EN==1))
                Pt.KeyApt=3;//2����ٴμ�ⰴ������
                #endif
            }
          }
        }
        else//�Ѿ��ͷ�
        {
          if(Key.DecT>4)
          {
              #if ((RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))
              if(Pt.Xz>0)
              {
                  //����Ƶ��
                  Pt.Fz=Pt.Xz;
                  SetRfFzToEE();//Pt.Fz���浽EE
                  GetRfFzFromEE();//��EE�ж�ȡPt.Fz
                  
                  LcdFz(Pt.Fz);
                  RfSetF(Pt.Fz);//���õ�RF
                  delay_ms(100);
                  RF_IN_REC;
                  
                  Pt.Xz=0;//���ý���
                  
                  #if RF_REC_EN==1
                  RfDataNULL();//��ʼNULL
                  Pt.SampPt=RF_SAMP_TIME-1;
                  #endif
                  
                  #if (RF_RP_EN==1)
                  Pt.LinkN=0;
                  Pt.ScanPt=0;
                  Rf_NULL_Rp();
                  #endif
                  
                  //��ʾ���ٳ���1��
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
                  U1_RF_PC_X();//U1-RF-PC�л�
                #else
                  KeyView();//�鿴               
                #endif
              }

              Key.DecT=0;
          }
        }
    }
    
    
    if(Key.Ct==4)//key4
    {
        if((KEY_PIN&KEY_4_BIT)==0)//û���ͷ�
        {
          if(Key.DecT==0)
          {
            if((KEY_PIN&KEY_3_BIT)==0)//key3û���ͷ�
            {
              
                #if LCD_NO==7
                PrintKeyClr();
                #endif


            }
            else
            {
              //RFͬ�����
              #if ((RF_REC_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))
                  RfReset();//��λ 
                  
                  GetRfFzFromEE();//��֤�Ƿ���ȷ  zz
                  LcdFz(Pt.Fz);//��ʾ��ǰƵ��
                  
                  #if RF_RP_EN==0
                    delay_ms(500);
                    WDT_CLR;
                    delay_ms(500);
                    WDT_CLR;
  
                    SendSnToSenior();//Rf����ͬ��ָ��
                    delay_ms(200);
                    WDT_CLR;
                    delay_ms(300);
                    WDT_CLR;
                    SendSnToSenior();//Rf����ͬ��ָ��
                  #endif
                    
               #endif
            }
          }
        }
        else//�Ѿ��ͷ�
        {
          if(Key.DecT>4)
          {
              #if PRINT_NEW_EN==1
                  Bell_Off();//�ر�����ͷ���
                  
                  #if (((RS232_ENABLE==1)&&(RRINT_NEW_N_EN==1))||(L_REALOUT_PRINT_EN==1))//��ӡ
                  
                  
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
                          
                          Print_Begin();//��ʼ
                          
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
        if((KEY_PIN&KEY_3_BIT)==0)//û���ͷ�
        {
          if(Key.DecT==0)
          {
            if((KEY_PIN&KEY_4_BIT)==0)//key4û���ͷ�
            {
             
            }
          }
        }
        else//�Ѿ��ͷ�
        {
          if(Key.DecT>4)
          {
              #if RF_U1_EN==1
                U1_RF_PC_X();//U1-RF-PC�л�
              #else
                KeyView();//�鿴               
              #endif

              Key.DecT=0;
          }
        }
    }
    
    if(Key.Ct==4)//key4����һ����LOG
    {
        if((KEY_PIN&KEY_4_BIT)==0)//û���ͷ�
        {
          if(Key.DecT==0)
          {
            #if SEN_U1_EN==1
    
                 U1_TO_RF;//�����л���������
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
                    FilterChClr(j);//��Ӧͨ�������˲�������0
                  #endif
              
          }
        }
        else//�Ѿ��ͷ�
        {
          if(Key.DecT>4)
          {
            Key.DecT=0;
          }
        }
    }


    
   #elif KEY_SET_ENABLE==1
    if(Key.Ct==1)//key1,���ĸ�������Դ
    {
        if((KEY_PIN&KEY_1_BIT)==0)//һֱû���ͷ�
        {
          if(Key.DecT==0)
          {
              #if ((U0U1_ENABLE==1)&&(RECORD_ENABLE==0))
              U0U1set();
              #endif
              
              #if WIFI_ENABLE==1//wifi reload
              Pt.ReloadPt=4;
              P4OUT|=BIT2;//���ͣ��ȴ�4����Զ��ͷ�
              LcdNN(WIFI_RELOAD);
              #endif
          }
        }
        else
        {
           if(Key.DecT>4)
           {
               Key.DecT=0;
             
               #if WIFI_ENABLE==1//���ػ�
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
    
    if(Key.Ct==3)//key3,�ڶ�����MAX
    {
        if((KEY_PIN&KEY_3_BIT)==0)//û���ͷ�
        {
          if(Key.DecT==0)
          {
            #if BSQ_ENABLE==1
            Key.FlagX1=1;
            Key.AddrPt=1;
            LcdNN(Key.AddrPt);//�ӵ�ַ1��ʼ��������ɺ󣬱�Ϊ0����ʾ�˳�����״̬
            #endif
          }
        }
        else//�Ѿ��ͷ�
        {
          if(Key.DecT>4)
          {

                  if(KeySetAddrQuit())//���õ�ַ
                  {
                    //���õ�ַ��ɺ�
                    #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
                    LcdRpSt();
                    #endif
                  }
                  else
                  { 
                     KeyView();//�鿴
                     Key.DecT=0;
                  }

          }
        }
    }
    
    
    if(Key.Ct==4)//key4����һ����LOG
    {
        if((KEY_PIN&KEY_4_BIT)==0)//û���ͷ�
        {
          if(Key.DecT==0)
          {
              #if RF_KP_EN==1
                          
              #else
            
                  #if ((U0U1_ENABLE==1)&&(RECORD_ENABLE==1))
                  U0U1set();
                  #else
                    #if ((RF_MULTI_EN==1)||(RF_RP_EN==1))//�����м���������ת����������
                    #else
                    Key.SetModePt=1;//��������ģʽ 
                    #endif
                  #endif
                    
              #endif
          }
        }
        else//�Ѿ��ͷ�
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
                   RF_IN_REC;//����״̬
                   
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
    if((Key.KeyTimePt==0)&&(Flag.HandJustStart==1))//�ֶ�����ʱ����ʱ���������
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
u8 KeyDoing(void)//���ڽ��а������������ڼ����β���
{
  if(Key.KeyTimePt>0)
    return 1;
  else
    return 0;
}
//�����жϴ���
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
  if(Key.Step)//�м�����
  {
    KeyIntClr();//������ʱ�������а����ж�
    TIME_KEY_START;//������ʱ�����ȴ���ʱ������
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
      Pt.U1toPC=0;//�������ߴ���������
      U1_TO_RF;
      
      VDC33_ON;
      
      #if RF_REC_EN==1
      RF_IN_REC;
      #endif
      
      
      #if RF_PP_115200_EN==1
      RfU_Init(115200,1);
      #endif
}
//U1-RF-PC �л�
void U1_RF_PC_X(void)
{
   if(Pt.U1toPC==0)
   {
      Pt.U1toPC=1;//����PC����
      U1_TO_PC;
      LcdNN(U1TOPC);
      
      #if GPS_MOUSE_EN==1
      StateC.SeeBDSing=0;
      #endif
      
            #if RF_PP_115200_EN==1
              RfU_Init(9600,1);
            #endif
                  
      #if RF_YQ_EN==1
          //��PCģʽ������������ʾNULL
          Flag.RstFirstSample=1;
          for(u8 j=0;j<CH_NUM;j++)
          {
            StateC.Value[j]=SENIOR_NULL;
          }
      #endif
      
         
   }
   else
   {
      Pt.U1toPC=0;//�������ߴ���������
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
    #if BSQ_ENABLE==1//������ʱ������ʾ03,��ʾF03
    LcdLn(LOOK_MAX_MIN);
    #else
    LcdNN(LOOK_MAX_MIN);
    #endif
    Pt.min_max_pt=7;//13;
    Pt.ChPt=0;//��ͨ����ʼ
    
  #else 
      //��¼�ǣ�ͬʱ��GSM��RF, �ڶ����л�08
      #if RF_U1_EN==1
        U1_RF_PC_X();
      #endif
    
    
  #endif
  Key.KeyTimePt=0;
}
void KeyHand(void)
{
 
    #if BSQ_ENABLE==0//�����������θù���
   
    if(!((StateB.StartMode==START_MODE_HAND)||(StateB.StopMode==STOP_MODE_HAND)))
      return;
    
    if(StateA2.WorkState==WORK_STATE_RECORDING)//��¼״̬
    {
      if((StateB.StartMode==START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND))//����ģʽΪ�ֶ������ֶ�ֹͣ
      {
         if(Flag.HandOff==0)//��¼״̬
         {
            #if LCD08_SD_SHIFT_EN==1
                if(Flag.LCD08_Sd_en==1)
                {
                      if(StateB.LcdRefreshMode==LCD_IMMEDIATE)//����ʱ������ͣ��
                      {
                        Flag.HandOff=1;//�ֶ�ͣ��
                        LcdOFFX(OFF_HAND);
                      }
                }
                else
                {
                      Flag.HandOff=1;//�ֶ�ͣ��
                      LcdOFFX(OFF_HAND);        
                }
            #else
                    #if ((SD_ENABLE==1)&&(LCD_NO==8))
                    if(StateB.LcdRefreshMode==LCD_IMMEDIATE)//����ʱ������ͣ��
                    {
                      Flag.HandOff=1;//�ֶ�ͣ��
                      LcdOFFX(OFF_HAND);
                    }
                    #else
                    Flag.HandOff=1;//�ֶ�ͣ��
                    LcdOFFX(OFF_HAND);
                    #endif
            #endif

         }
         else
         {
            Flag.HandOff=0;//�ֶ�����
            Flag.StartSample=START_SAMPLE_TIME;
            LcdNN(HAND_START_SAMPLE);//�ֶ������ɹ� ��ʾ
            
            #if RF_REC_EN==1
              RfDataNULL();//��ʼNULL
              Pt.SampPt=RF_SAMP_TIME-1;
            #endif
            
            #if ONLY_JS_ENABLE==1//��������
                SET_YL1_INT;
                StateC.Value[0]=Pt.RainPt=0;
                Flag.RstFirstSample=1;//��һ����ʱ��һֱ��1
                LcdValue();
            #endif
            
            #if POWER_TIME_ENABLE==1
            if(PowShutReady())
            {
              PowTimeClr();
            }
            #endif
         }
         SaveHandFlagToEE();//�����ֶ�����״̬

      }
      else if((StateB.StartMode!=START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND))//���ֶ����������ֶ�ͣ��
      {
         LoggerStop();
         StateA2.StopCause=OFF_HAND;
         StateA2ToEE();

      }
    }
    else if((StateB.StartMode==START_MODE_HAND)&&(StateA2.StopCause==OFF_HAND))//ͣ��״̬
    {
       LcdNN(HAND_START_SAMPLE);//�ֶ������ɹ� ��ʾ
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
void HandStart(void)//�ֶ�������¼
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
   SaveHandFlagToEE();//�����ֶ�����״̬
   
   Flag.StartSample=START_SAMPLE_TIME;
   
   RestartClr();
}

//����1���ӹ���״̬���ѣ�����0�����ǻ��Ѳ���
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
           FilterClr();//�ӹ�������ʱ���˲�������0
           #endif
          
           Pt.min_max_pt=0;
           //KeyDoStart();//zz
           Pt.sd_pt=SD_TIME;//ʡ��ģʽ����ģʽ�Զ��л�
           LcdShowOn(1);
           LcdLowMode(0);
           
           #if BSQ_ENABLE==1
           LcdLn(LCD_WAKE);
           #else
           LcdNN(LCD_WAKE);
           #endif
           
		   
	   Rtc.SS=ReadRtcD10();//����RTCʱ��
           
    
           return 1;//�ӹ���ģʽ�˳�ʱ����������������
        }
        Pt.sd_pt=SD_TIME;
      }
    #else
        #if SD_ENABLE==1//�˴�û��07��08 ֮��
        if(Pt.sd_pt==0)
        {
           #if POWER_TIME_ENABLE==1
           PowTimeClr();
           #endif
           
           #if DIGITAL_FILTER_EN==1
           FilterClr();//�ӹ�������ʱ���˲�������0
           #endif
          
           Pt.min_max_pt=0;
           //KeyDoStart();//zz
           Pt.sd_pt=SD_TIME;//ʡ��ģʽ����ģʽ�Զ��л�
           LcdShowOn(1);
           LcdLowMode(0);
           
           #if BSQ_ENABLE==1
           LcdLn(LCD_WAKE);
           #else
           LcdNN(LCD_WAKE);
           #endif
           
		   
	   Rtc.SS=ReadRtcD10();//����RTCʱ��
           
    
           return 1;//�ӹ���ģʽ�˳�ʱ����������������
        }
        Pt.sd_pt=SD_TIME;
        #endif
    #endif

    return 0;
}

#if WIFI_ENABLE==1//���ػ�
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
      LcdShowOn(1);//����
      LcdNN(StateB.Address);//��ʾ��ַ
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
