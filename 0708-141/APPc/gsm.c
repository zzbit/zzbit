#include "_config.h"
//------------------------------------------------------
#if GSM_ENABLE==1

u8 *sServerCode="300.10";//ƽ̨���������ȷ���ظ���� Code:300.10
//SIS: 0,0,22,Remote host has rejected the 
//SIS: 0,0,13,"The network is unavailabl
u8 *sRejected="SIS: 0,0,";


u8 *sCENG="CENG: 1,1";//zz


u8 *sRDY="RDY";//SIM800C
u8 *sCMGS="CMGS";

u8 *sAT_READY="Interpreter ready";//Interpreter ready
u8 *sSTART="SYSSTART";
u8 *sOK="OK";
u8 *sCONNECT="CONNECT";

u8 *sLN="\n>";
u8 *s_0891="0891";
u8 *sIPSEND="%IPSEND";
u8 *sIPDATA="%IPDATA";
u8 *sSHUTDOWN="SHUTDOWN";
u8 *sERROR="ERROR:";
u8 *sREADY="READY";
u8 *sATCSQ="CSQ:";
u8 *sCMTI="+CMTI:";
u8 *sCMT="+CMT:";
u8 *sRING="RING";
u8 *sCPIN="CPIN";
u8 *sCLIP="CLIP";


u8 *sSW01X="SISW: 0,1,";
u8 *sSTIN="STIN:";

u8 *sSI="SI";
u8 *sSW="SW";
u8 *sTcpSendOk="SISW: 0,1\r\n";
u8 *sMOBILE="MOBILE";//�й��ƶ�  
u8 *sMobile="Mobile";//�й��ƶ�
u8 *sCMGR="+CMGR: 0,,0";

u8 *sSMOND="^SMOND:";//��վ��Ϣ

u8 *sUTC="UTC(NIST)";//UTC��ʱ
u8 *sNWTIME="NWTIME";//MG301,�����ʱ

u8 *sCONNOK="CONNECT OK";//GU906���ӳɹ�
u8 *sLOCI="LOCI";//AGPS����γ������

u8 *sCPIN_NOT="CPIN: NOT READY";//zz
u8 *sDEACT="DEACT";//zz
u8 *sALREADY="ALREADY";//zz
u8 *sFAIL="FAIL";//zz


//SMS ok.
const char ffen_sms_ok[14]={
0x00,0x53,0x00,0x4D,0x00,0x53,0x00,0x20,0x00,0x6F,
0x00,0x6B,0x00,0x2E};

//Power off
const char ffen_power_off[20]={
0x00,0x50,0x00,0x6F,0x00,0x77,0x00,0x65,0x00,0x72,
0x00,0x20,0x00,0x6F,0x00,0x66,0x00,0x66,0x00,0x2e};

//Power on.
const char ffen_power_on[18]={
0x00,0x50,0x00,0x6F,0x00,0x77,0x00,0x65,0x00,0x72,
0x00,0x20,0x00,0x6f,0x00,0x6e,0x00,0x2e};


//��Դ�ѹضϣ���ע�⣡
const char ff_power_off[20]={
0x75,0x35,0x6e,0x90,0x5d,0xf2,0x51,0x73,0x65,0xad,
0xff,0x0c,0x8b,0xf7,0x6c,0xe8,0x61,0x0f,0xff,0x01};


//��Դ��������
const char ff_power_on[12]={
0x75,0x35,0x6e,0x90,0x5d,0xf2,0x6b,0x63,0x5e,0x38,
0xff,0x01};


//����
const char ff_set[4]={0x8B,0xBE,0x7F,0x6E};
//�ɹ�
const char ff_ok[4]={0x62,0x10,0x52,0x9F};
//ʧ��
const char ff_fail[4]={0x59,0x31,0x8D,0x25};


void GsmLoop(void)
{
    #if U0U1_TONG_EN==1
    return;
    #endif
    
    #if GPRS_TC_EN==1
    if(PrintDeel()||(Download.Downloading==1))//�ڴ�ӡ��zz��������ʱ��GPRS�ϴ���ͣ,��������
      return;
    #endif
    
    #if RECORD_ENABLE==1
    if((StateA2.WorkState==WORK_STATE_STOP)\
      ||((StateB.StartMode==START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND)&&(Flag.HandOff==1)))
      return;
    
    #endif
    
    #if GSM_KEY_SHUT_EN==1
       #if GPRS_TC_EN==1
          if((Sms.GsmKeyShut==1)&&(Pt.ToOff==0))//��ť�ر�
            return;
       #else
          if(Sms.GsmKeyShut==1)//��ť�ر�
            return;
       #endif
    #endif
    
    #if SMS_POW_EN==1
    PowInDeel();//�ϵ���ż��
    #endif
        
    #if GPRS_ENABLE==1
    if(Pt.GprsSleep==1)
    {
      if(Sms.GsmOff==1)//ʡ�磬ģ��ر�״̬
        return;
    }
    
    #endif
    
    if(GsmFireDeel())//���
    {
      return;
    }
    
    
    #if GPRS_TC_EN==1
    if(Gprs_Exit_Close_Deel())//�˳�͸��ģʽ���ر�����
    {
      return;
    }
    
    if(Pt.ToOff==1)//�˳�͸���󣬲��ܹػ� zz
    {
      Pt.ToOff=0;
      GsmToPowOff();//�ػ�
    }
    
    #endif
    
    
    DogDeel();//���Ź�
    GsmInitDeel();//GSM��ʼ��
    
    SmsDeel();//���ŷ��ʹ���
    
    #if SMS_INV_ENABLE==1//��������ģʽ-----------------
      GsmInvDeel();
      #if SMS_BATONLY_ENABLE==1
        GsmInvShutDeel();//�Զ������ػ�
      #endif
    #endif
        
    SmsAlarmDeel();//���ű�������
    #if SMS_POW_EN==1
    SmsPowDeel();//�ϵ��������
    #endif
    
    
    #if GPRS_TC_EN==1//zz
    if(Flag.TCing==0)//��͸��״̬������δ����ʱ
    {
      CsqDeel();//��ѯGSM�ź�ǿ��
      GsmOnErrDeel();//���߼��,������δ������ʱ�����޷�����
    }
    #else
    CsqDeel();//��ѯGSM�ź�ǿ��
    GsmOnErrDeel();//���߼��
    #endif
    
    #if GPRS_ENABLE==1
    GprsDeel();
    #endif
    
    #if GSM_KEY_TEST_EN==1
    SmsTestDeel();//���ŷ��Ͳ���
    #endif
    
    #if GSM_BATT_ALARM_EN==1//��ص�ѹ����
    SmsBattDeel();
    #endif
    
    
    #if GPRS_SERVER_OK_EN==1
      if(Pt.GprsWaitSpt>0)
      {
         if(--Pt.GprsWaitSpt==0)//��ʱ��û�յ�300.10
         {
               Pt.GprsStart=0;
               Pt.GprsIngStep=GPRS_END;
               Pt.GprsLongDog=0;
               Pt.GprsStepDog=0;
               
               U1SendStrPC("NO RESPONSE\r\n");//û���յ�Ӧ��
               
               
              #if SIM800C_EN==1
                   U0SendString("AT+CIPCLOSE\r\n");//zz
              #else
                   U0SendString("AT^SISC=0\r\n");
              #endif
  
         }
      }
    #endif
      
      


}

//-------------------------------------------------------------------------------
//���߼��
void GsmOnErrDeel(void)
{
  if(Download.Downloading==0)
  {
    if(Sms.OnErrPt>0)//ģ������Ӧʱ������
    {
      Sms.OnErrPt++;
      if(Sms.OnErrPt>GSM_ON_ERR_TIME)
      {
          U1SendStrPC("\r\nDOWN");
         
          #if GPRS_ENABLE==1
          if(Pt.GprsSleep==1)
          {
              Pt.GprsRstErrPt++;
              if(Pt.GprsRstErrPt>1)//������������1
              {
                GsmToPowOff();//ʡ��ģʽ���ر�ģ��
              }
              else
              {
                ErrorToRst();
              }
          }
          else
          {
              ErrorToRst();
          }
          #else
          ErrorToRst();
          #endif
        
 
          Sms.OnErrPt=1;//��ʱ�жϵ���,��ģ����Ӧʱ��1
      }
    }
  }
}
//ģ��û��Ӧʱ����������ʱ
void GsmReTime(u8 cc)
{
  if(Sms.OnErrPt<GSM_ON_ERR_TIME-cc)
  {
    Sms.OnErrPt=GSM_ON_ERR_TIME-cc;//���ʧ�ܣ�������
  }
}
//�ϵ���
void PowInDeel(void)
{
  #if GPRS_POW_EN==1
    if(Flag.RstFirstSample==0)//ȷ���ϵ��ϵ���ͨ������
    return;
  #endif
  
  if(HavePow())
  {
    if(Flag.PowIn==0)//��һ״̬���޵�
    {
      Flag.PowIn=1;//�ϵ�
      
      U1SendStrPC("POW ON\r\n");//zz
      
      #if ((ON_OFF_BJ_EN==1)&&(NO_EXPOW_EN==0))
      Rtc.SB=ReadRtcD10();
      PCBSQ_RecOne((u8)SD_BJ);//��¼�ϵ���
      #endif
      
      
      if(Sms.OnErrPt>0)//ģ������Ӧ,��ģ�����
      {
          Pt.SmsDueOut=0;
          Pt.PowSmsFg=0;
          SmsPowStart();
          
          #if ((GPRS_ENABLE==1)&&(GPRS_POW_EN==1))
            GprsPowToRam(Flag.PowIn);
          #endif
          
          GsmReTime(GSM_D_TIME);
      }
    }
  }
  else
  {
    if(Flag.PowIn==1)//��һ״̬���е�
    {
      Flag.PowIn=0;//�ϵ�
      
      U1SendStrPC("POW OFF\r\n");//zz
      
      #if ((ON_OFF_BJ_EN==1)&&(NO_EXPOW_EN==0))
      Rtc.SB=ReadRtcD10();
      PCBSQ_RecOne((u8)DD_BJ);//��¼�ϵ���
      #endif
      
      if(Sms.OnErrPt>0)//ģ������Ӧ,��ģ�����
      {
          Pt.SmsDueOut=0;
          Pt.PowSmsFg=0;
          SmsPowStart();
          
          #if ((GPRS_ENABLE==1)&&(GPRS_POW_EN==1))
            GprsPowToRam(Flag.PowIn);
          #endif
          
          GsmReTime(GSM_D_TIME);
      }
    }
  }
}
//�ϵ���ű���
void SmsPowDeel(void)
{
  if(HaveALEnable()==0)//������ʱ��������
    return;
  
  u8 PhPt;
  if(Pt.SmsDueOut==0)//���ŷ��Ϳ���
  { 
      if(Sms.PowDueOut>0)//�б����跢��
      {
        if(GsmIsFree(0))//ģ�����
        {
            u8 PhNN;
            PhNN=GetPhoneNumFromEE();
          
            if((PhNN>0)&&(PhNN<4))//���������ȷ
            {
              if(PhNN>=Sms.PowDueOut)
              {
                PhPt=PhNN-Sms.PowDueOut;
              }
              else
              {
                PhPt=Sms.PowDueOut=PhNN;
              }
              if(SmsPowReady(PhPt))
              {
                Pt.SmsDueOut=1;//����SMS
                Pt.PowSmsFg=1;
              }
            }
        }
      }
  }
}



void SmsBattDeel(void)
{
  u8 PhPt;
  u16 xx;
  
  //��ѹ���
  if(Flag.RstFirstSample==1)
  {
    xx=GetBatt();
    if(xx<=20)//20 ����3��С��ʱ�Ŵ���AL
    {
      Pt.BattLPt++;
      if(Pt.BattLPt>3)
      {
        Pt.BattLPt=0;
        if(Sms.BattLowFg==0)//û������
        {
          if(SmsBattStart())
          {
            Sms.BattLowFg=1;
          }
        }
      }
    }
    else
    {
      Pt.BattLPt=0;
      if(xx>=25)//���� 25 ʱ��һֱ���־���  
      {
        Sms.BattLowFg=0;
      }
    }
  }
  
  if(Pt.SmsDueOut==0)//���ŷ��Ϳ���
  { 
      if(Sms.BattDueOut>0)//�б����跢��
      {
        if(GsmIsFree(0))//ģ�����
        {
            u8 PhNN;
            PhNN=GetPhoneNumFromEE();
          
            if((PhNN>0)&&(PhNN<4))//���������ȷ
            {
              if(PhNN>=Sms.BattDueOut)
              {
                PhPt=PhNN-Sms.BattDueOut;
              }
              else
              {
                PhPt=Sms.BattDueOut=PhNN;
              }
              if(SmsBattReady(PhPt))
              {
                Pt.SmsDueOut=1;//����SMS
              }
            }
        }
      }
  }
}
//TEST�����Զ��ŷ���
void SmsTestDeel(void)
{
  u8 PhPt;
  if(Pt.SmsDueOut==0)//���ŷ��Ϳ���
  { 
      if(Sms.TestDueOut>0)//�б����跢��
      {
        if(GsmIsFree(0))//ģ�����
        {
            u8 PhNN;
            PhNN=GetPhoneNumFromEE();
          
            if((PhNN>0)&&(PhNN<4))//���������ȷ
            {
              if(PhNN>=Sms.TestDueOut)
              {
                PhPt=PhNN-Sms.TestDueOut;
              }
              else
              {
                PhPt=Sms.TestDueOut=PhNN;
              }
              if(SmsTestReady(PhPt))
              {
                Pt.SmsDueOut=1;//����SMS
              }
            }
        }
      }
  }
}


//���Զ��ųɹ�����һ��
void SmsTestOneOk(void)
{
  if(Sms.TestDueOut>0)
  {
    Sms.TestDueOut--;
  }
}

//���Ų��Կ�ʼ
void SmsTestStart(void)
{
   u8 PhNN;
   PhNN=GetPhoneNumFromEE();
   if((PhNN>0)&&(PhNN<4))
   {
     Sms.TestDueOut=PhNN;
   }
}
//׼��Test���Ͳ��Զ��ţ�����+����+����
u8 SmsTestReady(u8 PhonePt)
{
  ClrBuf(Gsm.SmsRxPhone,15);
  GetPhoneHaoFromEE(Gsm.SmsRxPhone,PhonePt);
  
  #if GSM_TXT_EN==1
  Gsm.SmsUniLen=GetTestTxt(Gsm.SmsRecUni);
  #else
  Gsm.SmsUniLen=GetTestUni(Gsm.SmsRecUni);
  #endif
  if(PhoneIsRight())//�ֻ�������ȷ
    return 1;
  else
    return 0;
}
u16 GetTestTxt(u8* Txt)
{
  u16 i;
  i=0;
  Txt[i++]='S';
  Txt[i++]='m';
  Txt[i++]='s';
  Txt[i++]=' ';
  Txt[i++]='o';
  Txt[i++]='k';
  return i;
}


//Test��������Uni����
u16 GetTestUni(u8* UniBf)
{

  u16 j;

  for(j=0;j<14;j++)
    UniBf[j]=ffen_sms_ok[j];

  return j;

}


//��Դ������ϵ磬����֪ͨ
void SmsPowStart(void)
{
   #if SMS_POW_EN==1
  
   if(HaveALEnable()==0)//������ʱ��������
     return;
  
   u8 PhNN;
   PhNN=GetPhoneNumFromEE();
   if((PhNN>0)&&(PhNN<4))
   {
     
     #if GPRS_ENABLE==1
     Pt.GprsRstErrPt=0;
     #endif
     
     
     #if SMS_TCP_EN==1
       if(SmsPowReady(0))//�ֻ�������ȷ
       {
         Pt.smsErrPt=0;
         Pt.SmsPow=1;
         Pt.Gprsing=1;//������ʱ�������ϴ�

         U1SendStrPC("smsPOW START\r\n");
       }
     
     #else
     
       Sms.PowDueOut=PhNN;
     
     #endif
     
     
   }
   #endif
}
//�ϵ籨���ɹ�����һ��
void SmsPowOneOk(void)
{
  #if SMS_POW_EN==1
  
  if(HaveALEnable()==0)//������ʱ��������
    return;
  
  if(Sms.PowDueOut>0)
  {
    if(Pt.PowSmsFg==1)//���뾭��ȡ������һ��
    {
      Sms.PowDueOut--;
    }
  }
  #endif
}
//�ֻ������Ƿ���ȷ
u8 PhoneIsRight(void)
{
  if(((Gsm.SmsRxPhone[0]>='0')&&(Gsm.SmsRxPhone[0]<='9'))&&((Gsm.SmsRxPhone[2]>='0')&&(Gsm.SmsRxPhone[2]<='9')))
  {
    if((Gsm.SmsRxPhone[0]=='0')&&(Gsm.SmsRxPhone[1]=='0')&&(Gsm.SmsRxPhone[2]=='0'))
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 0;
  }
}
//׼�����͵�Դ���ţ�����+����+����
u8 SmsPowReady(u8 PhonePt)
{
  ClrBuf(Gsm.SmsRxPhone,15);
  GetPhoneHaoFromEE(Gsm.SmsRxPhone,PhonePt);
  
  #if GSM_TXT_EN==1
  Gsm.SmsUniLen=GetPowTxt(Gsm.SmsRecUni,Flag.PowIn);
  #else
      #if SMS_TCP_EN==1
            Pt.SmsPowLen=GetPowUni(Pt.SmsPowUni,Flag.PowIn);

      #else
             Gsm.SmsUniLen=GetPowUni(Gsm.SmsRecUni,Flag.PowIn);
      #endif
  
  

  #endif
  
  if(PhoneIsRight())//�ֻ�������ȷ
    return 1;
  else
    return 0;
}
u16 GetPowTxt(u8* Txt,u8 PowFg)
{
  u16 i=0;
  
  Txt[i++]='P';
  Txt[i++]='o';
  Txt[i++]='w';
  Txt[i++]='e';
  Txt[i++]='r';
  Txt[i++]=' ';
  if(PowFg==0)
  {
    Txt[i++]='o';
    Txt[i++]='f';
    Txt[i++]='f';
  }
  else
  {
    Txt[i++]='o';
    Txt[i++]='n';
  }
  return i;
}

u16 GetRtcUni(u8* UniBf)
{
    u16 i=0;
    
    ReadRtcD10();//����ǰRTC  170927 17:49
    
    UniBf[i++]=0x00;
    UniBf[i++]=0x0a;
    
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Year/10+0x30;
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Year%10+0x30;
    
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Month/10+0x30;
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Month%10+0x30;
   
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Day/10+0x30;
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Day%10+0x30;
    
    UniBf[i++]=0x00;
    UniBf[i++]=0x20;
    
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Hour/10+0x30;
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Hour%10+0x30;
    
    UniBf[i++]=0x00;
    UniBf[i++]=0x3a;
    
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Minute/10+0x30;
    UniBf[i++]=0x00;
    UniBf[i++]=Rtc.Minute%10+0x30;  
    
    UniBf[i++]=0x00;
    UniBf[i++]=0x0a;
    
    return i;
}
//��������Uni����
u16 GetPowUni(u8* UniBf,u8 PowFg)
{
  u16 i=0,j;
  
  #if SMS_RTC_EN==1
  i=i+GetRtcUni(&UniBf[i]);
  #endif
  
  i=i+GetSmsTitleUni(&UniBf[i]);

  if(PowFg==0)
  {
    for(j=0;j<20;j++)
      UniBf[i++]=ff_power_off[j];
  }
  else
  {
    for(j=0;j<12;j++)
      UniBf[i++]=ff_power_on[j];
  }

  return i;
}

//Batt��ѹ�������ųɹ�����һ��
void SmsBattOneOk(void)
{
  if(Sms.BattDueOut>0)
  {
    Sms.BattDueOut--;
  }
}

//Batt��ѹ�������ſ�ʼ
u8 SmsBattStart(void)
{
   u8 PhNN;
   PhNN=GetPhoneNumFromEE();
   if((PhNN>0)&&(PhNN<4))
   {
     Sms.BattDueOut=PhNN;
     return 1;
   }
   return 0;
}
//׼����ص�ѹ�������ţ�����+����+����
u8 SmsBattReady(u8 PhonePt)
{
  ClrBuf(Gsm.SmsRxPhone,15);
  GetPhoneHaoFromEE(Gsm.SmsRxPhone,PhonePt);
  
  #if GSM_TXT_EN==1 
  Gsm.SmsUniLen=GetBattTxt(Gsm.SmsRecUni,1);
  #else
  Gsm.SmsUniLen=GetBattUni(Gsm.SmsRecUni,1);
  #endif
  
  if(PhoneIsRight())//�ֻ�������ȷ
    return 1;
  else
    return 0;
}
//��ѹ������������txt����
//AL=0,�鿴���
//AL=1,��ر���
u16 GetBattTxt(u8* ss,u8 AL)
{
        u16 xx;
        u16 i;
        
        xx=GetBatt();
        
        i=0;
        //Battery  56%
        ss[i++]='B';
        ss[i++]='a';
        ss[i++]='t';
        ss[i++]='t';
        ss[i++]='e';
        ss[i++]='r';
        ss[i++]='y';
        ss[i++]=' ';
        
        if(xx>99)
        {
          ss[i++]=((xx/100)%10+0x30);
        }
        if(xx>9)
        {
          ss[i++]=((xx/10)%10+0x30);
        }
        ss[i++]=(xx%10+0x30);
        
        ss[i++]=(0x25);//%
        
        if(AL==1)//��ص�ѹ���� (AL)
        {
          ss[i++]='(';
          ss[i++]='A';
          ss[i++]='L';
          ss[i++]=')';
        }
        
        ss[i++]=(0x0a);
        
        return i;
}
//��ѹ������������Uni����
//AL=0,�鿴���
//AL=1,��ر���
u16 GetBattUni(u8* UniBf,u8 AL)
{
        u16 xx;
        u16 i;
        
        xx=GetBatt();
        
        i=0;
        //Battery  56%
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x42);
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x61);
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x74);
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x74);
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x65);
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x72);
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x79);
        
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x20);
        
        if(xx>99)
        {
          UniBf[i++]=(0x00);
          UniBf[i++]=((xx/100)%10+0x30);
        }
        if(xx>9)
        {
          UniBf[i++]=(0x00);
          UniBf[i++]=((xx/10)%10+0x30);
        }
        UniBf[i++]=(0x00);
        UniBf[i++]=(xx%10+0x30);
        
        UniBf[i++]=(0x00);
        UniBf[i++]=(0x25);//%
        
        if(AL==1)//��ص�ѹ���� (AL)
        {
                UniBf[i++]=(0x00);
                UniBf[i++]=(0x28);
                UniBf[i++]=(0x00);
                UniBf[i++]=(0x41);
                UniBf[i++]=(0x00);
                UniBf[i++]=(0x4C);
                UniBf[i++]=(0x00);
                UniBf[i++]=(0x29);
        }
        
        UniBf[i++]=(0x00);//����
        UniBf[i++]=(0x0a);
        
        return i;
}
//------------------------------------------------

//-------------------------------------------------------------------------
void U0Clear(void)
{
  TIME_U0_STOP;
  ClrBuf(Uart_0.RxBuf,Uart_0.RecLen);
  Uart_0.RecPt=0;
  Uart_0.RecLen=0;
}
void EM310_Rst(void)
{
  GSMRST_H;
  Delay_100MS(7);
  GSMRST_L;
}
void EM310_PWon(void)
{
  IGT_H;
  Delay_100MS(17);
  IGT_L;
  delay_ms(100);
  WDT_CLR;

}
u8 GsmFireDeel(void)
{

    if(Sms.GsmOff==1)//�ػ�ʱ������
      return 0;
    
    if(Pt.GsmPt>=1)//������ģʽ
    {
        Pt.GsmPt++;
        if(Pt.GsmPt==2)
        {
             DogClear();
             
             GSMRST_L;//��
             U1SendStrPC("\r\nFIRE");

             #if GSM_MG323_EN==1
             Pt.BcCMGR=0;
             #endif
        }
        else if(Pt.GsmPt==4)
        {
            IE2 |= UCA0RXIE;
            UART_U0_TXD_SET;
          
            #if GPRS_TC_EN==1
            U0SendString("+++");//�˳�͸��ģʽzz
            SF_Exit_Tc_Clr();
            #endif
    
        }
        else if(Pt.GsmPt==7)
        {
            //EM310_Rst();//zz
            GsmAT2Off();//�ػ�
        }
        else if(Pt.GsmPt==9)
        {
            #if TC_115200_EN==1
            U0Init(9600,0);
            #else
            U0Init(115200,1);
            #endif
            
            IGT_H;//���
        }
        else if(Pt.GsmPt==12)
        {
            IGT_L;
        }
        else if(Pt.GsmPt==15)
        {
            #if TC_115200_EN==1
                U0SendString("AT+IPR=115200\r\n");//���ò�����
            #else
                U0SendString("AT+IPR=9600\r\n");//���ò�����
            #endif
        }
        else if(Pt.GsmPt==16)
        {
            #if TC_115200_EN==1
                U0SendString("AT+IPR=115200\r\n");//���ò�����
                delay_ms(50);
                U0Init(115200,1);
            #else
                U0SendString("AT+IPR=9600\r\n");//���ò�����
                delay_ms(50);
                U0Init(9600,0);
            #endif
                

        }
        else if(Pt.GsmPt>=18)
        {

            Pt.IniIngStep=ATE;
            Pt.IniStartTime=STEP_TIME;
            Pt.IniLongDog=INI_LONG_DOG_TIME;
            Pt.SmsSendLongDog=0;
    
            Sms.OnErrPt=1;
            GsmReTime(GSM_D_TIME);//10������Ƿ����
            
            Pt.GsmPt=0;//�˳����ģʽ
            
            //�������ʧ�ܣ���λMCU,�ٴ�����
            if(++Pt.RstErrPt>30)//zz,ֻҪ�����ɹ�����0
            {
                //U1SendStrPC("GPRS TIME OUT\r\n");
                Pt.RstErrPt=0;
                DogReset();
            }
            
            //3�������󣬳�ʼ����ʧ�ܣ��źŸ���0,˵���п�����SIM��û��
            if(++Pt.IniErrPt>2)//zz
            {
              Pt.IniErrPt=0;
              LcdGsmXh(0);
              
              GSMRST_H;//��
              U1SendStrPC("GSM_OF\r\n");
            }
            
        }
    }
    
    
    if(Pt.GsmPt==0)
      return 0;
    else 
      return 1;
    
}
void ErrorToRst(void)
{
    if(Sms.GsmOff==1)//�ػ�ʱ������
      return;
         
    Pt.GsmPt=1;//���
}
//cc=0,������GPRS, cc=1 ����GPRS
u8 GsmIsFree(u8 cc)
{
  if(cc==0)
  {
    #if LBS_JWD_EN==1
    if(((Pt.IniIngStep==INI_END)||((Pt.IniIngStep>CREG)&&(Sms.AlarmDueOut>0)))\
      &&(Pt.SmsIngStep==SMS_END))
    #else
    if((Pt.IniIngStep==INI_END)&&(Pt.SmsIngStep==SMS_END))
    #endif
    {
      return 1;
    }
  }
  #if GPRS_ENABLE==1
  else if(cc==1)
  {
    #if GPRS_TC_EN==1
    if((Pt.IniIngStep==INI_END)&&(Pt.SmsIngStep==SMS_END)&&(Flag.TCing==0))//��͸��״̬�²��ܷ�����
    #else
    if((Pt.IniIngStep==INI_END)&&(Pt.SmsIngStep==SMS_END)&&(Pt.GprsIngStep==GPRS_END))
    #endif
      return 1;
  }
  #endif
  return 0;

}

