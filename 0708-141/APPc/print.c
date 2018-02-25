#include "_config.h"


//y�����ֵ����Сֵ,�����д�ӡ�������(0,383)
#define Y_MIN                10//x���ͷλ��
#define Y_MAX                383

//���ݴ�ӡ�������,��ӡ�������¸�����40����հ�����
#define Y_DATA_MIN           (Y_MIN+40)
#define Y_DATA_MAX           (Y_MAX-40)

#define DIAN_X_JIANJU        ((u16)6*2)//8����/1mm, 2����ֵ����
#define PRINT_RECORD_MAX     50//ʵ�ʴ�ӡ������
#define PRINT_BAG_MAX_BYTES  ((u16)PRINT_RECORD_MAX*2*CH_NUM)

//..............................................................................
//��ȡ��ʷ������Ϣ
//����С��PRINT_RECORD_MAXʱ����������ѹ��
//���ݴ���PRINT_RECORD_MAXʱ����������ѹ��
//�����������ӡ�������ΪPRINT_RECORD_MAX��ÿ��Ϊ�����Сֵ2�㣬
//          ����������ΪPRINT_RECORD_MAX/2�����Download.BagNum���ܴ���PRINT_RECORD_MAX/2
//��������������PRINT_RECORD_MAXʱ����Ϊ�ֳ�<(PRINT_RECORD_MAX/2)����������������ѹ����<PRINT_RECORD_MAX������
void GetHisInfo(void)
{
  u16 j;
  
  //����������Download.DownloadNum..............................................
  Print.EndX=Download.DownloadNum=Download.CurrentNum=StateA1.RecordNum;//����ʱ�Ѽ�¼����
  if(Download.DownloadNum==0)
    return ;
  //EE��ʷ������ʼ���
  if(StateA2.RecordOverNum==0)
    Print.BeginX=0;
  else
    Print.BeginX=StateA1.RecordNum%StateC.RecordMaxNum+StateC.RecordMaxNum*(StateA2.RecordOverNum-1);
  Print.DianXh=Print.BeginX;
  for(j=0;j<8;j++)
  {
    Print.XhPt[j]=0;
  }
  
  if(Download.DownloadNum>=StateC.RecordMaxNum)//��¼����
  {
    Download.DownloadNum=StateC.RecordMaxNum;
  }
  //��ÿ�����������Download.BagMaxRecordNum
  if(Download.DownloadNum<=PRINT_RECORD_MAX)//��ӡ����δ���������ѹ��
  {
    Download.BagMaxRecordNum=(u16)PRINT_BAG_MAX_BYTES/StateC.RecordBytes;//ÿ���������
    //���ܰ�����Download.BagNum
    Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;
    if((Download.DownloadNum%Download.BagMaxRecordNum)!=0)
      Download.BagNum++;
  }
  else//��ӡ�����������ѹ��
  {
    //��ÿ���������Download.BagMaxRecordNum
    Download.BagMaxRecordNum=Download.DownloadNum/((PRINT_RECORD_MAX-3)/2);//������Ԥ��3����Ϊ3ƬEE����ʱʹ��
    if((Download.DownloadNum%((PRINT_RECORD_MAX-3)/2))>0)
    {
      Download.BagMaxRecordNum++;
    }
    //���ܰ�����Download.BagNum
    Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;
    if((Download.DownloadNum%Download.BagMaxRecordNum)>0)
    {
      Download.BagNum++;
    }
  }
}
//..............................................................................
//ֱ�Ӵ�EEȡ��ȡ��Xpt����ʷ���ݴ����DataBuf��������ð����ݵ������Сֵ�������MinMaxBuf
//PC:0 ������
//PC:1 ��������ͨ�����ݵ�DataBuf
//PC:2 ��������ͨ�����������Сֵ��MinMaxBuf�������Сֵʱ���Ⱥ����Ŷ�
//ͨ��1 ��СֵMinMaxBuf[0], ͨ��1 ���ֵMinMaxBuf[1]
//ͨ��2 ��СֵMinMaxBuf[2], ͨ��2 ���ֵMinMaxBuf[3]
u16 GetHisXpt(u16 Xpt,u16* MinMaxBuf,u8* DataBuf,u8 PC)
{
    u32 EE_32Pt;
    u16 NT;
    u16 PT;
    u8 QuHao;//��ǰ�洢Ƭ����
    u8 EE_Num;//�洢Ƭ��

    PT=0;
    Download.BagPt=Xpt;//���ذ����
    if(Download.BagPt+1>Download.BagNum)return 0;//����Ŵ���
  
    //�������ݰ����ֽ���
    if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//���һ�����Ҳ���һ��
    {
      Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
    }
    else
    {
      Download.BagRecordNum=Download.BagMaxRecordNum;//����
    }
    Download.BagDataBytes=Download.BagRecordNum*StateC.RecordBytes;
  
    //��ȡ���ݰ�
    EE_32Pt=0;
    if(Download.CurrentNum>=StateC.RecordMaxNum)//�Ѽ���
    {
      EE_32Pt+=(Download.CurrentNum%StateC.RecordMaxNum)*StateC.RecordBytes;//����ʱ�Ĵ洢ƫ��
    }

    EE_32Pt+=(u32)Download.BagPt*(Download.BagMaxRecordNum*StateC.RecordBytes);
    EE_32Pt=EE_32Pt%(StateC.RecordMaxNum*StateC.RecordBytes);
    if(EE_32Pt<=StateC.E1_MaxMem)//��һƬ�洢β��ַ
    {
      QuHao=0;
      if(EE_32Pt+Download.BagDataBytes>StateC.E1_MaxMem)
      {
          NT=GetBagMaxMinFromEE(EE_32Pt,StateC.E1_MaxMem-EE_32Pt,MinMaxBuf,DataBuf,PC,QuHao);
          PT+=NT;
          if(NT/StateC.RecordBytes==1)
          {
            Print.FgKuaPt=1;
          }
          
          if(I2cSeekEE()>1)//Ƭ������1
          {
            QuHao=1;//�ڶ�Ƭ
          }
          NT=GetBagMaxMinFromEE(0,EE_32Pt+Download.BagDataBytes-StateC.E1_MaxMem,MinMaxBuf,&DataBuf[PT],PC,QuHao);//ֻ�迼��PC==2ʱ״��
          PT+=NT;
          if(NT/StateC.RecordBytes==1)
          {
            Print.FgKuaPt=2;
          }
      }
      else
      {
        PT+=GetBagMaxMinFromEE(EE_32Pt,Download.BagDataBytes,MinMaxBuf,DataBuf,PC,QuHao);
      }
    }
    else//����Ƭ
    {
      QuHao=1+(EE_32Pt-StateC.E1_MaxMem)/StateC.E2_MaxMem;
      EE_32Pt=(EE_32Pt-StateC.E1_MaxMem)%StateC.E2_MaxMem;
      
      if(EE_32Pt+Download.BagDataBytes>StateC.E2_MaxMem)//����Ƭ�洢β��ַ
      {
        NT=GetBagMaxMinFromEE(EE_32Pt,StateC.E2_MaxMem-EE_32Pt,MinMaxBuf,DataBuf,PC,QuHao);
        PT+=NT;
        if(NT/StateC.RecordBytes==1)
        {
          Print.FgKuaPt=1;
        }
        
        QuHao++;
        EE_Num=I2cSeekEE();
        QuHao=QuHao%EE_Num;
        NT=GetBagMaxMinFromEE(0,EE_32Pt+Download.BagDataBytes-StateC.E2_MaxMem,MinMaxBuf,&DataBuf[PT],PC,QuHao);
        PT+=NT;
        if(NT/StateC.RecordBytes==1)
        {
          Print.FgKuaPt=2;
        }
      
      }
      else
      {
        PT+=GetBagMaxMinFromEE(EE_32Pt,Download.BagDataBytes,MinMaxBuf,DataBuf,PC,QuHao);
      }
    }
    return PT;
}

//..................................................................
//ֱ�Ӵ�EEȡ���ݰ��������Сֵ,�����MinMaxBuf, Len:��ȡ���ֽ�����һ��ͨ����ֵռ2���ֽ�
//ֱ�Ӵ�EEȡ����ͨ�����ݻ�ȡ����ͨ�����ݵ������Сֵ
//ע�⣺ȡ�����Сֵǰ��MinMaxBuf����ΪSENIOR_NULL
//PC:0 ������
//PC:1 �����������ݵ�DataBuf
//PC:2 �������а�����С���ֵ��DataBuf����С���ֵʱ���Ⱥ����Ŷ�
//ȫ�ֱ���Print.DianXh,Print.XhBuf[][],Print.XhPt[]���ӡ�����
u16 GetBagMaxMinFromEE(u16 ReadAddr,u16 Len,u16* MinMaxBuf,u8* DataBuf,u8 PC,u8 No)
{
  u16 j; 
  u16 VV;
  u16 OnePt;
  u16 MinBuf[8];
  u16 MaxBuf[8];
  u16 MinXh[8];
  u16 MaxXh[8];
  u16 DataPt;
  
  u8 CC;
  u8 LL;
  u8 OneBytePt;
  u8 OneChPt;
  u8 add;
   
  add=0;
  DataPt=0;

  WP_H;
  _DINT();
  I2cStart();
  #if EEPROM>=256
  I2cSendByte((0xa0+(No<<1)));
  I2cSendByte(ReadAddr/256);
  I2cSendByte(ReadAddr%256);
  #else
  #endif
  I2cStart();
  I2cSendByte(0xa0+(No<<1)+0x01);
  
  for(j=0;j<Len;j++)
  {
    add+=CC=I2cRecByte();//ȡһ���ֽ�����
    if(j==(Len-1))//��ȡ����
      I2cAck(0);
    else
      I2cAck(1);
    
    OnePt=j/StateC.RecordBytes;//��x��¼��
    OneBytePt=j%StateC.RecordBytes;//��x�ֽ�
    OneChPt=OneBytePt/2;//ͨ����
    
    if(OneBytePt==0)
    {
      Print.DianXh++;//ÿ����¼��+1
    }
    
    if(OneBytePt==OneChPt*2)//���ֽ�
    {
      LL=CC;
    }
    else if(OneBytePt==OneChPt*2+1)//���ֽ�
    {
      VV=CC;
      VV=(VV<<8)|LL;
      
      //ȡ�ð��������ݺ�MinMaxBuf�е�ֵ�������Сֵ����������MinMaxBuf
      MinMaxBuf[OneChPt*2]=get_min(MinMaxBuf[OneChPt*2],VV);
      MinMaxBuf[OneChPt*2+1]=get_max(MinMaxBuf[OneChPt*2+1],VV);
      
      if(PC==1)//ȡ�ð���������
      {
        DataBuf[DataPt++]=LL;
        DataBuf[DataPt++]=CC;
        Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=Print.DianXh;
      }
      else if(PC==2)//ȡÿ���е���С���ֵ,
      {
        if((Len/StateC.RecordBytes)==1)//�ð�ֻ��1����
        {
            DataBuf[DataPt++]=LL;
            DataBuf[DataPt++]=CC;
            Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=Print.DianXh;
        }
        else//����2����
        {
            if(OnePt==0)//��1����
            {
              MinBuf[OneChPt]=MaxBuf[OneChPt]=VV;
              MinXh[OneChPt]=MaxXh[OneChPt]=Print.DianXh;
            }
            else
            {
              MinBuf[OneChPt]=get_min(MinBuf[OneChPt],VV);//��ͨ��1��Сֵ
              if(MinBuf[OneChPt]==VV)
              {
                MinXh[OneChPt]=Print.DianXh;
              }
              
              MaxBuf[OneChPt]=get_max(MaxBuf[OneChPt],VV);//��ͨ��1���ֵ
              if(MaxBuf[OneChPt]==VV)
              {
                MaxXh[OneChPt]=Print.DianXh;
              }
              
              if(OnePt==(Len/StateC.RecordBytes)-1)//���һ����
              {
                if(MinXh[OneChPt]<MaxXh[OneChPt])//�����Сֵʱ���Ⱥ��Ŷ�
                {
                  DataBuf[DataPt++]=(MinBuf[OneChPt]%256);//��Сֵ��ǰ
                  DataBuf[DataPt++]=(MinBuf[OneChPt]/256);
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]%256);
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]/256);
    
                  Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=MinXh[OneChPt];
                  Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=MaxXh[OneChPt];
                }
                else
                {
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]%256);//���ֵ��ǰ
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]/256);
                  DataBuf[DataPt++]=(MinBuf[OneChPt]%256);
                  DataBuf[DataPt++]=(MinBuf[OneChPt]/256);
                  
                  if(MinXh[OneChPt]==MaxXh[OneChPt])//2��ֵ������ͬ
                  {
                    MaxXh[OneChPt]=MaxXh[OneChPt]-1;//���2��ֵ��ͬ�������㷨�����Ϊ��󣬹�������
                  }
                  Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=MaxXh[OneChPt];
                  Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=MinXh[OneChPt];
                }
              }
            }
        }
      }
    }
  }
  I2cStop();
  _EINT();
  NOP;
  NOP;
  NOP;
  return DataPt;
}

//#if PRINT_ALL_CURVE==1
//��ͨ��ChNo,��������Buf[0]����������Buf[1],��СֵBuf[2],���ֵBuf[3]
void GetCurveRange(u8 ChNo,u16* Buf)
{
  u16 AA;
  
  AA=*((char *)CH1_A_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_L_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x7f);
  if(((*((char *)CH1_A_L_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x80)==0x80)
    AA=AA|0x8000;
  Buf[0]=AA;//��������
  
  AA=*((char *)CH1_A_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_H_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x7f);
  if(((*((char *)CH1_A_H_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x80)==0x80)
    AA=AA|0x8000;
  Buf[1]=AA;//��������
  
  //����ʷ���ݼ�ֵ�뱨����ֵ����С���ֵ
  #if MIN_MAX_AVG_ENABLE==1
  Buf[2]=get_min(StateC.min[ChNo],Buf[0]);//��Сֵ
  Buf[3]=get_max(StateC.max[ChNo],Buf[1]);//���ֵ
  #endif
}

//..............................................................................
//����ͨ�������Сֵȫ��ΪSENIOR_NULL
void PutMaxMinNull(void)
{
  u8 j;
  for(j=0;j<(CH_NUM*2);j++)
  {
    Print.MinMaxBuf[j]=SENIOR_NULL;
  }
}
//...........................................................................................
//�Լ�¼����ʱ��Ϊ��׼����¼���ΪStateB.RecordTime����������ĳ��XXʱ�䣬BCD��ʽ
StrcutRtc GetXuhaoTimeBaseStartTimeBcd(u32 Xuhao)
{
  u32 log_start_time;
  StrcutRtc time_bcd;
  time_bcd.Year=StateA2.StartTime[0];
  time_bcd.Month=StateA2.StartTime[1];
  time_bcd.Day=StateA2.StartTime[2];
  time_bcd.Hour=StateA2.StartTime[3];
  time_bcd.Minute=StateA2.StartTime[4];
  time_bcd.Second=StateA2.StartTime[5];
  RtcBcdToD10(&time_bcd);
  log_start_time=DateToSeconds(&time_bcd);//��ʱ��ת��Ϊ����
  
  log_start_time+=Xuhao*StateB.RecordTime;//��ǰ��ŵ�ʱ������
  SecondsToDate(log_start_time,&time_bcd);//�����ǰ���ʱ��
  RtcD10ToBcd(&time_bcd);
  return time_bcd;
}
//......................................................................................................

//........................................................................................
//���ص�ǰʱ�䣬����
u32 GetRtcSS(void)
{
  RtcReadTime();
  RtcBcdToD10(&Rtc);
  return DateToSeconds(&Rtc);
}
//��ӡ��¼���ʱ��
void PrintJianGe(u32 DD)
{
  StrcutRtc rtc_d10;
  SecondsToDate(DD,&rtc_d10);
  U1SendByte(rtc_d10.Hour/10+0x30);
  U1SendByte(rtc_d10.Hour%10+0x30);
  U1SendByte(':');
  U1SendByte(rtc_d10.Minute/10+0x30);
  U1SendByte(rtc_d10.Minute%10+0x30);
  U1SendByte(':');
  U1SendByte(rtc_d10.Second/10+0x30);
  U1SendByte(rtc_d10.Second%10+0x30);
}
//��ӡ��ŵ�Xuhao��ʱ��ֵ��ʱ���׼Ϊ��ʼʱ��
void PrintTime(u32 Xuhao)
{
  StrcutRtc time_bcd;
  
  #if BSQ_ENABLE==1
  time_bcd=SS_To_BCD(Xuhao);
  #else
  time_bcd=GetXuhaoTimeBaseStartTimeBcd(Xuhao);
  #endif
  
  
#if RPTINT_ONLY_WD_EN==1
  
    
    if(Pt.PtDay!=time_bcd.Day)
    {
      //��ӡ����
      U1SendByte(time_bcd.Year/16+0x30);
      U1SendByte(time_bcd.Year%16+0x30);
      U1SendByte(time_bcd.Month/16+0x30);
      U1SendByte(time_bcd.Month%16+0x30);
      U1SendByte(time_bcd.Day/16+0x30);
      U1SendByte(time_bcd.Day%16+0x30);
      U1SendByte(0x0a);
    }
    Pt.PtDay=time_bcd.Day;
#else
  U1SendByte(time_bcd.Year/16+0x30);
  U1SendByte(time_bcd.Year%16+0x30);
  U1SendByte(time_bcd.Month/16+0x30);
  U1SendByte(time_bcd.Month%16+0x30);
  U1SendByte(time_bcd.Day/16+0x30);
  U1SendByte(time_bcd.Day%16+0x30);
  U1SendByte(' ');
#endif  
  
  U1SendByte(time_bcd.Hour/16+0x30);
  U1SendByte(time_bcd.Hour%16+0x30);
  U1SendByte(':');
  U1SendByte(time_bcd.Minute/16+0x30);
  U1SendByte(time_bcd.Minute%16+0x30);
  
#if RPTINT_ONLY_WD_EN==0
  U1SendByte(':');
  U1SendByte(time_bcd.Second/16+0x30);
  U1SendByte(time_bcd.Second%16+0x30);
#endif
}
//��ӡ��ֵ��Χ�����999.9����С-99.9��ռ5��ascii
void PrintValue(u16 Value)
{
  u8 Fuhao=0;
  u16 cc=Value;
  
  if(cc==SENIOR_NULL)
  {
    U1SendString(" NULL");
    return;
  }
  else if(cc==HAND_STOP)
  {
    U1SendString(" 3OFF");
    return;
  }
  
  #if DOOR_PRINT_EN==1
  else if(cc==DOOR1_OPEN)
  {
    U1SendString("һ���ſ�");
    return;
  }
  else if(cc==DOOR1_CLOSE)
  {
    U1SendString("һ���Ź�");
    return;
  }
  else if(cc==DOOR2_OPEN)
  {
    U1SendString("�����ſ�");
    return;
  }
  else if(cc==DOOR2_CLOSE)
  {
    U1SendString("�����Ź�");
    return;
  }
  #endif
  
  
  if(cc&0x8000)
  {
    Fuhao=1;
    cc=cc&0x7fff;
  }
  if(cc>999)
    U1SendByte((cc/1000)%10+0x30);
  else
  {
    if(Fuhao==1)
      U1SendByte('-');
    else
      U1SendByte(' ');//zz ����ʱǰ�治��ո�����ʱ�пո�
  }
  
  if(cc>99)
    U1SendByte((cc/100)%10+0x30);
  else
    U1SendByte(' ');
  U1SendByte((cc/10)%10+0x30);
  U1SendByte('.');
  U1SendByte(cc%10+0x30); 

}


//��ӡx��ļ�ͷ
void PrintXjt(u8 cc)
{
  //һ����ͷ�൱��3������
  u16 j;
  for(j=0;j<10;j++)
  {
    U1SendByte(0x1b);
    U1SendByte(0x27);
    
    U1SendByte(GetL8(3));
    U1SendByte(GetH8(3));
    
    U1SendByte(GetL8(cc));//�м���
    U1SendByte(GetH8(cc));
    
    U1SendByte(GetL8(20-j));
    U1SendByte(GetH8(20-j));
    
    U1SendByte(GetL8(j));
    U1SendByte(GetH8(j));
  
    U1SendByte(0x0d); 
  }
}
//���뺺�ִ�ӡ
void SetCnPrint(u8 cc)
{
  if(cc==1)
  {
    U1SendByte(0x1c);
    U1SendByte(0x26);  
  }
  else
  {
    U1SendByte(0x1c);
    U1SendByte(0x2e);
  }
}
void SetIni(void)//��ӡ����λ����ʼ��
{
    U1SendByte(0x1b);
    U1SendByte(0x40);
    delay_ms(2);
    //�����ߵ���ӡ
    U1SendByte(0x1b);
    U1SendByte(0x7b);
    U1SendByte(0x00);
}

void GoXDianHang(u8 cc)
{
  U1SendByte(0x1b);
  U1SendByte(0x4a);
  U1SendByte(cc);
}
//��ӡcc������
void GoOneHang(u8 cc)
{
  U1SendByte(0x00);
  U1SendByte(0x00);
  delay_ms(120);
  while(cc--)
  {
    U1SendString("  \n");
  }
}

//������BufData,2���ֽڱ�ʾһ���������ֽ���ǰ
void PrintCurve(u8* BufData,u16 DataN,u8 ChNo)
{
  u16 cc;
  u16 y1;
  u16 yy;
  float ft1;
  
  u8 fg=0;
  u16 Dpt;
  u16 i,j,k;
  u16 aa,nn;//2��������������
  u16 Y_AL,Y_AH;
  u16 CurveM[4];//��������,�������ޣ���Сֵ�����ֵ
  
  GetCurveRange(ChNo,CurveM);//��ȡͨ�������������ޣ�������������
  
  Y_AL=GetZvY(CurveM[2],CurveM[3],Y_DATA_MIN,Y_DATA_MAX,CurveM[0]);
  Y_AH=GetZvY(CurveM[2],CurveM[3],Y_DATA_MIN,Y_DATA_MAX,CurveM[1]);
  
  if(Y_AL>Y_AH)//���ݴ����˳�
    return;
  
  GoOneHang(1);
  
  //��ӡy���ϵı���������ֵ������������ֵ
  if(Y_AL/12>3)
  {
    for(j=0;j<Y_AL/12-3;j++)
      U1SendByte(' ');
  }
  else
  {
    U1SendByte(' ');
  }
  PrintValue(CurveM[0]);
  if((Y_AH-Y_AL)/12>5)
  {
    for(j=0;j<(Y_AH-Y_AL)/12-5;j++)
      U1SendByte(' ');
  }
  else
  {
    U1SendByte(' ');
  }
  PrintValue(CurveM[1]);
  U1SendByte(0x0d);
  
  //��ӡy��
  PrintYz();
  delay_us(100);
  
  //��ӡ����
  Dpt=0;
  for(i=0;i<DataN;i++)
  {
    cc=256*BufData[i*2+1]+BufData[i*2];
    
    if((cc==SENIOR_NULL)||(cc==HAND_STOP))//���������ݲ���ӡ
      continue;
    
    cc=GetZvY(CurveM[2],CurveM[3],Y_DATA_MIN,Y_DATA_MAX,cc);
    
    if(Dpt>0)
    {
      //��2���㿪ʼ��ӡ����
      for(k=0;k<DIAN_X_JIANJU;k++)
      {
        ft1=(((float)cc-y1)/DIAN_X_JIANJU)*(k+1)+y1;//��2�����߼�����һ��yֵ
        yy=(u16)(ft1*10);
        if((yy%10)>4){yy+=10;}
        yy=yy/10;
          
        if(yy>aa)
          nn=yy-aa;//����ֵ�������������
        else
          nn=aa-yy;
        if(nn==0)
          nn=1;
        
        U1SendByte(0x1b);
        U1SendByte(0x27);
        
        if((k%4)==0)//�������ޣ��������ޣ����߱�ʾ
        {
          fg=!fg;
        }
        //��������
        if(fg)
        {
          U1SendByte(GetL8(nn+3));//x�ᣬ�������ޣ���������
          U1SendByte(GetH8(nn+3));
        }
        else
        {
          U1SendByte(GetL8(nn+1));
          U1SendByte(GetH8(nn+1));
        }
  
        for(j=0;j<nn;j++)
        {
          if(yy>aa)
          {
            U1SendByte(GetL8(aa+j));
            U1SendByte(GetH8(aa+j));
          }
          else
          {
            U1SendByte(GetL8(yy+j));
            U1SendByte(GetH8(yy+j));
          }
  
        }
        U1SendByte(GetL8(Y_MIN));//x��
        U1SendByte(GetH8(Y_MIN));
        if(fg)
        {
          U1SendByte(GetL8(Y_AL));//��������
          U1SendByte(GetH8(Y_AL));
            
          U1SendByte(GetL8(Y_AH));//��������
          U1SendByte(GetH8(Y_AH));
        }
        U1SendByte(0x0d);
        aa=yy;
      }
    }
    y1=aa=cc;//��һ����
    Dpt++;
  }
  PrintXjt(Y_MIN);//��ӡx���ͷ
}
void PrintStart(void)
{
  U1Init(BAUDRATE_PRINT,0);//zz
  ComSendH();
  U1_RXD_UN;//��������ʱ���رս����жϣ���ֹ�����жϸ���
}
void PrintEnd(void)
{
  ComSendL();
  U1_RXD_EN;//���ͽ����󣬴򿪽����ж�
  U1Init(BAUDRATE_SYSTEM,0);//��ӡ��ϣ�����������
  
  #if PRINT_FZ_EN==1
  Pt.prt_x=0;
  #endif
  
}
//��ӡ��ϵ����Ϣ
void PrintMessageInfo(void)
{
  GoOneHang(1);
  U1SendString("********************************\n");
  U1SendString("��ʷ���ݴ�ӡ����\n\n");
  U1SendString("ǩ�֣�         ʱ�䣺           \n\n");
  GoOneHang(1);
  U1SendString("��ע��\n\n");
  GoOneHang(6);
}
//��ӡ��¼������Ϣ
void PrintInfo(void)
{
  GoOneHang(1);
  U1SendString("********************************\n");
  U1SendString("�û�����");
  i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("��  �⣺");
  i2c_ee_read_to_pc(TITLE_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("��ʼ��¼ʱ�䣺");
  PrintTime(Print.BeginX);
  U1SendByte(0x0a);
  U1SendString("������ӡʱ�䣺");
  PrintTime(Print.EndX);
  U1SendByte(0x0a);
  U1SendString("��¼ʱ������");
  PrintJianGe(StateB.RecordTime);
  U1SendByte(0x0a);
  U1SendString("�ܼƼ�¼������");
  U1SendValue(Download.DownloadNum);
  U1SendByte(0x0a);
}
void PrintChInfo(u8 ChNo)
{
  GoOneHang(1);
  U1SendString("********************************\n");
  if(_U_OC==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//�¶�
  {
    U1SendString("ͨ��");
    #if CH_PAIR_ENABLE==1
    U1SendValue(ChNo/2+1);
    #else
    U1SendValue(ChNo+1);
    #endif
    U1SendString("  �¶�   ��λ���棩");
  }
  else if(_U_RH==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//ʪ��
  {
    U1SendString("ͨ��");
    #if CH_PAIR_ENABLE==1
    U1SendValue(ChNo/2+1);
    #else
    U1SendValue(ChNo+1);
    #endif
    U1SendString("  ʪ��   ��λ��%RH��");
  }

  U1SendByte(0x0a);
  U1SendString("��Сֵ��");
  PrintValue(Print.MinMaxBuf[ChNo*2]);
  U1SendString("  ���ֵ��");
  PrintValue(Print.MinMaxBuf[ChNo*2+1]);
  U1SendByte(0x0a);
}
//��ӡһ�������б�һ������ռ2���ֽ�
void PrintList(u16* Xuhao,u8* BufData,u16 Num,u8 ChNo)
{
  u16 j,dd;
  
  GoOneHang(1);
  
    U1SendString(" ����   ʱ��   ͨ��");
    #if CH_PAIR_ENABLE==1
    U1SendValue(ChNo/2+1);
    #else
    U1SendValue(ChNo+1);
    #endif
  
  
  if(_U_OC==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//�¶�
  {
    U1SendString("�¶ȣ��棩\n");
  }
  else if(_U_RH==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//ʪ��
  {
    U1SendString("ʪ�ȣ�%RH��\n");
  }

  for(j=0;j<Num;j++)
  {
    dd=BufData[j*2+1];
    dd=(dd<<8)|BufData[j*2];
    if((dd==SENIOR_NULL)||(dd==HAND_STOP))//���������ݲ���ӡ
      continue;
    PrintTime(Xuhao[j]);//ʱ��
    U1SendByte(' ');
    PrintValue(dd);//��ֵ
    
    
    if(AlarmXSeek(dd,ChNo,1))//�Ƿ񱨾�����
      U1SendString("(����)");
    
    U1SendByte(0x0a);
  }
}

//ȡ���ӡ�����ݣ��������������������Сֵ���MinMaxBuf,
u16 GetPrintData(u16* MinMaxBuf,u8* BufD)
{
  u16 j;
  u16 AaPt;
  u16 NT;

  Print.FgKuaQu=0;
  Print.FgKuaPt=0;
  Print.FgLastBagOne=0;
  
  AaPt=0;
  if(Download.DownloadNum<=PRINT_RECORD_MAX)//��ӡ����δ���������ѹ��,����ȫ������
  {
     AaPt=GetHisXpt(0,MinMaxBuf,BufD,1);//ȡ��������
  }
  else//��ӡ�����������ѹ��������ѹ�����ͼ�ֵ����
  {
    for(j=0;j<Download.BagNum;j++)
    {
      NT=GetHisXpt(j,MinMaxBuf,&BufD[AaPt],2);//ȡ���ݰ��е������Сֵ��������ʱ���Ⱥ�����
      AaPt+=NT;
      if(NT/StateC.RecordBytes==1)//ֻ��һ������,ֻ���һ�����ܻ����������
      {
        Print.FgLastBagOne=1;
      }
      else if(NT/StateC.RecordBytes>2)//����2�����ݣ�ֻ�п����ſ��ܻ����������
      {
        Print.FgKuaQu=1;
        Print.BagKuaQuPt=j;//���������
      }
    }
  }
  AaPt=AaPt/StateC.RecordBytes;
  return AaPt;
}
//��ӡĳͨ�����߻��б�
void PrintChData(u16 DataNN,u8 ChNo)
{
      u16 CT;
      u16 BagNN;
      u16 xx;
      u16 i,j;
      u8 PrintDD[50*2];//���50������

      //ȡͨ����Ӧ����
      if(Download.DownloadNum<=PRINT_RECORD_MAX)//δ���
      {
        for(i=0,j=0;j<DataNN;j++)//ֻ��һ����һ��ȡ��
        {
          xx=j*StateC.RecordBytes+ChNo*2;
          PrintDD[i++]=Uart_1.RxBuf[xx];
          PrintDD[i++]=Uart_1.RxBuf[xx+1];
        }
      }
      else
      {
        BagNN=Download.BagNum;
        if(Print.FgKuaQu==1)//����ʱ�����һ��
        {
          BagNN++;
        }
                
        CT=0;
        for(i=0,j=0;j<BagNN;j++)
        {
          //������2�������һ�����ܳ���ֻ��һ�����ݵ����
          if((j==BagNN-1)&&(Print.FgLastBagOne==1))//���һ��
          {
            xx=CT+ChNo*2;
            PrintDD[i++]=Uart_1.RxBuf[xx];
            PrintDD[i++]=Uart_1.RxBuf[xx+1];
            CT+=2*CH_NUM;//���ֽ���
          }
          else if((j==Print.BagKuaQuPt)&&(Print.FgKuaQu==1)&&(Print.FgKuaPt==1))//������1�ְ�
          {
             xx=CT+ChNo*2;
             PrintDD[i++]=Uart_1.RxBuf[xx];
             PrintDD[i++]=Uart_1.RxBuf[xx+1];
             CT+=2*CH_NUM;//���ֽ���
          }
          else if((j==Print.BagKuaQuPt+1)&&(Print.FgKuaQu==1)&&(Print.FgKuaPt==2))//������2�ְ�
          {
             xx=CT+ChNo*2;
             PrintDD[i++]=Uart_1.RxBuf[xx];
             PrintDD[i++]=Uart_1.RxBuf[xx+1];
             CT+=2*CH_NUM;//���ֽ���
          }
          else//����ÿ������2������
          {
            xx=CT+ChNo*4;
            PrintDD[i++]=Uart_1.RxBuf[xx];
            PrintDD[i++]=Uart_1.RxBuf[xx+1];
            PrintDD[i++]=Uart_1.RxBuf[xx+2];
            PrintDD[i++]=Uart_1.RxBuf[xx+3];
            CT+=4*CH_NUM;//���ֽ���
          }
        }
      }
      PrintChInfo(ChNo);//��ӡͨ����Ϣ
      #if PRINT_ALL_CURVE==1
      PrintCurve(PrintDD,DataNN,ChNo);//��ӡ����
      #endif
      PrintList(Print.XhBuf[ChNo],PrintDD,DataNN,ChNo);//��ӡ�б�
      NOP;
}
//��ӡȫ�����ݣ�����50����ʱ��ȡ50������ֵ��ӡ���ܷ�Ӧ��������
//��ӡ������Ϣ�������б��������ߵ�
void PrintAll(void)
{
  u8 j=0;
  u16 PrintNum;
  
  if(Download.Downloading)//��������ʱ�˳���ӡ
   return;
  
  GetHisInfo();
  if(Download.DownloadNum<1)
    return;
  //��ʼ����ǰ������ͨ�������Сֵ��SENIOR_NULL
  PutMaxMinNull();
  
  //��ӡ��ʼ��ʾ------------------------------------------------------
  PrintStart();
  LcdNN(PRINTING);
  //KeyIntClr();//�������а����ж�
  Download.Downloading=1;
  GoOneHang(1);//��ʼ��ӡ��ʾ
  
  //ȡ���ݣ���EEȡ���ݣ�����ʱ��ϳ�
  PrintNum=GetPrintData(Print.MinMaxBuf,Uart_1.RxBuf);
  SetIni();
  delay_ms(2);
  SetCnPrint(1);
  delay_ms(2);
  PrintInfo();//��ӡ��¼��Ϣ
  if(PrintNum<=PRINT_RECORD_MAX)
  {
    for(j=0;j<CH_NUM;j++)
    {
      delay_us(100);
      PrintChData(PrintNum,j);//��ӡ���ߺ��б�
    }
  }
  PrintMessageInfo();//��ӡ��ϵ����Ϣ 

  //��ӡ����------------------------------------------------------
  PrintEnd();
  Download.Downloading=0;
  //KeyIntSet();//���¿������а����ж�
  
}

//..............................................................................

u8 GetL8(u16 cc)
{
  return (cc%256);
}
u8 GetH8(u16 cc)
{
  return (cc/256);
}
float GetU16ToFloat(u16 cc)
{
  float ff;
  if(cc&0x8000)
  {
    cc=cc&0x7fff;
    ff=(-1.0)*cc;
  }
  else
  {
    ff=cc;
  }
  return ff;
}

//16x8�����ַ�
const u16 A16x8[14][8]={
  {0x0000,0x07F0,0x0808,0x1004,0x1004,0x0808,0x07F0,0x0000},//0
  {0x0000,0x2010,0x2010,0x3FF8,0x2000,0x2000,0x0000,0x0000},//1
  {0x0000,0x3070,0x2808,0x2408,0x2208,0x2188,0x3070,0x0000},//2
  {0x0000,0x1830,0x2008,0x2088,0x2088,0x1148,0x0E30,0x0000},//3
  {0x0000,0x0700,0x04C0,0x2420,0x2410,0x3FF8,0x2400,0x0000},//4
  {0x0000,0x19F8,0x2108,0x2088,0x2088,0x1108,0x0E08,0x0000},//5
  {0x0000,0x0FE0,0x1110,0x2088,0x2088,0x1118,0x0E00,0x0000},//6
  {0x0000,0x0038,0x0008,0x3F08,0x00C8,0x0038,0x0008,0x0000},//7
  {0x0000,0x0E38,0x1144,0x1084,0x1084,0x1144,0x0E38,0x0000},//8
  {0x0000,0x00E0,0x3110,0x2208,0x2208,0x1110,0x0FE0,0x0000},//9
  {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},//��  10
  {0x0000,0x0000,0x0000,0x030C,0x030C,0x0000,0x0000,0x0000},//:   11
  {0x0080,0x0080,0x0080,0x0080,0x0080,0x0080,0x0080,0x0000},//-   12
  {0x0000,0x3000,0x3000,0x0000,0x0000,0x0000,0x0000,0x0000}//.    13
};


//��16x8�ַ��е�һ�У�����һ�еĴ�ӡ�����͸������λ������out
u8 GetOneHangForm16X8(u16 in,u8* out)
{
  u16 i=0,j;
  for(j=0;j<16;j++)
  {
    if(in&(1<<(15-j)))
    {
      out[i++]=j;
    }
  }
  return i;
}

//��ӡy��,�޼�ͷ
void PrintYz(void)
{  
  u16 i,j;
  u16 JG;
  JG=(Y_MAX-Y_MIN)/3;//y�ֳ�3��
  for(i=0;i<3;i++)
  {
    U1SendByte(0x1b);
    U1SendByte(0x27);
    U1SendByte(GetL8(JG));
    U1SendByte(GetH8(JG));
    for(j=0;j<JG;j++)
    {
      U1SendByte(GetL8(Y_MIN+j+JG*(2-i)));
      U1SendByte(GetH8(Y_MIN+j+JG*(2-i)));
    }
    U1SendByte(0x0d);
  }
}

//ȡһ������ÿλ��
void GetZvTobuf(u16 zv,u8* buf)
{
  u8 fsfg;
    
  fsfg=0;
  if(zv&0x8000)
  {
    fsfg=1;
    zv=zv&0x7fff;
  }
  
  if(fsfg)
  {
    buf[0]=12;//-
  }
  else
  {
    if(zv>999)
    {
      buf[0]=(zv/1000)%10;
    }
    else
    {
      buf[0]=10;//��
    }
  }
  
  if(zv>99)
  {
    buf[1]=(zv/100)%10;
  }
  else
  {
    buf[1]=10;//��
  }
  
  if(zv>9)
  {
    buf[2]=(zv/10)%10;
  }
  else
  {
    buf[2]=0;//0
  }
  
  buf[3]=13;//С����
  buf[4]=zv%10;
}
//����ֵzvvת��Ϊyֵ
//������ϵ(ymin,ymax)/(zv1,zv2)
u16 GetZvY(u16 zv1,u16 zv2,u16 ymin,u16 ymax,u16 zvv)
{
  u16 dd;
  float aa,bb,cc;
  aa=GetU16ToFloat(zv1);
  bb=GetU16ToFloat(zv2);
  cc=GetU16ToFloat(zvv);

  if((cc>=aa)&&(cc<=bb))
  {
    cc=((cc-aa)/(bb-aa))*(ymax-ymin)+ymin;
    dd=(u16)cc;
  }
  else if(cc>bb)
  {
    dd=ymax;
  }
  else if(cc<aa)
  {
    dd=ymin;
  }
  
  return dd;
}

#if PRINT_REAL_EN==1//================================================================
//��ӡy���y��5�����StateC.PrintXB[5]:ȫ�ֱ���
//5������yֵ��Χ(33,369)
void PrintY5Zv(void)
{
  u16 i,j,k,n;
  u8 potnn[5];
  u8 potbuf[5][16];
  u16 qxnn=0;
  u8 vbuf[5][5];
  u16 xxmin,xxmax;
  u16 tmp;
  
  //y���ϱ��
  xxmin=xxmax=StateC.PrintXB[0];
  for(j=0;j<5;j++)
  { 
    //ȡ5�����ߵ��е������Сֵ
    if(j<4)
    {
      xxmin=get_min(xxmin,StateC.PrintXB[j+1]);
      xxmax=get_max(xxmax,StateC.PrintXB[j+1]);
    }
    //ȡ��ȵ�ÿ������
    GetZvTobuf(StateC.PrintXB[j],vbuf[j]);
  }
  
  for(k=0;k<5;k++)//vbuf[0-4]
  {
        for(i=0;i<8;i++)//vbuf[].bit[0-7]
        {
          //ÿһ�����߻���
          //��������-----------------------------------------------------------
          //x��
          qxnn=1;
          
          //5��y����-����
          for(n=0;n<5;n++)//5��
          {
            potnn[n]=GetOneHangForm16X8(A16x8[vbuf[n][k]][i],potbuf[n]);//ȡһ��,���ַ�(1/8)
            qxnn+=potnn[n];
          }
          
          //��ӡ1��------------------------------------------------------------
          U1SendByte(0x1b);
          U1SendByte(0x27);
          U1SendByte(GetL8(qxnn));//һ�����ӡ�ĵ���
          U1SendByte(GetH8(qxnn));
          
          //x��
          U1SendByte(GetL8(19));
          U1SendByte(GetH8(19));
          
          //5�����ֵ
          for(j=0;j<5;j++)
          {
            tmp=GetZvY(xxmin,xxmax,33,369,StateC.PrintXB[j])-8;//��ÿ�����ߵ�λ��yֵ
            for(n=0;n<potnn[j];n++)
            {
              U1SendByte(GetL8(potbuf[j][n]+tmp));//y����������ʾλ��
              U1SendByte(GetH8(potbuf[j][n]+tmp));
            }
          }
          
          //ִ�б��д�ӡ--------------------------------------------------------
          U1SendByte(0x0d);
        }
  }
  
  PrintYz();//��ӡy��
}
//zv1,zv2: ��ӡ��2����ֵ������
//    xjj: x������xjj
//  Xuhao: ������ʱ��ͼ�¼������������ǰ��ŵ��ʱ��
// timeEn: 0,����ӡʱ�䣬1,��ӡʱ��
//��ӡͬʱ�����ߺ�x���עʱ��(Xuhao)
//5������yֵ��Χ(33,369)
//StateC.PrintXB[5]:5�����ߵ㣬ȫ�ֱ���
void PrintCC(u16 zv1,u16 zv2,u32 Xuhao,u8 timeEn)
{
   float ft1;
   u16 i,j,k,m,y1,y2,yy,ybc,qxnn,ynn,ytm;
   u16 xxmin,xxmax;
   u8 potnn,xspt,xsfg,xjpt;
   u8 potbuf[16];
   u8 timeBit[12];
   StrcutRtc time_bcd;
   
   if(timeEn)
   {
      //ȡ��ŵ�ʱ��ֵ
      time_bcd=GetXuhaoTimeBaseStartTimeBcd(Xuhao);
      
      //ȡʱ���ÿλ���֣���ʽΪ ������ ʱ:��,�� 130422 08:30
      m=0;
      timeBit[m++]=time_bcd.Year/16;
      timeBit[m++]=time_bcd.Year%16;
      timeBit[m++]=time_bcd.Month/16;
      timeBit[m++]=time_bcd.Month%16;
      timeBit[m++]=time_bcd.Day/16;
      timeBit[m++]=time_bcd.Day%16;
      timeBit[m++]=10;//�ո�
      timeBit[m++]=time_bcd.Hour/16;
      timeBit[m++]=time_bcd.Hour%16;
      timeBit[m++]=11;//:
      timeBit[m++]=time_bcd.Minute/16;
      timeBit[m++]=time_bcd.Minute%16;
   }
   
  //y���������Сֵ
  xxmin=xxmax=StateC.PrintXB[0];
  for(j=0;j<4;j++)
  {
    //ȡ5�����ߵ��е������Сֵ
    xxmin=get_min(xxmin,StateC.PrintXB[j+1]);
    xxmax=get_max(xxmax,StateC.PrintXB[j+1]);
  }
  
  //�Ѵ�ӡ��2����zv1,zv2ת��Ϊyֵ,������ϵ(xxmin,xxmax)/(33,369)
  y1=GetZvY(xxmin,xxmax,33,369,zv1);
  y2=GetZvY(xxmin,xxmax,33,369,zv2);
  ybc=y1;//ǰһ����λ��=��һ����λ��
  
  xspt=0;
  xsfg=0;
  xjpt=0;
  
  for(k=0;k<12;k++)//12λʱ��������,��96��
  {
        for(i=0;i<8;i++)
        {
          //ÿһ�����߻���
          //��������-----------------------------------------------------------
          
          if(((zv1==SENIOR_NULL)||(zv1==HAND_STOP))||((zv2==SENIOR_NULL)||(zv2==HAND_STOP)))//���������ݲ���ӡʵʱ����
          {
            ynn=0;
          }
          else
          {
              //��2�����߼�����һ��λ���yֵ
              xjpt++;
              ft1=(((float)y2-y1)/96)*xjpt+y1;
              yy=(u16)(ft1*10);
              if((yy%10)>4){yy+=10;}
              yy=yy/10;
              
              if(yy>ybc)//��2��λ��������������
                ynn=yy-ybc;
              else
                ynn=ybc-yy;
              if(ynn==0)
                ynn=1;
          }
          
          qxnn=ynn;//��������

          //ʱ��-����
          if(timeEn)
          {
            potnn=GetOneHangForm16X8(A16x8[timeBit[k]][i],potbuf);//ȡһ��,���ַ�(1/8)
          }
          else
          {
            potnn=0;
          }
          qxnn+=potnn;

          //x��
          qxnn+=1;
          
          //����-����
          if(xspt%4==0)//ÿ4����任һ������-Ч����������
          {
            xsfg=!xsfg;
          }
          if(xsfg)
          {
            qxnn+=5;
          }
          if(++xspt>3)
          {
            xspt=0;
          }
          
          //���һ����,��ʾx��ʱ���Ϻ����������ߣ�ÿ������4����
          if(k==11&&i==7)
          {
            if(timeEn)
              qxnn+=8;
            else
              qxnn+=4;
          }
          
          //��ӡ1������------------------------------------------------------------
          U1SendByte(0x1b);
          U1SendByte(0x27);
          U1SendByte(GetL8(qxnn));//һ�����ӡ�ĵ���
          U1SendByte(GetH8(qxnn)); 
          
          //2�����ݼ�����
          for(j=0;j<ynn;j++)
          {
            if(yy>y1)
            {
              ytm=ybc+j;
            }
            else
            {
              ytm=yy+j;
            }
            
            U1SendByte(GetL8(ytm));
            U1SendByte(GetH8(ytm));
          }

          if(ynn>0)
          {
            if(k==11&&i==7)
            {
              U1SendByte(GetL8(ytm-2));//ʵʱ����������
              U1SendByte(GetH8(ytm-2));
              U1SendByte(GetL8(ytm-1));
              U1SendByte(GetH8(ytm-1));
              U1SendByte(GetL8(ytm+1));
              U1SendByte(GetH8(ytm+1));
              U1SendByte(GetL8(ytm+2));
              U1SendByte(GetH8(ytm+2));
            }
            
            ybc=yy;//ǰһ����λ��
          }

          //ʱ��
          for(j=0;j<potnn;j++)
          {
            U1SendByte(GetL8(potbuf[j]));
            U1SendByte(GetH8(potbuf[j]));
          }
          
          //x��
          if(k==11&&i==7&&timeEn)
          {
              U1SendByte(GetL8(17));
              U1SendByte(GetH8(17));
              U1SendByte(GetL8(18));
              U1SendByte(GetH8(18));
              U1SendByte(GetL8(19));
              U1SendByte(GetH8(19));
              U1SendByte(GetL8(20));
              U1SendByte(GetH8(20));
              U1SendByte(GetL8(21));
              U1SendByte(GetH8(21));
          }
          else
          {
              U1SendByte(GetL8(19));
              U1SendByte(GetH8(19));
          }
          
          //����
          if(xsfg)
          {
            for(j=0;j<5;j++)
            {
              ytm=GetZvY(xxmin,xxmax,33,369,StateC.PrintXB[j]);//���ߵ�λ
              U1SendByte(GetL8(ytm));
              U1SendByte(GetH8(ytm));
            }
          }
          //ִ�б��д�ӡ--------------------------------------------------------
          U1SendByte(0x0d);
        }
  }
}
#endif//PRINT_REAL_EN


//#if PRINT_NEW_EN==1//================================================================
#if ((PRINT_NEW_EN==1)&&(GPRS_TC_EN==0))

void PrintNewToEE(void)
{
  u8 buf[2];
  
  buf[0]=0xff&Print.NewNum;
  buf[1]=0xff&(Print.NewNum>>8);
  I2C_EE_BufferWrite(buf,PRINT_NEW_NUM_ADDR,2,0);
  
  buf[0]=0xff&Print.NewPt;
  buf[1]=0xff&(Print.NewPt>>8);
  I2C_EE_BufferWrite(buf,PRINT_NEW_PT_ADDR,2,0);
}
void PrintNewFromEE(void)
{
  u8 buf[2];
  
  I2C_EE_BufferRead(buf,PRINT_NEW_NUM_ADDR,2,0);
  Print.NewNum=U8_TO_U16(buf[1],buf[0]);
  
  I2C_EE_BufferRead(buf,PRINT_NEW_PT_ADDR,2,0);
  Print.NewPt=U8_TO_U16(buf[1],buf[0]);
}
void PrintNewClr(void)
{
  Print.NewNum=0;
  Print.NewPt=0;
  PrintNewToEE();
}

void PrintDataSave(void)
{
      u8 buf[PRINT_CH_BYTE];
      u16 PrintZv[CH_NUM];
      u16 EE_16Pt;
      u16 i=0,j;

      //ȡ��ǰ��¼�����ݣ���������
      for(j=0;j<CH_NUM;j++)
      {
          PrintZv[j]=StateC.Value[j];
          if(0x01&(StateC.FuhaoBit>>j))//����
          {
            PrintZv[j]=StateC.Value[j]|0x8000;
          }
      }
      //�ֶ�ͣ���򴫸������ź�ʱ����ӡ
      
     #if CH_NUM==1 
     if((PrintZv[0]==SENIOR_NULL)||(PrintZv[0]==HAND_STOP))
        return;
     #endif
      
      #if BSQ_ENABLE==1
      buf[i++]=0xff&Rtc.SB;
      buf[i++]=0xff&(Rtc.SB>>8);
      buf[i++]=0xff&(Rtc.SB>>16);
      buf[i++]=0xff&(Rtc.SB>>24);//ʱ��ռ4��
      #else
      buf[i++]=0xff&StateA1.RecordNum;
      buf[i++]=0xff&(StateA1.RecordNum>>8);
      buf[i++]=0xff&(StateA1.RecordNum>>16);
      buf[i++]=0xff&(StateA1.RecordNum>>24);//ʱ��ռ4��
      #endif
      
      for(j=0;j<CH_NUM;j++)
      {
        buf[i++]=0xff&PrintZv[j];
        buf[i++]=0xff&(PrintZv[j]>>8);//����ռ2���ֽ� FF FF
      }
  
      //��¼��ַ��PRINT_CH1_ADDR��ʼ
      EE_16Pt=Print.NewPt;
      EE_16Pt=EE_16Pt*i;
      EE_16Pt=EE_16Pt+PRINT_CH1_ADDR;
            
      //��¼һ��
     I2C_EE_BufferWrite(buf,EE_16Pt,i,0);
            
     //��¼����
     Print.NewPt++;
     if(Print.NewPt>=PRINT_NEW_CAP)//�������
       Print.NewPt=0;
     
     //��¼����
     if(Print.NewNum<PRINT_NEW_CAP)
       Print.NewNum++;
     else
       Print.NewNum=PRINT_NEW_CAP;
            
     PrintNewToEE();//���������EE
     
     NOP;
}

#if PRINT_MIN_MAX_EN==1

//ͨ����, ��͵ĸ���,�����Сƽ��
void Print_Max_Min_Avg(u8 ch,u16 cc)
{
                      
                      
                      U1SendString("ͨ��");U1SendByte(0x30+ch+1);U1SendByte(0x0a);
                      /*
                      u16 buf[2];
                      GetCurveRange(ch,buf);//��ȡͨ�������������ޣ�������������
                      U1SendString("�趨��Χ����");
                      PrintValue(buf[0]);//��������
                      U1SendString(",");
                      PrintValue(buf[1]);//��������
                      U1SendString("��");
                      U1SendByte(0x0a);
                      */
                      
                      u16 avg;
                      
                      U1SendString("���");
                      PrintValue(StateC.print_max[ch]);//��ֵ
                      U1SendByte(',');
                      U1SendString("��С");
                      PrintValue(StateC.print_min[ch]);//��ֵ
                      U1SendByte(',');
                      
                      avg=get_avg(StateC.print_sum[ch],cc);
                      U1SendString("ƽ��");
                      PrintValue(avg);//��ֵ
                      U1SendByte(0x0a);  
}
#endif



void PrintNewData(void)
{
        u32 tt,EE_32Pt,Current32Pt;
        u16 i,j,k,m,dd,DownN,BagMaxRecN,BagN,BagRecN,BagDataBytes;
        
        #if PRINT_INFOR_EN==1
        u32 time_begin,time_end;
        #endif
        
        #if PRINT_MIN_MAX_EN==1
        u16 cc[CH_NUM],ch=0;
        for(j=0;j<CH_NUM;j++)
          cc[j]=0;
        #endif
        
        #if PRINT_NEW_CURVE_EN==1
        u8 cpt=0;
        u8 PrintCV[200];//���100������
        #endif
        
        
        //��ӡ��ʼ��ʾ------------------------------------------------------
        PrintStart();
        SetIni();
        delay_ms(2);
        SetCnPrint(1);//����ģʽ
        delay_ms(2);
        LcdNN(PRINTING);
        
      #if PRINT_INFOR_EN==1
        U1SendString("*********��ӡ��ʼ***************\n");
      #else
        //ֻ��ӡ�û����ͱ���
        GoOneHang(1);
        U1SendString("�� �� ����");
        i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
        U1SendByte(0x0a);
        U1SendString("��    �⣺");
        i2c_ee_read_to_pc(TITLE_ADDR,20,0);
        U1SendByte(0x0a);
        GoOneHang(1);
        
      #endif
        
        //ȡ������Ϣ
        if(Print.NewNum>PRINT_NEW_CAP)
          Print.NewNum=PRINT_NEW_CAP;
        
        BagMaxRecN=(u16)70/PRINT_CH_BYTE;//ÿ��������� zz
        DownN=Print.NewNum;//����������
        if(DownN>=PRINT_NEW_CAP)//��¼����
          DownN=PRINT_NEW_CAP;
        BagN=DownN/BagMaxRecN;//�ܰ���
        if(DownN%BagMaxRecN!=0)
          BagN++;
        
        if(DownN>0)//������
        {
          #if PRINT_ALL_CH_EN==1//��ӡȫ��ͨ��
          for(j=0;j<CH_NUM;j++)
          {
              u8 ChT;//ͨ������
              ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//��ȡ��ͨ������
              
                //ͨ����.......................................................
              U1SendString("ͨ��");
              U1SendByte(0x31+j);
              U1SendByte(':');
              //ͨ������......................................................
              if(ChT==_T_T)//Temp/�¶�
              {
                 U1SendString("�¶�(��)");
              }
              else if(ChT==_T_H)//Humi/ʪ��
              {
                 U1SendString("ʪ��(%RH)");
              }
              if(j%2==1)
              {
                //U1SendByte(0x0d);
                U1SendByte(0x0a);
              }
          }
          U1SendByte(0x0a);
          
          #elif PRINT_4_EN==1
          for(j=0;j<4;j++)
          {
              u8 ChT;//ͨ������
              ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//��ȡ��ͨ������
              
                //ͨ����.......................................................
              U1SendString("ͨ��");
              U1SendByte(0x31+j);
              U1SendByte(':');
              //ͨ������......................................................
              if(ChT==_T_T)//Temp/�¶�
              {
                 U1SendString("�¶�(��)");
              }
              else if(ChT==_T_H)//Humi/ʪ��
              {
                 U1SendString("ʪ��(%RH)");
              }
              if(j%2==1)
              {
                //U1SendByte(0x0d);
                U1SendByte(0x0a);
              }
          }
          U1SendByte(0x0a);
          
          #elif PRINT_1_3_EN==1
            U1SendString("����    ʱ��   �¶�1(��) �¶�3(��)\n");
          
          #elif PRINT_2WD_EN==1
           U1SendString("����    ʱ��   �¶�1(��) �¶�2(��)\n");
          #elif PRINT_1W1S_EN==1
           U1SendString("����    ʱ��   �¶ȡ� ʪ��%RH\n");
          
          #else

                   U1SendString("����    ʱ��     �¶ȣ��棩\n");

           

          #endif
        }
        else
        {
          U1SendString("û������\n");
          GoOneHang(6);
          
          //��ӡ����------------------------------------------------------
          PrintEnd();
          return;
        }
        
        //��ʼ���ذ�
        #if PRINT_NEW_CURVE_EN==1
        cpt=0;
        #endif
        for(j=0;j<BagN;j++)
        {
             //��ӡǰÿ�����1.2s, ����쿻ʹ�ӡ�Ỻ���ҵ�
             WDT_CLR;
             delay_ms(400);//zz
             WDT_CLR;
             delay_ms(400);
             WDT_CLR;
             delay_ms(400);
             WDT_CLR;
             delay_ms(400);
             WDT_CLR;
             delay_ms(400);
             WDT_CLR;
             
            //ȡÿ������
            if((j+1==BagN)&&(DownN%BagMaxRecN!=0))//���һ�����Ҳ���һ��
            {
              BagRecN=DownN%BagMaxRecN;
            }
            else
            {
              BagRecN=BagMaxRecN;//����
            }
            BagDataBytes=BagRecN*PRINT_CH_BYTE;
            
            i=0;
            if(DownN<PRINT_NEW_CAP)//��¼δ��
            {
                 EE_32Pt=(u32)j*(BagMaxRecN*PRINT_CH_BYTE);
                 EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                 I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,BagDataBytes,0);
            }
            else//��¼����
            {
                if(j==0)
                  Current32Pt=(u32)Print.NewPt*PRINT_CH_BYTE;//��1��λ��
                EE_32Pt=Current32Pt+(u32)j*(BagMaxRecN*PRINT_CH_BYTE);
                EE_32Pt=EE_32Pt%((u16)PRINT_NEW_CAP*PRINT_CH_BYTE);
                
                if(EE_32Pt+BagDataBytes<=(u32)PRINT_NEW_CAP*PRINT_CH_BYTE)
                {
                  EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                  I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,BagDataBytes,0);
                }
                else//���
                {
                  u32 TmpPt;
                  TmpPt=(EE_32Pt+BagDataBytes)%((u32)PRINT_NEW_CAP*PRINT_CH_BYTE);
                  
                  EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                  I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,BagDataBytes-TmpPt,0);  
                  EE_32Pt=PRINT_CH1_ADDR;
                  I2C_EE_BufferRead(&Uart_1.RxBuf[i+BagDataBytes-TmpPt],EE_32Pt,TmpPt,0);  
                }
            }
/*************************************
<=2ͨ��
131203 13:52:40   11.7  53.6
>2ͨ��
131203 13:52:40
11.7  53.6 11.7  53.6
11.7  53.6 11.7  53.6
********************************/
            
            //��1�����ݴ�ӡ,ÿ����������BagRecN
            for(k=0;k<BagRecN;k++)
            {
                delay_ms(2);//����
                WDT_CLR;
                
                tt=U8_TO_U32(Uart_1.RxBuf[k*PRINT_CH_BYTE+3],Uart_1.RxBuf[k*PRINT_CH_BYTE+2],Uart_1.RxBuf[k*PRINT_CH_BYTE+1],Uart_1.RxBuf[k*PRINT_CH_BYTE]);
                PrintTime(tt);//ʱ��
                
                #if PRINT_INFOR_EN==1
                if((j==0)&&(k==0))
                  time_begin=tt;//��1��
                if((j==BagN-1)&&(k==BagRecN-1))
                  time_end=tt;//���1��
                #endif
                
                #if PRINT_ALL_CH_EN==1//��ӡȫ��ͨ��-------------------------------------
                if(CH_NUM>2)
                {
                  U1SendByte(0x0a);//����
                }
                
                for(m=0;m<CH_NUM;m++)
                {
                  U1SendByte(' ');
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//��ֵ
                  if((CH_NUM>4)&&(m==3))//1�������ʾ4��ͨ��
                  {
                    U1SendByte(0x0a);
                  }
                  
                  #if PRINT_MIN_MAX_EN==1
                  ch=m;
                  if(cc[ch]==0)
                  {
                    StateC.print_sum[ch]=StateC.print_min[ch]=StateC.print_max[ch]=dd;
                  }
                  else
                  {
                    StateC.print_min[ch]=get_min(StateC.print_min[ch],dd);
                    StateC.print_max[ch]=get_max(StateC.print_max[ch],dd);

                    StateC.print_sum[ch]=get_mum(StateC.print_sum[ch],dd);
                  }
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//�������ų�NULL
                    cc[ch]++;
                  
                  #endif
                  
                  
                }
                
                
                #elif PRINT_4_EN==1
                if(CH_NUM>2)
                {
                  U1SendByte(0x0a);//����
                }
                
                for(m=0;m<4;m++)
                {
                  U1SendByte(' ');
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//��ֵ

                  
                  #if PRINT_MIN_MAX_EN==1
                  ch=m;
                  if(cc[ch]==0)
                  {
                    StateC.print_sum[ch]=StateC.print_min[ch]=StateC.print_max[ch]=dd;
                  }
                  else
                  {
                    StateC.print_min[ch]=get_min(StateC.print_min[ch],dd);
                    StateC.print_max[ch]=get_max(StateC.print_max[ch],dd);

                    StateC.print_sum[ch]=get_mum(StateC.print_sum[ch],dd);
                  }
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//�������ų�NULL
                    cc[ch]++;
                  
                  #endif
                  
                  
                }
                
                #elif PRINT_2WD_EN==1//G95-4P,��ӡ�¶�1���¶�2����Ӧͨ��1��ͨ��3-------------------------------
                  U1SendByte(' ');
                  m=0;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//��ֵ
                  
                  #if PRINT_MIN_MAX_EN==1
                  ch=m;
                  if(cc[ch]==0)
                  {
                    StateC.print_sum[ch]=StateC.print_min[ch]=StateC.print_max[ch]=dd;
                  }
                  else
                  {
                    StateC.print_min[ch]=get_min(StateC.print_min[ch],dd);
                    StateC.print_max[ch]=get_max(StateC.print_max[ch],dd);

                    StateC.print_sum[ch]=get_mum(StateC.print_sum[ch],dd);
                  }
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//�������ų�NULL
                    cc[ch]++;
                  
                  #endif
                  
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  
                  
                  #if ((JLY_MODEL==_3_NTC)||(JLY_MODEL==_2_NTC)||(JLY_MODEL==_4_NTC))
                    m=1;
                  #else
                    m=2;                
                  #endif
                  
                  
                     
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//��ֵ
                  
                  #if PRINT_MIN_MAX_EN==1
                  ch=m;
                  if(cc[ch]==0)
                  {
                    StateC.print_sum[ch]=StateC.print_min[ch]=StateC.print_max[ch]=dd;
                  }
                  else
                  {
                    StateC.print_min[ch]=get_min(StateC.print_min[ch],dd);
                    StateC.print_max[ch]=get_max(StateC.print_max[ch],dd);

                    StateC.print_sum[ch]=get_mum(StateC.print_sum[ch],dd);
                  }
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//�������ų�NULL
                    cc[ch]++;
                  
                  #endif
                  
                  
                #elif PRINT_1W1S_EN==1//��Ӧͨ��1�¶Ⱥ�ͨ��2ʪ��------------------------------------------
                  U1SendByte(' ');
                  m=0;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//��ֵ
                  
                  #if PRINT_MIN_MAX_EN==1
                  ch=m;
                  if(cc[ch]==0)
                  {
                    StateC.print_sum[ch]=StateC.print_min[ch]=StateC.print_max[ch]=dd;
                  }
                  else
                  {
                    StateC.print_min[ch]=get_min(StateC.print_min[ch],dd);
                    StateC.print_max[ch]=get_max(StateC.print_max[ch],dd);

                    StateC.print_sum[ch]=get_mum(StateC.print_sum[ch],dd);
                  }
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//�������ų�NULL
                    cc[ch]++;
                  
                  #endif
                  
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  
                  m=1;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//��ֵ
                  
                  #if PRINT_MIN_MAX_EN==1
                  ch=m;
                  if(cc[ch]==0)
                  {
                    StateC.print_sum[ch]=StateC.print_min[ch]=StateC.print_max[ch]=dd;
                  }
                  else
                  {
                    StateC.print_min[ch]=get_min(StateC.print_min[ch],dd);
                    StateC.print_max[ch]=get_max(StateC.print_max[ch],dd);

                    StateC.print_sum[ch]=get_mum(StateC.print_sum[ch],dd);
                  }
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//�������ų�NULL
                    cc[ch]++;
                  
                  #endif
                  
                
                
                #else//ֻ��ӡͨ��1----------------------------------------------------------------------
                  U1SendByte(' ');
                  m=0;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//��ֵ
                  
                  #if PRINT_MIN_MAX_EN==1
                  ch=0;
                  if(cc[ch]==0)
                  {
                    StateC.print_sum[ch]=StateC.print_min[ch]=StateC.print_max[ch]=dd;
                  }
                  else
                  {
                    StateC.print_min[ch]=get_min(StateC.print_min[ch],dd);
                    StateC.print_max[ch]=get_max(StateC.print_max[ch],dd);

                    StateC.print_sum[ch]=get_mum(StateC.print_sum[ch],dd);
                  }
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//�������ų�NULL
                    cc[ch]++;
                  
                  #endif
                  
                  #if PRINT_NEW_CURVE_EN==1
                  PrintCV[cpt++]=Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2];
                  PrintCV[cpt++]=Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2];
                  #endif
                
                #endif
              
                //U1SendByte(0x0d);
                U1SendByte(0x0a);
            }

        }
        

        
        #if PRINT_NEW_CURVE_EN==1
        PrintCurve(PrintCV,cpt/2,0);//��ӡͨ��1����
        #endif
        
        
        //zz
#if PRINT_INFOR_EN==1
  U1SendString("*********��ӡ��Ϣͳ��***********\n");
  U1SendString("�� �� ����");
  i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("��    �⣺");
  i2c_ee_read_to_pc(TITLE_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("S      N��");
  i2c_ee_read_to_pc(SERIAL_NO_ADDR,10,0);
  U1SendByte(0x0a);
  U1SendString("��ʼʱ�䣺");
  PrintTime(time_begin);
  U1SendByte(0x0a);
  U1SendString("����ʱ�䣺");
  PrintTime(time_end);
  U1SendByte(0x0a);
  U1SendString("��¼�����");
  PrintJianGe(PRINT_JIANGE);
  U1SendByte(0x0a);
  U1SendString("��¼������");
  U1SendValue(DownN);
  U1SendByte(0x0a);
#endif
        
        #if PRINT_MIN_MAX_EN==1
          GoOneHang(1);
        
          #if PRINT_ALL_CH_EN==1
        
                  for(j=0;j<CH_NUM;j++)
                  {
                      ch=j;
                      Print_Max_Min_Avg(ch,cc[ch]);
                  }
          #elif PRINT_4_EN==1        
                  for(j=0;j<4;j++)
                  {
                      ch=j;
                      Print_Max_Min_Avg(ch,cc[ch]);
                  }
                  
          #elif PRINT_2WD_EN==1//G95-4P,��ӡ�¶�1���¶�2����Ӧͨ��1��ͨ��3         
                      ch=0;
                      Print_Max_Min_Avg(ch,cc[ch]);
                      
                      #if ((JLY_MODEL==_3_NTC)||(JLY_MODEL==_2_NTC)||(JLY_MODEL==_4_NTC))
                        ch=1;
                      #else
                        ch=2;                    
                      #endif
                      
                      
                      Print_Max_Min_Avg(ch,cc[ch]);
                  
          #elif PRINT_1W1S_EN==1//��Ӧͨ��1�¶Ⱥ�ͨ��2ʪ��         
                      ch=0;
                      Print_Max_Min_Avg(ch,cc[ch]);
                      ch=1;
                      Print_Max_Min_Avg(ch,cc[ch]);
          #else
                  ch=0;
                  Print_Max_Min_Avg(ch,cc[ch]);

          #endif
        #endif
        
        GoOneHang(1);
        U1SendString("********************************\n");
        
      #if RPINT_B_EN==1
        U1SendString("��Ч��ʼʱ�䣺                \n\n");
        GoOneHang(1);
        U1SendString("��Ч����ʱ�䣺                \n\n");
        GoOneHang(1);
        U1SendString("��  ��  ��  ��                \n\n");
        GoOneHang(1);
        U1SendString("��  ��  ��  ��                \n\n");
        GoOneHang(6);
        
     #elif RPINT_A_EN==1
        U1SendString("�ջ�����                \n\n");
        GoOneHang(1);
        U1SendString("�˵��ţ�                \n\n");
        GoOneHang(1);
        U1SendString("���ƺţ�                \n\n");
        GoOneHang(1);
        U1SendString("�ջ��ˣ�                \n\n");
        GoOneHang(1);
        U1SendString("�����ˣ�                \n\n");
        GoOneHang(1);
        U1SendString("ʱ  �䣺                \n\n");
        GoOneHang(6);
        
      #elif PRINT_D_EN==1  
        U1SendString("�ջ�����                \n\n");
        GoOneHang(1);
        U1SendString("�ջ��ˣ�                \n\n");
        GoOneHang(1);
        U1SendString("�����ˣ�                \n\n");
        GoOneHang(6);
        
                    #elif PRINT_E_EN==1   
                        
                        U1SendString("��ҵ���ƣ�              \n\n");
                        GoOneHang(1);
                        U1SendString("�豸���ƣ�              \n\n");
                        GoOneHang(1);
                        U1SendString("�ͻ����ƣ�              \n\n");
                        GoOneHang(1);
                        U1SendString("Ʒ    ����              \n\n");
                        GoOneHang(1);
                        U1SendString("�� �� ����              \n\n");
                        GoOneHang(1);
                        U1SendString("�� ʻ Ա��              \n\n");
                        GoOneHang(1);
                        U1SendString("�� �� �ˣ�              \n\n");
                        GoOneHang(6);
        
      #elif PRINT_C_EN==1
                        U1SendString("��ӡ���ڣ�");
                        Rtc.SS=ReadRtcD10();
                        PrintTime(Rtc.SS);
                        
                        GoOneHang(1);
                        U1SendString("ǩ��ȷ�ϣ�                \n\n");
                        GoOneHang(6);

      #else
        
        U1SendString("ǩ�֣�         ʱ�䣺           \n\n");
        GoOneHang(1);
        U1SendString("��ע��\n\n");
        GoOneHang(6);
      #endif
        
        //��ӡ����------------------------------------------------------
        PrintEnd();
}



#endif//PRINT_NEW_EN

//��¼һ���㣬��ӡһ�����ߵ�
#if PRINT_REAL_EN==1//-------------------------------------------------------------
void PrintOneCurve(void)
{
      u16 PrintZv;
      
      //ȡ��ǰ��¼�����ݣ���������
      PrintZv=StateC.Value[0];
      if(0x01&(StateC.FuhaoBit>>0))//����
      {
        PrintZv=StateC.Value[0]|0x8000;
      }
      
      //�ֶ�ͣ���򴫸������ź�ʱ����ӡʵʱ����
      if((PrintZv==SENIOR_NULL)||(PrintZv==HAND_STOP))
        return;
      
      PrintStart();//��ӡ׼��------------------------------------------------------
      SetIni();
      delay_ms(2);
      SetCnPrint(1);
      delay_ms(2);

      if(StateA1.RecordNum==1)
        StateC.HaveBc=0;
      
      if(StateC.HaveBc==0)
      {
        StateC.ValueBc=PrintZv;
        PrintY5Zv();//��ӡ5��y��Ⱥ�y��
      }
  
      PrintCC(StateC.ValueBc,PrintZv,StateA1.RecordNum,((StateA1.RecordNum%5)==1));//ÿ5������ʾһ��ʱ��
      
      StateC.ValueBc=PrintZv;//������һ����
      StateC.HaveBc=1;
      
      PrintEnd();//��ӡ����-------------------------------------------------------  
}
#endif

//��¼һ������ӡһ���б�
#if PRINT_RT_LIST_EN==1//-------------------------------------------------------
void PrintOneList(void)
{
      
      #if PRINT_RT_AL_EN==1
      if(ChAlarm()==0)//û��ͨ�����꣬����ӡ
        return;
      #endif
  
      u16 j,PrintZv[CH_NUM];
      
      //ȡ��ǰ��¼�����ݣ���������
      for(j=0;j<CH_NUM;j++)
      {
        PrintZv[j]=StateC.Value[j];
        if(0x01&(StateC.FuhaoBit>>j))//����
        {
          PrintZv[j]=StateC.Value[j]|0x8000;
        }
      }
      
      //�ֶ�ͣ���򴫸������ź�ʱ����ӡʵʱ����
      if((PrintZv[0]==SENIOR_NULL)||(PrintZv[0]==HAND_STOP))
        return;
      
      PrintStart();//��ӡ׼��------------------------------------------------------
      SetIni();
      delay_ms(2);
      SetCnPrint(1);
      delay_ms(2);
      
      #if BSQ_ENABLE==0
      if(StateA1.RecordNum==1)
      {
            U1SendString("�û�����");
            i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
            U1SendByte(0x0a);
            U1SendString("��  �⣺");
            i2c_ee_read_to_pc(TITLE_ADDR,20,0);
            U1SendByte(0x0a);
            GoOneHang(1);
            #if PRINT_RT_LIST_BK_SD==1
            U1SendString("����    ʱ��     �¶ȣ��棩 ʪ�ȣ�%RH��\n");
            #elif (PRINT_RT_LIST_BK_2WD==1)
            U1SendString("����    ʱ��     �¶�1���棩 �¶�2���棩\n");
            #elif PRINT_RT_ALL==1
              for(j=0;j<CH_NUM;j++)
              {
                  u8 ChT;//ͨ������
                  ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//��ȡ��ͨ������
                  
                    //ͨ����.......................................................
                  U1SendString("ͨ��");
                  U1SendByte(0x31+j);
                  U1SendByte(':');
                  //ͨ������......................................................
                  if(ChT==_T_T)//Temp/�¶�
                  {
                     U1SendString("�¶�(��)");
                  }
                  else if(ChT==_T_H)//Humi/ʪ��
                  {
                     U1SendString("ʪ��(%RH)");
                  }
                  if(j%2==1)
                  {
                    //U1SendByte(0x0d);
                    U1SendByte(0x0a);
                  }
              }
            
            #else
              

                   U1SendString("����    ʱ��     �¶ȣ��棩\n");

     
            #endif
      }
      PrintTime(StateA1.RecordNum);//ʱ��
      #else
      PrintTime(Rtc.SB);
      #endif

      #if ((PRINT_RT_LIST_BK_SD==1)||(PRINT_RT_LIST_BK_2WD==1))
        U1SendByte(' ');
        PrintValue(PrintZv[0]);//��ֵ
        U1SendByte(' ');
        PrintValue(PrintZv[1]);//��ֵ,ʪ��
      #elif PRINT_RT_ALL==1
        if(CH_NUM>2)
        {
           U1SendByte(0x0a);//����
        }
                
        for(j=0;j<CH_NUM;j++)
        {
           U1SendByte(' ');
           PrintValue(PrintZv[j]);//��ֵ
           if((CH_NUM>4)&&(j==3))//1�������ʾ4��ͨ��
           {
              U1SendByte(0x0a);
           }
        }
      
      #else
      U1SendByte(' ');
      PrintValue(PrintZv[0]);//��ֵ
      #endif
      U1SendByte(0x0a);
      
      PrintEnd();//��ӡ����-------------------------------------------------------
}
#endif

//===================================================================================================================

#if GPRS_TC_EN==1

#if PRINT_MIN_MAX_EN==1
//������С
void Print_MaxMin(u8 ch)
{

                      U1SendString("ͨ��");U1SendByte(0x30+ch+1);U1SendByte(0x0a);
                      
                      U1SendString("���");
                      PrintValue(StateC.print_max[ch]);//��ֵ
                      U1SendByte(',');
                      U1SendString("��С");
                      PrintValue(StateC.print_min[ch]);//��ֵ


                      U1SendByte(0x0a);  
}
#endif

//buf: RTC(4)+BAT(1)+GPS(8)+2*CH_NUM
//��buf��ȡ�����ݲ���ת��Ϊ��ӡ��ʽ�����ݣ����͵�����U1
/*************************************
1��2ͨ����
131203 13:52:40   11.7  53.6
>2ͨ����
131203 13:52:40
11.7  53.6 11.7  53.6
11.7  53.6 11.7  53.6
********************************/
void Buf_To_Print(u8* buf)
{
    u32 tt;
    u16 j,vv;
    u8 cc=0;//�����ű���ж�
    
    
    #if ((ON_OFF_BJ_EN==1)||(DOOR1_BJ_EN==1)||(DOOR2_BJ_EN==1)||(PRINT_NO_NULL_EN==1))
    
        #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
        vv=U8_TO_U16(buf[13],buf[14]);
        #else
        vv=U8_TO_U16(buf[5],buf[6]);
        #endif
        
   #endif
    
    
    #if ON_OFF_BJ_EN==1
        if((vv==KJ_BJ)||(vv==GJ_BJ)||(vv==SD_BJ)||(vv==DD_BJ))//�������ػ����ϵ硢�ϵ��ǲ���ӡ
        {
          return;
        }
    #endif
        
    #if (PRINT_NO_NULL_EN==1)   
        if(vv==SENIOR_NULL)
        {
          return; 
        }
    #endif
        
        
    #if ((DOOR1_BJ_EN==1)||(DOOR2_BJ_EN==1))
        
        #if DOOR_PRINT_EN==1
        if((vv==DOOR1_OPEN)||(vv==DOOR1_CLOSE)||(vv==DOOR2_OPEN)||(vv==DOOR2_CLOSE))
        {
          cc=1;//��Ҫ��ӡ
        }  
        #else

        if((vv==DOOR1_OPEN)||(vv==DOOR1_CLOSE)||(vv==DOOR2_OPEN)||(vv==DOOR2_CLOSE))//�ſ��ز���ӡ
        {
          return;
        }   
       #endif
        
        
    #endif
    
    
        
    tt=U8_TO_U32(buf[0],buf[1],buf[2],buf[3]);

    #if GPRS_PRT_JG_EN==1
    if(PcBsq.RecTime<=60)//ֻ�м�¼���<=1����ʱ�����ж�
    {
       #if PRINT_5_2_PER_EN==1
        u8 alarm=0;
        for(j=0;j<CH_NUM;j++)
        {
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
          
            alarm|=AlarmXSeek(vv,j,1);
        }
        
        if(alarm)//�����ݳ���
        {
           if(((tt%120)!=0)&&(cc==0))//2��������Ŵ�ӡ
              return;
        }
        else
        {
           if(((tt%PRINT_SF_TIME)!=0)&&(cc==0))//5��������Ŵ�ӡ
              return;
        }
        
      
       #else
           if(((tt%PRINT_SF_TIME)!=0)&&(cc==0))//5��������Ŵ�ӡ
              return;
       #endif
    }
    #endif
    
      
    PrintTime(tt);//ʱ��
    U1SendByte(' ');
    

    #if RPTINT_ONLY_WD_EN==1
    
        #if ((JLY_MODEL==_2_NTC)||(JLY_MODEL==_3_NTC)||(JLY_MODEL==_4_NTC)||(JLY_MODEL==_3_PT_F100_200)||(JLY_MODEL==_8_134678NTC_25HIH))
        for(j=0;j<CH_NUM;j++)
        #elif ((JLY_MODEL==_5_4NTC_1HIH)||(JLY_MODEL==_4_3NTC_1HIH)||(JLY_MODEL==_3_2_NTC_1HIH))
        for(j=0;j<CH_NUM-1;j++)
        #else
        for(j=0;j<CH_NUM;j=j+2)
        #endif
        {
          
            #if (JLY_MODEL==_8_134678NTC_25HIH)
            if(!(_U_OC==*((char*)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES)))
              continue;
            #endif
          
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//��ֵ
            
            
            //����ǿ����ű�ǣ��������������Сֵ����������ͨ��������
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//����ͨ��������
            #endif

            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
            
            
            #if (JLY_MODEL==_8_134678NTC_25HIH)
            if((CH_NUM>4)&&(j==5))//1�������ʾ4��ͨ��
            {
              U1SendByte(0x0a);
              U1SendString("         ");
            }
            #endif
  
        }
        
    #elif PRINT_FZ_EN==1//�����ӡ 
        
        for(j=Pt.prt_x*2-2;j<Pt.prt_x*2;j++)
        {
            if(j>CH_NUM-1)
              break;
          
          
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//��ֵ
            
            
            //����ǿ����ű�ǣ��������������Сֵ����������ͨ��������
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//����ͨ��������
            #endif

            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
            
            
            #if PRINT_FZ_WD_EN==1
            j=j+1;
            #endif
  
        }
    
    
    #elif PRINT_ALL_CH_EN==1
    
        //ȫ��ͨ����ӡ
        if(CH_NUM>2)
        {
            U1SendByte(0x0a);//����
        }
        
        for(j=0;j<CH_NUM;j++)
        {
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//��ֵ
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//����ͨ��������
            #endif
                
            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
            
            
            if((CH_NUM>4)&&(j==3))//1�������ʾ4��ͨ��
            {
              U1SendByte(0x0a);
            }
    
        }
        
    #elif PRINT_1W1S_EN==1
        
        for(j=0;j<2;j++)//ֻ��ӡǰ����¶�1��ʪ��1
        {
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//��ֵ
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//����ͨ��������
            #endif
            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif

        }
    #elif PRINT_2WD_EN==1    
        #if ((JLY_MODEL==_3_NTC)||(JLY_MODEL==_2_NTC)||(JLY_MODEL==_4_NTC))
        for(j=0;j<2;j++)
        #else
        for(j=0;j<=2;j=j+2)//ֻ��ӡǰ����¶�1��ʪ��1
        #endif
        {
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);
            #endif
            
            PrintValue(vv);//��ֵ
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//����ͨ��������
            #endif
            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
            
        }
        
    
    #else//ֻ��ӡ�¶�1
        
        for(j=0;j<1;j++)
        {
            j=0;
        
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//��ֵ
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//����ͨ��������
            #endif
            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
        }
        
    #endif
    
    
    U1SendByte(0x0a);
    
}


//��ѭ���У�1s��ӡһ������
u8 PrintDeel(void)
{ 
        //2���ӡһ��
        if(Pt.PrintFG==1)
          Pt.PrintFG=0;
        else
          Pt.PrintFG=1;
        

  
        if(Pt.PrintThisDueout>0)
        {
            u8 PerCC;
                        
            if(Pt.PrintFG==1)
            {
                #if RF_U1_EN==1
              
                  #if RF_PP_115200_EN==1
                    RfU_Init(9600,1);
                  #endif
                  delay_ms(50);   
                  U1_TO_PC;//��ӡǰ���Ȱ�U1�л���PC
                  delay_ms(50);  
                  
                #endif

                //ÿ����ӡ����
                u8 nn,aa;
                #if ((RPTINT_ONLY_WD_EN==1)||(PRINT_FZ_EN==1))
                    //CH_NUM  G95-4.6.8   ->1��  
                    
                    nn=1;
                    
                    #if CH_NUM==8
                       aa=8/nn;               
                    #else
                       aa=9/nn;//2������ӡ9��
                    #endif
     
                #elif PRINT_ALL_CH_EN==1
                    //ȫ��ͨ��
                    //CH_NUM  1.2 ->1��  3.4->2��  5.6.7.8->3��
                    
                    if(CH_NUM<3)
                      nn=1;
                    else if(CH_NUM<5)
                      nn=2;
                    else
                      nn=3;
                    
                    aa=9/nn;//2������ӡ9��
                    
                #else
                    
                    
                    nn=1;
                    
                    #if CH_NUM==8
                       aa=8/nn;               
                    #else
                       aa=9/nn;//2������ӡ9��
                    #endif 
                    
                #endif
                
                
                
                
                //��¼���<=1����ʱ��ÿx����Ŵ�ӡһ�����ݹʣ�aa �� x��
                //�����¼���>1����ʱ����������
                #if GPRS_PRT_JG_EN==1
                if(PcBsq.RecTime<=60)
                {
                  aa=aa*(PRINT_SF_TIME/PcBsq.RecTime);
                }
                #endif
                
                
                if(Pt.PrintThisDueout>aa)//2���ӡ3������9��
                  PerCC=aa;
                else
                  PerCC=Pt.PrintThisDueout;

                
                Gprs_SF_Send_Data(Pt.PrintThisDueout,PerCC,2);
                
                Pt.PrintThisDueout=Pt.PrintThisDueout-PerCC;
                
                WDT_CLR;
                
                if(Pt.PrintThisDueout==0)
                {
                  
                      #if PRINT_MIN_MAX_EN==1
                         GoOneHang(1);
                    
                       
                         #if PRINT_ALL_CH_EN==1
                         for(u8 j=0;j<CH_NUM;j++)
                         {
                           Print_MaxMin(j);
                         }
                         
                         #elif PRINT_FZ_EN==1//�����ӡ
                         
                         
                         for(u8 j=Pt.prt_x*2-2;j<Pt.prt_x*2;j++)
                         {
                           if(j>CH_NUM-1)
                             break;
                           Print_MaxMin(j);
                           #if PRINT_FZ_WD_EN==1
                           j=j+1;
                           #endif
                         }
                         
                         
                         #elif PRINT_2WD_EN==1 
                           #if ((JLY_MODEL==_3_NTC)||(JLY_MODEL==_2_NTC)||(JLY_MODEL==_4_NTC))
                           for(u8 j=0;j<2;j++)
                           #else
                           for(u8 j=0;j<=2;j=j+2)
                           #endif
                           {
                             Print_MaxMin(j);
                           }
                         
                         #else//��ӡͨ��1

                         u8 ch=0;
                         Print_MaxMin(ch);
                         #endif
            
                      #endif
                  
                  
                  
                      GoOneHang(1);
                      U1SendString("********************************\n");
                  
                      #if RPINT_B_EN==1
                        U1SendString("��Ч��ʼʱ�䣺                \n\n");
                        GoOneHang(1);
                        U1SendString("��Ч����ʱ�䣺                \n\n");
                        GoOneHang(1);
                        U1SendString("��  ��  ��  ��                \n\n");
                        GoOneHang(1);
                        U1SendString("��  ��  ��  ��                \n\n");
                        GoOneHang(6);
                        
                     #elif RPINT_A_EN==1
                        U1SendString("�ջ�����                \n\n");
                        GoOneHang(1);
                        U1SendString("�˵��ţ�                \n\n");
                        GoOneHang(1);
                        U1SendString("���ƺţ�                \n\n");
                        GoOneHang(1);
                        U1SendString("�ջ��ˣ�                \n\n");
                        GoOneHang(1);
                        U1SendString("�����ˣ�                \n\n");
                        GoOneHang(1);
                        U1SendString("ʱ  �䣺                \n\n");
                        GoOneHang(6);
                        
                     #elif PRINT_D_EN==1
                        
                        U1SendString("�ջ�����                \n\n");
                        GoOneHang(1);
                        U1SendString("�ջ��ˣ�                \n\n");
                        GoOneHang(1);
                        U1SendString("�����ˣ�                \n\n");
                        GoOneHang(6);
                        
                    #elif PRINT_E_EN==1   
                        
                        U1SendString("��ҵ���ƣ�              \n\n");
                        GoOneHang(1);
                        U1SendString("�豸���ƣ�              \n\n");
                        GoOneHang(1);
                        U1SendString("�ͻ����ƣ�              \n\n");
                        GoOneHang(1);
                        U1SendString("Ʒ    ����              \n\n");
                        GoOneHang(1);
                        U1SendString("�� �� ����              \n\n");
                        GoOneHang(1);
                        U1SendString("�� ʻ Ա��              \n\n");
                        GoOneHang(1);
                        U1SendString("�� �� �ˣ�              \n\n");
                        GoOneHang(6);
                        
                     #elif PRINT_C_EN==1
                        U1SendString("��ӡ���ڣ�");
                        Rtc.SS=ReadRtcD10();
                        PrintTime(Rtc.SS);
                        
                        GoOneHang(1);
                        U1SendString("ǩ��ȷ�ϣ�                \n\n");
                        GoOneHang(6);
                        
                     #else
                
                                    U1SendString("ǩ�֣�         ʱ�䣺           \n\n");
                                    GoOneHang(1);
                                    U1SendString("��ע��\n\n");
                                    GoOneHang(6);
                                    
                     #endif
                    
                    //��ӡ����------------------------------------------------------
                    PrintEnd();
                    
                    #if RF_U1_EN==1
                    U1ToRf();//��ӡ��ɺ��л���RF
                    #endif
                    
                }
                

            
            }
            

            
            return 1;
        }

        

        
        return 0;
}


//��ʼ��ӡ
void Print_SF_Data(void)
{
       //������ڴ�ӡʱ���ٴΰ��´�ӡ�������˳���ӡ
       if(Pt.PrintThisDueout>0)//zz
       {
         Pt.PrintThisDueout=0;
         return;
       }
  

        //��ӡ��ʼ��ʾ------------------------------------------------------
        PrintStart();
        SetIni();
        delay_ms(2);
        SetCnPrint(1);//����ģʽ
        delay_ms(2);
        
        #if PRINT_FZ_EN==0
        LcdNN(PRINTING);
        #endif
        
        //ֻ��ӡ�û����ͱ���
        GoOneHang(1);
        U1SendString("�� �� ����");
        i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
        U1SendByte(0x0a);
        U1SendString("��    �⣺");
        i2c_ee_read_to_pc(TITLE_ADDR,20,0);
        U1SendByte(0x0a);
        GoOneHang(1);
        
        //��ӡ��ʷ����----------------------------------
        
        //��ǰ����״̬, �����ȸ�ֵ������Gprs_SF_Send_Data �����
        Pt.GprsThisRec16Num=PcBsq.Rec16Num;
        Pt.GprsThisRecPt=PcBsq.RecPt;
  
        Pt.PrintThisDueout=Pt.PrintDueout;//���ӡ������  
        
        if(Pt.PrintThisDueout==0)
        {
          U1SendString("û�м�¼����");
          U1SendByte(0x0a);
          GoOneHang(5);
          //��ӡ����
          PrintEnd();
        }
        else
        {
            #if PRINT_ALL_CH_EN==1//��ӡȫ��ͨ��
          
            u8 j;
            for(j=0;j<CH_NUM;j++)
            {
                u8 ChT;//ͨ������
                ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//��ȡ��ͨ������
                
                  //ͨ����.......................................................
                U1SendString("ͨ��");
                U1SendByte(0x31+j);
                U1SendByte(':');
                //ͨ������......................................................
                if(ChT==_T_T)//Temp/�¶�
                {
                   U1SendString("�¶�(��)");
                }
                else if(ChT==_T_H)//Humi/ʪ��
                {
                   U1SendString("ʪ��(%RH)");
                }
                if(j%2==1)
                {
                  //U1SendByte(0x0d);
                  U1SendByte(0x0a);
                }
            }
            U1SendByte(0x0a);
            
            #elif RPTINT_ONLY_WD_EN==1
                  //�¶�
                  #if ((JLY_MODEL==_2_NTC)||(JLY_MODEL==_3_2NTC_1HIH)||(JLY_MODEL==_3_2_NTC_1HIH))
                    U1SendString("ʱ��   �¶�1 �¶�2 \n");
                  #elif ((JLY_MODEL==_3_NTC)||(JLY_MODEL==_3_PT_F100_200)||(JLY_MODEL==_4_3NTC_1HIH))
                    U1SendString("ʱ��   �¶�1 �¶�2 �¶�3 \n");
                  #elif ((JLY_MODEL==_4_NTC)||(JLY_MODEL==_5_4NTC_1HIH))
                    U1SendString("ʱ��   �¶�1 �¶�2 �¶�3 �¶�4\n");
                    
                  #elif (JLY_MODEL==_8_134678NTC_25HIH)
                    U1SendString("ʱ��   �¶�1 �¶�2 �¶�3 �¶�4\n");
                    U1SendString("       �¶�5 �¶�6 \n");

                    
                
                  //��ʪ��
                  #elif CH_NUM==2
                    U1SendString("ʱ��   �¶�1  \n");
                  #elif CH_NUM==4
                    U1SendString("ʱ��   �¶�1 �¶�2 \n");
                  #elif CH_NUM==6
                    U1SendString("ʱ��   �¶�1 �¶�2 �¶�3 \n");
                  #elif CH_NUM==8
                    U1SendString("ʱ��   �¶�1 �¶�2 �¶�3 �¶�4\n");
                  #endif
                    
                    
            #elif PRINT_1_3_EN==1//������ PRINT_2WD_EN ����
            U1SendString("����    ʱ��   �¶�1(��) �¶�3(��)\n");         
                    
            #elif PRINT_2WD_EN==1
             U1SendString("����    ʱ��     �¶�1  �¶�2\n");

             
            #elif PRINT_1W1S_EN==1
             U1SendString("����    ʱ��   �¶ȡ� ʪ��%RH\n");
             
            #elif PRINT_FZ_EN==1//G95-4  �����ӡ
             
             
                 U1SendString("��");U1SendValue(Pt.prt_x);U1SendString("\n");
             
             
                 #if PRINT_FZ_WD_EN==1
                 U1SendString("����    ʱ��   �¶ȡ� \n");
                 #else
                 
                    #if(JLY_MODEL==_3_2NTC_1HIH)
                    
                    if(Pt.prt_x==2)
                      U1SendString("����    ʱ��   �¶ȡ� \n");
                    else
                      U1SendString("����    ʱ��   �¶ȡ� ʪ��%RH\n");
                    
                    #else
                      U1SendString("����    ʱ��   �¶ȡ� ʪ��%RH\n");
                    
                    #endif
                    
                    
                 #endif
             
                      
            #else

                   U1SendString("����    ʱ��     �¶ȣ��棩\n");

                
            #endif
            
            U1SendByte(0x0a);
            
            
            
            #if PRINT_MIN_MAX_EN==1
            for(u8 j=0;j<CH_NUM;j++)
            {
              StateC.print_min[j]=SENIOR_NULL;
              StateC.print_max[j]=SENIOR_NULL;
            }
            #endif
        }
        
        Pt.Tmp=1;//zz
        Pt.PtDay=0;
        
}

void PrintDueout_Clr(void)
{
   Pt.PrintDueout=0;
   PrintDueOutToEE();
}


#endif


void Print_Begin(void)
{
                        #if RF_U1_EN==1
                      
                            #if RF_PP_115200_EN==1
                              RfU_Init(9600,1);
                            #endif
                            delay_ms(50);   
                            U1_TO_PC;//��ӡǰ���Ȱ�U1�л���PC
                            delay_ms(50);  
                            
                            #if GPRS_TC_EN==1
                                Print_SF_Data();//zz
                            #else
                                PrintNewData();
                            #endif
                            
                            U1ToRf();//��ӡ��ɺ��л���RF
                          
        
                        #else
                          
                            #if GPRS_TC_EN==1
                                Print_SF_Data();//zz
                            #else
                                PrintNewData();
                            #endif
                                
                        #endif
                          
                        //����ǹػ�״̬��Ҫ��ʾ�ػ�
                        #if GPRS_6OFF==1
                        if(Sms.GsmKeyShut==1)
                        {
                           LcdOFFX(OFF_BSQ_GPRS_HAND_STOP);//60FF
                        }
                        #endif
}

#if PRINT_FZ_EN==1
void Print_BG_Deel(void)
{
    if(Pt.tout>0)
    {
          Pt.tout--;
          
          if(Pt.tout==0)
          {

              if(Pt.prt_x>PRT_CT)
                Pt.prt_x=PRT_CT;
                

              LcdNN(Pt.prt_x);
            
              if((Pt.prt_x>=1)&&(Pt.prt_x<=2))
                Print_Begin();//��ʼ��ӡ
              else
                Pt.prt_x=0;

          }
      
    }
    

}

#endif

void PrintKeyClr(void)
{
               #if (PRINT_NEW_EN==1)
                  #if GPRS_TC_EN==1
                     PrintDueout_Clr();
                  #else
                     PrintNewClr();
                  #endif
                  LcdNN(PRINT_NEW_CLR);
                  WDT_CLR;
                  delay_ms(300);
                  WDT_CLR;
                  delay_ms(300);
                  WDT_CLR;
                  delay_ms(300);
                  WDT_CLR;
                  
                  
                  //�����ɺ��л���RF
                  #if RF_U1_EN==1 
                  U1ToRf();
                  #endif
                  
               #endif
}


void PrintTest(void)
{
  /*
  PrintCC(60+0x8000,10,1,0);
  PrintCC(10,40,1,1);
  PrintCC(40,30,1,0);
  PrintCC(30,30+0x8000,1,1);
  PrintCC(30+0X8000,0,1,0);
  PrintCC(0,5,1,0);*/


  //StateC.FuhaoBit=0xff;
  //for(j=0;j<5000;j++)
  //{
   
    //StateC.Value[0]=SENIOR_NULL;
    //StateC.Value[1]=SENIOR_NULL;
    //StateC.Value[2]=SENIOR_NULL;
    //StateC.Value[3]=SENIOR_NULL;
    //StateC.Value[4]=500+j;
    //StateC.Value[5]=600+j;
    //StateC.Value[6]=700+j;
    //StateC.Value[7]=800+j;

   // StateC.Value[0]=1;
   // StateC.Value[1]=2;
   // StateC.Value[2]=3;
   // StateC.Value[3]=4;
   // StateC.Value[4]=5;
   // StateC.Value[5]=6;
   // StateC.Value[6]=7;
   // StateC.Value[7]=8;
    
    //LoggerRecord();
  //}

  //Download.Downloading=0;
  //PrintAll();
  NOP;
}