//������ʼ���Ͷ���
void SmsAlarmStart(void)
{
   #if GSM_KEY_SHUT_EN==1
   if(Sms.GsmKeyShut==1)//�ػ�ʱ������
     return ;
   #endif
   
   #if ALARM_KEY_CONTINUE_EN==1
   if(Pt.AlarmKC==1)
     return;
   #endif

   if(Sms.IntervalPt==0)
   {
         u8 PhNN;
         PhNN=GetPhoneNumFromEE();
         if((PhNN>0)&&(PhNN<4))
         {
                     #if SMS_SAME_EN==1
           
                     for(u8 j=0;j<8;j++)
                     {
                       StateC.SmsValue[j]=StateC.Value[j];
                     }
                     StateC.SmsVBit=StateC.FuhaoBit;//��ֵ������
                     StateC.SmsChAFlag=Flag.AlarmB;//������־
              
                 
                     #endif
           
           

                     #if GPRS_ENABLE==1
                     Pt.GprsRstErrPt=0;
                     #endif
                     
                     

                     Sms.IntervalPt=SMS_INTERVAL_TIME;//�������
                     
                     
                     #if SMS_TCP_EN==1
                       if(SmsAlarmReady(0))//�ֻ�������ȷ
                       {

                         Pt.smsErrPt=0;
                         Pt.SmsAL=1;
                         Pt.Gprsing=1;//������ʱ�������ϴ�
                         U1SendStrPC("smsAL START\r\n");
                       }
                     
                     #else
                     
                       Sms.AlarmDueOut=PhNN;
                     
                     #endif
                     
                     
                     
                     
         }
   }
}
void SmsAlarmEnd(void)
{
  Sms.AlarmDueOut=0;
}
void CsqDeel(void)
{
    #if SMS_INV_ENABLE==1
    if(Sms.GsmOff)//�ػ�ʱ����
    {
     return;
    }
    #endif
    
    #if GPRS_ENABLE==1
    
    
    
        #if GPRS_TC_EN==1
    
            #if SIM800C_EN==0
               if(Pt.Linking==1)//MG301, �������ӷ�����ʱ����ֹCSQ
                 return;
                      
            #endif
    
    
        if(Flag.TC_WaitPt<5)//Flag.TC_WaitPt>5, ��������GPRS����״̬ʱ�����ж� GPRS��������
        #else
        #endif
          
        {
              u8 cc=0;
              #if RTC_UTC_EN==1
              
                  #if GPRS_POW_EN==1
                  if((Pt.GprsDueOut>0)||(Pt.GprsPowDueOut>0)||(Pt.UtcEn==1))
                    cc=1;
                  #else
                  if((Pt.GprsDueOut>0)||(Pt.UtcEn==1))
                    cc=1;
                  #endif
              
              #else
             
                  #if GPRS_POW_EN==1
                  if((Pt.GprsDueOut>0)||(Pt.GprsPowDueOut>0))
                    cc=1;
                  #else
                  if(Pt.GprsDueOut>0)
                    cc=1;
                  #endif
              
              #endif
                  
                  
              #if GPRS_UP_TIME_EN==1
                 if(Pt.Gprsing==0)//�����ϴ�ʱ�䣬����ִ��CSQָ������ʡ��ģʽ��������
                   cc=0;
              #endif
                 
              if(cc==1)
                return;
                  
        }
      
    #endif
    
    if(Pt.SmsDueOut)
      return;
    if(Sms.AlarmDueOut)
      return;
      
    if(Gsm.ReadCsqDog>0)//��CSQ�źſ��Ź�
    {
        #if GPRS_ENABLE==1
        if(GsmIsFree(1))
        #else
        if(GsmIsFree(0))  
        #endif
        {
          if(--Gsm.ReadCsqDog==0)
          {
             Gsm.ReadCsqDog=READ_CSQ_DOG_TIME;//���Ź���
             U0Clear();

             U0SendString("AT+CSQ\r\n");

          }
        }
    }
}

void SmsSendOk(void)
{
  LED_OFF;  
  Pt.SmsDueOut=0;
  Pt.SmsTErrCt=0;
  Pt.SmsIngStep=SMS_END;//SMS�������
  Pt.SmsSendLongDog=0;
  Pt.SmsSendStepDog=0;
  
  SmsAlarmOneOk();
  SmsPowOneOk();
  
  //SmsTestOneOk();//zz
  //SmsBattOneOk();
  
  #if SMS_INV_ENABLE==1
  GsmInvSendOk();
  #endif
  
  Gsm.ReadCsqDog=READ_CSQ_DOG_TIME;//��ʼ��ȡ�ź�
}
//Ӣ���ַ�Unicode����תΪAscii����
//�� 00 41 00 31 00 35-> 41 31 35
u16 EnUniToAsc(u8* buf,u16 ulen)
{
  u16 j;
  for(j=0;j<ulen/2;j++)
  {
    WDT_CLR;
    buf[j]=buf[j*2+1];
  }
  return ulen/2;
}
//�ж������ַ�
u8 IsNum(u8 cc)
{
  if((cc>='0')&&(cc<='9'))
    return 1;
  return 0;
}
//10��cc�η�
u16 cf10(u8 cc)
{
  u16 vv=1;
  while(cc--)
   vv*=10;
  return vv;  
}
//���ַ�����ȡ����
u16 GetNumFromStr(u8* str,u8* num,u8 start,u8 len)
{
  u8 cc,j;
  
  cc=0;
  for(j=0;j<len;j++)
  {
    WDT_CLR;
    if(IsNum(str[start+j]))
    {
      num[cc++]=str[start+j];
    }
  }
  return cc;
}
//���ַ����е�start�ֽڿ�ʼȡlen�ֽڵ���ֵ,תΪ1λС����z����ģʽ
//-20.3  100.5         
u16 GetZvalueFromStr(u8* str,u8 start,u8 len)
{
  u8 cc,j;
  u8 buf[5];
  u16 vv;
  
  cc=GetNumFromStr(str,buf,start,len);
  
  vv=0;
  for(j=0;j<cc;j++)
  {
    vv+=(buf[j]-0x30)*cf10(cc-1-j);
  }
  
  if(GetPosC(str,start,start+len,'.')==255)//û��С��
  {
    vv=vv*10;
  }
  
  if(GetPosC(str,start,start+len,'-')!=255)//�и���
  {
    vv=vv|0x8000;
  }
  return vv;
}

//�����ޣ�LHD��ʽ����  L-2H8D
u8 bbFromLHDstr(u8* str,u8 len,u16* bb)
{
          u8 xL,xH,xD;
         
          xL=GetPosC(str,0,len,'L');
          if(xL==255)
            xL=GetPosC(str,0,len,'l');
          if(xL==255)
            return 0;
          
          xH=GetPosC(str,0,len,'H');
          if(xH==255)
            xH=GetPosC(str,0,len,'h');
          if(xH==255)
            return 0;
          
          xD=GetPosC(str,0,len,'D');
          if(xD==255)
            xD=GetPosC(str,0,len,'d');
          if(xD==255)
            return 0;
          
          bb[0]=GetZvalueFromStr(str,xL,xH-xL);
          bb[1]=GetZvalueFromStr(str,xH,xD-xH);
          
          return 1;
}
//�ֻ����룬XYZD��ʽ����
//CP3X15988458051Y18705769627Z15988458053D
//CP1X15988458051YZD
//CP0XYZD
u8 bbFromXYZDstr(u8* str,u8 len,u8* bb)
{
          u8 xX,xY,xZ,xD;
          u8 buf[11];
          u8 i,j,cc;
         
          xX=GetPosC(str,0,len,'X');
          if(xX==255)
            xX=GetPosC(str,0,len,'x');
          if(xX==255)
            return 0;
          
          xY=GetPosC(str,0,len,'Y');
          if(xY==255)
            xY=GetPosC(str,0,len,'y');
          if(xY==255)
            return 0;
          
          xZ=GetPosC(str,0,len,'Z');
          if(xZ==255)
            xZ=GetPosC(str,0,len,'z');
          if(xZ==255)
            return 0;
          
          xD=GetPosC(str,0,len,'D');
          if(xD==255)
            xD=GetPosC(str,0,len,'d');
          if(xD==255)
            return 0;
          
          i=0;
          bb[i++]=str[xX-1]-0x30;//�������
          if(bb[0]>3)
            return 0;
          
          if(bb[0]==0)//����ȫ�����
          {
            for(j=0;j<33;j++)
              bb[i++]=0x00;
            return 1;
          }
          
          cc=GetNumFromStr(str,buf,xX,xY-xX);
          if(!(cc>0&&cc<12))
            return 0;
          for(j=0;j<cc;j++)
            bb[i++]=buf[j];
          if(cc<11)
          {
            for(j=0;j<11-cc;j++)//δ��11λ����0����
              bb[i++]=0x00;
          }
          if(bb[0]==1)//ֻ��X����
          {
            //YZ��0
            for(j=0;j<22;j++)//δ��22λ����0����
              bb[i++]=0x00;
            return 1;
          }
            
          
          cc=GetNumFromStr(str,buf,xY,xZ-xY);
          if(!(cc>0&&cc<12))
            return 0;
          for(j=0;j<cc;j++)
            bb[i++]=buf[j];
          if(cc<11)
          {
            for(j=0;j<11-cc;j++)//δ��11λ����0����
              bb[i++]=0x00;
          }
          if(bb[0]==2)//ֻ��XY����
          {
            //Z��0
            for(j=0;j<11;j++)//δ��11λ����0����
              bb[i++]=0x00;
            return 1;
          }
          
          cc=GetNumFromStr(str,buf,xZ,xD-xZ);
          if(!(cc>0&&cc<12))
            return 0;
          for(j=0;j<cc;j++)
            bb[i++]=buf[j];
          if(cc<11)
          {
            for(j=0;j<11-cc;j++)//δ��11λ����0����
              bb[i++]=0x00;
          }
          if(bb[0]==3)//��XYZ����
            return 1;

          return 1;
}

//ͬ��ʱ��
//CT170517133500D
u8 rtcFromCTDstr(u8* str,u8 len)
{
          u8 xT,xD;
          u8 buf[12];
          u8 cc;
         
          xT=GetPosC(str,0,len,'T');
          if(xT==255)
            xT=GetPosC(str,0,len,'t');
          if(xT==255)
            return 0;
          
          xD=GetPosC(str,0,len,'D');
          if(xD==255)
            xD=GetPosC(str,0,len,'d');
          if(xD==255)
            return 0;
          

          cc=GetNumFromStr(str,buf,xT,xD-xT);
          if(cc!=12)
            return 0;
          

          StrcutRtc  Utc;
                                          
          Utc.Year=(buf[0]-0x30)*10+ (buf[1]-0x30);
          Utc.Month=(buf[2]-0x30)*10+ (buf[3]-0x30);
          Utc.Day=(buf[4]-0x30)*10+ (buf[5]-0x30);
                                          
          Utc.Hour=(buf[6]-0x30)*10+ (buf[7]-0x30);
          Utc.Minute=(buf[8]-0x30)*10+ (buf[9]-0x30);
          Utc.Second=(buf[10]-0x30)*10+ (buf[11]-0x30);
                                          
          if(Rtc_Is_Right(&Utc))//ʱ���ʽ��ȷ
          {
               Utc.SS=DateToSeconds(&Utc);
               SecondsToDate(Utc.SS,&Rtc);
                                                
         
               //����ʱ��
               RtcD10ToBcd(&Rtc);//ת��BCD
               RtcSetTime();
               RtcBcdToD10(&Rtc);
                                                    
               U1SendStrPC("RTC OK");
               
               
               return 1;
                                  
          }
          else
          {
              return 0;
          }

}

//1,���óɹ��� 0 ����ʧ��
u16 GetUniSet(u8* uni,u8 cc)
{
  u16 j,len;
  len=0;
  for(j=0;j<4;j++)
  {
    uni[len++]=ff_set[j];
  }
  for(j=0;j<4;j++)
  {
    if(cc==1)
      uni[len++]=ff_ok[j];
    else 
      uni[len++]=ff_fail[j];
  }
  return len;
}
//������յ�SMS���ݣ�������ط���SMS����
u8 SmsRecDeel(void)
{
  // WSD WD SD ��ѯ��ǰ��ʪ��
  if(    (((Gsm.SmsRecUni[1]=='w')||(Gsm.SmsRecUni[1]=='W'))
       &&((Gsm.SmsRecUni[3]=='s')||(Gsm.SmsRecUni[3]=='S'))
       &&((Gsm.SmsRecUni[5]=='d')||(Gsm.SmsRecUni[5]=='D')))
    ||(((Gsm.SmsRecUni[1]=='s')||(Gsm.SmsRecUni[1]=='S'))
       &&((Gsm.SmsRecUni[3]=='d')||(Gsm.SmsRecUni[3]=='D')))
    ||(((Gsm.SmsRecUni[1]=='w')||(Gsm.SmsRecUni[1]=='W'))
       &&((Gsm.SmsRecUni[3]=='d')||(Gsm.SmsRecUni[3]=='D')))     )
  {
    Gsm.SmsUniLen=GetSmsUni(Gsm.SmsRecUni,SMS_LOOK);//���ŷ��� ��ѯ������
    Pt.SmsDueOut=1;//����SMS
    return 1;
  }
  // ���� �ַ���ͨ����Ϊ0ʱ���������ͨ������
  // C1L-2.3H126.7D   C2L2H8D    c2l2h8d  ���ñ���������, Aͨ��L����H����A
  // C1AK ������  C1AG ������
  //�ֻ����룬XYZD��ʽ����
  //CP3X15988458051Y18705769627Z15988458053D
  //CP1X15988458051YZD
  //CP0XYZD
  #if SMS_SET_EN==1
  else if(Gsm.SmsRecUni[1]=='c'||Gsm.SmsRecUni[1]=='C')
  {
      
      u16 bb16[2];
      u8 bb8[46];
      u8 ch;
      u8 setFlag=0;
      u16 j;
    
      Gsm.SmsUniLen=EnUniToAsc(Gsm.SmsRecUni,Gsm.SmsUniLen);//UniתΪASCII���� 
      if(Gsm.SmsRecUni[0]=='c'||Gsm.SmsRecUni[0]=='C')
      {
        if(IsNum(Gsm.SmsRecUni[1]))//����ͨ������-------------------------------
        {
          //ԭʼ����ͨ�� 
          ch=Gsm.SmsRecUni[1]-0X30;
          if(ch)
            ch=ch-1;
          else//0ʱΪ����ͨ��
            ch=99;
            
          //ͨ�����޺���������  C1L-20H45.6D---------------------------------------
          if(Gsm.SmsRecUni[2]=='l'||Gsm.SmsRecUni[2]=='L')
          {
              if(bbFromLHDstr(Gsm.SmsRecUni,Gsm.SmsUniLen,bb16))
              {
                  bb8[0]=0xff&(bb16[0]);
                  bb8[1]=0xff&(bb16[0]>>8);
                  bb8[2]=0xff&(bb16[1]);
                  bb8[3]=0xff&(bb16[1]>>8);
                  
                  if(ch==99)//����ͨ��
                  {
                    for(j=0;j<CH_NUM;j++)
                    {
                      bb8[4]=0;
                      if(ChALEnable(j))
                        bb8[4]=1;
                      WDT_CLR;
                      alarm_buf_to_flash(bb8,j,1);
                      delay_us(1000);
                    }
                  }
                  else//����ͨ��
                  {
                    bb8[4]=0;
                    if(ChALEnable(ch))
                      bb8[4]=1;
                    alarm_buf_to_flash(bb8,ch,1);
                  }
                  setFlag=1;//���óɹ�
              }
          }
        
          //ͨ��������������  C1AK  C1AG-----------------------------------------------
          else if(Gsm.SmsRecUni[2]=='a'||Gsm.SmsRecUni[2]=='A')
          {
            if(Gsm.SmsRecUni[3]=='k'||Gsm.SmsRecUni[3]=='K')//��
            {
              ChAEnToFlash(ch,1);
              setFlag=1;//���óɹ�
            }
            else if(Gsm.SmsRecUni[3]=='g'||Gsm.SmsRecUni[3]=='G')//��
            {
              ChAEnToFlash(ch,0);
              setFlag=1;//���óɹ�
            }
          }
        }
        //�����ֻ�����-------------------------------------------------------------
        else if(Gsm.SmsRecUni[1]=='p'||Gsm.SmsRecUni[1]=='P')
        {
          if(bbFromXYZDstr(Gsm.SmsRecUni,Gsm.SmsUniLen,bb8))
          {
            SetPhoneToEE(bb8);
            setFlag=1;//���óɹ�
          }
        }
        
        //����ʱ��
        else if(Gsm.SmsRecUni[1]=='t'||Gsm.SmsRecUni[1]=='T')
        {
          if(rtcFromCTDstr(Gsm.SmsRecUni,Gsm.SmsUniLen))
          {
            setFlag=1;//���óɹ�
          }
        }
      }
    
      //���ض���setFlag,���óɹ�����ʧ��
      Gsm.SmsUniLen=GetUniSet(Gsm.SmsRecUni,setFlag);
      Pt.SmsDueOut=1;//����SMS
      return 1;
  }
  #endif

  #if (RY_GSM_ENABLE==1)
  // KD ����
  else if(((Gsm.SmsRecUni[1]=='k')||(Gsm.SmsRecUni[1]=='K'))
          &&((Gsm.SmsRecUni[3]=='d')||(Gsm.SmsRecUni[3]=='D')))
  {
    Flag.Ry1ed=1;
    return 1;
  }
  // GD �ص�
  else if(((Gsm.SmsRecUni[1]=='g')||(Gsm.SmsRecUni[1]=='G'))
          &&((Gsm.SmsRecUni[3]=='d')||(Gsm.SmsRecUni[3]=='D')))
  {
    Flag.Ry1ed=0;
    return 1;
  }
  #endif
  
  return 0;
}

//׼�����Ͷ��ţ�����+����+����..������ȷ�ſ�ʼ����
u8 SmsAlarmReady(u8 PhonePt)
{
  ClrBuf(Gsm.SmsRxPhone,15);
  GetPhoneHaoFromEE(Gsm.SmsRxPhone,PhonePt);
  
  #if GSM_TXT_EN==1
  Gsm.SmsUniLen=GetSmsTxt(Gsm.SmsRecUni,SMS_ALARM);
  #else
  Gsm.SmsUniLen=GetSmsUni(Gsm.SmsRecUni,SMS_ALARM);
  #endif
    
  if(PhoneIsRight())//�ֻ�������ȷ
    return 1;
  else
    return 0;
}
//���ű�������
void SmsAlarmDeel(void)
{
  u8 PhPt;
  if(Pt.SmsDueOut==0)//���ŷ��Ϳ���
  {
      if(GsmIsFree(0))//ģ�����
      {
        if(Sms.AlarmDueOut>0)//�б����跢��
        {
          u8 PhNN;
          PhNN=GetPhoneNumFromEE();
          if(PhNN>=Sms.AlarmDueOut)
          {
            PhPt=PhNN-Sms.AlarmDueOut;
          }
          else
          {
            PhPt=Sms.AlarmDueOut=PhNN;
          }
          if(SmsAlarmReady(PhPt))
          {
            Pt.SmsDueOut=1;//����SMS
          }
        }
      }
  }
}
void SmsAlarmOneOk(void)
{
  if(Sms.AlarmDueOut>0)
  {
    Sms.AlarmDueOut--;
  }
}
void SmsDeel(void)
{
  //�ж����޶���.........................................................................
  if(Pt.SmsDueOut>0)
  {
       #if GPRS_TC_EN==1
       if(Flag.TCing>0)//����͸��ʱ�����ܽ���
         return;
       #endif
    
       if(GsmIsFree(0))
       {
         Pt.SmsTErrCt=0;
         Pt.SmsIngStep=SMSLEN;
         Pt.SmsSendLongDog=SMS_SEND_DOG_TIME;
         Pt.SmsTime=2;//��ʼ���Ͷ���
       }
    
      //SMS���͹���.......................................................................
      if(Pt.SmsTime>0)
      {
        if(--Pt.SmsTime==0)
        {
           Pt.SmsBcStep=Pt.SmsIngStep;
           #if LBS_JWD_EN==1
           if(Pt.IniIngStep>CREG)
           #else
           if(Pt.IniIngStep==INI_END)//Gsm��ʼ���������
           #endif
           {
             if(Pt.SmsIngStep==SMSLEN)
             {
               Pt.SmsSendLongDog=SMS_SEND_DOG_TIME;
               
               #if GSM_TXT_EN==1
               SmsSendPhone(Gsm.SmsRxPhone);
               #else
               U0SmsLen(Gsm.SmsUniLen+15);
               #endif
             }
           }
        }
      }
    
  }
}


#if GPRS_POW_EN==1
//�ϵ��ϵ�ʱȡGPRS����
void GprsPowToRam(u8 PowIn)
{
     u16 j,i=0;
     
     RtcReadTime();
     
     Pt.GprsPowRam[i++]=Rtc.Year;
     Pt.GprsPowRam[i++]=Rtc.Month;
     Pt.GprsPowRam[i++]=Rtc.Day;
     Pt.GprsPowRam[i++]=Rtc.Hour;
     Pt.GprsPowRam[i++]=Rtc.Minute;
     Pt.GprsPowRam[i++]=Rtc.Second;
     
     RtcBcdToD10(&Rtc);
     
     Pt.GprsPowRam[i++]=GetBatt();//����
     
     //��Դ(1)
     if(PowIn==1)//�ϵ紥��
     {
        Pt.GprsPowRam[i++]=0x02;
     }
     else if(PowIn==0)//�ϵ紥��
     {
        Pt.GprsPowRam[i++]=0x03;
     }
            
     for(j=0;j<9;j++)//Ԥ��
       Pt.GprsPowRam[i++]=0x00;
            
     for(j=0;j<CH_NUM;j++)
     {
        if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))//3.4
        {
           Pt.GprsPowRam[i++]=0xff&(StateC.Value[j]>>8);//HSB8
           Pt.GprsPowRam[i++]=0xff&StateC.Value[j];//LSB8
        }
        else
        {
          //HSB8
          if(0x01&(StateC.FuhaoBit>>j))
          {
             Pt.GprsPowRam[i++]=0x80|(0xff&(StateC.Value[j]>>8));
          }
          else
          {
             Pt.GprsPowRam[i++]=0x7f&(StateC.Value[j]>>8);
          }
          //LSB8
          Pt.GprsPowRam[i++]=0xff&StateC.Value[j];
                
        }
     }
     
     //���ϵ��ϵ紥�� Ҫ����
     Pt.GprsPowDueOut=1;
     
     Pt.GprsRstErrPt=0;
}

//�ϵ���ϵ紥����1����ʱ����
void GprsPowTcpSend(void)
{
        u8 add=0;
        u16 j;
        
        //���͵�U0--------------------------------------------------------------------
        //ͷ--------------------------------------------------------------------
        add+=U0SendByte(0x31);
        
        //�ֽ���------------------------------------------------------------------
        add+=U0SendByte(0xff&(((u16)1*(17+2*CH_NUM)+3*CH_NUM+23)>>8));//�ֽ���H
        add+=U0SendByte(0xff&((u16)1*(17+2*CH_NUM)+3*CH_NUM+23));//�ֽ���L
        
        //SN
        add+=i2c_ee_read_to_u0(SERIAL_NO_ADDR,10,0);
        
        
        //��λ��־�� GPSģ��
        #if ((GPS_MOUSE_EN==1))
         add+=U0SendByte(0x01);        
        #else
         add+=U0SendByte(0x00);           
        #endif
         
         
        //�̼��汾
        add+=U0SendByte(C_VER);
         
        //Ԥ��
        for(j=0;j<8;j++)
          add+=U0SendByte(0x00);
        

        //ͨ����
        add+=U0SendByte(CH_NUM);
        
        //ͨ������
        for(j=0;j<CH_NUM;j++)
        {
          add+=U0SendByte(*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES));
          add+=U0SendByte(*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES));
          add+=U0SendByte(*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES));
        }
        
        //��������
        add+=U0SendByte(0xff&(1>>8));//�ֽ���H
        add+=U0SendByte(0xff&(1));//�ֽ���L

        //�ϵ��ϵ紥����1�����ݣ��ֽ���(17+2n) 
        add+=U0SendBuf(Pt.GprsPowRam,17+2*CH_NUM);

        //����β---------------------------------------------------------------
        add+=U0SendByte(0x32);
        
        //У���---------------------------------------------------------------
        U0SendByte(add);
        

}
#endif

#if GPRS_ENABLE==1




//ȡ���µ�mm������  �ֽ���x=m(17+2n)+3n+23
void GprsGetRealData(u16 mm)
{
      if(mm==0)
        return;

        u8 add=0;
        u16 j,ss;
        
        u8 bb;
        bb=17+2*CH_NUM;//ÿ���ֽ���
      
        //���͵�U0--------------------------------------------------------------------
        //ͷ--------------------------------------------------------------------------
        add+=U0SendByte(0x31);
        
        //�ֽ���-----------------------------------------------------------------------

        ss=mm*bb+3*CH_NUM+23;

        add+=U0SendByte(0xff&(ss>>8));//�ֽ���H
        add+=U0SendByte(0xff&(ss));//�ֽ���L
        
        //SN
        add+=i2c_ee_read_to_u0(SERIAL_NO_ADDR,10,0);
        
        //��λ��־
        #if ((GPS_MOUSE_EN==1))
         add+=U0SendByte(0x01);
        #else
         add+=U0SendByte(0x00);
        #endif
         
        //�̼��汾
         add+=U0SendByte(C_VER);
         
        //Ԥ��
        for(j=0;j<8;j++)
          add+=U0SendByte(0x00);

        //ͨ����
        add+=U0SendByte(CH_NUM);
        
        //ͨ������
        for(j=0;j<CH_NUM;j++)
        {
          add+=U0SendByte(*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES));
          add+=U0SendByte(*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES));
          add+=U0SendByte(*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES));
        }
        
        //��������
        add+=U0SendByte(0xff&(mm>>8));//�ֽ���H
        add+=U0SendByte(0xff&(mm));//�ֽ���L
        
        
        //mm���������ֽ���
        ss=mm*bb;

        //���µ�mm������----------------------------------------------------
        if(Pt.GprsThisRec16Num<PcBsq.RecCap)//��¼δ��
        {
          add+=i2c_ee_read_to_u0((Pt.GprsThisRecPt-mm)*bb,ss,0);
        }
        else//��¼����
        {
          
          if(Pt.GprsThisRecPt==0)
          {
            add+=i2c_ee_read_to_u0((Pt.GprsThisRec16Num-mm)*bb,ss,0);
          }
          else if(Pt.GprsThisRecPt>=mm)
          {

            add+=i2c_ee_read_to_u0((Pt.GprsThisRecPt-mm)*bb,ss,0);
          }
          else if(Pt.GprsThisRecPt<mm)
          {
            add+=i2c_ee_read_to_u0((Pt.GprsThisRec16Num+Pt.GprsThisRecPt-mm)*bb,(mm-Pt.GprsThisRecPt)*bb,0);
            add+=i2c_ee_read_to_u0(0,Pt.GprsThisRecPt*bb,0);
          }

        }

        NOP;
        
        //����β---------------------------------------------------------------
        add+=U0SendByte(0x32);
        
        //У���---------------------------------------------------------------
        U0SendByte(add);
        
        
        #if GPRS_FFEEDD_EN==1
        U0SendByte(0xFF);
        U0SendByte(0xEE);
        U0SendByte(0xDD);
        
        #endif
        

        U0SendByte(0x1A);//zz

}

void Gprs_PrtCAP_FromEE(void)
{
   u8 buf[2];
   I2C_EE_BufferRead(buf,GPRS_PRT_CAP_ADDR,2,0);
   Pt.PrtCap=U8_TO_U16(buf[1],buf[0]);//��ӡ����
   
   
   //��¼���<=1����ʱ����Ϊ�ǹ̶�5���Ӵ�ӡһ���㣬��˴�ӡ����Ҫ����
   #if GPRS_PRT_JG_EN==1
   if(PcBsq.RecTime<=60)
   {
     u32 aa;
     aa=Pt.PrtCap;
     aa=aa*(PRINT_SF_TIME/PcBsq.RecTime);
     if(aa>PcBsq.RecCap)//����ȷ�� bsq_init �Ѿ�ִ�й�
       aa=PcBsq.RecCap;
     Pt.PrtCap=aa;
   }
   #endif
   
   
   if(Pt.PrtCap>PcBsq.RecCap)//����ȷ�� bsq_init �Ѿ�ִ�й�
     Pt.PrtCap=PcBsq.RecCap;
   
   if(Pt.PrintDueout>Pt.PrtCap)//��ӡ�����ݲ��ܳ�����ӡ����
     Pt.PrintDueout=Pt.PrtCap;

}


void Gprs_UpTime_FromEE(void)
{
   u8 buf[1];
   I2C_EE_BufferRead(buf,GPRS_UPTIME_ADDR,1,0);
   Pt.GprsSendTime=(u16)60*buf[0];//�ϴ���� , EE�е�λ�Ƿ���
}

void GprsSleepFromEE(void)
{
   u8 buf[1];
   I2C_EE_BufferRead(buf,GPRS_SLEEP_ADDR,1,0);
   Pt.GprsSleep=buf[0];
   if(Pt.GprsSleep!=1)
     Pt.GprsSleep=0;
   
   #if ((POWER_TIME_ENABLE==1)&&(GPRS_ENABLE==1))
   LcdMode();
   #endif
}
void GprsSleepToEE(void)
{
  u8 buf[1];
  buf[0]=Pt.GprsSleep;
  I2C_EE_BufferWrite(buf,GPRS_SLEEP_ADDR,1,0);
}

#if GPRS_TC_EN==1
//��ӡ��PC���ؼ�¼
void PrintDueOutFromEE(void)
{
   u8 buf[2];
   I2C_EE_BufferRead(buf,PRINT_DUEOUT_ADDR,2,0);
   Pt.PrintDueout=U8_TO_U16(buf[1],buf[0]);
}
void PrintDueOutToEE(void)
{
  u8 buf[2];
  buf[0]=0xff&Pt.PrintDueout;
  buf[1]=0xff&(Pt.PrintDueout>>8);
  I2C_EE_BufferWrite(buf,PRINT_DUEOUT_ADDR,2,0);
}

#endif

void SendIpPortFromEE(void)
{
                  u16 i,j;
                  u8 buf[30];
                  u8 ips[4][3];
                  u8 iplen[4];
                  u16 port;
                  u8 ports[5];
                  u8 portlen;
                  u8 domain_en;
                  
                  //ȡIP��ַ�Ͷ˿�
                  I2C_EE_BufferRead(buf,GPRS_IP_ADDR,6,0);
                  
                  //ip
                  for(j=0;j<4;j++)
                  {
                    iplen[j]=u16ToS5(buf[j],ips[j]);
                  }
                  
                  //port
                  port=U8_TO_U16(buf[5],buf[4]);
                  portlen=u16ToS5(port,ports);
                  
                  //��������
                  I2C_EE_BufferRead(buf,GPRS_DOMAIN_EN_ADDR,1,0);
                  domain_en=buf[0];
                  
                  //����
                  I2C_EE_BufferRead(buf,GPRS_DOMAIN_ADDR,30,0);
    
                  //���͵�PC���U1
                  if(1==Flag.SeeComing)
                  {
                       ComSendH();
                       if(domain_en==1)
                       {
                          U1SendString(buf);//����
                       }
                       else
                       {
                          for(i=0;i<4;i++)//IP
                          {
                              for(u16 j=0;j<iplen[i];j++)
                                U1SendByte(ips[i][j]);
                              if(i<3)
                                U1SendByte('.');
                          }
                       }
                       U1SendByte(':');
                       //PORT
                       for(j=0;j<portlen;j++)
                         U1SendByte(ports[j]);
                       
                       U1SendByte(0x0d);
                       U1SendByte(0x0a);
                      
                       ComSendL();
                  }
                  
                  //��ģ�鷢��ip��ַ�Ͷ˿�
                  #if SIM800C_EN==1
                  
                     #if M26_EN==1

                          U1SendStrPC("QIOPEN\r\n");
                          U0SendString("AT+QIOPEN=\"TCP\",\"");                  

                     #else
                  
                          #if GPRS_UDP_EN==1
                          //U1SendStrPC("UDP\r\n");
                          U0SendString("AT+CIPSTART=\"UDP\",\"");
                          #else
                          //U1SendStrPC("TCP\r\n");
                          U0SendString("AT+CIPSTART=\"TCP\",\"");                  
                          #endif
                      
                      #endif
                      
                      if(domain_en==1)
                      {
                        U0SendString(buf);//����
                      }
                      else
                      {
                          for(i=0;i<4;i++)//ip
                          {
                            for(u16 j=0;j<iplen[i];j++)
                            {
                              U0SendByte(ips[i][j]);
                            }
                            if(i<3)
                            {
                              U0SendByte('.');
                            }
                          }
                      }
                      U0SendString("\",");
                      //port
                      for(j=0;j<portlen;j++)
                      {
                         U0SendByte(ports[j]);
                      }
                      U0SendString("\r\n");
                  #else
                      
                      #if GPRS_UDP_EN==1
                      U0SendString("AT^SISS=0,\"address\",\"sockudp://");
                      #else
                      U0SendString("AT^SISS=0,\"address\",\"socktcp://");                      
                      #endif
                      
                      if(domain_en==1)
                      {
                        U0SendString(buf);//����
                      }
                      else
                      {
                          for(i=0;i<4;i++)//ip
                          {
                            for(u16 j=0;j<iplen[i];j++)
                            {
                              U0SendByte(ips[i][j]);
                            }
                            if(i<3)
                            {
                              U0SendByte('.');
                            }
                          }
                      }
                      U0SendByte(':');
                      //port
                      for(j=0;j<portlen;j++)
                      {
                         U0SendByte(ports[j]);
                      }
                      U0SendString("\"\r\n");                
                  #endif
                  

}







#endif


void GsmInitDeel(void)
{
    if(Pt.IniStartTime>0)
    {
      if(--Pt.IniStartTime==0)
      {
        GsmReTime(GSM_D_TIME);
        Pt.IniStepDog=2;
        Pt.IniBcStep=Pt.IniIngStep;
        U0Clear();
        switch(Pt.IniIngStep)
        {
          case ATE:
            U1SendStrPC("ATE\r\n");
            U0SendString("ATE0\r\n");
            break;
            
          case GMR:
            U1SendStrPC("GMR\r\n");
            U0SendString("AT+GMR\r\n");
            break;
            
          #if GSM_EM310_EN==0
          case CFUN:
            U1SendStrPC("AT\r\n");
            //U0SendString("AT+CFUN=1\r\n");
            U0SendString("AT\r\n");
            break;
          #endif
            

          case CPIN:
            U1SendStrPC("CPIN\r\n");
            U0SendString("AT+CPIN?\r\n");
            break;
            
          #if GPRS_SMS_CNMI_EN==1
          case CNI_INT:
            U1SendStrPC("CNI_INT\r\n");
            U0SendString("AT+CNMI=3,2,0,1,0\r\n");
            break;
          #endif
            
          case CMGF:
            
            #if GSM_TXT_EN==1
            U1SendStrPC("CMGF_TEXT\r\n");
            U0SendString("AT+CMGF=1\r\n");
            #else
            U1SendStrPC("CMGF_PDU\r\n");
            U0SendString("AT+CMGF=0\r\n");
            #endif
            break;
            
            
          #if SMS_TCP_EN==1   
          case CIMI:
            U1SendStrPC("IMSI\r\n");
            U0SendString("AT+CIMI\r\n");
            break;
          case CCID:
            U1SendStrPC("ICCID\r\n");
            U0SendString("AT+CCID\r\n");
            break;
          case IMEI:
            U1SendStrPC("IMEI\r\n");
            U0SendString("AT+CGSN\r\n");
            break;
          #endif
            
            
          #if GPRS_AUTO_LIS_EN==1  
          case LSING:
            U1SendStrPC("ATS0=1\r\n");
            U0SendString("ATS0=1\r\n");
            break;
          #endif
            
            
          case T_CSQ:
            U0SendString("AT+CSQ\r\n");
            break;
            
          #if LBS_LAC_EN==1  
          case CENG:
            U1SendStrPC("CENG\r\n");
            U0SendString("AT+CENG=1,1\r\n");//zz
            break;
          case LBS0:
            U1SendStrPC("LBS0\r\n");
            U0SendString("AT+CENG?\r\n");
            break; 
            
            
          #elif LBS_JWD_EN==1
            
          case L_CGREG:  
            U1SendStrPC("CGREG\r\n");
            U0SendString("AT+CGREG?\r\n");
            break;

            
#if M26_EN==0
          case L_CGATT:
            U1SendStrPC("CGATT?\r\n");
            U0SendString("AT+CGATT?\r\n");
            break;
          case L_CIPSHUT:
            
            #if M26_EN==1
                U1SendStrPC("QIDEACT\r\n");
                U0SendString("AT+QIDEACT\r\n");
            #else
                U1SendStrPC("SHUT\r\n");
                U0SendString("AT+CIPSHUT\r\n");
            #endif
                
            break;
          case L_CIPMODE:
            
            U1SendStrPC("MODE0\r\n");
            #if M26_EN==1
              U0SendString("AT+QIMODE=0\r\n");
            #else
              U0SendString("AT+CIPMODE=0\r\n");
            #endif
            

            break;
          case L_COPS:
            U1SendStrPC("COPS\r\n");
            U0SendString("AT+COPS?\r\n");

            break;
          case L_CSTT:

            if(Pt.ChinaMobile==1)
            {
              U1SendStrPC("CMNET\r\n");
              
              
              #if M26_EN==1
                 U0SendString("AT+QICSGP=1,\"CMNET\"\r\n"); 
              #else
                 U0SendString("AT+CSTT=\"CMNET\"\r\n");              
              #endif
              
              
            }
            else
            {
              U1SendStrPC("UNINET\r\n");
              
              #if M26_EN==1
                 U0SendString("AT+QICSGP=1,\"UNINET\"\r\n"); 
              #else
                 U0SendString("AT+CSTT=\"UNINET\"\r\n");              
              #endif
              

            }

            break;
          case L_CIICR:
            
            #if M26_EN==1
              U1SendStrPC("QIACT AT\r\n");
              //U0SendString("AT+QIACT\r\n");
              U0SendString("AT\r\n");
            #else
              U1SendStrPC("CIICR\r\n");
              U0SendString("AT+CIICR\r\n");       
            #endif
            
            Pt.GtposPt=0;
            
            break;
#endif
          case L_GTPOS:
            
            #if M26_EN==1
              U1SendStrPC("LLOC\r\n");
              U0SendString("AT+QCELLLOC=1\r\n");  
            #else
              U1SendStrPC("GTPOS\r\n");
              U0SendString("AT+GTPOS\r\n");    
            #endif
              
            Pt.IniStepDog=10;//zz r���ʧ�ܣ�����ط�  
              
              
            if(++Pt.GtposPt>3)//��������޷���ȡ��γ�Ⱥ����� 10-4   2-25
            {
              Pt.IniIngStep=L_GTPOS+1;
              Pt.IniStartTime=5;
              Pt.GtposPt=0;
            }
            
            break;
            

            
          #endif  
            

          case CREG:
            U0SendString("AT+CREG?\r\n");
            break;
          case CSQ:
            U0SendString("AT+CSQ\r\n");
            break;
            
          #if GPRS_ENABLE==1   
          case TC_CGREG:
            U1SendStrPC("CGREG?\r\n");
            U0SendString("AT+CGREG?\r\n");
            break;
          #endif

            
      #if SIM800C_EN==1      
         #if GPRS_TC_EN==1  //zz 
            
          #if LBS_JWD_EN==0  
          case TC_CGATT:
            U1SendStrPC("CGATT?\r\n");
            U0SendString("AT+CGATT?\r\n");
            break;
          #endif
            
            
          case TC_CIPSHUT:
            
              #if M26_EN==1
                   U1SendStrPC("QIDEACT\r\n");
                   U0SendString("AT+QIDEACT\r\n");
              #else
                   //U1SendStrPC("SHUT\r\n");
                   U0SendString("AT+CIPSHUT\r\n");
              #endif
            break;
            
          #if M26_EN==1  
          case TC_QIDNSIP:
              NOP;
              static u8 cc=0;
              I2C_EE_BufferRead(&cc,GPRS_DOMAIN_EN_ADDR,1,0);
              if(cc==1)//����ģʽ
              {
                   U1SendStrPC("DNS\r\n");
                   U0SendString("AT+QIDNSIP=1\r\n"); 
              }
              else//IPģʽ
              {
                   U1SendStrPC("IP\r\n");
                   U0SendString("AT+QIDNSIP=0\r\n"); 
              }
            
            break;
          #endif
            
            
          case TC_CIPMUX:
            
            #if M26_EN==1
               U1SendStrPC("MUX0\r\n");
               U0SendString("AT+QIMUX=0\r\n");//������
            #else
               U0SendString("AT+CIPMUX=0\r\n");//������         
            #endif
            
            
            break;
            

          case TC_CIPMODE:
            
            #if GPRS_TC_EN==1
              U1SendStrPC("MODE1\r\n");
            
              #if M26_EN==1
               U0SendString("AT+QIMODE=1\r\n");
              #else
               U0SendString("AT+CIPMODE=1\r\n");//͸��ģʽ        
              #endif
               
            
            #else
            U0SendString("AT+CIPMODE=0\r\n");//��͸��ģʽ            
            #endif
            
            break;
          case TC_CIPCCFG:
            #if GPRS_TC_EN==1
            //U1SendStrPC("CIPCCFG\r\n");
            
              #if TC_115200_EN==1
                  //11500bit/s->11520B/1000ms -> ÿ�ֽ���ʱ 0.09ms, 1400�ֽ��� 126ms
                  U0SendString("AT+CIPCCFG=3,3,1400,1\r\n");
              #else
                  //9600bit/s-> 960B/1000ms -> ÿ�ֽ���ʱ 1.042ms, 1400�ֽ��� 1458ms
                  //SIM800C �ȴ�ʱ���������10����1s,��TCP������ֽ���������̫��
                  
                  #if M26_EN==1
                      U1SendStrPC("QITCFG\r\n");
                      U0SendString("AT+QITCFG=3,10,1400,1\r\n");
                  #else
                      U0SendString("AT+CIPCCFG=3,10,1400,1\r\n");//͸��ģʽ  �ش�����3���ȴ�����ʱ�� 5*0.1s,���ݻ��壬����+++�˳�ģʽ
                  #endif
              #endif
                  
            #else
                  
              U0SendString("AT\r\n");
            #endif
            break;
          case TC_CIFSR:
            
            #if M26_EN==1
              U1SendStrPC("QILOCIP\r\n");
              U0SendString("AT+QILOCIP\r\n");
            #else
              U1SendStrPC("CIFSR\r\n");
              U0SendString("AT+CIFSR\r\n");
            #endif
            break;
            

          #if ((LBS_JWD_EN==0)||((LBS_JWD_EN==1)&&(M26_EN==1)))
          case TC_COPS:
            U1SendStrPC("COPS\r\n");
            U0SendString("AT+COPS?\r\n");
            break;
          case TC_CSTT:
            
            
            if(Pt.ChinaMobile==1)
            {
              U1SendStrPC("CMNET\r\n");
              
              #if M26_EN==1
                 U0SendString("AT+QIREGAPP=\"CMNET\"\r\n");//�ƶ�CMNET
              #else
                 U0SendString("AT+CSTT=\"CMNET\"\r\n");//�ƶ�CMNET           
              #endif
              
            }
            else
            {
              U1SendStrPC("UNINET\r\n");
              
              #if M26_EN==1
                U0SendString("AT+QIREGAPP=\"UNINET\"\r\n");//�ƶ�CMNET
              #else
              
                U0SendString("AT+CSTT=\"UNINET\"\r\n");//��ͨUNINET
              #endif
            }
            break;
            
          #endif
          case TC_CIICR:
            
            #if LBS_JWD_EN==1

  
              #if M26_EN==1
                U1SendStrPC("QIACT\r\n");
                U0SendString("AT+QIACT\r\n");  
              #else
                U1SendStrPC("AT\r\n");
                U0SendString("AT\r\n");          
              #endif

            #else
              
              #if M26_EN==1
                U1SendStrPC("QIACT\r\n");
                U0SendString("AT+QIACT\r\n");  
              #else
                U1SendStrPC("CIICR\r\n");
                U0SendString("AT+CIICR\r\n");          
              #endif
              
         
            #endif
              
            Pt.IniStepDog=4;
            break;

          #endif

            
      #else
            
               #if GPRS_TC_EN==1


                case M_CGATT:
                  U1SendStrPC("CGATT?\r\n");
                  U0SendString("AT+CGATT?\r\n");//���ø���GPRS
                  break;

                case COPS:
                  U0SendString("AT+COPS?\r\n");
                  break;
                  
                case IOMODE:
                  U0SendString("AT^IOMODE=0,1\r\n");
                  break;
                  
                case SICS1:
                  U1SendStrPC("GPRS0\r\n");
                  U0SendString("AT^SICS=0,\"conType\",\"GPRS0\"\r\n");
                  break;
                case SICS2:
                  if(Pt.ChinaMobile)
                  {
                    U1SendStrPC("CMNET\r\n");//�ƶ�CMNET,CMWAP
                    U0SendString("AT^SICS=0,\"apn\",\"CMNET\"\r\n");
                  }
                  else
                  {
                    U1SendStrPC("UNINET\r\n");//��ͨUNINET,UNIWAP  
                    U0SendString("AT^SICS=0,\"apn\",\"UNINET\"\r\n");
                  }
                  break;
                case SISS1:
                  U1SendStrPC("Socket\r\n");
                  U0SendString("AT^SISS=0,\"srvType\",\"Socket\"\r\n");
                  break;
                case SISS2:
                  U1SendStrPC("conId\r\n");
                  U0SendString("AT^SISS=0,\"conId\",0\r\n");
                  break;
                  
                
                case SISSIP:
                      SendIpPortFromEE();
                  break; 
                  
                  
                case SISO:
                  U1SendStrPC("OPEN\r\n");//������
                  U0SendString("AT^SISO=0\r\n");
                  Pt.IniStepDog=10;
                  break;

                  
                case IPCFL1:
                   U1SendStrPC("IPCFL1\r\n");//���ó�ʱʱ��1s
                   U0SendString("AT^IPCFL=5,10\r\n");
                   break;
                case IPCFL2:
                   U1SendStrPC("IPCFL2\r\n");//��������ֽ���
                   U0SendString("AT^IPCFL=10,1400\r\n");
                   break;


            
             #endif

       #endif
            
          case ATW:
            U1SendStrPC("ATW\r\n");//zz
            U0SendString("AT&W0\r\n");
            break;
          #if GSM_TXT_EN==0

            #if GSM_MG323_EN==1
          case CMGR:
            U0SendString("AT+CMGR=10\r\n");//����10������λ�ã��������������ж���
            break;
          case CMGD:
            if(Pt.BcCMGR==1)//�ж��Ŵ洢ʱ����ն���
            {
              Pt.BcCMGR=0;
              U0SendString("AT+CMGD=1,4\r\n");
            }
            else
            {
              U0SendString("AT+CSQ\r\n");
            }
            break;
            #endif
            
          #endif
            
          #if GPRS_ENABLE==0  
          case CNMI:
            U1SendStrPC("CNMI\r\n");
            #if GSM_EM310_EN==1
            U0SendString("AT+CNMI=1,2,0,0,0\r\n");
            #else
            U0SendString("AT+CNMI=3,2,0,1,0\r\n");//301 ֻ������ 
           #endif
            break;
          case CLIP:
            U1SendStrPC("CLIP\r\n");
            U0SendString("AT+CLIP=1\r\n");//������Ϣ��ʾ���������
            break;
         #endif
              
          default:break;
        }
      }
    }
}

#if GPRS_ENABLE==1
void GprsDeel(void)
{
   
#if GPRS_TC_EN==1
   Gprs_SF_Deel();
#else
   
  if(GprsDataHav())//������Ҫ����
  {
    if(GsmIsFree(1))
    {
      Pt.GprsTErrCt=0;
      
      Pt.GprsIngStep=GPRS_BEGIN;
      Pt.GprsLongDog=GPRS_DOG_TIME;
      Pt.GprsStart=1;

      //�ȶϿ����ӣ��ٿ�ʼ����
      #if SIM800C_EN==1
           U0SendString("AT+CIPCLOSE\r\n");//zz
      #else
           U0SendString("AT^SISC=0\r\n");
           Pt.FgSISW=0;
      #endif

    }
  }
  
  if(GsmIsFree(0)&&(Pt.GprsStart>0))
  {
    if(--Pt.GprsStart==0)
    {
      if(Pt.IniIngStep==INI_END)//Gsm��ʼ���������
      {
          u16 i=0;
          u8 bb,cc;
        
          Pt.GprsStepDog=5;
          Pt.GprsBcStep=Pt.GprsIngStep;
          
          switch(Pt.GprsIngStep)
          {
          #if SIM800C_EN==1
            
                case CIP1SHUT:
                  
                    #if M26_EN==1
                          U1SendStrPC("QIDEACT\r\n");//ȡ������
                          U0SendString("AT+QIDEACT\r\n");          
                    #else
                          //U1SendStrPC("SHUT\r\n");//ȡ������
                          U0SendString("AT+CIPSHUT\r\n");
                    #endif
    
                  break;  
                  
                case CIPSTART:
                  Pt.GprsStepDog=15;
                  
                  #if RTC_UTC_EN==1
                      if(Pt.UtcEn==1)
                      {
                        U1SendStrPC("UT\r\n");
                        U0SendString("AT+CIPSTART=\"TCP\",\"time.nist.gov\",14\r\n");
                      }
                      else
                      {
                        SendIpPortFromEE();
                      }
                  #else
                      SendIpPortFromEE();
                  #endif
                  break;
                  
                case CIPSEND:
                  
                  bb=17+2*CH_NUM;
                  #if GPRS_FFEEDD_EN==1
                     cc=8;
                  #else
                     cc=5;
                  #endif
                     
                     
                  #if RTC_UTC_EN==1
                  if(Pt.UtcEn==0)
                  #endif
                  {
                      #if GPRS_POW_EN==1
                      if(Pt.GprsPowDueOut>0)
                      {
                        i=(u16)1*bb+3*CH_NUM+23+cc;//1������
                      }
                      else
                      {
                        i=Pt.GprsDueOut*bb+3*CH_NUM+23+cc;
                      }
                      #else

                        i=Pt.GprsDueOut*bb+3*CH_NUM+23+cc;//Pt.GprsDueOut������                

                      #endif
                  
                      Pt.GprsThisDueOut=Pt.GprsDueOut;
                      Pt.GprsThisRec16Num=PcBsq.Rec16Num;
                      Pt.GprsThisRecPt=PcBsq.RecPt;
        
                      if(1==Flag.SeeComing)
                      {
                        ComSendH();
                        U1SendByte('C');
                        U1SendValue(Pt.GprsDueOut);
                        ComSendL();
                      }
                      
                      U0SendString("AT+CIPSEND=");
                      U0SendValue(i);
                      U0SendString("\r\n"); 
                  }
                     
                  break;
                  
                case SENDDAT:
                  
                      Pt.GprsStepDog=30;
                      U1SendStrPC("DT\r\n");

                      #if GPRS_POW_EN==1
                      if(Pt.GprsPowDueOut>0)
                      {
                        GprsPowTcpSend();//�����ϵ��ϵ�TCP���ݰ���1����ʱ����
                      }
                      else
                      {
                        GprsGetRealData(Pt.GprsThisDueOut);//����TCP���ݰ� 
                      }
                      #else
                      GprsGetRealData(Pt.GprsThisDueOut);//����TCP���ݰ� 
                      #endif
                      
                      #if GPRS_SERVER_OK_EN==1
                      Pt.GprsWaitSpt=GPRS_SERVER_TIMEOUT;
                      U1SendStrPC("LISING\r\n");//�ȴ�����������
                      #endif
                  
                      break;
                  
                case S_LISING:
                  
                      U0SendString("AT\r\n");
                      //U0SendString("AT+CIPSHUT\r\n");

                      break;  

            #else//MG301��MG323B-------------------------------------------------
                
                 //ע�⣺MG301  ��SISO��һ������CLOSE �� MG323B  ���С�
                  
                case G_CGATT:
                  U1SendStrPC("CGATT?\r\n");
                  U0SendString("AT+CGATT?\r\n");//���ø���GPRS
                  break;

                case COPS:
                  U0SendString("AT+COPS?\r\n");
                  break;
                  
                case IOMODE:
                  U0SendString("AT^IOMODE=0,1\r\n");
                  break;
                  
                case SICS1:
                  U1SendStrPC("GPRS0\r\n");
                  U0SendString("AT^SICS=0,\"conType\",\"GPRS0\"\r\n");
                  break;
                case SICS2:
                  if(Pt.ChinaMobile)
                  {
                    U1SendStrPC("CMNET\r\n");//�ƶ�CMNET,CMWAP
                    U0SendString("AT^SICS=0,\"apn\",\"CMNET\"\r\n");
                  }
                  else
                  {
                    U1SendStrPC("UNINET\r\n");//��ͨUNINET,UNIWAP  
                    U0SendString("AT^SICS=0,\"apn\",\"UNINET\"\r\n");
                  }
                  break;
                case SISS1:
                  U1SendStrPC("Socket\r\n");
                  U0SendString("AT^SISS=0,\"srvType\",\"Socket\"\r\n");
                  break;
                case SISS2:
                  U1SendStrPC("conId\r\n");
                  U0SendString("AT^SISS=0,\"conId\",0\r\n");
                  break;
                  
                
                case SISSIP:
                  
                  #if RTC_UTC_EN==1
                      if(Pt.UtcEn==1)
                      {
                        U1SendStrPC("UT\r\n");
                        
                        //MG323B��MG301
                        U0SendString("AT^SISS=0,\"address\",\"socktcp://");
                        U0SendString("time.nist.gov:14");
                        U0SendString("\"\r\n");
                      }
                      else
                      {
                        SendIpPortFromEE();
                      }
                  #else
                      SendIpPortFromEE();
                  #endif
                  break; 
                  

                case SISO:
                  U1SendStrPC("OPEN\r\n");//������
                  U0SendString("AT^SISO=0\r\n");
                  Pt.GprsStepDog=10;
                  break;
                  
                        
                      case SISW://ȡ�ֽ���  
      
                        bb=17+2*CH_NUM;
                        #if GPRS_FFEEDD_EN==1
                           cc=8;
                        #else
                           cc=5;
                        #endif
                        
                        #if GPRS_POW_EN==1
                        if(Pt.GprsPowDueOut>0)
                        {
                          i=(u16)1*bb+3*CH_NUM+23+cc;//1������
                        }
                        else
                        {
                          i=Pt.GprsDueOut*bb+3*CH_NUM+23+cc;
                        }
                        #else
                        i=Pt.GprsDueOut*bb+3*CH_NUM+23+cc;//Pt.GprsDueOut������
                        #endif
                        
                        Pt.GprsThisDueOut=Pt.GprsDueOut;
                        Pt.GprsThisRec16Num=PcBsq.Rec16Num;
                        Pt.GprsThisRecPt=PcBsq.RecPt;
                        
                        if(1==Flag.SeeComing)
                        {
                           ComSendH();
                           U1SendByte('C');
                           U1SendValue(Pt.GprsThisDueOut);
                           ComSendL();
                        }
                        
                        
                        U0SendString("AT^SISW=0,");
                        if(i>999)
                          U0SendByte((i/1000)%10+0x30);
                        if(i>99)
                          U0SendByte((i/100)%10+0x30);
                        if(i>9)
                          U0SendByte((i/10)%10+0x30);
                        U0SendByte(i%10+0x30);
                        U0SendString("\r\n");
                        
                        Pt.FgSISW=1;//���ڷ��ͳ���
                        
                        break;
                      case SENDDAT:
                        Pt.GprsStepDog=30;
                        
                        U1SendStrPC("DT\r\n");
                        
                        #if GPRS_POW_EN==1
                        if(Pt.GprsPowDueOut>0)
                        {
                          GprsPowTcpSend();//�����ϵ��ϵ�TCP���ݰ���1����ʱ����
                        }
                        else
                        {
                          GprsGetRealData(Pt.GprsThisDueOut);//����TCP���ݰ� 
                        }
                        #else
                        GprsGetRealData(Pt.GprsThisDueOut);//����TCP���ݰ� 
                        #endif
                        
                        #if GPRS_SERVER_OK_EN==1
                        Pt.GprsWaitSpt=GPRS_SERVER_TIMEOUT;
                        U1SendStrPC("LISING\r\n");//�ȴ�����������
                        #endif
                        
                        NOP;
                        break;
                        
                      case MG_LISING:
                        
                        U0SendString("AT\r\n");//zz
                        //U0SendString("AT^SISC=0\r\n");
          
                        break;
 
              #endif
              
              default:break;
          }
      
      }

    }
    
  }
#endif
}

#endif



void DogDeel(void)
{
    //���Ź�..................................................................
    if(Pt.IniStepDog>0)//��ʼ���������Ź�
    {
      if(--Pt.IniStepDog==0)
      {
        if(Pt.IniIngStep==Pt.IniBcStep)
        {
          Pt.IniStartTime=STEP_TIME;
        }
      }
    }
    if(Pt.IniLongDog>0)//��ʼ���������Ź�
    {
      if(--Pt.IniLongDog==0)
      {
          #if GPRS_ENABLE==1
          if(Pt.GprsSleep==1)
          {
              Pt.GprsRstErrPt++;
              GsmToPowOff();//ʡ��ģʽ���ر�ģ��
          }
          else
          {
              #if SMS_BATONLY_ENABLE==1
              Sms.SmsErr=1;//�����־����ʱʱ�䵽ʱ��0
              GsmToPowOff();
              #else
              ErrorToRst();
              #endif
          }
          #else
              #if SMS_BATONLY_ENABLE==1
              Sms.SmsErr=1;//�����־����ʱʱ�䵽ʱ��0
              GsmToPowOff();
              #else
              ErrorToRst();
              #endif  
          #endif
      }
    }
    if(Pt.SmsSendStepDog>0)//SMS�������Ź�
    {
       if(--Pt.SmsSendStepDog==0)
       {
          if(Pt.SmsBcStep==Pt.SmsIngStep)
            Pt.SmsTime=STEP_TIME;
       }
    }
    if(Pt.SmsSendLongDog>0)//SMS�������Ź�
    {
       if(--Pt.SmsSendLongDog==0)
       {
           //LED_OFF;
           if(Pt.SmsTErrCt<SMS_T_ERR_CT)
           {
              Pt.SmsTErrCt++;
              
              Pt.SmsSendLongDog=SMS_SEND_DOG_TIME;
              if(Pt.SmsDueOut>0)
              {
                Pt.SmsIngStep=SMSLEN;
                Pt.SmsTime=STEP_TIME;
              }
           }
           else//�����ط�����������ģ��
           {
              Pt.SmsDueOut=0;
              
              #if GPRS_ENABLE==1
              if(Pt.GprsSleep==1)
              {
                Pt.GprsRstErrPt++;
                GsmToPowOff();//ʡ��ģʽ���ر�ģ��
              }
              else
              {
                #if SMS_BATONLY_ENABLE==1
                Sms.SmsErr=1;//�����־����ʱʱ�䵽ʱ��0
                GsmToPowOff();
                #else
                ErrorToRst();
                #endif
              }
              #else
                #if SMS_BATONLY_ENABLE==1
                Sms.SmsErr=1;//�����־����ʱʱ�䵽ʱ��0
                GsmToPowOff();
                #else
                ErrorToRst();
                #endif        
              #endif
           }
       }
    }
  #if ((GPRS_ENABLE==1)&&(GPRS_TC_EN==0))//zz
    
    
    
    #if RTC_UTC_EN==1
    if(Pt.UtcDog>0)
    {
         if(--Pt.UtcDog==0)//ʱ�䳬ʱ
         {
            Pt.UtcErr++;
            if(Pt.UtcErr<UTC_ERR_TIME)
            {
              U1SendStrPC("again\r\n");
              Pt.UtcEn=1;//����һ��
              Pt.UtcDog=UTC_DOG_TIME;//���Ź�������ʱʱ�䵽��δ��ɣ�˵����ʱʧ�ܣ�ֹͣ��ʱ
            }
            else
            {
              U1SendStrPC("end\r\n");
              Pt.UtcErr=0;
              Pt.UtcEn=0;//�ظ���ζ�ʧ�ܣ�ֹͣ��ʱ
            }
            
            ErrorToRst();
         }
    }
    #endif
    
    
    if(Pt.GprsStepDog>0)//GPRS�������Ź�
    {
       if(--Pt.GprsStepDog==0)
       {
          if(Pt.GprsBcStep==Pt.GprsIngStep)
            Pt.GprsStart=1;
       }
    }
    if(Pt.GprsLongDog>0)//GPRS�������Ź�
    {
      if(--Pt.GprsLongDog==0)//��ʱ������ʧ��
      {
        if(Pt.GprsTErrCt<GPRS_T_ERR_CT)
        {
          
          Pt.GprsTErrCt++;
          
          //����
          Pt.GprsIngStep=GPRS_BEGIN;//zz
          Pt.GprsLongDog=GPRS_DOG_TIME;
          Pt.GprsStart=1;
          

          #if SIM800C_EN==1
               U0SendString("AT+CIPCLOSE\r\n");//zz
          #else
               U0SendString("AT^SISC=0\r\n");
               Pt.FgSISW=0;
          #endif
        }
        else//�����ط�����������ģ��
        {
          
            U1SendStrPC("G-LONG\r\n");//zz
          
            #if GPRS_ENABLE==1
            if(Pt.GprsSleep==1)
            {
                Pt.GprsRstErrPt++;
                if(Pt.GprsRstErrPt>1)//������������1
                {
                  GsmToPowOff();//ʡ��ģʽ���ر�ģ��
                }
                else
                {
                  ErrorToRst();
                }
            }
            else
            {  
                 ErrorToRst();
            }
           #else
           ErrorToRst();
           #endif

        }
      }
    }

    #endif
}


#if GPRS_ENABLE==1
//GPRS LG31,����ģʽ�����ͳɹ�
void Grps_TCP_Ok(void)
{
     #if GPRS_TC_EN==0
     Pt.GprsStart=0;
     Pt.GprsIngStep=GPRS_END;
     Pt.GprsLongDog=0;
     Pt.GprsStepDog=0;
     #endif
     
     GprsSendOk();
     
     #if GPRS_SERVER_OK_EN==1
     Pt.GprsWaitSpt=0;//���ͳɹ�����0
     #endif
     
     #if GPRS_UP_TIME_EN==1
     Pt.Gprsing=0;//�����ϴ����
     #endif
                      
     
    #if SIM800C_EN==1
         #if M26_EN==1
         U0SendString("AT+QICLOSE\r\n");//zz
         #else
         U0SendString("AT+CIPCLOSE\r\n");//zz
         #endif
    #else
         U0SendString("AT^SISC=0\r\n");
    #endif
                          
}
#endif


void U0Deel(void)//��ģ����յ����ݴ���
{
  u8 PosS;
  
  #if LBS_JWD_EN==1
  u16 jj,ww;
  #endif
  
  if(Uart_0.FlagFinish==1)
  {
      //IE2 &=~ UCA0RXIE;//��������ɣ����´��ж�
      Uart_0.RecPt=0;
      
      #if GPRS_TC_EN==1
      if(Gprs_SF_Recv_Do())
      {
        ClrBuf(Uart_0.RxBuf,Uart_0.RecLen);
        Uart_0.FlagFinish=0;//zz
        IE2 |= UCA0RXIE;
        return;
      }
      #endif

      
      Sms.OnErrPt=1;//��ʱ�жϵ���,��ģ����Ӧʱ��1
      
      
      //���ŷ��ͳɹ���CMGS OK  zz
      if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCMGS,4)&&CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2))
      {
                  U1SendStrPC("SMS OK\r\n");
                  
                  SmsSendOk();//SMS�������
                  
                  #if GSM_TXT_EN==0
                  if(Sms.SmsRe)
                  {
                    Sms.SmsRe=0;
                    
                     #if GSM_MG323_EN==1
                    
                      Pt.BcCMGR=1;
                      U0SendString("AT+CMGR=10\r\n");//����10������λ�ã��������������ж���
                     #endif
                  }
                  #endif
      }
      
      
      #if GPRS_SERVER_OK_EN==1
      //300.10 160829,151556 Save Data SN:89898989
      else if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sServerCode,6))
      {
         U1SendStrPC("300.10\r\n");
         Grps_TCP_Ok();
      }
      #endif
      
      
     /* 
        +CENG: 1,1
        
        +CENG: 0,"0011,64,00,460,00,62,2b5b,10,05,58b2,255"
        +CENG: 1,"0082,41,49,62e8,460,00,58b2"
        +CENG: 2,"0086,33,00,62e6,460,00,58b2"
        +CENG: 3,"0570,23,30,62f2,460,00,58b2"
        +CENG: 4,"0593,1312,8a02,460,00,58b2"
        +CENG: 5,"0080,26,52,ffff,,,0000"
        +CENG: 6,"0070,26,07,ffff,,,0000"
        
        OK
      */
      
      #if LBS_LAC_EN==1
      else if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCENG,8))
      {
        
         if(Get_LAC_CID())
         {
            if(Pt.IniIngStep==LBS0)
            {
              Pt.IniIngStep=LBS0+1;
              Pt.RstErrPt=0;//zz
              Pt.IniStartTime=STEP_TIME;
            }
         }
         
         else
         {
         
            U1SendStrPC("LBS_ERR\r\n");
            if(1==Flag.SeeComing)
            {
                ComSendH();
                U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                ComSendL();
            }
         }
         
      }
      #endif
      
        
      
      #if RTC_UTC_EN==1 //�Զ���ʱ, ����ʱ����Ϣ 
            else if(CompBuf(&Uart_0.RxBuf[0],Uart_0.RecLen,sUTC,8))
            {
                  //ʱ�����������ʱ  time.nist.gov:14,57571 16-07-02 06:34:10 50 0 0 417.2 UTC(NIST)
              
                  u16 aa;
                  StrcutRtc Utc;
              
                  aa=GetXpt(Uart_0.RxBuf,0,Uart_0.RecLen,'-',2);//��2��-
                  if(aa==10000)return;//zz
                  
                  Utc.Year=(Uart_0.RxBuf[aa-5]-0x30)*10+(Uart_0.RxBuf[aa-4]-0x30);
                  Utc.Month=(Uart_0.RxBuf[aa-2]-0x30)*10+(Uart_0.RxBuf[aa-1]-0x30);
                  Utc.Day=(Uart_0.RxBuf[aa+1]-0x30)*10+(Uart_0.RxBuf[aa+2]-0x30);
                  
                  Utc.Hour=(Uart_0.RxBuf[aa+4]-0x30)*10+(Uart_0.RxBuf[aa+5]-0x30);
                  Utc.Minute=(Uart_0.RxBuf[aa+7]-0x30)*10+(Uart_0.RxBuf[aa+8]-0x30);
                  Utc.Second=(Uart_0.RxBuf[aa+10]-0x30)*10+(Uart_0.RxBuf[aa+11]-0x30);
                  


                  
                  if(Rtc_Is_Right(&Utc))//ʱ���ʽ��ȷ
                  {
                        Utc.SS=DateToSeconds(&Utc);
                        Utc.SS=Utc.SS+3600*8;//ת��Ϊ����ʱ��
                                                
                        //�ȶ�ȡ�豸RTC
                        Rtc.SS=ReadRtcD10();
                                                
                       //�Ƚϣ����ʱ�� �Զ���ʱ
                       if((Rtc.SS>Utc.SS+D_RTC_TIME)||(Rtc.SS<Utc.SS-D_RTC_TIME))
                       {
                            SecondsToDate(Utc.SS,&Rtc);
                                                              
                            //����ʱ��
                            RtcD10ToBcd(&Rtc);//ת��BCD
                            RtcSetTime();
                            RtcBcdToD10(&Rtc);
                                                    
                            U1SendStrPC("RTC ");//zz
                                                    
                      
                        }
                                                
                                  
                  }
                  
                  
            }
      #endif      
      
      //+CPIN: NOT READY  zz
      else if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCPIN_NOT,12))
      {
        U1SendStrPC("CPIN NOT READY\r\n");
        if(++Pt.CpinPt>8)//����8��ʧ�ܲ��ж�����ʧ��
        {
           Pt.CpinPt=0;
           ErrorToRst();
        }
      }
      
      /*
      //DEACT
      else if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sDEACT,5))
      {
        U1SendStrPC("DEACT\r\n");
        U0SendString("AT+CIPSHUT\r\n");
      }
      */
      
      //����ʱ��ȡ�ź�����
      else if((Pt.IniIngStep==INI_END)&&CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2)&&CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sATCSQ,3))
      {
           if(1==Flag.SeeComing)
           {
                ComSendH();
                U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                ComSendL();
           }
           
           Gsm.GsmXh=GetGsmXh();
           if(Gsm.GsmXh<32)//ȷ���ź�ֵ����ȷ��
           {
                 #if SMS_BATONLY_ENABLE==0//��ʱ����ģʽʱ��Ҫ�ػ�����ʼ��ʱ����ʾ�ź�
                 LcdGsmXh(Gsm.GsmXh);
                 #endif
                 Gsm.ReadCsqDog=READ_CSQ_DOG_TIME;//���Ź���
                                
                                  
                 #if GPRS_ENABLE==1
        
                    if(Pt.GprsSleep==1)//ʡ��ģʽʱ������״̬ʱ�ر�ģ��
                     {
                        if(GsmIsFree(1)&&(Pt.SmsDueOut==0)&&(Sms.AlarmDueOut==0))
                        {
                           if(GprsDataHav()==0)
                           {
                             #if L206_SLP_EN==1
                             GsmSlp();
                             #else
                             GsmToPowOff();
                             #endif
                             
                             
                           }
                        }
                                          
                     }
                     else//����ģʽ�£�ÿ30���һ��LBS
                    {
        
                        #if LBS_LAC_EN==1
                        U0SendString("AT+CENG?\r\n");//zz
                        #elif LBS_JWD_EN==1
                        
                        #if M26_EN==1
                         Pt.IniIngStep=L_CGREG;
                        #else
                         Pt.IniIngStep=L_CGATT;
                        #endif
                        Pt.IniStartTime=2;
                        
                        #endif
                                         
                    }
                                       
              #else
                                       
                   #if (SIM800C_EN==0)
        
                         //���յ����ţ�ֱ�ӷ��͵�TE, MG301��BUG, �޷����õ�ģ�飬��һ��ʱ�䣬CNMI���Զ���ص�+CNMI: 3,0,0,1,0��
                         //�����޷����յ����ţ�����취��ƽʱ����ʱ��һֱ����AT+CNMI=3,2,0,1,0 
                         U1SendStrPC("CNMI\r\n");
                         #if GSM_EM310_EN==1
                         U0SendString("AT+CNMI=1,2,0,0,0\r\n");
                         #else
                         U0SendString("AT+CNMI=3,2,0,1,0\r\n");//301 ֻ������ 
                         #endif
                                          
                   #endif
                                          
              #endif
           }
      }
      
      
      //OK,CONNECT,SISW..............................................................
      #if ((SIM800C_EN==1)&&(GPRS_ENABLE==0))
      else if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2))
      
      #elif ((SIM800C_EN==1)&&(GPRS_ENABLE==1))
         
          #if GPRS_TC_EN==1
          else if((CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2))\
            ||(GetPosC(Uart_0.RxBuf,0,Uart_0.RecLen,'.')!=255)\
            ||(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCONNECT,6)))
          #else
          else if((CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2))\
            ||(GetPosC(Uart_0.RxBuf,0,Uart_0.RecLen,'.')!=255)\
            ||(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCONNECT,6))\
            ||((CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sLN,2))&&(Pt.GprsIngStep==CIPSEND)))
          #endif

      #else
            
          #if GPRS_TC_EN==1
          else if (CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2)\
               ||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCONNECT,6))
            
          #else
          else if (CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2)\
               ||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCONNECT,6)\
              ||((CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sSI,2)||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sSW,2))\
                &&(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sRejected,8)==0)))
          #endif
      #endif
      {

          if(Pt.IniIngStep!=INI_END)//��ʼ��δ���
          {
            
            Pt.RstErrPt=0;//zz
            Pt.IniStartTime=STEP_TIME;
            
            switch(Pt.IniIngStep)
            {
              case ATE:
                Pt.IniIngStep=ATE+1;
                break;
                
              case GMR:
                Pt.IniIngStep=GMR+1;
                break;
                
              #if GSM_EM310_EN==0
               case CFUN:
                Pt.IniIngStep=CFUN+1;
                break;
              #endif
                
              case CMGF:
                Pt.IniIngStep=CMGF+1;
                break;
              
              #if SMS_TCP_EN==1
              case CIMI:

                PosS=GetPos15N(Uart_0.RxBuf,1,30);
                if(PosS!=255)
                {
                    for(u16 j=0;j<15;j++)
                      Pt.SIM_IMSI[j]=Uart_0.RxBuf[PosS+j];

                    Pt.IniIngStep=CIMI+1;
                }
                Pt.IniStartTime=2;
                break;
                
              case CCID:
                
                PosS=GetPos20NC(Uart_0.RxBuf,1,30);
                if(PosS!=255)
                {
                    for(u16 j=0;j<20;j++)
                      Pt.SIM_CCID[j]=Uart_0.RxBuf[PosS+j];

                    Pt.IniIngStep=CCID+1;
                }

                Pt.IniStartTime=2;
                break;
                
              case IMEI:

                PosS=GetPos15N(Uart_0.RxBuf,1,30);
                if(PosS!=255)
                {
                    for(u16 j=0;j<15;j++)
                      Pt.G_IMEI[j]=Uart_0.RxBuf[PosS+j];
                     Pt.IniIngStep=IMEI+1;
                }
                Pt.IniStartTime=2;
                break;
                
              #endif
                
              #if GPRS_SMS_CNMI_EN==1
                  case CNI_INT:
                    Pt.IniIngStep=CNI_INT+1;
                    break;
              #endif
                
                
                
              #if GPRS_AUTO_LIS_EN==1  
              case LSING:
                Pt.IniIngStep=LSING+1;
                break;
              #endif
                
              #if LBS_LAC_EN==1   
              case CENG:
                Pt.IniIngStep=CENG+1;//zz
                break;
                
              #elif LBS_JWD_EN==1  
                
              case L_CGREG:
                
                PosS=GetPosS(Uart_0.RxBuf,1,30,"G:");
                if(PosS!=255)
                {
                  //+CREG: 0,0  ��ֹͣ��ѰGSM����
                  //+CREG: 0,1  ��ע�����磬����
                  //+CREG: 0,2  ������ѰGSM����
                  //+CREG: 0,3  ע�ᱻ�ܾ�
                  //         4  δ֪
                  //+CREG: 0,5  ��ע�����磬����
                  if((Uart_0.RxBuf[PosS+5]=='1')||(Uart_0.RxBuf[PosS+5]=='5'))
                  {
                    Pt.IniIngStep=L_CGREG+1;
                  }
                }
                Pt.IniStartTime=2;
                break;
                
                
#if M26_EN==0//L206
              case L_CGATT:
                //+CGATT: 1
                PosS=GetPosS(Uart_0.RxBuf,1,30,"T:");
                if(PosS!=255)
                {
                  if(Uart_0.RxBuf[PosS+3]=='1')
                  {
                    Pt.IniIngStep=L_CGATT+1;//zz
                    Pt.IniLongDog=INI_LONG_DOG_TIME;

                  }

                }
                Pt.IniStartTime=2;
                break;
                
              case L_CIPSHUT:
                Pt.IniIngStep=L_CIPSHUT+1;//zz
                break;
              case L_CIPMODE:
                Pt.IniIngStep=L_CIPMODE+1;//zz
                break;
              case L_COPS:
                    if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMOBILE,6)||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMobile,6))
                      Pt.ChinaMobile=1;//�ƶ��Ŀ�
                    else
                      Pt.ChinaMobile=0;//��ͨ�Ŀ�
                Pt.IniIngStep=L_COPS+1;//zz
                break;
              case L_CSTT:
                Pt.IniIngStep=L_CSTT+1;
                break;
              case L_CIICR:
                Pt.IniIngStep=L_CIICR+1;
                break;
#endif
              case L_GTPOS:
                //ȡ��γ������, С�����̶�7λС����0Ҳ����
                //+GTPOS:120.3795527,30.3067144, �й������� ���� 73-135�� γ��17-54
                //u16 jj,ww;
                
                //M26
                //AT+QCELLLOC=1
                //+QCELLLOC: 120.372269,30.306774
                
                
                jj=GetXpt(Uart_0.RxBuf,0,Uart_0.RecLen,'.',1);
                ww=GetXpt(Uart_0.RxBuf,0,Uart_0.RecLen,'.',2);
                if((jj>5)&&(ww>5)&&(jj!=10000)&&(ww!=10000))//��Ч����
                {
                    #if M26_EN==1
                    if(CK_Buf_Num(&Uart_0.RxBuf[jj-2],2)&&CK_Buf_Num(&Uart_0.RxBuf[jj+1],6)\
                      &&CK_Buf_Num(&Uart_0.RxBuf[ww-2],2)&&CK_Buf_Num(&Uart_0.RxBuf[ww+1],6))            
                    #else
                    if(CK_Buf_Num(&Uart_0.RxBuf[jj-2],2)&&CK_Buf_Num(&Uart_0.RxBuf[jj+1],7)\
                      &&CK_Buf_Num(&Uart_0.RxBuf[ww-2],2)&&CK_Buf_Num(&Uart_0.RxBuf[ww+1],7))
                     #endif
                    {

                        //����
                        if(CK_Buf_Num(&Uart_0.RxBuf[jj-3],1))//��λ��Ϊ����
                        {
                          
                          StateC.LBS_JdInt=100*(Uart_0.RxBuf[jj-3]-0x30)+10*(Uart_0.RxBuf[jj-2]-0x30)+(Uart_0.RxBuf[jj-1]-0x30);
                        }
                        else//�ް�λ��
                        {
                          
                          StateC.LBS_JdInt=10*(Uart_0.RxBuf[jj-2]-0x30)+(Uart_0.RxBuf[jj-1]-0x30);
                        }
                        
                        
                        StateC.LBS_JdDec=(u32)100000*(Uart_0.RxBuf[jj+1]-0x30)+(u32)10000*(Uart_0.RxBuf[jj+2]-0x30)\
                          +(u32)1000*(Uart_0.RxBuf[jj+3]-0x30)+(u32)100*(Uart_0.RxBuf[jj+4]-0x30)+(u32)10*(Uart_0.RxBuf[jj+5]-0x30)\
                            +(u32)1*(Uart_0.RxBuf[jj+6]-0x30);

                        //γ��
                        
                        StateC.LBS_WdInt=10*(Uart_0.RxBuf[ww-2]-0x30)+(Uart_0.RxBuf[ww-1]-0x30);
                        
                        
                        StateC.LBS_WdDec=(u32)100000*(Uart_0.RxBuf[ww+1]-0x30)+(u32)10000*(Uart_0.RxBuf[ww+2]-0x30)\
                          +(u32)1000*(Uart_0.RxBuf[ww+3]-0x30)+(u32)100*(Uart_0.RxBuf[ww+4]-0x30)+(u32)10*(Uart_0.RxBuf[ww+5]-0x30)\
                            +(u32)1*(Uart_0.RxBuf[ww+6]-0x30);
                        
                        
                        StateC.LBSpt=Pt.GprsSendTime*6;//6���ϴ����û���»��Զ���0  zz
                        
                        U1SendStrPC("LBS_OK\r\n");
                        
                        Pt.IniIngStep=L_GTPOS+1; 
                        

                        
                    }
                }
                Pt.IniStartTime=2;

                break;
                
                
              #endif
                
              case CPIN:
                PosS=GetPosS(Uart_0.RxBuf,1,30,"N:");
                if(PosS!=255)
                {
                    if((Uart_0.RxBuf[PosS+3]=='R')//CPIN  READY ˵�����PIN��Ȩ������˵����Ҫ����PIN��
                       &&(Uart_0.RxBuf[PosS+4]=='E')
                         &&(Uart_0.RxBuf[PosS+5]=='A')
                           &&(Uart_0.RxBuf[PosS+6]=='D')
                             &&(Uart_0.RxBuf[PosS+7]=='Y'))
                    {
                      Pt.IniIngStep=CPIN+1;
                    }
                }
                break;
              case CREG:
                PosS=GetPosS(Uart_0.RxBuf,1,30,"G:");
                if(PosS!=255)
                {
                  //+CREG: 0,0  ��ֹͣ��ѰGSM����
                  //+CREG: 0,1  ��ע�����磬����
                  //+CREG: 0,2  ������ѰGSM����
                  //+CREG: 0,3  ע�ᱻ�ܾ�
                  //         4  δ֪
                  //+CREG: 0,5  ��ע�����磬����
                  if((Uart_0.RxBuf[PosS+5]=='1')||(Uart_0.RxBuf[PosS+5]=='5'))
                  {
                    Pt.IniIngStep=CREG+1;
                    
                  
                  }
                }
                Pt.IniStartTime=2;
                break;
                
              #if GPRS_ENABLE==1  
              case TC_CGREG:
                
                PosS=GetPosS(Uart_0.RxBuf,1,30,"G:");
                if(PosS!=255)
                {
                   //+CREG: 0,0  ��ֹͣ��ѰGSM����
                  //+CREG: 0,1  ��ע�����磬����
                  //+CREG: 0,2  ������ѰGSM����
                  //+CREG: 0,3  ע�ᱻ�ܾ�
                  //         4  δ֪
                  //+CREG: 0,5  ��ע�����磬����
                  if(((Uart_0.RxBuf[PosS+5]=='1')||(Uart_0.RxBuf[PosS+5]=='5'))||((Uart_0.RxBuf[PosS+4]=='1')||(Uart_0.RxBuf[PosS+4]=='5')))//MG323 �����+CGREG:0,1���������4λ��
                  {
                    Pt.IniIngStep=TC_CGREG+1;
                  }
                }
                Pt.IniStartTime=2;
                break;
               #endif
                
                
                
              case CSQ:
                
                Gsm.GsmXh=GetGsmXh();
                if((Gsm.GsmXh>0)&&(Gsm.GsmXh<32))//ȷ���ź�ֵ����ȷ��
                {
                      #if SMS_BATONLY_ENABLE==0//��ʱ����ģʽʱ��Ҫ�ػ�����ʼ��ʱ����ʾ�ź�
                      LcdGsmXh(Gsm.GsmXh);
                      #endif
                      
                      Pt.IniIngStep=CSQ+1;
                }

                Pt.IniStartTime=2;
                
                       // U1SendStrPC("LBS2\r\n");
                       // U0SendString("AT+CENG?\r\n");//zz

                break;
                
              case T_CSQ:
                
                Gsm.GsmXh=GetGsmXh();
                if((Gsm.GsmXh>0)&&(Gsm.GsmXh<32))//ȷ���ź�ֵ����ȷ��
                {
                      LcdGsmXh(Gsm.GsmXh);
                      Pt.IniIngStep=T_CSQ+1;
                }

                Pt.IniStartTime=2;
                

                break;
                
                
                
                
             #if SIM800C_EN==1
                #if GPRS_TC_EN==1
                
                  #if LBS_JWD_EN==0
                  case TC_CGATT:
                    //+CGATT: 1
                    PosS=GetPosS(Uart_0.RxBuf,1,30,"T:");
                    if(PosS!=255)
                    {
                      if(Uart_0.RxBuf[PosS+3]=='1')
                      {
                        Pt.IniIngStep=TC_CGATT+1;//zz
                        Pt.IniLongDog=INI_LONG_DOG_TIME;
    
                      }
    
                    }
                    Pt.IniStartTime=2;
                    break;
                    
                  #endif
                
                  case TC_CIPSHUT:
                    Pt.IniIngStep=TC_CIPSHUT+1;
                    break;
                    
                  #if M26_EN==1  
                  case TC_QIDNSIP:
                    Pt.IniIngStep=TC_QIDNSIP+1;
                    break;
                  #endif
                    
                  case TC_CIPMUX:
                    Pt.IniIngStep=TC_CIPMUX+1;
                    break;
                  case TC_CIPMODE:
                    Pt.IniIngStep=TC_CIPMODE+1;
                    break;
                  case TC_CIPCCFG:
                    Pt.IniIngStep=TC_CIPCCFG+1;
                    break;
                    
                  #if ((LBS_JWD_EN==0)||((LBS_JWD_EN==1)&&(M26_EN==1)))
                  case TC_COPS:
                    if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMOBILE,6)||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMobile,6))
                      Pt.ChinaMobile=1;//�ƶ��Ŀ�
                    else
                      Pt.ChinaMobile=0;//��ͨ�Ŀ�
                    Pt.IniIngStep=TC_COPS+1;
                    break;

                  case TC_CSTT:
                    Pt.IniIngStep=TC_CSTT+1;
                    Sms.OnErrPt=1;

                    break;
                  #endif  
                    
                  case TC_CIICR:
                    Pt.IniIngStep=TC_CIICR+1;
                    break;
    
                  case TC_CIFSR:
                    Pt.IniIngStep=TC_CIFSR+1;
                    break;
                 #endif
                
              #else //MG301--------------------------------------------------------
                
                
                #if GPRS_TC_EN==1
                


                              case M_CGATT:
                                //+CGATT: 1
                                PosS=GetPosS(Uart_0.RxBuf,1,30,"T:");
                                if(PosS!=255)
                                {
                                  if(Uart_0.RxBuf[PosS+3]=='1')
                                  {
                                    Pt.IniIngStep=M_CGATT+1;//zz
                                    Pt.IniLongDog=INI_LONG_DOG_TIME;
                
                                  }
                
                                }
                                Pt.IniStartTime=2;
                                break;
                                  
        
                                case COPS:
                                  if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMOBILE,6)||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMobile,6))
                                    Pt.ChinaMobile=1;//�ƶ��Ŀ�
                                  else
                                    Pt.ChinaMobile=0;//��ͨ�Ŀ�
                                  Pt.IniIngStep=COPS+1;
                                  break;
          
                                case IOMODE:
                                  Pt.IniIngStep=IOMODE+1;
                                  break;
                                case SICS1:
                                  Pt.IniIngStep=SICS1+1;
                                  break;
                                case SICS2:
                                  Pt.IniIngStep=SICS2+1;
                                  break;
                                case SISS1:
                                  Pt.IniIngStep=SISS1+1;
                                  break;
                                case SISS2:
                                  Pt.IniIngStep=SISS2+1;
                                  break;
                                 
                                case SISSIP:
                                    Pt.IniIngStep=SISSIP+1;
                                  break; 
                                  
                                case SISO:
                                      Pt.IniIngStep=SISO+1;
                                      Pt.GprsStart=2;
                                 
                                 break;
                                 
                                 
                                case IPCFL1:
                                    Pt.IniIngStep=IPCFL1+1;
                                  break; 
                                case IPCFL2:
                                    Pt.IniIngStep=IPCFL2+1;
                                  break; 

                
                    #endif
                
                
                
             #endif
                
                
                
                

              #if GPRS_ENABLE==0   
              case CNMI:
                Pt.IniIngStep=CNMI+1;
                break;
              case CLIP:
                Pt.IniIngStep=CLIP+1;

                break;
              #endif
                
              #if GSM_TXT_EN==0
                
                #if GSM_MG323_EN==1
              case CMGR:
                if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCMGR,11))//û�ж��Ŵ洢��SIM��
                {
                  Pt.BcCMGR=0;
                }
                else
                {
                  Pt.BcCMGR=1;
                }
                Pt.IniIngStep=CMGR+1;
                break;
              case CMGD:
                Pt.IniIngStep=CMGD+1;
                break;
                #endif
                
              #endif
 
              case ATW:
                Pt.IniStartTime=0;
                Pt.IniStepDog=0;
                Pt.IniLongDog=0;
                Pt.IniIngStep=INI_END;//��ʼ�����
                Pt.SmsIngStep=SMS_END;
                
                #if GPRS_ENABLE==1
                  #if GPRS_TC_EN==0
                  Pt.GprsIngStep=GPRS_END;
                  #endif
                  
                #endif
                
                Pt.IniErrPt=0;

                Gsm.ReadCsqDog=2;//��ʼ��ȡCSQ�ź�ǿ��
                break;
                
              default:break;
            }
          }
          #if GPRS_TC_EN==0
          else//��ʼ�������
          {
        
                    #if GPRS_ENABLE==1
                    
                      #if RTC_UTC_EN==1
                        
                          #if GPRS_POW_EN==1
                          if(((Pt.GprsDueOut>0)||(Pt.GprsPowDueOut>0)||(Pt.UtcEn==1))&&GsmIsFree(0))
                          #else
                          if(((Pt.GprsDueOut>0)||(Pt.UtcEn==1))&&GsmIsFree(0))
                          #endif
                        
                      #else
                        
                          #if GPRS_POW_EN==1
                          if(((Pt.GprsDueOut>0)||(Pt.GprsPowDueOut>0))&&GsmIsFree(0))
                          #else
                          if((Pt.GprsDueOut>0)&&GsmIsFree(0))
                          #endif
               
                      #endif
                      {
                          Pt.GprsStart=1;
                          switch(Pt.GprsIngStep)
                          {
                            #if SIM800C_EN==1
        
                                case CIP1SHUT:
                                   Pt.GprsIngStep=CIP1SHUT+1;
                                  break;
                                case CIPSEND:
                                  Pt.GprsIngStep=CIPSEND+1;
                                  Pt.GprsStart=2;
                                  break;
                                  
                                case CIPSTART:
                                  
                                  #if RTC_UTC_EN==1
                                  if(Pt.UtcEn==1)
                                  {
                                       Pt.GprsStart=0;//��������һ��
                                  }
                                  else
                                  {
                                      if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCONNOK,10))//CONNECT OK ���ӳɹ�
                                        Pt.GprsIngStep=CIPSTART+1;
                                      else//OK ,�������ӳɹ�����˼
                                        Pt.GprsStart=0;
                                    
                                  }
                                  #else
                                  
                                      if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCONNOK,10))//CONNECT OK ���ӳɹ�
                                        Pt.GprsIngStep=CIPSTART+1;
                                      else//OK ,�������ӳɹ�����˼, �ȴ���ʱ�ٷ�  zz
                                        Pt.GprsStart=0;
                                  #endif
                                  break;
                                  
                                case SENDDAT:
                                  Pt.GprsIngStep=SENDDAT+1;
                                  Pt.GprsStart=4;//���ͳɹ��󣬵ȸ�������ٶϿ� 
                                  break;
                                  
                                case S_LISING:
                                  #if GPRS_SERVER_OK_EN==0
                                  Grps_TCP_Ok();
                                  #endif
        
                                  break;
                                  

                            #else //MG323 �� MG301----------------------------------------
                              
                                  
                                case G_CGATT:
                                  //+CGATT: 1
                                  PosS=GetPosS(Uart_0.RxBuf,1,30,"T:");
                                  if(PosS!=255)
                                  {
                                    if(Uart_0.RxBuf[PosS+3]=='1')
                                    {
                                      Pt.GprsIngStep=G_CGATT+1;//zz
                  
                                    }
                  
                                  }
                                  Pt.IniStartTime=2;
                                  break;
                                  
        
                                case COPS:
                                  if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMOBILE,5)||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sMobile,5))
                                    Pt.ChinaMobile=1;//�ƶ��Ŀ�
                                  else
                                    Pt.ChinaMobile=0;//��ͨ�Ŀ�
                                  Pt.GprsIngStep=COPS+1;
                                  break;
          
                                case IOMODE:
                                  Pt.GprsIngStep=IOMODE+1;
                                  break;
                                case SICS1:
                                  Pt.GprsIngStep=SICS1+1;
                                  break;
                                case SICS2:
                                  Pt.GprsIngStep=SICS2+1;
                                  break;
                                case SISS1:
                                  Pt.GprsIngStep=SISS1+1;
                                  break;
                                case SISS2:
                                  Pt.GprsIngStep=SISS2+1;
                                  break;
                                 
                                case SISSIP:
                                    Pt.GprsIngStep=SISSIP+1;
                                  break; 
                                  
                                case SISO:
                                      #if RTC_UTC_EN==1
                                      if(Pt.UtcEn==1)//time.nist.gov:14 �Զ���ʱ
                                      {
                                          Pt.GprsStart=0;//��������һ��
                                      }
                                      else
                                      {
                                          Pt.GprsIngStep=SISO+1;
                                          Pt.GprsStart=2;
                                      }
                                      
                                      #else
                                      Pt.GprsIngStep=SISO+1;
                                      Pt.GprsStart=2;
                                      #endif
        
                                 
                                 break;
                                case SISW:
                                  
                                  if(Pt.FgSISW==1)
                                  {
                                    Pt.FgSISW=0;
                                    Pt.GprsIngStep=SISW+1;
                                  }
                                  Pt.GprsStart=2;
                                  
                                  break;
                                case SENDDAT:
                                  Pt.GprsIngStep=SENDDAT+1;
                                  Pt.GprsStart=4;//���ͳɹ��󣬵ȸ�������ٶϿ� 
                                  break;
                                case MG_LISING:
                                  
                                  #if GPRS_SERVER_OK_EN==0
                                  Grps_TCP_Ok();
                                  #endif
                                  
                                  break;
                                  
                             #endif
                             default:break;
                          }
                      }
                      #endif

            }
            #endif//end #if GPRS_TC_EN==1

            if(1==Flag.SeeComing)
            {
                ComSendH();
                U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                ComSendL();
            }

      }
      //�ȴ�����SMS " > "..................................................
      else if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sLN,2))
      {
        
        if(Pt.SmsDueOut>0)
        {
          if(Pt.SmsIngStep==SMSLEN)
          {
            Pt.SmsIngStep=SMSLEN+1;
            if(1==Flag.SeeComing)
            {
              ComSendH();
              
              U1SendString("\r\nlen=");
              U1SendValue(Gsm.SmsUniLen/2);
              U1SendString(",To");
              for(u8 i=0;i<15;i++)
                U1SendByte(Gsm.SmsRxPhone[i]);

              
              #if GSM_TXT_EN==1
              for(u16 j=0;j<Gsm.SmsUniLen;j++)
                U1SendByte(Gsm.SmsRecUni[j]);
              U1SendByte(0x1a);
              #endif
              ComSendL();
            }
            
            #if GSM_TXT_EN==1
            for(u16 j=0;j<Gsm.SmsUniLen;j++)
              U0SendByte(Gsm.SmsRecUni[j]);
            U0SendByte(0x1a);
            #else
            U0SmsPdu(Gsm.SmsRxPhone,Gsm.SmsRecUni,Gsm.SmsUniLen);
            #endif
                        
            Pt.SmsSendStepDog=3;
            
          }
        }
      }
      
      //���յ�SMS................................................................
      #if GSM_TXT_EN==0
          #if ((GPRS_ENABLE==0)||(F248_EN==1)||(SMS_SET_EN==1))//GPRS�豸�����ζ��Ž��չ���
          else if(CompBuf(&Uart_0.RxBuf[0],Uart_0.RecLen,s_0891,4))
          {
            
              if(Uart_0.RecLen>30)
              {
                  ClrBuf(Gsm.SmsRxPhone,15);
                  Gsm.SmsUniLen=U0PduAnalyze(Gsm.SmsRxPhone,Gsm.SmsRecUni);
                  if(PhoneIsRight()&&SmsRecDeel())//���ն����������Զ���������д���
                  {
                    BellNn(1);//���յ�������ȷ
                    Sms.SmsRe=1;
                  }
                  else
                  {
                      #if GSM_MG323_EN==1
                      Pt.BcCMGR=1;
                      U0SendString("AT+CMGR=10\r\n");//����10������λ�ã��������������ж���
                      #endif
                  }
              }
              else
              {
                  #if GSM_MG323_EN==1 
                  Pt.BcCMGR=1;
                  U0SendString("AT+CMGR=10\r\n");//����10������λ�ã��������������ж���
                  #endif
              }
          }
          #endif
      #endif

     #if GPRS_ENABLE==0//GPRS�豸������RING���Ž��չ���     
      //RING,����
      //RING
      //+CLIP: "15988458051",161,,,,0    //������û�й������� ?
      else if(CompBuf(&Uart_0.RxBuf[0],Uart_0.RecLen,sCLIP,4))//��������
      {
        
        NOP;
        if(++Sms.RingCt>=RING_CT)//���֮�󣬹Ҷ�ͨ��
        {
          Sms.RingCt=0;
          #if (RY_GSM_ENABLE==1)//���ż̵�������
              U1SendStrPC("ATH\r\n");
              U0SendString("ATH\r\n");//�Ҷ�ͨ��
              if(Flag.Ry1ed==1)//�л��̵���
                Flag.Ry1ed=0;
              else
                Flag.Ry1ed=1;
          #else
              //ȡ����
              ClrBuf(Gsm.SmsRxPhone,15);
              u8 i=0;
              u16 j;
                
              for(j=0;j<50;j++)
              {
                 if((Uart_0.RxBuf[j]>=0x30)&&(Uart_0.RxBuf[j]<=0x39))
                 {
                   Gsm.SmsRxPhone[i++]=Uart_0.RxBuf[j];
                   if(!((Uart_0.RxBuf[j+1]>=0x30)&&(Uart_0.RxBuf[j+1]<=0x39)))
                   {
                      NOP;
                      break;
                   }
                 }
              }
              if(PhoneIsRight()&&(i>4))//����λ�����ٴ���4
              {
                  //��������
                  #if GSM_TXT_EN==1
                  Gsm.SmsUniLen=GetSmsTxt(Gsm.SmsRecUni,SMS_LOOK);//���ŷ��� ��ѯ������
                  #else
                  Gsm.SmsUniLen=GetSmsUni(Gsm.SmsRecUni,SMS_LOOK);//���ŷ��� ��ѯ������
                  #endif
                  
                  delay_ms(50);
                  Pt.SmsDueOut=1;//����SMS
              }
              else
              {
                U1SendStrPC("�������\r\n");
              }
              
              U1SendStrPC("ATH\r\n");
              U0SendString("ATH\r\n");//�Ҷ�ͨ��
          #endif
        }
      }
      #endif
      
      //�յ�����txt
      //+CMT: "+8615988458051",,"13/06/22,18:26:24+32"wd
      #if GSM_TXT_EN==1
      else if(CompBuf(&Uart_0.RxBuf[0],Uart_0.RecLen,sCMT,5))
      {
          SmsTxtAnalyze(Gsm.SmsRxPhone,Gsm.SmsRecUni);
          if(PhoneIsRight()&&SmsTxtDeel())//���ն����������Զ���������д���
          {
             BellNn(1);//���յ�������ȷ
             Sms.SmsRe=1;
          }
      }
      #endif
      
      //��������δ֪����..............................................................
      else
      {
          if(1==Flag.SeeComing)
          {
            ComSendH();
            U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
            ComSendL();
          }
          
          #if GPRS_SERVER_OK_EN==1
          //^SISR: 0,66,2016/6/2 15:26:51 GPRS Code:300.10 Message:201712345  Save Num 2/3,,�ж� 300.10
          if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sServerCode,6))
          {
             U1SendStrPC("300.10-B\r\n");//���ж�һ��
             Grps_TCP_Ok();
          }
          #endif
          
          //ERROR
          if(CompBuf(&Uart_0.RxBuf[0],Uart_0.RecLen,sERROR,5))
          {
              
              //SIM��û�壬�ػ�  CPIN ERROR  SIMû��
              #if SMS_BATONLY_ENABLE==1
              if(Pt.IniIngStep==CPIN)
              {
                Sms.SmsErr=1;
                GsmToPowOff();
              }
              #endif  
          }

      }
      
      ClrBuf(Uart_0.RxBuf,Uart_0.RecLen);
  }
  
  Uart_0.FlagFinish=0;//zz
  IE2 |= UCA0RXIE;

}
u8 SmsTxtDeel(void)
{
  // WSD WD SD ��ѯ��ǰ��ʪ��
  if(    (((Gsm.SmsRecUni[0]=='w')||(Gsm.SmsRecUni[0]=='W'))
       &&((Gsm.SmsRecUni[1]=='s')||(Gsm.SmsRecUni[1]=='S'))
       &&((Gsm.SmsRecUni[2]=='d')||(Gsm.SmsRecUni[2]=='D')))
    ||(((Gsm.SmsRecUni[0]=='s')||(Gsm.SmsRecUni[0]=='S'))
       &&((Gsm.SmsRecUni[1]=='d')||(Gsm.SmsRecUni[1]=='D')))
    ||(((Gsm.SmsRecUni[0]=='w')||(Gsm.SmsRecUni[0]=='W'))
       &&((Gsm.SmsRecUni[1]=='d')||(Gsm.SmsRecUni[1]=='D')))     )
  {
    Gsm.SmsUniLen=GetSmsTxt(Gsm.SmsRecUni,SMS_LOOK);//���ŷ��� ��ѯ������
    delay_ms(50);
    Pt.SmsDueOut=1;//����SMS
    return 1;
  }
  //DK  �򿪱���
  else if(((Gsm.SmsRecUni[0]=='d')||(Gsm.SmsRecUni[0]=='D'))\
    &&((Gsm.SmsRecUni[1]=='k')||(Gsm.SmsRecUni[1]=='K')))
  {
        ChAEnToFlash(99,1);//�򿪱���
        LcdAlarmOn(1);
        #if ALARM_TIME_ENABLE==1
        AlarmedClr();
        #endif
        
        u8 i=0;
        Gsm.SmsRecUni[i++]='D';
        Gsm.SmsRecUni[i++]='K';
        Gsm.SmsRecUni[i++]=' ';
        Gsm.SmsRecUni[i++]='O';
        Gsm.SmsRecUni[i++]='K';
        Gsm.SmsRecUni[i++]=0x0d;
        Gsm.SmsRecUni[i++]=0x0a;
        Gsm.SmsUniLen=i;
        delay_ms(50);
        Pt.SmsDueOut=1;//����SMS
        return 1;
        
  }
  //GB  �رձ���
  else if(((Gsm.SmsRecUni[0]=='g')||(Gsm.SmsRecUni[0]=='G'))\
    &&((Gsm.SmsRecUni[1]=='b')||(Gsm.SmsRecUni[1]=='B')))
  {
        ChAEnToFlash(99,0);//�رձ���
        LcdAlarmOn(0);
        BELL_OFF;
        SG_OFF;
        
        u8 i=0;
        Gsm.SmsRecUni[i++]='G';
        Gsm.SmsRecUni[i++]='B';
        Gsm.SmsRecUni[i++]=' ';
        Gsm.SmsRecUni[i++]='O';
        Gsm.SmsRecUni[i++]='K';
        Gsm.SmsRecUni[i++]=0x0d;
        Gsm.SmsRecUni[i++]=0x0a;
        Gsm.SmsUniLen=i;
        delay_ms(50);
        Pt.SmsDueOut=1;//����SMS
        return 1;
  }
  
  return 0;
}
void SmsTxtAnalyze(u8* phone,u8* txt)
{
  u16 i,j,k,t;
  //u8 buf[15];
  
  //ȡ�ֻ�����
  ClrBuf(phone,15);
  i=0;
  for(j=0;j<40;j++)
  {
     if((Uart_0.RxBuf[j]>=0x30)&&(Uart_0.RxBuf[j]<=0x39))
     {
        phone[i++]=Uart_0.RxBuf[j];
        if(!((Uart_0.RxBuf[j+1]>=0x30)&&(Uart_0.RxBuf[j+1]<=0x39)))
        {
            NOP;
            break;
        }
     }
  }
  
  //ȡtxt����,λ��
  i=0;
  for(j=0;j<100;j++)
  {
     if(Uart_0.RxBuf[j]=='"')
     {
        i++;
        if(i==4)
        {
            NOP;
            i=j;//���һ��"λ��
            break;
        }
     }
  }
  
  k=0;
  for(j=i;j<i+30;j++)
  {
     if(Uart_0.RxBuf[j]==0x0d)
     {
        k++;
        if(k==2)
        {
            NOP;
            k=j;//���һ��0x0dλ��
            break;
        }
     }
  }
  
  t=0;
  for(j=i+3;j<k;j++)
  {
    txt[t++]=Uart_0.RxBuf[j];
  }
  
  NOP;
}
//Ӣ�ĸ�ʽ����ֻ֧��2ͨ��
//Temp -12.3(AL)
//Humi 45.2(AL)
//1Temp -12.3(AL)
//2Humi 56.3(AL)
//1,���걨����0����ѯ
u16 GetSmsTxt(u8* ss,u8 Alarm)
{
   u16 i=0;
   for(u16 j=0;j<CH_NUM;j++)
   {
      if((Alarm==SMS_LOOK)//�鿴
         ||((Alarm==SMS_ALARM)&&(1&(Flag.AlarmB>>j))))//�г���
      {
          u8 ChT;//ͨ������
          ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//��ȡ��ͨ������
          
            //ͨ����.......................................................
            //ͨ������......................................................
            if(ChT==_T_T)//Temp/�¶�
            {
              ss[i++]='T';
              ss[i++]='e';
              ss[i++]='m';
              ss[i++]='p';
              ss[i++]=' ';
            }
            else if(ChT==_T_H)//Humi/ʪ��
            {
              ss[i++]='H';
              ss[i++]='u';
              ss[i++]='m';
              ss[i++]='i';
              ss[i++]=' ';
            }
            
            if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
            {
              ss[i++]='n';
              ss[i++]='u';
              ss[i++]='l';
              ss[i++]='l';
            }
            else
            {
              //ͨ����ֵ......................................................
              if(StateC.FuhaoBit&(1<<j))
              {
                ss[i++]='-';
              }
              
              if(StateC.Value[j]>999)
              {
                ss[i++]=((StateC.Value[j]/1000)%10+0x30);
              }
              
              if(StateC.Value[j]>99)
              {
                ss[i++]=((StateC.Value[j]/100)%10+0x30);
              }
              
              ss[i++]=((StateC.Value[j]/10)%10+0x30);
              ss[i++]='.';
              ss[i++]=(StateC.Value[j]%10+0x30);
              
              //�Ƿ񳬱�............................................................
              if(Alarm==SMS_ALARM)
              {
                ss[i++]='(';
                ss[i++]='A';
                ss[i++]='L';
                ss[i++]=')';
              }
            }
            ss[i++]=(0x0d);
            ss[i++]=(0x0a);
      }
   }
    //�����Ϣ״��
    #if GSM_BATT_INFOR_EN==1
    if(Alarm==SMS_LOOK)
    {
        i=i+GetBattTxt(&ss[i],0);
    }
    #endif 
   
   return i;
}



void SmsSendPhone(u8* phone)
{
  u16 j;
  u8 ppbuf[15];
  u8 pplen;
  
  pplen=GetInternationalPhone(phone,&ppbuf[0]);

  
  if(1==Flag.SeeComing)
  {
    ComSendH();
    U1SendString("AT+CMGS=");
    
    U1SendByte('"');
    for(j=0;j<pplen;j++)
    {
      U1SendByte(ppbuf[j]);
    }
    U1SendByte('"');
    U1SendString("\r\n");
    ComSendL();
  }
  
  U0SendString("AT+CMGS=");
  
  U0SendByte('"');
  
  for(j=0;j<pplen;j++)
  {
    U0SendByte(ppbuf[j]);
  }
  U0SendByte('"');
  U0SendString("\r\n");
}


void U0SmsLen(u16 Len)
{
    u8 Lenx,Leny,Lenz;
    Lenz=(Len/100)%100+0x30;
    Lenx=(Len/10)%10+0x30;
    Leny=Len%10+0x30;

    U0SendString("AT+CMGS=");
    U0SendByte(Lenz);
    U0SendByte(Lenx);
    U0SendByte(Leny);
    U0SendString("\r\n");
}
//���ֽ�phone��ȡ ��ȷ�ĺ����λ��
u8 GetInternationalPhone(u8* Phone,u8* out)
{
  u8 j;
  u8 len=0;
  for(j=0;j<15;j++)
  {
    if((Phone[j]>=0x30)&&(Phone[j]<=0x39))
    {
      out[len++]=Phone[j];
      if(!((Phone[j+1]>=0x30)&&(Phone[j+1]<=0x39)))
      {
        return len;
        //����
      }
    }
  }
  return 0;//���ݴ���
}
void U0SmsPdu(u8* Phone,u8* BufUni,u8 LenUni)
{
  u16 j;
  u8 LenA[3];
  u8 ppbuf[15];
  u8 pplen;
  
  U0SendString("0011000D91");
  
  //Ŀ�ĺ���-------------------------------
  U0SendString("68");//ֻ����й��ֻ�����
  pplen=GetInternationalPhone(Phone,&ppbuf[0]);
  
  for(j=0;j<pplen/2;j++)
  {
    U0SendByte(ppbuf[2*j+1]);
    U0SendByte(ppbuf[2*j]);
  }
  if((pplen%2)==1)//����ż��
  {
    U0SendByte('F');//��һ��F ��ż��
    U0SendByte(ppbuf[pplen-1]);
  }
  //------------------------------------------------
  WDT_CLR;
  U0SendString("000800");
  LenA[0]=LenUni;
  hex_2_ascii(&LenA[0],&LenA[1],1);
  U0SendByte(LenA[1]);
  U0SendByte(LenA[2]);
  for(j=0;j<LenUni;j++)
  {
    WDT_CLR;
    LenA[0]=BufUni[j];
    hex_2_ascii(&LenA[0],&LenA[1],1);
    U0SendByte(LenA[1]);
    U0SendByte(LenA[2]);
  }
  U0SendByte(0x1A); 
}

u16 U0PduAnalyze(u8 *phone,u8 *BufUni)
{
    u16 Pos;
    u16 i,j;
    u8 code;
    u8 LenP[1];
    
    Pos=GetPosC(Uart_0.RxBuf,36,60,'F');
    if(Pos==255)return 0;
    i=0;
    for(j=5;j>0;j--)
    {
       phone[i++]=Uart_0.RxBuf[Pos-j*2+1];//���ͷ��ֻ�����
       phone[i++]=Uart_0.RxBuf[Pos-j*2];
    }
    phone[i]=Uart_0.RxBuf[Pos+1];
    WDT_CLR;
    if(Uart_0.RxBuf[Pos+4]=='0'&&Uart_0.RxBuf[Pos+5]=='8')
      code=0x08;//UCS2
    else if(Uart_0.RxBuf[Pos+4]=='0'&&Uart_0.RxBuf[Pos+5]=='0')
      code=0x00;//7bit
    else if(Uart_0.RxBuf[Pos+4]=='0'&&Uart_0.RxBuf[Pos+5]=='4')
      code=0x04;//8bit
    if(code==0x08)//ucs2
    {
      ascii_2_hex(&Uart_0.RxBuf[Pos+20],LenP,2);//UCS2����
      i=0;
      for(j=Pos+22;j<(Pos+22+LenP[0]*2);j++)
      {
        WDT_CLR;
        ascii_2_hex(&Uart_0.RxBuf[j],&BufUni[i],2);//ucs2����
        i++;
        j++;
      }
    }
    else if(code==0x00)//7bit
    {
      ascii_2_hex(&Uart_0.RxBuf[Pos+20],LenP,2);//UCS2����
      i=0;
      for(j=Pos+22;j<(Pos+22+LenP[0]*2);j++)
      {
        WDT_CLR;
        ascii_2_hex(&Uart_0.RxBuf[j],&Uart_1.RxBuf[i],2);//ucs2����
        i++;
        j++;
      }
      LenP[0]=decode(Uart_1.RxBuf,Uart_0.RxBuf,i);//7bitԭʼ���� ת��Ϊ ASCII
      i=0;
      for(j=0;j<LenP[0];j++)//����ת��ΪUCS2��ʽ����
      {
          BufUni[i++]=0x00;
          BufUni[i++]=Uart_0.RxBuf[j];
          WDT_CLR;
      }
      LenP[0]=i;//7BIT ASIC��ʽת��ΪUCS2��ʽ���ݺ�ĳ���
    }
    else if(code==0x04)//8bit
    {
      NOP;
    }
    return LenP[0];
}
//SIM800C  SENG
/*
+CENG: 1,1

+CENG: 0,"0011,64,00,460,00,62,2b5b,10,05,58b2,255"
+CENG: 1,"0082,41,49,62e8,460,00,58b2"
+CENG: 2,"0086,33,00,62e6,460,00,58b2"
+CENG: 3,"0570,23,30,62f2,460,00,58b2"
+CENG: 4,"0593,1312,8a02,460,00,58b2"
+CENG: 5,"0080,26,52,ffff,,,0000"
+CENG: 6,"0070,26,07,ffff,,,0000"

OK
*/

#if LBS_LAC_EN==1
u8 Get_LAC_CID(void)
{
  u8 buf[2],cc[2];
  u16 a1,a2;
  
  a1=GetXpt(Uart_0.RxBuf,0,Uart_0.RecLen,',',11);
  if(a1==10000)return 0;
  
  a2=GetXpt(Uart_0.RxBuf,0,Uart_0.RecLen,',',8);
  if(a2==10000)return 0;
  
  if(ascii_2_hex(&Uart_0.RxBuf[a1+1],buf,4)==(-1))
    return 0;
  
  if(ascii_2_hex(&Uart_0.RxBuf[a2+1],cc,4)==(-1))
    return 0;
  
  Sms.LAC=buf[0];
  Sms.LAC=(Sms.LAC<<8)|buf[1];

  Sms.CID=cc[0];
  Sms.CID=(Sms.CID<<8)|cc[1];
  
  StateC.LBSpt=Pt.GprsSendTime*3+60;//3���ϴ����û���»��Զ���0
  

  if(1==Flag.SeeComing)
  {
    ComSendH();
    U1SendString("\r\nLAC=");
    U1SendValue(Sms.LAC);
    U1SendString("\r\nCID=");
    U1SendValue(Sms.CID);
    U1SendString("\r\n");
    ComSendL();
  }
  
  return 1;
}

#endif

//����ת��..........................................................................
//1���ַ�
u8 GetPos2C(u8* BufIn,u8 StartPt,u8 EndPt,u8 CC,u8* out)
{
  u8 i,j;

  i=0;
  for(j=StartPt;j<EndPt;j++)
  {
    WDT_CLR;
    if(BufIn[j]==CC)
    {
      out[i++]=j;
      if(i==2)
      {
        return 1;
      }
    }

  }
  return 0;
}
//1���ַ�
u8 GetPosC(u8* BufIn,u8 StartPt,u8 EndPt,u8 CC)
{
  u8 j;
  for(j=StartPt;j<EndPt;j++)
  {
    WDT_CLR;
    if(BufIn[j]==CC)
      return j;
  }
  return 255;
}



u16 StrLen(u8* str)
{
  u16 len=0;
  while(*str)
  {
    str++;
	  len++;
  }
  return len;
}
/***************************************** 
ASCII �� HEX ��ת������ 
��ڲ����� O_data: ת�����ݵ����ָ�룬 
N_data: ת���������ݵ����ָ�� 
len : ��Ҫת���ĳ��� 
���ز�����-1: ת��ʧ�� 
������ת�������ݳ��� 
ע�⣺O_data[]�����е�������ת�������лᱻ�޸ġ� */
int ascii_2_hex(unsigned char *O_data, unsigned char *N_data, int len) 
{ 
        int i,j,tmp_len; 
        unsigned char tmpData; 
        unsigned char *O_buf = O_data; 
        unsigned char *N_buf = N_data; 
        for(i = 0; i < len; i++) 
        { 
            WDT_CLR;
            if ((O_buf[i] >= '0') && (O_buf[i] <= '9')) 
            { 
              tmpData = O_buf[i] - '0'; 
            } 
            else if ((O_buf[i] >= 'A') && (O_buf[i] <= 'F')) //A....F 
            {
              tmpData = O_buf[i] - 0x37; 
            }
            else if((O_buf[i] >= 'a') && (O_buf[i] <= 'f')) //a....f 
            {
              tmpData = O_buf[i] - 0x57; 
            }
            else 
            {
               return -1; 
             }
             O_buf[i] = tmpData; 
        }
        for(tmp_len = 0,j = 0; j < i; j+=2) 
        { 
            N_buf[tmp_len++] = (O_buf[j]<<4) | O_buf[j+1]; 
        } 
        return tmp_len;
}
/***************************************** 
HEX �� ASCII ��ת������ 
��ڲ����� data: ת�����ݵ����ָ�� 
buffer: ת�����������ָ�� 
len : ��Ҫת���ĳ��� 
���ز�����ת�������ݳ��� */
int hex_2_ascii(unsigned char *data, unsigned char *buffer, int len) 
{ 
        int i, pos; 
        const unsigned char ascTable[17] = {"0123456789ABCDEF"}; 
        unsigned char *tmp_p = buffer; 
        pos = 0; 
        for(i = 0; i < len; i++) 
        { 
          WDT_CLR;
           tmp_p[pos++] = ascTable[data[i] >> 4]; 
           tmp_p[pos++] = ascTable[data[i] & 0x0f]; 
        } 
        tmp_p[pos] = '\0'; 
        return pos; 
}
//------------------7BIT ASIC ��λת��ΪASCII--------------------------
unsigned int decode(unsigned char in[],unsigned char out[],unsigned int nLen)
{
    unsigned int nOrigin = 0;
    unsigned int nCode = 0;
    unsigned int nLenTemp;
    nLenTemp = nLen * 8 / 7;
    while(1)
    {
        WDT_CLR;

    	if(nOrigin >= nLen) break;
    	out[nCode] = (unsigned char)(in[nOrigin] & 0x7f);
    	
    	if((nOrigin + 1) >= nLen) break;
    	out[nCode + 1] = (unsigned char)((in[nOrigin + 1] & 0x3f) << 1);
    	out[nCode + 1] += (unsigned char)((in[nOrigin] & 0x80) >> 7);
    	
    	if((nOrigin + 2) >= nLen) break;
    	out[nCode + 2] = (unsigned char)((in[nOrigin + 2] & 0x1f) << 2);
    	out[nCode + 2] += (unsigned char)((in[nOrigin + 1] & 0xc0) >> 6);
    	
    	if((nOrigin + 3) >= nLen) break;
    	out[nCode + 3] = (unsigned char)((in[nOrigin + 3] & 0x0f) << 3);
    	out[nCode + 3] += (unsigned char)((in[nOrigin + 2] & 0xe0) >> 5);
    	
    	if((nOrigin + 4) >= nLen) break;
    	out[nCode + 4] = (unsigned char)((in[nOrigin + 4] & 0x07) << 4);
    	out[nCode + 4] += (unsigned char)((in[nOrigin + 3] & 0xf0) >> 4);
    	
    	if((nOrigin + 5) >= nLen) break;
    	out[nCode + 5] = (unsigned char)((in[nOrigin + 5] & 0x03) << 5);
    	out[nCode + 5] += (unsigned char)((in[nOrigin + 4] & 0xf8) >> 3);
    	
    	if((nOrigin + 6) >= nLen) break;
    	out[nCode + 6] = (unsigned char)((in[nOrigin + 6] & 0x01) << 6);
    	out[nCode + 6] += (unsigned char)((in[nOrigin + 5] & 0xfc) >> 2);
    	            	            	
    	out[nCode + 7] = (unsigned char)((in[nOrigin + 6] & 0xfe) >> 1);
    	
    	nCode += 8;
    	nOrigin += 7;
    }
    return nLenTemp;
}

void DogClear(void)
{
   Pt.IniStartTime=0;
   Pt.IniStepDog=0;
   Pt.IniLongDog=0;
   
   Pt.SmsTime=0;
   Pt.SmsTErrCt=0;
   Pt.SmsSendStepDog=0;
   Pt.SmsSendLongDog=0;
   
   Gsm.ReadCsqDog=0;

   Pt.IniIngStep==INI_END;
   Pt.SmsIngStep==SMS_END;
   
   #if GPRS_ENABLE==1
   Pt.GprsStepDog=0;
   Pt.GprsLongDog=0;
   #endif
   

}
//������������ģʽ=======================================================================
#if SMS_INV_ENABLE==1
//��һ���е����е㣬ת��Ϊһ��uni��
u16 GsmDayPotsToUni(u8* Buf,u8* UniBf)
{
  //�ȷ������е��ʱ���Ƿ�Ϊͬһ��,����ͬ�죬ֻȡ���ĵ�
    u16 i;
    u16 j;
    u16 k;
    u16 VV;
    u8 ipt;
    u8 FH;
    u8 SameDayN=1;//ͬһ��ĵ���,��1��ʼ

    StrcutRtc Rtime[SMS_DAYPOT_MAX];
    
    //ȡ�������ʱ��
    for(j=0;j<SMS_DAYPOT_MAX;j++)
    {
      Rtime[j].SS=U8_TO_U32(Buf[j*ONEPOT_BYTES+3],Buf[j*ONEPOT_BYTES+2],Buf[j*ONEPOT_BYTES+1],Buf[j*ONEPOT_BYTES]);
      SecondsToDate(Rtime[j].SS,&Rtime[j]);
      NOP;
    }
    //�ж������һ����ʱ����ͬ��
    for(j=0;j<SMS_DAYPOT_MAX-1;j++)
    {
       //�Ƚ������Ƿ���ͬ
       if((Rtime[SMS_DAYPOT_MAX-1].Year==Rtime[SMS_DAYPOT_MAX-2-j].Year)
          &&(Rtime[SMS_DAYPOT_MAX-1].Month==Rtime[SMS_DAYPOT_MAX-2-j].Month)
          &&(Rtime[SMS_DAYPOT_MAX-1].Day==Rtime[SMS_DAYPOT_MAX-2-j].Day))
       {
         SameDayN++;
       }
       else
       {
         break;//��ͬ�������˳�
       }
    }
    
    /*******************************************************
    ��ʽ��
    120429
    02:-12.2,-10,4
    08:-12.2,-10,4
    14:-12.2,-10,4
    20:-12.2,-10,4
    **********************************************************/
    i=0;
    //�����գ�ȡ�����
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime[SMS_DAYPOT_MAX-1].Year/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime[SMS_DAYPOT_MAX-1].Year%10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime[SMS_DAYPOT_MAX-1].Month/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime[SMS_DAYPOT_MAX-1].Month%10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime[SMS_DAYPOT_MAX-1].Day/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime[SMS_DAYPOT_MAX-1].Day%10+0x30);
    
    UniBf[i++]=(0x00);
    UniBf[i++]=(0x0a);//����
    
    ipt=SMS_DAYPOT_MAX-SameDayN;//���ĸ��㿪ʼ
    for(k=0;k<SameDayN;k++)
    {
      //ʱ
      UniBf[i++]=(0x00);
      UniBf[i++]=(Rtime[ipt+k].Hour/10+0x30);
      UniBf[i++]=(0x00);
      UniBf[i++]=(Rtime[ipt+k].Hour%10+0x30);
      
      UniBf[i++]=(0x00);
      UniBf[i++]=(0x3a);//:
      
      for(j=0;j<CH_NUM;j++)
      {
            VV=U8_TO_U16(Buf[(k+ipt)*ONEPOT_BYTES+5+j*2],Buf[(k+ipt)*ONEPOT_BYTES+4+j*2]);
            
            if(VV==SENIOR_NULL)
            {
              UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
              UniBf[i++]=(0x20);
              UniBf[i++]=(0x00);//null
              UniBf[i++]=(0x6e);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x75);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x6c);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x6c);
            }
            else if(VV==HAND_STOP)
            {
              UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
              UniBf[i++]=(0x20);
              UniBf[i++]=(0x00);//hand
              UniBf[i++]=(0x68);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x61);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x6e);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x64);
            }
            else//������ֵ
            {
              if((VV&0x8000)==0x8000)
              {
                FH=1;//����
              }
              else
              {
                FH=0;//����
              }
              VV=VV&0x7fff;
              
              if(FH)
              {
                 UniBf[i++]=(0x00);//-
                 UniBf[i++]=(0x2d);
                  if(VV>99)
                  {
                    UniBf[i++]=(0x00);
                    UniBf[i++]=((VV/100)%10+0x30);
                  }
                  else
                  {
                    UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
                    UniBf[i++]=(0x20);
                  }
              }
              else
              {
                  if(VV>999)
                  {
                    UniBf[i++]=(0x00);
                    UniBf[i++]=((VV/1000)%10+0x30);
                    UniBf[i++]=(0x00);
                    UniBf[i++]=((VV/100)%10+0x30);
                  }
                  else
                  {
                    UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
                    UniBf[i++]=(0x20);
                    if(VV>99)
                    {
                      UniBf[i++]=(0x00);
                      UniBf[i++]=((VV/100)%10+0x30);
                    }
                    else
                    {
                      UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
                      UniBf[i++]=(0x20);
                    }
                  }
              }
              
              UniBf[i++]=(0x00);
              UniBf[i++]=((VV/10)%10+0x30);
              UniBf[i++]=(0x00);//.
              UniBf[i++]=(0x2e);
              UniBf[i++]=(0x00);
              UniBf[i++]=(VV%10+0x30);
            }
            
            if(j==CH_NUM-1)//���һ��ͨ��
            {
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x0a);//����
            }
            else
            {
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x2c);//,
            }
      }//ͨ��ѭ��
    }//����ѭ��
    
    return i;
 
}

//�Ѵ洢������ת��ΪUni--------------------------------------------------------
u16 GsmRecToUni(u8* Buf,u8* UniBf)
{
    u8 i=0;
    u8 j;
    u8 FH;
    u16 VV;
    
    //ʱ��
    StrcutRtc Rtime;
    Rtime.SS=U8_TO_U32(Buf[3],Buf[2],Buf[1],Buf[0]);
    SecondsToDate(Rtime.SS,&Rtime);

    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Year/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Year%10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Month/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Month%10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Day/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Day%10+0x30);
    
    UniBf[i++]=(0x00);
    UniBf[i++]=(0x20);
    
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Hour/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Hour%10+0x30);
    
    UniBf[i++]=(0x00);
    UniBf[i++]=(0x3a);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Minute/10+0x30);
    UniBf[i++]=(0x00);
    UniBf[i++]=(Rtime.Minute%10+0x30);
    
    UniBf[i++]=(0x00);
    UniBf[i++]=(0x0a);//����
    
    
      for(j=0;j<CH_NUM;j++)
      {
              #if SMS_DAY_POT_EN==0
              u8 ChT;//ͨ������
              ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//��ȡ��ͨ������

              //ͨ����.......................................................
               #if (CH_PAIR_ENABLE==1)//���߽��ջ�û�гɶ�ģʽ
                   UniBf[i++]=(0x00);
                   UniBf[i++]=(1+j/2+0x30);//�ɶԵ����
               #else
                   UniBf[i++]=(0x00);
                   UniBf[i++]=(1+j+0x30);
               #endif
                
              //ͨ������......................................................
              if(ChT==_T_T)//�¶�
              {
                UniBf[i++]=(0x6E);
                UniBf[i++]=(0x29);
              }
              else if(ChT==_T_H)//ʪ��
              {
                UniBf[i++]=(0x6E);
                UniBf[i++]=(0x7F);
              }
              #endif
              
            VV=U8_TO_U16(Buf[5+j*2],Buf[4+j*2]);
            
            if(VV==SENIOR_NULL)
            {
              UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
              UniBf[i++]=(0x20);
              UniBf[i++]=(0x00);//null
              UniBf[i++]=(0x6e);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x75);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x6c);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x6c);
            }
            else if(VV==HAND_STOP)
            {
              UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
              UniBf[i++]=(0x20);
              UniBf[i++]=(0x00);//hand
              UniBf[i++]=(0x68);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x61);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x6e);
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x64);
            }
            else//������ֵ
            {
              if((VV&0x8000)==0x8000)
              {
                FH=1;//����
              }
              else
              {
                FH=0;//����
              }
              VV=VV&0x7fff;
              
              if(FH)
              {
                 UniBf[i++]=(0x00);//-
                 UniBf[i++]=(0x2d);
                  if(VV>99)
                  {
                    UniBf[i++]=(0x00);
                    UniBf[i++]=((VV/100)%10+0x30);
                  }
                  else
                  {
                    UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
                    UniBf[i++]=(0x20);
                  }
              }
              else
              {
                  if(VV>999)
                  {
                    UniBf[i++]=(0x00);
                    UniBf[i++]=((VV/1000)%10+0x30);
                    UniBf[i++]=(0x00);
                    UniBf[i++]=((VV/100)%10+0x30);
                  }
                  else
                  {
                    UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
                    UniBf[i++]=(0x20);
                    if(VV>99)
                    {
                      UniBf[i++]=(0x00);
                      UniBf[i++]=((VV/100)%10+0x30);
                    }
                    else
                    {
                      UniBf[i++]=(0x00);//�ո�ȷ��5���ַ�
                      UniBf[i++]=(0x20);
                    }
                  }
              }
              
              UniBf[i++]=(0x00);
              UniBf[i++]=((VV/10)%10+0x30);
              UniBf[i++]=(0x00);//.
              UniBf[i++]=(0x2e);
              UniBf[i++]=(0x00);
              UniBf[i++]=(VV%10+0x30);
            }
            
            if((((j+1)%2)==0)||(j==CH_NUM-1))//ÿ2��ͨ�� �� ���һ��ͨ�� ����
            {
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x0a);//����
            }
            else
            {
              #if SMS_DAY_POT_EN==0
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x0a);//����
              #else
              UniBf[i++]=(0x00);
              UniBf[i++]=(0x2c);//,
              #endif
            }
      }//ͨ��ѭ��
      return i;
}
//------------------------------------------------------------------------------
//������ʼ��
void GsmInvInit(void)
{
    GsmInvRead();//��ȡ����ʱ����
    
    #if SMS_DAY_POT_EN==1
    if(Sms.InvTimePt>=DAY_SS)//���ڴ���1��
      Sms.InvOneBytes=((u16)4+StateC.RecordBytes)*SMS_DAYPOT_MAX;
    else
      Sms.InvOneBytes=(u16)4+StateC.RecordBytes;
    #else
    Sms.InvOneBytes=(u16)4+StateC.RecordBytes;
    #endif
    
    Sms.InvRecMax=(u16)EE_SMS_SIZE/Sms.InvOneBytes;//���洢����
    
    Sms.InvRecPt=0;
    Sms.InvRecNum=0;

    ClrBuf(Sms.SmsPotBuf,DAYPOT_MAX);
}

//-------------------------------------------------------------------------------

void GsmInvRecord(u32 SS,u8 ipt)
{
  #if SMS_RECORD_ENABLE==1//----------------------------------------------------------
      #if SMS_DAY_POT_EN==1
      //�洢��Ҫ���͵�һ���еĵ�ipt������ݣ�һ���������,���һ�����ȡ���ʱ���Ŵ洢��EE
      if(Sms.InvTimePt>=DAY_SS)
      {
        GetOnePotData(&Sms.SmsPotBuf[ONEPOT_BYTES*ipt],SS);
        if(ipt==SMS_DAYPOT_MAX-1)//һ�������һ����ʱ����ʼ�洢���е����ݵ�EE
        {
          //��¼+1
          InvRecordAcc(Sms.SmsPotBuf,ONEPOT_BYTES*SMS_DAYPOT_MAX);//1����¼��SMS_DAYPOT_MAX����
        }
      }
      else
      {
        u16 Len;
        u8 buf[ONEPOT_BYTES];
        //ȡһ�����ݣ�����ʱ�������ͨ������
        Len=GetOnePotData(buf,SS);
        //��¼+1
        InvRecordAcc(buf,Len);
      }
      #else
        //�洢��Ҫ���͵����ݣ�һ��һ������
        u16 Len;
        u8 buf[ONEPOT_BYTES];
        //ȡһ�����ݣ�����ʱ�������ͨ������
        Len=GetOnePotData(buf,SS);
        //��¼+1
        InvRecordAcc(buf,Len);
      #endif
  #else//-----------------------------------------------------------------------------
     #if SMS_DAY_POT_EN==1
     if(Sms.InvTimePt>=DAY_SS)
     {
       GetOnePotData(&Sms.SmsPotBuf[ipt*ONEPOT_BYTES],SS);
       if(ipt==SMS_DAYPOT_MAX-1)//һ�������һ����ʱ����ʼ�洢���е����ݵ�EE
       {
         Sms.InvRecNum=1;
         Sms.SmsErr=0;
       }
     }
     else
     {
       GetOnePotData(Sms.SmsPotBuf,SS);
       Sms.InvRecNum=1;
       Sms.SmsErr=0;
     }
     #else
       GetOnePotData(Sms.SmsPotBuf,SS);
       Sms.InvRecNum=1;
       Sms.SmsErr=0;
     #endif
        
  #endif
    
}

//----------------------------------------------------------------------------------
//��¼+1
void InvRecordAcc(u8* buf,u16 Len)
{
      u16 EE_16Pt;
      EE_16Pt=EE_MEM_ADDR+Sms.InvRecPt*Len;
      
      //��¼ָ��
      I2C_EE_BufferWrite(buf,EE_16Pt,Len,0);


      Sms.InvRecPt++;  
      if(Sms.InvRecPt>=Sms.InvRecMax)//��¼���
        Sms.InvRecPt=0;
      //��¼����
      if(Sms.InvRecNum<Sms.InvRecMax)
        Sms.InvRecNum++;
      
      #if SMS_BATONLY_ENABLE==1
      Sms.SmsErr=0;
      #endif

}
//--------------------------------------------------------------------------------      
//һ�����ݣ�SSΪ�豣���ʱ�䣬BufAΪ��ȡ��ͨ������
u16 GetOnePotData(u8* BufA,u32 SS)
{
  u8 Len,j;
  
  Len=0;
  //��ǰʱ��
  BufA[Len++]=(0xff&SS);
  BufA[Len++]=(0xff&(SS>>8));
  BufA[Len++]=(0xff&(SS>>16));
  BufA[Len++]=(0xff&(SS>>24));
  //��ǰ����
  for(j=0;j<CH_NUM;j++)
  {
    if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))//3.4
    {
      BufA[Len++]=0xff&StateC.Value[j];//LSB8
      BufA[Len++]=0xff&(StateC.Value[j]>>8);//HSB8
    }
    else
    {
      BufA[Len++]=0xff&StateC.Value[j];//LSB8
      if(0x01&(StateC.FuhaoBit>>j))//����HSB8
      {
        BufA[Len]=0xff&(StateC.Value[j]>>8);
        BufA[Len]|=0x80;
      }
      else//����HSB8
      {
        BufA[Len]=0x7f&(StateC.Value[j]>>8);
      }
      Len++;
    }
  }
  return Len;
}
//-------------------------------------------------------------------------------

//ȡ���룬������󷵻�1��������ȷ����0
u8 GetPhoneError(u8 PhonePt)
{
  ClrBuf(Gsm.SmsRxPhone,15);
  GetPhoneHaoFromEE(Gsm.SmsRxPhone,PhonePt);//ȡ����
  
  if(PhoneIsRight())//�ֻ�������ȷ
    return 0;
  else
    return 1;
}
//------------------------------------------------------------------------------
//׼�����ŷ��͵����ݣ�
u8 SmsInvReady(u8 PhonePt)
{
  if(GetPhoneError(PhonePt))
    return 0;

  #if SMS_RECORD_ENABLE==1//----------------------------------------------
      u8 buf[DAYPOT_MAX];
      u16 EE_16Pt;
      u16 OnePt;
  
      //���һ��ָ��
      OnePt=Sms.InvRecPt;
      for(u16 j=0;j<Sms.InvRecMax-Sms.InvRecNum;j++)
      {
        OnePt++;
        if(OnePt==Sms.InvRecMax)
        {
          OnePt=0;
        }
      }
      
      //����һ������
      EE_16Pt=EE_MEM_ADDR+OnePt*Sms.InvOneBytes;
      I2C_EE_BufferRead(buf,EE_16Pt,Sms.InvOneBytes,0);
      NOP;
      
      #if SMS_DAY_POT_EN==1
      if(Sms.InvTimePt>=DAY_SS)//���ڴ���1��
        Gsm.SmsUniLen=GsmDayPotsToUni(buf,Gsm.SmsRecUni);//ת��Ϊuni
      else
        Gsm.SmsUniLen=GsmRecToUni(buf,Gsm.SmsRecUni);
      #else
      Gsm.SmsUniLen=GsmRecToUni(buf,Gsm.SmsRecUni);//ת��Ϊuni
      #endif
  #else//----------------------------------------------------------------------
      #if SMS_DAY_POT_EN==1
      if(Sms.InvTimePt>=DAY_SS)//���ڴ���1��
        Gsm.SmsUniLen=GsmDayPotsToUni(Sms.SmsPotBuf,Gsm.SmsRecUni);//ת��Ϊuni
      else
        Gsm.SmsUniLen=GsmRecToUni(Sms.SmsPotBuf,Gsm.SmsRecUni);
      #else
      Gsm.SmsUniLen=GsmRecToUni(Sms.SmsPotBuf,Gsm.SmsRecUni);//ת��Ϊuni
      #endif
  #endif//---------------------------------------------------------------------
  
  return 1;

}
//------------------------------------------------------------------------------
//���ͳɹ�����
void GsmInvSendOk(void)
{
  if(Sms.InvRecNum>0)//��������
  {
    Sms.InvRecNum--;
  }
}
//-------------------------------------------------------------------------------
//��ȡ���Ͳ��������ͼ��+ʱ���
void GsmInvRead(void)
{
  u8 Buf[8];
  
  I2C_EE_BufferRead(Buf,INV_TIME_ADDR,8,0);
  Sms.InvTimePt=U8_TO_U32(Buf[3],Buf[2],Buf[1],Buf[0]);
  Sms.InvPointPt=U8_TO_U32(Buf[7],Buf[6],Buf[5],Buf[4]);

}
//------------------------------------------------------------------------------
//���÷��Ͳ��������ͼ��+ʱ��㣨���ͼ������1��ʱ��ʱ�����Ч��
void GsmInvSet(u32* bufin)
{
  u8 Buf[8];
  
  Buf[0]=0xff&bufin[0];
  Buf[1]=0xff&(bufin[0]>>8);
  Buf[2]=0xff&(bufin[0]>>16);
  Buf[3]=0xff&(bufin[0]>>24);
  
  Buf[4]=0xff&bufin[1];
  Buf[5]=0xff&(bufin[1]>>8);
  Buf[6]=0xff&(bufin[1]>>16);
  Buf[7]=0xff&(bufin[1]>>24);
  
  I2C_EE_BufferWrite(Buf,INV_TIME_ADDR,8,0);
  delay_us(200);

  GsmInvInit();
}

//--------------------------------------------------------------------------------
void GsmInvDeel(void)
{
  vu16 j;
  u32 TT;
  
  
  if(Sms.InvWaitPt>0)
  {
    Sms.InvWaitPt--;
  }
  
  if(Sms.InvTimePt==0)//���ʱ��Ϊ0ʱ���ζ�ʱ����
  {
    return;
  }
  
  if(StateA2.WorkState==WORK_STATE_STOP)//��¼ͣ��ʱ������
  {
    return;
  }
  
  //��һ��RTC ��� READ_RTC_TIME
  if(++Sms.InvRtcPt>READ_RTC_TIME)
  {
    Sms.InvRtcPt=0;
  }
  else
  {
    return;
  }
  
  
  TT=Rtc.SS=ReadRtcD10();
  

  
  #if SMS_DAY_POT_EN==1//����һ�������ݴ������
  u32 DD[SMS_DAYPOT_MAX];//1������
  if(Sms.InvTimePt>=DAY_SS)//���ڴ���1��  
  {
    DD[0]=Rtc.SS-(u32)HOUR_SS*2;//2��
    DD[1]=Rtc.SS-(u32)HOUR_SS*8;//8��
    DD[2]=Rtc.SS-(u32)HOUR_SS*14;//14��
    DD[3]=Rtc.SS-(u32)HOUR_SS*20;//20��
    
    for(j=0;j<SMS_DAYPOT_MAX;j++)
    {
      if(((DD[j]%Sms.InvTimePt)>0)&&((DD[j]%Sms.InvTimePt)<READ_RTC_TIME*2))//�洢ʱ�䵽
      {
        if(Sms.InvWaitPt==0)//һ��ѭ���У�ֻ�ܽ�һ��
        {
          Sms.InvWaitPt=READ_RTC_TIME*3;
          GsmInvRecord(Rtc.SS,j);//��¼
          break;//����һ���㣬ʣ�µĵ㲻��ɨ�裬�ȵ���һ��
        }
      }
    }
  }
  else//С��һ������
  {
    if(((TT%Sms.InvTimePt)>0)&&((TT%Sms.InvTimePt)<READ_RTC_TIME*2))//�洢ʱ�䵽
    {
      if(Sms.InvWaitPt==0)//һ��ѭ���У���0��1��ֻ�ܽ�һ��
      {
        Sms.InvWaitPt=READ_RTC_TIME*3;
        GsmInvRecord(Rtc.SS,0);//��¼
      }
    }
  }
  

  #else
  
  if(Sms.InvTimePt>=(DAY_SS/2))//���ڴ���12Сʱ
  {
    TT=Rtc.SS-Sms.InvPointPt;//����ʱ���
  }
  if(((TT%Sms.InvTimePt)>0)&&((TT%Sms.InvTimePt)<READ_RTC_TIME*2))//�洢ʱ�䵽
  {
    if(Sms.InvWaitPt==0)//һ��ѭ���У�ֻ�ܽ�һ��
    {
      Sms.InvWaitPt=READ_RTC_TIME*3;
      GsmInvRecord(Rtc.SS,0);//��¼
    }
  }
  #endif
  
  #if SMS_BATONLY_ENABLE==1
  if(Sms.GsmOff==0)//����״̬
  #endif
  {
    if(Pt.SmsDueOut==0)//���ŷ��Ϳ���
    {
      if(GsmIsFree(0))//ģ�����
      {
        if(Sms.InvRecNum>0)//�ж����跢��
        {
          if(SmsInvReady(0))//��ȡ���ݣ�ֻ���͵���һ������
          {
            Pt.SmsDueOut=1;
          }
        }
      }
    }
  }
}


//ģ�鿪�ػ�����,ֻ��ֻ�е�ع�����Ч
void GsmInvShutDeel(void)
{
  //�ػ�
  if(Sms.GsmOff==0)//��һ״̬�����ǿ���
  {
    if((Sms.InvRecNum==0)&&(Pt.SmsDueOut==0)&&(GsmIsFree(0)))//û�ж���Ҫ����
    {
      GsmToPowOff();
    }
  }
  
  //����
  if(Sms.GsmOff==1)//��һ״̬�����ǹػ�
  {
    if(Sms.SmsErr==1)//��һ�ζ��ŷ���ʧ�ܣ�˵��ģ����ź������⣬�´ζ�ʱʱ�䵽�ٷ���
      return;
    
    if(Sms.InvRecNum>0)//������Ҫ����
    {
      if(GetPhoneError(0))//�������
        return;

      GsmToPowOn();

    }
  }
  
}
#endif
//GSM�ػ�
void GsmAT2Off(void)
{
  #if SIM800C_EN==1
    #if M26_EN==1
    U0SendString("AT+QPOWD=1\r\n");
    #else
    U0SendString("AT+CPOWD=1\r\n");
    #endif
  #else
    //U0SendString("AT%MSO\r\n");//EM310
    U0SendString("AT^SMSO\r\n");//MG323,MG301
  #endif

}

void GsmToPowOn(void)
{
   UART_U0_TXD_SET;
   
   #if SMS_BATONLY_ENABLE==1
   GSMPOW_ON;
   #endif

   DogClear();
   Sms.GsmOff=0;
   
   #if GPRS_TC_EN==1
   Pt.ExitClosePt=0;
   Pt.ToOff=0;//����ʱ������0
   #endif
   
   ErrorToRst();//����
}

#if L206_SLP_EN==1
void GsmWakeUp(void)//����
{
   UART_U0_TXD_SET;
   
   DogClear();
   Sms.GsmOff=0;
   
   #if GPRS_TC_EN==1
   Pt.ExitClosePt=0;
   Pt.ToOff=0;//����ʱ������0
   #endif
  
  DTR_H;
  delay_ms(500);
  WDT_CLR;
  DTR_L;
  
  delay_ms(200);
  U1SendStrPC("AT\r\n");
  U0SendString("AT\r\n");
}
void GsmSlp(void)
{
  Sms.GsmOff=1;
  LED_OFF;
        #if LCD_NO==20
        Lcd20Up(0);
        #endif
  
  DogClear();
  
  U1SendStrPC("AT+ESLP=1\r\n");
  U0SendString("AT+ESLP=1\r\n");//����
  
  DTR_L;
  delay_ms(500);
  WDT_CLR;
  DTR_H;
  
  U1SendStrPC("AT+ESLP=1\r\n");
  U0SendString("AT+ESLP=1\r\n");//����
  
  delay_ms(200);
  
  UART_U0_TXD_CLR;
  

  #if GPRS_UP_TIME_EN==1
  Pt.Gprsing=0;//����ʡ��ģʽ���´������ϴ�������Ҫ���´�ʱ�䵽
  #endif
}
#endif 


void GsmToPowOff(void)
{
#if  GPRS_ENABLE==1
  Pt.GprsRstErrPt=0;
#endif
  
  U1SendStrPC("GPRS SLP\r\n");
  
  Sms.GsmOff=1;
  LED_OFF;
  
    //    #if LCD_NO==20
    //    Lcd20Up(0);
    //    #endif
  
  DogClear();
  
  
  delay_ms(200);
  WDT_CLR;
  GsmAT2Off();
  delay_ms(300);
  WDT_CLR;
  GsmAT2Off();


  UART_U0_TXD_CLR;
  
  #if SMS_BATONLY_ENABLE==1
  GSMPOW_OFF;
  #endif
  
  
   #if GPRS_UP_TIME_EN==1
   Pt.Gprsing=0;//����ʡ��ģʽ���´������ϴ�������Ҫ���´�ʱ�䵽
   #endif
  
  
}
#if GSM_KEY_SHUT_EN==1

void SmsKeyOff(void)
{
    #if SIM68_EN==1
      #if SIM68_SLP_EN==1
      SIM68_to_sleep();
      #endif
    StateC.AwakeWait=0;
    StateC.GpsOutPt=0;
    #endif
  
    Sms.GsmKeyShut=1;
    LcdGsmXh(100);

    #if GPRS_ENABLE==1

      #if GPRS_6OFF==1
        StateA2.WorkState=WORK_STATE_STOP;
        POWER_CTL_ALL_OFF;
        LcdOFFX(OFF_BSQ_GPRS_HAND_STOP);//60FF
        
        #if RF_REC_EN==1
        RF_IN_SLP;//���ߣ�ʡ��
        #endif

      #else
        LcdOFFX(OFF_GPRS_RECORD_STOP);//70FF(����ģʽ)
        #if RF_U1_EN==1
          U1ToRf();
        #endif
        
      #endif

     
      #if GPRS_DATA_CLR_EN==1
      Pt.GprsDueOut=0;
      GprsDueOutToEE();
      #endif
        
    #endif

        
    #if GPRS_TC_EN==1//zz
      Pt.ExitClosePt=1;
      Pt.ToOff=1;
    #else  
      GsmToPowOff();//�ػ�
      
    #endif
      
    
    
}
void SmsKeyOn(void)
{
    #if SIM68_EN==1
  
      #if SIM68_SLP_EN==1
       SIM68_to_wake();
       StateC.AwakeWait=10;
      #endif
       
    #endif
  
    Sms.GsmKeyShut=0;
    //LcdGsmXh(0);//��ť���ܴ���ʾ�����ź�ǿ�ȱ�־

    #if GPRS_ENABLE==1
       Pt.RtcPt=0;
       Pt.RecordPt=0;
       Pt.RealSamplePt=0;
       StateA2.WorkState=WORK_STATE_RECORDING;
       #if DIGITAL_FILTER_EN==1
       FilterClr();
       #endif
       LcdShowOn(1);//����
     
                    #if ((ZX_EN==1)||(GPRS_UP_REAL_EN==1))
                    Pt.UpCmd=0;
                    #endif
     
        #if RF_REC_EN==1
          RF_IN_REC;//����״̬
          RfDataNULL();//��ʼNULL
          Pt.SampPt=RF_SAMP_TIME-1;
        #endif

        #if RF_U1_EN==1
          U1ToRf();
        #endif
           
         
        LcdNN(HAND_START_SAMPLE);
        
        #if CMD_CTRL_EN==1
        Pt.wdkai=0;
        Pt.sdkai=0;
        #endif
       
        #if MIN_MAX_AVG_ENABLE==1
          StateA1.RecordNum=0;//�տ���ʱ����ʼ���� 
        #endif
          
          
        #if (PRINT_52_42_EN==1)
          
          
           if(Pt.bcSS>0)//��һ����¼��ʱ
           {
               Rtc.SS=ReadRtcD10();
               
               if(Rtc.SS>=Pt.bcSS+120)
               {
               
                   Pt.bcSS=0;//��������0��������ʼ��¼
                   Pt.bcAL=0;
                   SaveBcToEE();
               
               }
           
           }
           
           
        #endif
          

     
    #endif
    
    #if SMS_INV_ENABLE==1
      GsmInvInit();//����״̬������SMS_INV_ENABLEģʽʱ�䵽�Զ����Ͷ���
      #if SMS_BATONLY_ENABLE==0
      GsmToPowOn();
      #endif
    #else
      GsmToPowOn();
    #endif
      
      
      LcdGsmXh(0);//��ť���ܴ���ʾ�����ź�ǿ�ȱ�־
}



void SmsKeySet(void)
{
  u8 Buf[1];
  Buf[0]=Sms.GsmKeyShut;
  I2C_EE_BufferWrite(Buf,GSM_KEY_ADDR,1,0);
}
void SmsKeyRead(void)
{
  
  u8 Buf[1];
  I2C_EE_BufferRead(Buf,GSM_KEY_ADDR,1,0);
  Sms.GsmKeyShut=Buf[0];
}
#endif




#if RTC_UTC_EN==1
void UtcStart(void)
{
   Pt.UtcErr=0;
   Pt.UtcEn=1;//��ʼ��ʱ 
   Pt.UtcDog=UTC_DOG_TIME;//���Ź�������ʱʱ�䵽��δ��ɣ�˵����ʱʧ�ܣ�ֹͣ��ʱ  
}
#endif

//Uart_0.RxBuf
u8 GetGsmXh(void)
{
  u8 PosS,xh=0;
  PosS=GetPosS(Uart_0.RxBuf,1,30,"Q:");
  if(PosS!=255)
  {
     if(Uart_0.RxBuf[PosS+4]==',')//1λ��
     {
         xh=Uart_0.RxBuf[PosS+3]-0x30;
     }
     else if(Uart_0.RxBuf[PosS+5]==',')//2λ��
     {
         xh=(Uart_0.RxBuf[PosS+3]-0x30)*10+(Uart_0.RxBuf[PosS+4]-0x30);
     }
  }
  if(xh==99)
    xh=0;
  return xh;
  
}

void GsmTest(void)
{
  u16 j,len,x;
  u8 buf[]="CP3X1Y2Z3D";
  len=StrLen(buf);
  NOP;
  
  x=0;
  for(j=0;j<len;j++)
  {
    Gsm.SmsRecUni[x++]=0;
    Gsm.SmsRecUni[x++]=buf[j];
  }
  Gsm.SmsUniLen=x;
  
  SmsRecDeel();
  
}


#if GPRS_TC_EN==1
//20171234�տ�->20 17 12 34 00 ->00 20 17 12 34
void GetID_FromEE(u8* ID)
{
      u8 j,cc,buf[10],bbb[10];
      I2C_EE_BufferRead(buf,SERIAL_NO_ADDR,10,0);

      //����յ�����
      cc=0;
      for(j=0;j<10;j++)
      {
         if(buf[j]==0x00)
           cc++;
      }
      
      //�յı��0x30���ᵽǰ��
      for(j=0;j<cc;j++)
        bbb[j]=0x30;
      for(j=0;j<10-cc;j++)
        bbb[cc+j]=buf[j];
      
      //ת��Ϊʮ������
      for(j=0;j<5;j++)
      {
        ID[j]=(bbb[j*2]-0x30)*16+(bbb[j*2+1]-0x30);
      } 
}

//Buf[6] �� 0x01+Flag.ID[5]  �Ƚ� ����ȷ����1
u8 CheckID(u8* buf)
{
  if(buf[0]!=0x01)
    return 0;
 
  u8 j;
  for(j=0;j<5;j++)
  {
    if(buf[1+j]!=Flag.ID[j])
      return 0;
  }
  
  return 1;
}
//�ж�SN���Ƿ���ȷ
u8 CheckSN(u8* buf)
{
  u8 sn[10];
  I2C_EE_BufferRead(sn,SERIAL_NO_ADDR,10,0);
 
  u8 j;
  for(j=0;j<10;j++)
  {
    if(buf[j]!=sn[j])
      return 0;
  }
  
  return 1;
}          
//�ϴ���ǰ����,���������
u8 SF_Send_Head(u8 type,u16 pt)
{
      u16 j;
      u16 xh;
      u8 bbc=0;
      
      xh=pt;//���
      if(type==0xEE)//EEʱ���Ϊ0
      {
        xh=0x00;
      }
      
      //���˰�ͷ��β����0x7E, ��U0SendByte��������U0_SF_SendByte
      //���У��ʱ�����ų�ͷβ��7E

      if(1==Flag.SeeComing)
      {
        ComSendH();
        U1SendValue(xh);//zz
        U1SendByte(',');
        
        if(type==0x0A)
          U1SendString("HAND_ON");
        else if(type==0x0B)
          U1SendString("HAND_OFF");
        else if(type==0x01)
          U1SendString("DAT");
        else if(type==0xEE)
          U1SendString("FIN");
        U1SendByte(',');
        
        ComSendL();
      }

      
      U0SendByte(0x7E);//��ͷ
      bbc=0;
      bbc^=U0_SF_SendByte(type);//������
      bbc^=U0_SF_SendByte(xh>>8);//���
      bbc^=U0_SF_SendByte(xh);
      
      bbc^=U0_SF_SendByte(0x01);//���̱�־
      
      GetID_FromEE(Flag.ID);//��EEȡID
      for(j=0;j<5;j++)
        bbc^=U0_SF_SendByte(Flag.ID[j]);
      
      bbc^=U0_SF_SendByte(0x01);//Э��汾
      
      //�ϴ�ʱ��
      RtcReadTime();
      bbc^=U0_SF_SendByte(0x20);
      bbc^=U0_SF_SendByte(Rtc.Year);
      bbc^=U0_SF_SendByte(Rtc.Month);
      bbc^=U0_SF_SendByte(Rtc.Day);
      bbc^=U0_SF_SendByte(Rtc.Hour);
      bbc^=U0_SF_SendByte(Rtc.Minute);
      bbc^=U0_SF_SendByte(Rtc.Second);
      RtcBcdToD10(&Rtc);
      
      return bbc;
}

//�����ϴ���ϱ�־
void SF_Send_Finish(void)
{
      u8 bbc=0;
      
      
      Flag.TC_Type=0xEE;
      bbc^=SF_Send_Head(Flag.TC_Type,Flag.XlhPt);

      RtcReadTime();
      bbc^=U0_SF_SendByte(0x20);
      bbc^=U0_SF_SendByte(Rtc.Year);
      bbc^=U0_SF_SendByte(Rtc.Month);
      bbc^=U0_SF_SendByte(Rtc.Day);
      bbc^=U0_SF_SendByte(Rtc.Hour);
      bbc^=U0_SF_SendByte(Rtc.Minute);
      bbc^=U0_SF_SendByte(Rtc.Second);
      RtcBcdToD10(&Rtc);
      
      
      U0_SF_SendByte(bbc);//���У����
      U0SendByte(0x7E);//��β

}


void Gprs_SF_XLH_FromEE(void)
{
    u8 buf[2];
    I2C_EE_BufferRead(buf,GPRS_SF_XLH_ADDR,2,0);
    Flag.XlhPt=U8_TO_U16(buf[1],buf[0]);
}
void Gprs_SF_XLH_ToEE(void)
{
    u8 buf[2];
    buf[0]=0xff&Flag.XlhPt;
    buf[1]=0xff&(Flag.XlhPt>>8);
    I2C_EE_BufferWrite(buf,GPRS_SF_XLH_ADDR,2,0);
}

//�˳�͸��ģʽ����0
void SF_Exit_Tc_Clr(void)
{
    Flag.TCing=0;
    Flag.TC_LinkTryPt=0;
    Flag.TC_DataTryPt=0;
    Flag.TC_Type=0x00;
}



//�˳�͸�����Ͽ����ӣ�����
u8 Gprs_Exit_Close_Deel(void)
{
    #if ZX_EN==1
      if(Pt.ZxRtcPt>0)
        Pt.ZxRtcPt--;
    #endif
      
    #if GPRS_UP_REAL_EN==1  
    if(Pt.CmdHave>0)
    {
      Pt.CmdHave--;
      return 0;
    }
    #endif
    
  
  
    if(Pt.ExitClosePt>=1)
    {
      
        #if ZX_EN==1
      
      
           if(Pt.ZxRtcPt==0)
           {
             Pt.ExitClosePt++;
           }
           else if(Pt.ZxRtcPt==3)
           {
               U1SendStrPC("TX\r\n");
               ZX_Rtc_Send();
           }
                  
        #else
             Pt.ExitClosePt++;     
        #endif
      
        
        
        if(Pt.ExitClosePt==3)
        {
           SF_Exit_Tc_Clr();
           U0SendString("+++");//�˳�͸��ģʽzz,  +++ ǰ��1��������������ݣ������˳�
        }
        
        else if(Pt.ExitClosePt==6)
        {
          #if SIM800C_EN==1
          
              #if M26_EN==1
                  U0SendString("AT+QICLOSE\r\n");  
              #else
                  U0SendString("AT+CIPCLOSE\r\n");
              #endif
          #else
               U0SendString("AT^SISC=0\r\n");
          #endif
        }
        

        
        else if(Pt.ExitClosePt==8)
        {
           #if SIM800C_EN==1
          
             #if M26_EN==1
               U0SendString("AT+QIDEACT\r\n");
             #else
               U0SendString("AT+CIPSHUT\r\n");
             #endif
             
           #else
             Pt.ExitClosePt=0;
             //U0SendString("AT^SISC=0\r\n");
           #endif
        }
        else if(Pt.ExitClosePt==9)//�˳�͸������΢�ӳٺ������־
        {
           Pt.ExitClosePt=0;
        }
    }
    
    
    if(Pt.ExitClosePt==0)
      return 0;
    else
      return 1;
}


//�������FA->0A  �ػ����FB->0B ����ʪ������01
u8 Is_WSD(u8* buf)
{
    u8 j;
    
    for(j=0;j<(2*CH_NUM);j++)//������ô���ֽ�
    {
      if((buf[j]!=0xFA)&&(buf[j]!=0xFB))//���ǿ��ػ����
      {
        return 0x01;
      }
  
    }
    
    if(buf[0]==0xFA)
      return 0x0A;
    
    return 0x0B;
}




//��in�����е�7D 01תΪ 7D, �����е�7D 02 תΪ 7E,���������out
u16 Buf_7E_Decode(u8* in,u16 inlen,u8* out)
{
  u16 i,j;
  i=0;
  for(j=0;j<inlen;j++)
  {
     if((in[j]==0x7D)&&(in[j+1]==0x01))
     {
       out[i++]=0x7D;
       j++;
     }
     else if((in[j]==0x7D)&&(in[j+1]==0x02))
     {
       out[i++]=0x7E;
       j++;
     }
     else
     {
       out[i++]=in[j];
     }
  }
  return i;
}





//����͸��״̬�󣬽��յ����ݴ���
u8 Gprs_SF_Recv_Do(void)
{
    u8 cc=0;

    if(Flag.TCing==0)
    {
        #if SIM800C_EN==1
        //CONNECT������͸��״̬ ���ų� ALREADY CONNECT  �� CONNECT FAIL
        if((CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sCONNECT,6))\
          &&(!(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sALREADY,6)||CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sFAIL,4)))\
            &&GsmIsFree(1))//zz
        #else
        if((Pt.Linking==1)&&CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sOK,2)&&GsmIsFree(1))//zz
        #endif
        {
              cc=1;
            /*zz
              if(1==Flag.SeeComing)
              {
                 ComSendH();
                 U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                 ComSendL();
              }
              */
              
              U1SendStrPC("LINK OK\r\n");
              
              #if ((GPRS_TC_EN==1)&&(SIM800C_EN==0))
              Pt.Linking=0;
              #endif
              
              
              //�����Ϻ��Զ�����͸��״̬
              Flag.TCing=1;
              Flag.TC_LinkTryPt=0;
              Pt.LinkFailPt=0;
              
              
              Flag.TC_DataTryPt=0;//��ʼ�������ݣ��ظ�������0
              Flag.TC_WaitPt=1;//������ʼ��������

        }
    }
    
    else//͸��״̬ʱ���������ݴ���
    {
        cc=1;
        
        u8 ok=0;
        
        #if GPRS_LG31_3000_EN==0
        u8 out[2];
        #endif
        
        if((Uart_0.RecLen>=6)&&(Uart_0.RecLen<U0MAX))
        {
              #if GPRS_LG31_3000_EN==1
          

                    //300.10
                    if(CompBuf(Uart_0.RxBuf,Uart_0.RecLen,sServerCode,6))
                    {
                        if(Flag.TC_Type==0x01)//zz
                        {
                            Flag.TC_Type=0x00;
                            
                            #if  ((TCP_RTC_EN==1)||(TCP_SET_EN==1))
                            ok=ServerCmdSet(Uart_0.RxBuf);//ARTC��ȷ����A  ,BSET��ȷ����B
                            #endif

                            Pt.SendFailPt=0;
                            Flag.TC_DataTryPt=0;
                            
                            GprsSendOk();
                            
                            Flag.TCing=2;//��1�η��ͣ����ͳɹ�����������Ҫ������2
                            
                            if(Pt.GprsDueOut>0)
                            {
                              Flag.TC_WaitPt=1;//����������һ��
                              

                              if(ok=='B')//����ָ����豸��ظ��ɹ�ָ�֮��ż�����һ������
                              {
                                Flag.TC_WaitPt=3;
                              }

                            }
                            else
                            {
                               Pt.ExitClosePt=1;//�˳�
                               
                               #if GPRS_UP_TIME_EN==1
                               Pt.Gprsing=0;//�����ϴ����
                               #endif
                               
                            }
                            
                           // ok=1;  ,�������A �� B �����������
                        }
                                                                    
                        
                        
                    }
                    
                    #if ((SMS_TCP_EN==1)||(GPRS_UP_REAL_EN==1))
                    else
                    {
                        //39 00 19 38 33 31 31 32 31 30 30 31 32 17 10 12 13 50 05 00 00 02 01 00 03 00 01 00 30 1D
                        u16 ww;
                        u16 len;
                        ww=GetXpt(Uart_0.RxBuf,0,Uart_0.RecLen,'9',1);//��1����ͷ
                        if(ww!=10000)//��Ч����
                        {
                            len=U8_TO_U16(Uart_0.RxBuf[ww+1],Uart_0.RxBuf[ww+2]);
                            if(len<100)
                            {
                                if(Uart_0.RxBuf[ww+3+len]=='0')//β
                                {
                                    if(get_add(&Uart_0.RxBuf[ww],len+4)==Uart_0.RxBuf[ww+4+len])//У�����ȷ
                                    {
                                        if(CheckSN(&Uart_0.RxBuf[ww+3]))//SN��ȷ
                                        {
                                           //if(1)
                                           if(Flag.XlhPt==U8_TO_U16(Uart_0.RxBuf[19],Uart_0.RxBuf[20]))//�����ȷ zz
                                           {

                                           
                                              u16 xx;
                                              
                                              
                                              xx=U8_TO_U16(Uart_0.RxBuf[ww+23],Uart_0.RxBuf[ww+24]);//���峤��
                                              

                                              #if SMS_TCP_EN==1
                                              if(Uart_0.RxBuf[ww+21]==0x01)//�����ͣ����Ű�---------------------
                                              {
                                                  
                                                  if(xx==1)
                                                  {
                                                        if(Uart_0.RxBuf[ww+25]==0x01)//���ս����ȷ
                                                        {
                                                              Pt.smsErrPt=0;
                                                          
                                                              if(Pt.SmsPow)
                                                              {
                                                                U1SendStrPC("smsPOW OK\r\n");
                                                                Pt.SmsPow=0;
                                                                Flag.TC_WaitPt=1;
                                                              }
                                                              else if(Pt.SmsAL)
                                                              {
                                                                U1SendStrPC("smsAL OK\r\n");
                                                                Pt.SmsAL=0;
                                                                Flag.TC_WaitPt=1;
                                                              }
                                                              
                                                              
                                                              if((Pt.SmsPow==0)&&(Pt.SmsAL==0)&&(Pt.GprsDueOut==0))
                                                              {
                                                                Pt.ExitClosePt=1;//�˳�
                                                                Pt.Gprsing=0;//ֹͣ�ϴ�
                                                              }
                                                              
                                                              ok=1;
                                                              
                                                        }
                                                  }
                                                  
                                              }
                                              #endif
                                              
                                              #if GPRS_UP_REAL_EN==1
                                              u8 tmp[5];
                                              
                                              if(Uart_0.RxBuf[ww+21]==0x02)//������:��ʪ�Ȱ�----------------------------
                                              {
                                                  if(xx==3)////���峤��3
                                                  {
                                                
                                                      if(Uart_0.RxBuf[ww+26]==0x01)//���ս����ȷ
                                                      {
                                                            if(Uart_0.RxBuf[ww+25]==0x01)//��¼��־
                                                            {
                                                                if(Pt.UpCmd!=0x24)
                                                                {
                                                                  if(Flag.TC_Type==0x01)//zz
                                                                  {
                                                                        Flag.TC_Type=0x00;
                                                                        
                                            
                                                                        Pt.SendFailPt=0;
                                                                        Flag.TC_DataTryPt=0;
                                                                        
                                                                        GprsSendOk();
                                                                        
                                                                        Flag.TCing=2;//��1�η��ͣ����ͳɹ�����������Ҫ������2
                                                                        
                                                                        if(Pt.GprsDueOut>0)
                                                                        {
                                                                          Flag.TC_WaitPt=1;//����������һ��
    
                                                                        }
                                                                        else
                                                                        {
                                                                           Pt.ExitClosePt=1;//�˳�
                                                                           
                                                                           #if GPRS_UP_TIME_EN==1
                                                                           Pt.Gprsing=0;//�����ϴ����
                                                                           #endif
                                                                           
                                                                        }
                                                                        
                                                                        ok=1;
                                                                  }
                                                                }
                                                            }
                                                            else if(Uart_0.RxBuf[ww+25]==0x00)//��¼��־��ʵʱ����
                                                            {
                                                                if(Pt.UpCmd==0x24)
                                                                {
                                                                        Pt.UpCmd=0X00;//ʵʱ�����ϴ��ɹ�
                                                                        U1SendStrPC("wsdRT OK\r\n");
                                                                  
                                                                        if(Pt.GprsDueOut>0)
                                                                        {
                                                                          Flag.TC_WaitPt=1;//����������һ��
    
                                                                        }
                                                                        else
                                                                        {
                                                                           Pt.ExitClosePt=1;//�˳�
                                                                           
                                                                           #if GPRS_UP_TIME_EN==1
                                                                           Pt.Gprsing=0;//�����ϴ����
                                                                           #endif
                                                                           
                                                                        }
                                                                        
                                                                        ok=1;
                                                                }
                                                            }
                                                            
                                                            
                                                           //��ʱ----------------------------------
                                                            StrcutRtc  Utc;
                                                
                                                            Utc.Year=BCD_TO_D10(Uart_0.RxBuf[ww+13]);
                                                            Utc.Month=BCD_TO_D10(Uart_0.RxBuf[ww+14]);
                                                            Utc.Day=BCD_TO_D10(Uart_0.RxBuf[ww+15]);
                                                            Utc.Hour=BCD_TO_D10(Uart_0.RxBuf[ww+16]);
                                                            Utc.Minute=BCD_TO_D10(Uart_0.RxBuf[ww+17]);
                                                            Utc.Second=BCD_TO_D10(Uart_0.RxBuf[ww+18]);
                                                            
      
                                                            if(Rtc_Is_Right(&Utc))//ʱ���ʽ��ȷ
                                                            {
                                                                  Utc.SS=DateToSeconds(&Utc);
                                                                  
                                                                  //�ȶ�ȡ�豸RTC
                                                                  Rtc.SS=ReadRtcD10();
                                                                  
                                                                  //�Ƚϣ����ʱ�Ӳ�Զ���ʱ
                                                                  if((Rtc.SS>Utc.SS+D_RTC_TIME)||(Rtc.SS<Utc.SS-D_RTC_TIME))
                                                                  {
                                                                      SecondsToDate(Utc.SS,&Rtc);
                                                                                
                                                                      //����ʱ��
                                                                      RtcD10ToBcd(&Rtc);//ת��BCD
                                                                      RtcSetTime();
                                                                      RtcBcdToD10(&Rtc);
                                                                      
                                                                      U1SendStrPC("RTC OK\r\n");//zz
      
                                                                  }
                                                            } 
                                                            

                                                      }
 
                                                      


                                                  }

                                                 
                                                  if(ok==1)
                                                  {
                                                      Flag.XlhPt++;//���кż�1��ֱ��65535��0
                                                      Gprs_SF_XLH_ToEE();
                                                  }
                                                 
                                              }
                                              
                                              //�·�ָ��--------------------------------
                                              else if(Uart_0.RxBuf[ww+21]==0x03)//������:���ñ��������ޱ�������
                                              {
                                                
                                                  if(xx==2+CH_NUM*5)//���峤����ȷ
                                                  {
                                                      if(Uart_0.RxBuf[ww+25]==CH_NUM)//ͨ������ȷ
                                                      {
                                                        
                                                           
                                                           u16 j;
                                                          
                                                           for(j=0;j<CH_NUM;j++)
                                                            {
                                                                tmp[0]=Uart_0.RxBuf[ww+27+5*j];//���޵��ֽ�
                                                                tmp[1]=Uart_0.RxBuf[ww+26+5*j];//���޸��ֽ�
                                                                tmp[2]=Uart_0.RxBuf[ww+29+5*j];
                                                                tmp[3]=Uart_0.RxBuf[ww+28+5*j];
                                                                tmp[4]=Uart_0.RxBuf[ww+30+5*j];//����
                                                              
                                                                alarm_buf_to_flash(tmp,j,1);
                                                                
                                                            }
                                                           
                                                            LG37_ReCmd(0x03);
                                                            U1SendStrPC("setAL OK\r\n");
                                                      }
                                                  }
                                                
                                                
                                                
                                              }
                                              //-----------------------------------
                                              else if(Uart_0.RxBuf[ww+21]==0x04)//������:���÷�������ַ�Ͷ˿�
                                              {
                                                       //��������
                                                       I2C_EE_BufferWrite(&Uart_0.RxBuf[ww+25],GPRS_DOMAIN_ADDR,30,0);
                                                         
                                                       //���ö˿�
                                                       tmp[0]=Uart_0.RxBuf[ww+56];//���ֽ�
                                                       tmp[1]=Uart_0.RxBuf[ww+55];
                                                       
                                                       I2C_EE_BufferWrite(tmp,GPRS_PORT_ADDR,2,0);
                                                         
                                                       //�������أ��Զ���
                                                       tmp[0]=0x01;
                                                       I2C_EE_BufferWrite(tmp,GPRS_DOMAIN_EN_ADDR,1,0);
                                                       
                                                       LG37_ReCmd(0x04);
                                                       U1SendStrPC("setPORT OK\r\n");
                                                       
                                              }
                                              
                                              //--------------------------------------
                                              if(Uart_0.RxBuf[ww+21]>0x01)  
                                              {
                                              
                                                  //�ж��Ƿ����·�ָ��------------------------
                                                  if(Uart_0.RxBuf[ww+2+len]==0x01)//�·�ָ���־
                                                  {
                                                       U1SendStrPC("cmdV\r\n");
                                                            
                                                       Pt.CmdHave=10;//10�뱣������״̬
                                                  }
                                                  else
                                                  {
                                                       U1SendStrPC("cmdN\r\n");
                                                            
                                                       Pt.CmdHave=0;
                                                  }
                                              }
                                                   
                                              #endif
                                              
                                              
                                          }
                                          else
                                          {
                                              U1SendStrPC("xhERR\r\n");
                                          }

                                        }
                                        else
                                        {
                                            U1SendStrPC("snERR\r\n");
                                        }
                                    }

                                }
                            }
                          
                        }
                      
                      
                    }

                    
                    #endif
                    
                    
                    
                    
              #elif ZX_EN==1
                    /*
                      if(1==Flag.SeeComing)
                      {
                          ComSendH();
                          U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                          U1SendByte(0X0D);U1SendByte(0X0A);
                          ComSendL();
                      }
                    */
                    
                    
                    out[0]=GetPosC(Uart_0.RxBuf,0,Uart_0.RecLen,0x02);//ͷ
                    out[1]=GetPosC(Uart_0.RxBuf,0,Uart_0.RecLen,0x03);//β
                    if((out[0]!=255)&&(out[1]!=255)&&(Uart_0.RecLen<30))
                    {

                        u8 crcbuf[4],ascbuf[8];
                        u8 cmdAA=0;
                        u8 crcok;
                        
                        if((Uart_0.RxBuf[out[0]+1]=='A')&&(Uart_0.RxBuf[out[0]+2]=='A'))
                        {
                            Get_u8buf_formU32(crcbuf,get_crc32(&Uart_0.RxBuf[out[0]+1],16,0,0));//AA20170628161235 75519C6E
                            cmdAA=1;
                        }
                        else
                        {
                        
                            Get_u8buf_formU32(crcbuf,get_crc32(&Uart_0.RxBuf[out[0]+1],2,0,0));//24 CCC4A0C4  23   25
                        
                        }
                        
                        
                        hex_2_ascii(crcbuf,ascbuf,4);
                        
                        if(cmdAA==1)
                        {
                            crcok=CheckBuf(ascbuf,&Uart_0.RxBuf[out[0]+17],8);
                        }
                        else
                        {
                            crcok=CheckBuf(ascbuf,&Uart_0.RxBuf[out[0]+3],8);
                        }
                          

                        if(crcok)//CRC32  ��ȷ
                        {

                                if(Flag.TC_Type==0x01)//zz
                                {
                                          Flag.TC_Type=0x00;
                                          
                                          
                                          if((Uart_0.RxBuf[out[0]+1]=='2')&&(Uart_0.RxBuf[out[0]+2]=='4'))//ʵʱָ��24
                                          {
                                              if(Pt.UpCmd==0x24)
                                              {
                                                
                                                
                                                  U1SendStrPC("ROK\r\n");
                                                  Pt.UpCmd=0x25;
                                                  
                                              }
                                              else
                                              {
                                                  U1SendStrPC("RERR\r\n");
                                              }
                                            
                                          }
                                          else if((Uart_0.RxBuf[out[0]+1]=='2')&&(Uart_0.RxBuf[out[0]+2]=='3'))//���޼�¼ָ��23
                                          {
                                              if(Pt.UpCmd==0x23)
                                              {
                                                  U1SendStrPC("AOK\r\n");
                                                  Pt.UpCmd=0x25;
                                                  
                                                  Pt.UpALing=2;//�������ݷ������
                                              }
                                              else
                                              {
                                                 U1SendStrPC("AERR\r\n");
                                              }
                                          }
                                          else if((Uart_0.RxBuf[out[0]+1]=='2')&&(Uart_0.RxBuf[out[0]+2]=='5'))//��ʷ��¼ָ��25
                                          {
                                              if(Pt.UpCmd==0x25)
                                              {
                                                  GprsSendOk();//ȷ�Ϸ�����һ��
                                                  Pt.UpCmd=0;
                                                  Pt.UpALing=0;
                                                  
                                              }
                                              else
                                              {
                                                 U1SendStrPC("DERR\r\n");
                                              }
                                          }
                                          else
                                          {
                                              if(1==Flag.SeeComing)
                                              {
                                                ComSendH();
                                                U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                                                U1SendByte(0X0D);U1SendByte(0X0A);
                                                ComSendL();
                                              }
                                          }
                                          
                                          
                                          
                                          Pt.SendFailPt=0;
                                          Flag.TC_DataTryPt=0;
                                                
                                          Flag.TCing=2;//��1�η��ͣ����ͳɹ�����������Ҫ������2
                                                
                                          if(Pt.GprsDueOut>0)
                                          {
                                             Flag.TC_WaitPt=1;//����������һ��
                                          }
                                          else
                                          {
                                            

                                              Pt.ZxRtcPt=4;//��ʼ��ʱ��4���ڲ��˳�����
                                              
                                           
                                              Pt.ExitClosePt=1;//�˳�
                                                   
                                              #if GPRS_UP_TIME_EN==1
                                              Pt.Gprsing=0;//�����ϴ����
                                              #endif
                                                   
                                          }
                                                
                                          ok=1;
            
                                }
								
								
				//��ʱ  AA20170713114006DAFC2101
                                if((Uart_0.RxBuf[out[0]+1]=='A')&&(Uart_0.RxBuf[out[0]+2]=='A'))//AAָ��
                                 {

                                     U1SendStrPC("TR\r\n");
                                     
                                     
                                     if(TcpRtcAuto(Uart_0.RxBuf,Uart_0.RecLen,1))
                                     {
                                         U1SendStrPC("TOK\r\n");
                                     }
                                     
                                     

                                     ok=1;      
                                }
                             
                        
                        }
                        else
                        {
                          U1SendStrPC("CRCERR\r\n");
                        }
                      
                    }
                    else
                    {
                      U1SendStrPC("TWERR\r\n");
                      
                    }
                    
                    
                    
              #else
          
                    if(GetPos2C(Uart_0.RxBuf,0,Uart_0.RecLen,0x7E,out))//��⵽2��7E
                    {

                            //����������ָ��zz

                            u8 len=out[1]-out[0]+1;
                            if(len>9)
                            {
                                u8 j,bbc=0;
                                
                                WDT_CLR;
                                
                                //��ȥ��ͷβ7E,�ٰ����ݰ����7D 01 תΪ7D ,��7D 02תΪ 7E���ٽ��뵽Uart_1.RxBuf
                                Uart_1.RecLen=Buf_7E_Decode(&Uart_0.RxBuf[out[0]+1],len-2,Uart_1.RxBuf);
                                
                                
                                for(j=0;j<Uart_1.RecLen-1;j++)//�������У����
                                  bbc^=Uart_1.RxBuf[j];
                                
                                
                                if(bbc==Uart_1.RxBuf[Uart_1.RecLen-1])//У����ȷ
                                {
                                  
                                    //      ���  ID               ���
                                    //7E FF 00 00 01 02 01 71 23 45 01 EB 7E
                                    if(Uart_1.RxBuf[0]==0xFF)//������Ӧ����-----------------------------------
                                    {
                                         //if(Flag.XlhPt==U8_TO_U16(Uart_1.RxBuf[1],Uart_1.RxBuf[2]))//�����ȷ zz
                                         {
                                               if(CheckID(&Uart_1.RxBuf[3]))//IDһ��
                                               {
                                                     WDT_CLR;
                                                     
                                                     if(Uart_1.RxBuf[9]==0x01)//���������ճɹ�
                                                     {
                                                            
                                                            //��ʪ�ȡ����ػ���ǣ����ݰ�Ӧ��
                                                            if((Flag.TC_Type==0x01)||(Flag.TC_Type==0x0A)||(Flag.TC_Type==0x0B))
                                                            {
                                                                    Flag.TC_Type=0x00;
                             
                                                                    GprsSendOk();
                    
                                                                    Flag.XlhPt++;//���кż�1��ֱ��65535��0
                                                                    Gprs_SF_XLH_ToEE();
                                                                    
                                                                    Pt.SendFailPt=0;
                                                                    Flag.TC_DataTryPt=0;
                                                                    
                                                                    
                                                                    if(Pt.GprsDueOut==0)
                                                                    {
                                                                      Flag.TCing=3;//���ݷ�����ϣ���3,��ʼ������ϱ��EE
                                                                    }
                                                                    else
                                                                    {
                                                                      Flag.TCing=2;//��1��ֻ����1������(������ػ���ǻ���ʪ������)�����ͳɹ�����������Ҫ������2
                                                                    }
                                                                    
                                                                    Flag.TC_WaitPt=1;//����������һ��
                                                                    
                                                                    ok=1;
                                                            }
                                                            else
                                                            {
                                                              //U1SendStrPC("TYPE ERR\r\n");//zz
                                                            }
                                
                                                     }
                                               }
                                         }
                                    }
                                    
                                    //      ID
                                    //7E EE 01 02 01 71 23 45 FB 7E //�������
                                    else if(Uart_1.RxBuf[0]==0xEE)//������ͨѶ�������-------------------------------
                                    {
                                        if(CheckID(&Uart_1.RxBuf[1]))//IDһ��
                                        {
                                            if(Flag.TC_Type==0xEE)//�ȴ����ս������EE
                                            {
                                                Flag.TC_Type=0x00;
                                                
                                                U1SendStrPC("FINISH OK\r\n");
                                                Pt.ExitClosePt=1;//�˳�
                                                
                                                
                                                #if GPRS_UP_TIME_EN==1
                                                Pt.Gprsing=0;//�����ϴ����
                                                #endif
                                               
                                                ok=1;
                                                
                                            }
                                        }
                                    
                                    }
                                }
                            }
                      
                   }
             #endif
                   
                   
                 
        }
        
        
        if(ok==0)//��������
        {
           if(1==Flag.SeeComing)
           {
             

                   ComSendH();
                   U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                   U1SendByte(0X0D);U1SendByte(0X0A);
                   ComSendL();

             
  
           }
        }
		
		
	U0Clear();//zz  
        
        
    }
    
    return cc;

}

//͸���л������ݴ���
void Gprs_SF_Deel(void)
{
    if(Flag.TC_WaitPt>0)
      Flag.TC_WaitPt--;
    
    if(Uart_0.FlagFinish==1)//���ڴ����������ʱ������zz
      return;
    
    #if ((ZX_EN==1)||(GPRS_UP_REAL_EN==1))
    if((Flag.TC_WaitPt==0)&&(GprsDataHav()||(Flag.TCing==3)||(Pt.UpCmd==0x24)))//������Ҫ����
    #else
    if((Flag.TC_WaitPt==0)&&(GprsDataHav()||(Flag.TCing==3)))//������Ҫ����
    #endif
    {
          
          if(Flag.TCing==0)//��͸��״̬,�������ݷ���ʱ���Զ�����͸��״̬
          {
                if(GsmIsFree(1))
                {
                        Flag.TC_WaitPt=TC_WAIT_T;
                        
                        if(++Flag.TC_LinkTryPt<=2)
                        {
                            #if SIM800C_EN==1
                          
                                #if SMS_TCP_EN==1
                     
                                 if((Pt.SmsPow)||(Pt.SmsAL))
                                 {
                                   //���Ŷ˿�
                                   Pt.smsPort=1;
                                   U1SendStrPC("SMS PORT\r\n");
                                   
                                   #if M26_EN==1
                                     //U0SendString("AT+QIOPEN=\"TCP\",\"1707ba7c.nat123.cc\",34812\r\n");
                                     U0SendString("AT+QIOPEN=\"TCP\",\"sms.hzjly.cn\",10001\r\n");
                                   #else
                                     //U0SendString("AT+CIPSTART=\"TCP\",\"1707ba7c.nat123.cc\",34812\r\n");
                                     U0SendString("AT+CIPSTART=\"TCP\",\"sms.hzjly.cn\",10001\r\n");
                                   #endif
                                   
                                   
                                   //SendIpPortFromEE();//��ʪ������
                                   
                                 }
                                 else
                                 {
                                   Pt.smsPort=0;
                                   SendIpPortFromEE();//��ʪ������
                                 }
                          
                                #else
                                   SendIpPortFromEE();

                                 #endif
                            #else
                               Pt.Linking=0;
                               

                               U1SendStrPC("SO\r\n");
                               U0SendString("AT^SISO=0\r\n");//������
                               delay_ms(500);
                               WDT_CLR;
                               delay_ms(500);
                               WDT_CLR;
                               
                               delay_ms(500);
                               WDT_CLR;
                               delay_ms(500);
                               WDT_CLR;
                               
                               delay_ms(500);
                               WDT_CLR;
                               delay_ms(500);
                               WDT_CLR;
                               

                               
  
                               U0SendString("AT^IPENTRANS=0\r\n");//����͸�� 
                               Pt.Linking=1;
                            #endif
                            Gsm.ReadCsqDog=10;//CSQ������10��
                        }
                        else
                        {
                          #if ((GPRS_TC_EN==1)&&(SIM800C_EN==0))
                          Pt.Linking=0;
                          #endif
                          
                          
                           //����͸��״̬ʧ��
                           U1SendStrPC("LINK FAIL\r\n");
                           Pt.ExitClosePt=1;//�����˳�͸�����Ͽ����ӣ�������
                           
                           Pt.LinkFailPt++;
                           
                           #if GPRS_UP_TIME_EN==1
                           if(Pt.LinkFailPt==2)
                           {
                              if(Pt.GprsSleep==1)
                              {
                                GsmToPowOff();//ʡ��ģʽ���ر�ģ��
                              }
                           }
                           #endif
                           
                           
                           if(Pt.LinkFailPt>=3)//����2�����Ӳ��ϣ�ģ������ zz
                           {
                              Pt.LinkFailPt=0;
                              ErrorToRst();
                           }
                        }
                            

                }
          }
          
          
          else//͸��״̬1��2��3����������
          {
                     WDT_CLR;
                     
                     if(++Flag.TC_DataTryPt<=2)//���ͺ󣬵ȴ�Ӧ�����Flag.TC_DataTryPt
                     {
                         #if GPRS_LG31_3000_EN==1
                       
                             #if SMS_TCP_EN==1
                     
                                 if((Pt.SmsPow)||(Pt.SmsAL))
                                 {
                                   
                                    if(Pt.smsPort==1)
                                    {
                                        Flag.TC_Type=0x00;
                                        
                                        TcpSmsSend();//���Ͷ���TCP��
                                        
                                        if(Pt.SmsPow)
                                          U1SendStrPC("smsPOW SEND\r\n");
                                        else
                                          U1SendStrPC("smsAL SEND\r\n");
                                        
                                        if(++Pt.smsErrPt>5)//����5�η���ʧ�ܣ��˳����ŷ���
                                        {
                                          Pt.SmsPow=0;
                                          Pt.SmsAL=0;
                                          Pt.ExitClosePt=1;//�����˳�����
                                        }
                                    }
                                    else
                                    {
                                        Pt.ExitClosePt=1;//�����˳�����
                                    }
                                 }
                                 else
                                 {
                                    if(Pt.smsPort==0)
                                    {
                                      Gprs_SF_Tramit_Data();//������ʪ������01
                                    }
                                    else
                                    {
                                       Pt.ExitClosePt=1;//�����˳�����
                                    }
                                    
                                 }
                                 
                       
                             #else
                                 Gprs_SF_Tramit_Data();//������ʪ������01
                             #endif
                           
                           
                           
                         #elif ZX_EN==1
                           
                           Gprs_SF_Tramit_Data();//������ʪ������01
                                                 
                         #else
                           
                           if(Flag.TCing<=2)
                           {
                              Gprs_SF_Tramit_Data();//������ʪ������01���򿪻����0A, ��ػ����0B
                           }
                           else if(Flag.TCing==3)
                           {
                              SF_Send_Finish();//������ϱ��EE
                              Flag.TC_DataTryPt=2;//ֻ��һ�Σ����û���ģ����ط�
                           }
                         
                         #endif

                     }
                     else
                     {
                         //һֱûӦ��
                          if(1==Flag.SeeComing)//zz
                          {
                                ComSendH();
                                U1SendByte('S');
                                U1SendValue(Pt.GprsThisDueOut);//ʵ�ʷ�������
                                U1SendByte('/');
                                U1SendValue(Pt.GprsDueOut);//������
                                U1SendByte(',');
                                U1SendString("NO RESPONSE\r\n");
                                ComSendL();
                          }
                       
                         //�˳�͸�����Ͽ�����
                         Pt.ExitClosePt=1;//�����˳�����
                         Flag.TC_WaitPt=TC_WAIT_LONG;//60�������
                         Flag.TC_Type=0x00;
                         
                         Pt.SendFailPt++;
                         if(Pt.SendFailPt>2)//����3��ʧ�ܣ�����ģ��
                         {
                             Pt.SendFailPt=0;
                             
                             Pt.ExitClosePt=0;
                             Flag.TC_WaitPt=0;
                             ErrorToRst();
                         }
                           
                          
                     }
                     
                     
                     Flag.TC_WaitPt=TC_WAIT_T;

            
          }
    
    }

}


/*

�ػ�-����3��
��Ž���
ID����
���ػ���Ǽ�¼�����������б����һ��

͸��ʱ�������ݣ�������λ��ʱ�ӱ��0 ��45 85  ��ʾ���6OFF���쳣״̬?????  ��λ������
�ڲ����Ź���λ���޷��ָ�����������λ�Ÿ�λ���ϵ縴λOK
�ʣ��ɹر��ڲ����Ź��������쳣ʱ�����ⲿ���Ź���λ
1.ʱ�Ӷ�ȡʱ�ж�ʱ�������ⲿ���Ź���λ
2.EE�ػ�״̬��⣬�ж������ⲿ���Ź���λ


͸��ʱ����ô�ػ�
���˳�͸�����Ͽ����ӣ��ٹػ�ָ��

���ŷ�����

7E FF 00 00 01 02 01 71 23 45 01 EB 7E
7E EE 01 02 01 71 23 45 FB 7E

iot.sit.sf-express.com

8888
*/

#endif


//---------------------------------------------------------------
#endif
