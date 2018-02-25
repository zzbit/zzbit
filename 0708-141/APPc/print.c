#include "_config.h"


//y轴最大值，最小值,可所有打印最大区域(0,383)
#define Y_MIN                10//x轴箭头位置
#define Y_MAX                383

//数据打印最大区域,打印区域上下各留有40个点空白区域
#define Y_DATA_MIN           (Y_MIN+40)
#define Y_DATA_MAX           (Y_MAX-40)

#define DIAN_X_JIANJU        ((u16)6*2)//8个点/1mm, 2个数值点间距
#define PRINT_RECORD_MAX     50//实际打印最大点数
#define PRINT_BAG_MAX_BYTES  ((u16)PRINT_RECORD_MAX*2*CH_NUM)

//..............................................................................
//获取历史数据信息
//数据小于PRINT_RECORD_MAX时，不用数据压缩
//数据大于PRINT_RECORD_MAX时，采用数据压缩
//极限情况：打印点数最大为PRINT_RECORD_MAX，每包为最大最小值2点，
//          这样最大包数为PRINT_RECORD_MAX/2，因此Download.BagNum不能大于PRINT_RECORD_MAX/2
//当数据条数大于PRINT_RECORD_MAX时，人为分成<(PRINT_RECORD_MAX/2)包，即把所有数据压缩成<PRINT_RECORD_MAX条数据
void GetHisInfo(void)
{
  u16 j;
  
  //求总条数：Download.DownloadNum..............................................
  Print.EndX=Download.DownloadNum=Download.CurrentNum=StateA1.RecordNum;//下载时已记录次数
  if(Download.DownloadNum==0)
    return ;
  //EE历史数据起始序号
  if(StateA2.RecordOverNum==0)
    Print.BeginX=0;
  else
    Print.BeginX=StateA1.RecordNum%StateC.RecordMaxNum+StateC.RecordMaxNum*(StateA2.RecordOverNum-1);
  Print.DianXh=Print.BeginX;
  for(j=0;j<8;j++)
  {
    Print.XhPt[j]=0;
  }
  
  if(Download.DownloadNum>=StateC.RecordMaxNum)//记录已满
  {
    Download.DownloadNum=StateC.RecordMaxNum;
  }
  //求每包最大条数：Download.BagMaxRecordNum
  if(Download.DownloadNum<=PRINT_RECORD_MAX)//打印条数未溢出，不需压缩
  {
    Download.BagMaxRecordNum=(u16)PRINT_BAG_MAX_BYTES/StateC.RecordBytes;//每包最大条数
    //求总包数：Download.BagNum
    Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;
    if((Download.DownloadNum%Download.BagMaxRecordNum)!=0)
      Download.BagNum++;
  }
  else//打印条数溢出，需压缩
  {
    //求每包最大条数Download.BagMaxRecordNum
    Download.BagMaxRecordNum=Download.DownloadNum/((PRINT_RECORD_MAX-3)/2);//最大包数预留3个，为3片EE跨区时使用
    if((Download.DownloadNum%((PRINT_RECORD_MAX-3)/2))>0)
    {
      Download.BagMaxRecordNum++;
    }
    //求总包数：Download.BagNum
    Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;
    if((Download.DownloadNum%Download.BagMaxRecordNum)>0)
    {
      Download.BagNum++;
    }
  }
}
//..............................................................................
//直接从EE取读取第Xpt包历史数据存放在DataBuf，并求出该包数据的最大最小值，存放在MinMaxBuf
//PC:0 不发送
//PC:1 发送所有通道数据到DataBuf
//PC:2 发送所有通道包的最大最小值到MinMaxBuf，最大最小值时间先后需排定
//通道1 最小值MinMaxBuf[0], 通道1 最大值MinMaxBuf[1]
//通道2 最小值MinMaxBuf[2], 通道2 最大值MinMaxBuf[3]
u16 GetHisXpt(u16 Xpt,u16* MinMaxBuf,u8* DataBuf,u8 PC)
{
    u32 EE_32Pt;
    u16 NT;
    u16 PT;
    u8 QuHao;//当前存储片区号
    u8 EE_Num;//存储片数

    PT=0;
    Download.BagPt=Xpt;//下载包序号
    if(Download.BagPt+1>Download.BagNum)return 0;//包序号错误
  
    //计算数据包总字节数
    if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//最后一包并且不足一包
    {
      Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
    }
    else
    {
      Download.BagRecordNum=Download.BagMaxRecordNum;//满包
    }
    Download.BagDataBytes=Download.BagRecordNum*StateC.RecordBytes;
  
    //读取数据包
    EE_32Pt=0;
    if(Download.CurrentNum>=StateC.RecordMaxNum)//已记满
    {
      EE_32Pt+=(Download.CurrentNum%StateC.RecordMaxNum)*StateC.RecordBytes;//记满时的存储偏量
    }

    EE_32Pt+=(u32)Download.BagPt*(Download.BagMaxRecordNum*StateC.RecordBytes);
    EE_32Pt=EE_32Pt%(StateC.RecordMaxNum*StateC.RecordBytes);
    if(EE_32Pt<=StateC.E1_MaxMem)//第一片存储尾地址
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
          
          if(I2cSeekEE()>1)//片数大于1
          {
            QuHao=1;//第二片
          }
          NT=GetBagMaxMinFromEE(0,EE_32Pt+Download.BagDataBytes-StateC.E1_MaxMem,MinMaxBuf,&DataBuf[PT],PC,QuHao);//只需考虑PC==2时状况
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
    else//其他片
    {
      QuHao=1+(EE_32Pt-StateC.E1_MaxMem)/StateC.E2_MaxMem;
      EE_32Pt=(EE_32Pt-StateC.E1_MaxMem)%StateC.E2_MaxMem;
      
      if(EE_32Pt+Download.BagDataBytes>StateC.E2_MaxMem)//其他片存储尾地址
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
//直接从EE取数据包中最大最小值,存放在MinMaxBuf, Len:读取的字节数，一个通道数值占2个字节
//直接从EE取所有通道数据或取所有通道数据的最大最小值
//注意：取最大最小值前，MinMaxBuf必须为SENIOR_NULL
//PC:0 不发送
//PC:1 发送所有数据到DataBuf
//PC:2 发送所有包的最小最大值到DataBuf，最小最大值时间先后需排定
//全局变量Print.DianXh,Print.XhBuf[][],Print.XhPt[]求打印点序号
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
    add+=CC=I2cRecByte();//取一个字节数据
    if(j==(Len-1))//读取结束
      I2cAck(0);
    else
      I2cAck(1);
    
    OnePt=j/StateC.RecordBytes;//第x记录点
    OneBytePt=j%StateC.RecordBytes;//第x字节
    OneChPt=OneBytePt/2;//通道号
    
    if(OneBytePt==0)
    {
      Print.DianXh++;//每个记录点+1
    }
    
    if(OneBytePt==OneChPt*2)//低字节
    {
      LL=CC;
    }
    else if(OneBytePt==OneChPt*2+1)//高字节
    {
      VV=CC;
      VV=(VV<<8)|LL;
      
      //取该包所有数据和MinMaxBuf中的值的最大最小值，结果存放在MinMaxBuf
      MinMaxBuf[OneChPt*2]=get_min(MinMaxBuf[OneChPt*2],VV);
      MinMaxBuf[OneChPt*2+1]=get_max(MinMaxBuf[OneChPt*2+1],VV);
      
      if(PC==1)//取该包所有数据
      {
        DataBuf[DataPt++]=LL;
        DataBuf[DataPt++]=CC;
        Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=Print.DianXh;
      }
      else if(PC==2)//取每包中的最小最大值,
      {
        if((Len/StateC.RecordBytes)==1)//该包只有1个点
        {
            DataBuf[DataPt++]=LL;
            DataBuf[DataPt++]=CC;
            Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=Print.DianXh;
        }
        else//至少2个点
        {
            if(OnePt==0)//第1个点
            {
              MinBuf[OneChPt]=MaxBuf[OneChPt]=VV;
              MinXh[OneChPt]=MaxXh[OneChPt]=Print.DianXh;
            }
            else
            {
              MinBuf[OneChPt]=get_min(MinBuf[OneChPt],VV);//求通道1最小值
              if(MinBuf[OneChPt]==VV)
              {
                MinXh[OneChPt]=Print.DianXh;
              }
              
              MaxBuf[OneChPt]=get_max(MaxBuf[OneChPt],VV);//求通道1最大值
              if(MaxBuf[OneChPt]==VV)
              {
                MaxXh[OneChPt]=Print.DianXh;
              }
              
              if(OnePt==(Len/StateC.RecordBytes)-1)//最后一个点
              {
                if(MinXh[OneChPt]<MaxXh[OneChPt])//最大最小值时间先后排队
                {
                  DataBuf[DataPt++]=(MinBuf[OneChPt]%256);//最小值在前
                  DataBuf[DataPt++]=(MinBuf[OneChPt]/256);
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]%256);
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]/256);
    
                  Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=MinXh[OneChPt];
                  Print.XhBuf[OneChPt][Print.XhPt[OneChPt]++]=MaxXh[OneChPt];
                }
                else
                {
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]%256);//最大值在前
                  DataBuf[DataPt++]=(MaxBuf[OneChPt]/256);
                  DataBuf[DataPt++]=(MinBuf[OneChPt]%256);
                  DataBuf[DataPt++]=(MinBuf[OneChPt]/256);
                  
                  if(MinXh[OneChPt]==MaxXh[OneChPt])//2个值不能相同
                  {
                    MaxXh[OneChPt]=MaxXh[OneChPt]-1;//如果2个值相同，按照算法，序号为最大，故做减法
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
//求通道ChNo,报警下限Buf[0]，报警上限Buf[1],最小值Buf[2],最大值Buf[3]
void GetCurveRange(u8 ChNo,u16* Buf)
{
  u16 AA;
  
  AA=*((char *)CH1_A_L_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_L_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x7f);
  if(((*((char *)CH1_A_L_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x80)==0x80)
    AA=AA|0x8000;
  Buf[0]=AA;//报警下限
  
  AA=*((char *)CH1_A_H_ADDR+ONE_CH_CONFIG_BYTES*ChNo)+(u16)256*((*((char *)CH1_A_H_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x7f);
  if(((*((char *)CH1_A_H_ADDR+1+ONE_CH_CONFIG_BYTES*ChNo))&0x80)==0x80)
    AA=AA|0x8000;
  Buf[1]=AA;//报警上限
  
  //求历史数据极值与报警极值的最小最大值
  #if MIN_MAX_AVG_ENABLE==1
  Buf[2]=get_min(StateC.min[ChNo],Buf[0]);//最小值
  Buf[3]=get_max(StateC.max[ChNo],Buf[1]);//最大值
  #endif
}

//..............................................................................
//所有通道最大最小值全置为SENIOR_NULL
void PutMaxMinNull(void)
{
  u8 j;
  for(j=0;j<(CH_NUM*2);j++)
  {
    Print.MinMaxBuf[j]=SENIOR_NULL;
  }
}
//...........................................................................................
//以记录启动时间为基准，记录间隔为StateB.RecordTime，返回区间某点XX时间，BCD格式
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
  log_start_time=DateToSeconds(&time_bcd);//把时间转换为秒数
  
  log_start_time+=Xuhao*StateB.RecordTime;//当前序号点时间秒数
  SecondsToDate(log_start_time,&time_bcd);//求出当前点的时间
  RtcD10ToBcd(&time_bcd);
  return time_bcd;
}
//......................................................................................................

//........................................................................................
//返回当前时间，秒数
u32 GetRtcSS(void)
{
  RtcReadTime();
  RtcBcdToD10(&Rtc);
  return DateToSeconds(&Rtc);
}
//打印记录间隔时间
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
//打印序号第Xuhao点时间值，时间基准为起始时间
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
      //打印日期
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
//打印数值范围：最大999.9，最小-99.9，占5个ascii
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
    U1SendString("一号门开");
    return;
  }
  else if(cc==DOOR1_CLOSE)
  {
    U1SendString("一号门关");
    return;
  }
  else if(cc==DOOR2_OPEN)
  {
    U1SendString("二号门开");
    return;
  }
  else if(cc==DOOR2_CLOSE)
  {
    U1SendString("二号门关");
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
      U1SendByte(' ');//zz 负数时前面不需空格，正数时有空格
  }
  
  if(cc>99)
    U1SendByte((cc/100)%10+0x30);
  else
    U1SendByte(' ');
  U1SendByte((cc/10)%10+0x30);
  U1SendByte('.');
  U1SendByte(cc%10+0x30); 

}


//打印x轴的箭头
void PrintXjt(u8 cc)
{
  //一个箭头相当于3条曲线
  u16 j;
  for(j=0;j<10;j++)
  {
    U1SendByte(0x1b);
    U1SendByte(0x27);
    
    U1SendByte(GetL8(3));
    U1SendByte(GetH8(3));
    
    U1SendByte(GetL8(cc));//中间轴
    U1SendByte(GetH8(cc));
    
    U1SendByte(GetL8(20-j));
    U1SendByte(GetH8(20-j));
    
    U1SendByte(GetL8(j));
    U1SendByte(GetH8(j));
  
    U1SendByte(0x0d); 
  }
}
//进入汉字打印
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
void SetIni(void)//打印机复位，初始化
{
    U1SendByte(0x1b);
    U1SendByte(0x40);
    delay_ms(2);
    //消除颠倒打印
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
//打印cc个空行
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

//数据在BufData,2个字节表示一个数，低字节在前
void PrintCurve(u8* BufData,u16 DataN,u8 ChNo)
{
  u16 cc;
  u16 y1;
  u16 yy;
  float ft1;
  
  u8 fg=0;
  u16 Dpt;
  u16 i,j,k;
  u16 aa,nn;//2点间最佳曲线条数
  u16 Y_AL,Y_AH;
  u16 CurveM[4];//报警下限,报警上限，最小值和最大值
  
  GetCurveRange(ChNo,CurveM);//获取通道报警下限上限，曲线下限上限
  
  Y_AL=GetZvY(CurveM[2],CurveM[3],Y_DATA_MIN,Y_DATA_MAX,CurveM[0]);
  Y_AH=GetZvY(CurveM[2],CurveM[3],Y_DATA_MIN,Y_DATA_MAX,CurveM[1]);
  
  if(Y_AL>Y_AH)//数据错误，退出
    return;
  
  GoOneHang(1);
  
  //打印y轴上的报警下限数值，报警上限数值
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
  
  //打印y轴
  PrintYz();
  delay_us(100);
  
  //打印曲线
  Dpt=0;
  for(i=0;i<DataN;i++)
  {
    cc=256*BufData[i*2+1]+BufData[i*2];
    
    if((cc==SENIOR_NULL)||(cc==HAND_STOP))//非正常数据不打印
      continue;
    
    cc=GetZvY(CurveM[2],CurveM[3],Y_DATA_MIN,Y_DATA_MAX,cc);
    
    if(Dpt>0)
    {
      //第2个点开始打印曲线
      for(k=0;k<DIAN_X_JIANJU;k++)
      {
        ft1=(((float)cc-y1)/DIAN_X_JIANJU)*(k+1)+y1;//求2点连线间任意一点y值
        yy=(u16)(ft1*10);
        if((yy%10)>4){yy+=10;}
        yy=yy/10;
          
        if(yy>aa)
          nn=yy-aa;//求数值点最佳曲线条数
        else
          nn=aa-yy;
        if(nn==0)
          nn=1;
        
        U1SendByte(0x1b);
        U1SendByte(0x27);
        
        if((k%4)==0)//报警上限，报警下限，虚线表示
        {
          fg=!fg;
        }
        //曲线条数
        if(fg)
        {
          U1SendByte(GetL8(nn+3));//x轴，报警上限，报警下限
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
        U1SendByte(GetL8(Y_MIN));//x轴
        U1SendByte(GetH8(Y_MIN));
        if(fg)
        {
          U1SendByte(GetL8(Y_AL));//报警下限
          U1SendByte(GetH8(Y_AL));
            
          U1SendByte(GetL8(Y_AH));//报警上限
          U1SendByte(GetH8(Y_AH));
        }
        U1SendByte(0x0d);
        aa=yy;
      }
    }
    y1=aa=cc;//第一个点
    Dpt++;
  }
  PrintXjt(Y_MIN);//打印x轴箭头
}
void PrintStart(void)
{
  U1Init(BAUDRATE_PRINT,0);//zz
  ComSendH();
  U1_RXD_UN;//发送数据时，关闭接收中断，防止接收中断干扰
}
void PrintEnd(void)
{
  ComSendL();
  U1_RXD_EN;//发送结束后，打开接收中断
  U1Init(BAUDRATE_SYSTEM,0);//打印完毕，调整波特率
  
  #if PRINT_FZ_EN==1
  Pt.prt_x=0;
  #endif
  
}
//打印联系人信息
void PrintMessageInfo(void)
{
  GoOneHang(1);
  U1SendString("********************************\n");
  U1SendString("历史数据打印结束\n\n");
  U1SendString("签字：         时间：           \n\n");
  GoOneHang(1);
  U1SendString("备注：\n\n");
  GoOneHang(6);
}
//打印记录参数信息
void PrintInfo(void)
{
  GoOneHang(1);
  U1SendString("********************************\n");
  U1SendString("用户名：");
  i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("标  题：");
  i2c_ee_read_to_pc(TITLE_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("开始记录时间：");
  PrintTime(Print.BeginX);
  U1SendByte(0x0a);
  U1SendString("结束打印时间：");
  PrintTime(Print.EndX);
  U1SendByte(0x0a);
  U1SendString("记录时间间隔：");
  PrintJianGe(StateB.RecordTime);
  U1SendByte(0x0a);
  U1SendString("总计记录条数：");
  U1SendValue(Download.DownloadNum);
  U1SendByte(0x0a);
}
void PrintChInfo(u8 ChNo)
{
  GoOneHang(1);
  U1SendString("********************************\n");
  if(_U_OC==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//温度
  {
    U1SendString("通道");
    #if CH_PAIR_ENABLE==1
    U1SendValue(ChNo/2+1);
    #else
    U1SendValue(ChNo+1);
    #endif
    U1SendString("  温度   单位（℃）");
  }
  else if(_U_RH==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//湿度
  {
    U1SendString("通道");
    #if CH_PAIR_ENABLE==1
    U1SendValue(ChNo/2+1);
    #else
    U1SendValue(ChNo+1);
    #endif
    U1SendString("  湿度   单位（%RH）");
  }

  U1SendByte(0x0a);
  U1SendString("最小值：");
  PrintValue(Print.MinMaxBuf[ChNo*2]);
  U1SendString("  最大值：");
  PrintValue(Print.MinMaxBuf[ChNo*2+1]);
  U1SendByte(0x0a);
}
//打印一组数据列表，一个数据占2个字节
void PrintList(u16* Xuhao,u8* BufData,u16 Num,u8 ChNo)
{
  u16 j,dd;
  
  GoOneHang(1);
  
    U1SendString(" 日期   时间   通道");
    #if CH_PAIR_ENABLE==1
    U1SendValue(ChNo/2+1);
    #else
    U1SendValue(ChNo+1);
    #endif
  
  
  if(_U_OC==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//温度
  {
    U1SendString("温度（℃）\n");
  }
  else if(_U_RH==*((char*)CH1_U_ADDR+ChNo*ONE_CH_CONFIG_BYTES))//湿度
  {
    U1SendString("湿度（%RH）\n");
  }

  for(j=0;j<Num;j++)
  {
    dd=BufData[j*2+1];
    dd=(dd<<8)|BufData[j*2];
    if((dd==SENIOR_NULL)||(dd==HAND_STOP))//非正常数据不打印
      continue;
    PrintTime(Xuhao[j]);//时间
    U1SendByte(' ');
    PrintValue(dd);//数值
    
    
    if(AlarmXSeek(dd,ChNo,1))//是否报警超标
      U1SendString("(超标)");
    
    U1SendByte(0x0a);
  }
}

//取需打印的数据，返回数据条数，最大最小值存放MinMaxBuf,
u16 GetPrintData(u16* MinMaxBuf,u8* BufD)
{
  u16 j;
  u16 AaPt;
  u16 NT;

  Print.FgKuaQu=0;
  Print.FgKuaPt=0;
  Print.FgLastBagOne=0;
  
  AaPt=0;
  if(Download.DownloadNum<=PRINT_RECORD_MAX)//打印条数未溢出，不需压缩,数据全部发送
  {
     AaPt=GetHisXpt(0,MinMaxBuf,BufD,1);//取所有数据
  }
  else//打印条数溢出，需压缩，数据压缩后发送极值数据
  {
    for(j=0;j<Download.BagNum;j++)
    {
      NT=GetHisXpt(j,MinMaxBuf,&BufD[AaPt],2);//取数据包中的最大最小值，并进行时间先后排序
      AaPt+=NT;
      if(NT/StateC.RecordBytes==1)//只有一个数据,只最后一包可能会出现这个情况
      {
        Print.FgLastBagOne=1;
      }
      else if(NT/StateC.RecordBytes>2)//大于2个数据，只有跨区才可能会出现这个情况
      {
        Print.FgKuaQu=1;
        Print.BagKuaQuPt=j;//跨区包序号
      }
    }
  }
  AaPt=AaPt/StateC.RecordBytes;
  return AaPt;
}
//打印某通道曲线或列表
void PrintChData(u16 DataNN,u8 ChNo)
{
      u16 CT;
      u16 BagNN;
      u16 xx;
      u16 i,j;
      u8 PrintDD[50*2];//最大50条数据

      //取通道对应数据
      if(Download.DownloadNum<=PRINT_RECORD_MAX)//未溢出
      {
        for(i=0,j=0;j<DataNN;j++)//只有一包，一次取完
        {
          xx=j*StateC.RecordBytes+ChNo*2;
          PrintDD[i++]=Uart_1.RxBuf[xx];
          PrintDD[i++]=Uart_1.RxBuf[xx+1];
        }
      }
      else
      {
        BagNN=Download.BagNum;
        if(Print.FgKuaQu==1)//跨区时，多出一包
        {
          BagNN++;
        }
                
        CT=0;
        for(i=0,j=0;j<BagNN;j++)
        {
          //跨区的2包和最后一包可能出现只有一条数据的情况
          if((j==BagNN-1)&&(Print.FgLastBagOne==1))//最后一包
          {
            xx=CT+ChNo*2;
            PrintDD[i++]=Uart_1.RxBuf[xx];
            PrintDD[i++]=Uart_1.RxBuf[xx+1];
            CT+=2*CH_NUM;//包字节数
          }
          else if((j==Print.BagKuaQuPt)&&(Print.FgKuaQu==1)&&(Print.FgKuaPt==1))//跨区第1分包
          {
             xx=CT+ChNo*2;
             PrintDD[i++]=Uart_1.RxBuf[xx];
             PrintDD[i++]=Uart_1.RxBuf[xx+1];
             CT+=2*CH_NUM;//包字节数
          }
          else if((j==Print.BagKuaQuPt+1)&&(Print.FgKuaQu==1)&&(Print.FgKuaPt==2))//跨区第2分包
          {
             xx=CT+ChNo*2;
             PrintDD[i++]=Uart_1.RxBuf[xx];
             PrintDD[i++]=Uart_1.RxBuf[xx+1];
             CT+=2*CH_NUM;//包字节数
          }
          else//其余每包返回2条数据
          {
            xx=CT+ChNo*4;
            PrintDD[i++]=Uart_1.RxBuf[xx];
            PrintDD[i++]=Uart_1.RxBuf[xx+1];
            PrintDD[i++]=Uart_1.RxBuf[xx+2];
            PrintDD[i++]=Uart_1.RxBuf[xx+3];
            CT+=4*CH_NUM;//包字节数
          }
        }
      }
      PrintChInfo(ChNo);//打印通道信息
      #if PRINT_ALL_CURVE==1
      PrintCurve(PrintDD,DataNN,ChNo);//打印曲线
      #endif
      PrintList(Print.XhBuf[ChNo],PrintDD,DataNN,ChNo);//打印列表
      NOP;
}
//打印全程数据，大于50个点时，取50个特征值打印，能反应总体趋势
//打印数据信息，数据列表，数据曲线等
void PrintAll(void)
{
  u8 j=0;
  u16 PrintNum;
  
  if(Download.Downloading)//下载数据时退出打印
   return;
  
  GetHisInfo();
  if(Download.DownloadNum<1)
    return;
  //开始计算前，所有通道最大最小值置SENIOR_NULL
  PutMaxMinNull();
  
  //打印开始提示------------------------------------------------------
  PrintStart();
  LcdNN(PRINTING);
  //KeyIntClr();//屏蔽所有按键中断
  Download.Downloading=1;
  GoOneHang(1);//开始打印提示
  
  //取数据，从EE取数据，消耗时间较长
  PrintNum=GetPrintData(Print.MinMaxBuf,Uart_1.RxBuf);
  SetIni();
  delay_ms(2);
  SetCnPrint(1);
  delay_ms(2);
  PrintInfo();//打印记录信息
  if(PrintNum<=PRINT_RECORD_MAX)
  {
    for(j=0;j<CH_NUM;j++)
    {
      delay_us(100);
      PrintChData(PrintNum,j);//打印曲线和列表
    }
  }
  PrintMessageInfo();//打印联系人信息 

  //打印结束------------------------------------------------------
  PrintEnd();
  Download.Downloading=0;
  //KeyIntSet();//重新开启所有按键中断
  
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

//16x8点阵字符
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
  {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000},//空  10
  {0x0000,0x0000,0x0000,0x030C,0x030C,0x0000,0x0000,0x0000},//:   11
  {0x0080,0x0080,0x0080,0x0080,0x0080,0x0080,0x0080,0x0000},//-   12
  {0x0000,0x3000,0x3000,0x0000,0x0000,0x0000,0x0000,0x0000}//.    13
};


//以16x8字符中的一行，返回一行的打印点数和各个点的位置数组out
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

//打印y轴,无箭头
void PrintYz(void)
{  
  u16 i,j;
  u16 JG;
  JG=(Y_MAX-Y_MIN)/3;//y分成3段
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

//取一个数的每位数
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
      buf[0]=10;//空
    }
  }
  
  if(zv>99)
  {
    buf[1]=(zv/100)%10;
  }
  else
  {
    buf[1]=10;//空
  }
  
  if(zv>9)
  {
    buf[2]=(zv/10)%10;
  }
  else
  {
    buf[2]=0;//0
  }
  
  buf[3]=13;//小数点
  buf[4]=zv%10;
}
//把数值zvv转换为y值
//比例关系(ymin,ymax)/(zv1,zv2)
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
//打印y轴和y轴5个标度StateC.PrintXB[5]:全局变量
//5条虚线y值范围(33,369)
void PrintY5Zv(void)
{
  u16 i,j,k,n;
  u8 potnn[5];
  u8 potbuf[5][16];
  u16 qxnn=0;
  u8 vbuf[5][5];
  u16 xxmin,xxmax;
  u16 tmp;
  
  //y轴上标度
  xxmin=xxmax=StateC.PrintXB[0];
  for(j=0;j<5;j++)
  { 
    //取5个虚线点中的最大最小值
    if(j<4)
    {
      xxmin=get_min(xxmin,StateC.PrintXB[j+1]);
      xxmax=get_max(xxmax,StateC.PrintXB[j+1]);
    }
    //取标度的每个数字
    GetZvTobuf(StateC.PrintXB[j],vbuf[j]);
  }
  
  for(k=0;k<5;k++)//vbuf[0-4]
  {
        for(i=0;i<8;i++)//vbuf[].bit[0-7]
        {
          //每一行曲线汇总
          //曲线条数-----------------------------------------------------------
          //x轴
          qxnn=1;
          
          //5个y轴标度-条数
          for(n=0;n<5;n++)//5个
          {
            potnn[n]=GetOneHangForm16X8(A16x8[vbuf[n][k]][i],potbuf[n]);//取一行,即字符(1/8)
            qxnn+=potnn[n];
          }
          
          //打印1行------------------------------------------------------------
          U1SendByte(0x1b);
          U1SendByte(0x27);
          U1SendByte(GetL8(qxnn));//一行需打印的点数
          U1SendByte(GetH8(qxnn));
          
          //x轴
          U1SendByte(GetL8(19));
          U1SendByte(GetH8(19));
          
          //5个标度值
          for(j=0;j<5;j++)
          {
            tmp=GetZvY(xxmin,xxmax,33,369,StateC.PrintXB[j])-8;//求每个虚线点位的y值
            for(n=0;n<potnn[j];n++)
            {
              U1SendByte(GetL8(potbuf[j][n]+tmp));//y轴标度数字显示位置
              U1SendByte(GetH8(potbuf[j][n]+tmp));
            }
          }
          
          //执行本行打印--------------------------------------------------------
          U1SendByte(0x0d);
        }
  }
  
  PrintYz();//打印y轴
}
//zv1,zv2: 打印的2个数值间连线
//    xjj: x方向间距xjj
//  Xuhao: 以启动时间和记录间隔，计算出当前序号点的时间
// timeEn: 0,不打印时间，1,打印时间
//打印同时出虚线和x轴标注时间(Xuhao)
//5条虚线y值范围(33,369)
//StateC.PrintXB[5]:5个虚线点，全局变量
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
      //取序号点时间值
      time_bcd=GetXuhaoTimeBaseStartTimeBcd(Xuhao);
      
      //取时间的每位数字，格式为 年月日 时:分,例 130422 08:30
      m=0;
      timeBit[m++]=time_bcd.Year/16;
      timeBit[m++]=time_bcd.Year%16;
      timeBit[m++]=time_bcd.Month/16;
      timeBit[m++]=time_bcd.Month%16;
      timeBit[m++]=time_bcd.Day/16;
      timeBit[m++]=time_bcd.Day%16;
      timeBit[m++]=10;//空格
      timeBit[m++]=time_bcd.Hour/16;
      timeBit[m++]=time_bcd.Hour%16;
      timeBit[m++]=11;//:
      timeBit[m++]=time_bcd.Minute/16;
      timeBit[m++]=time_bcd.Minute%16;
   }
   
  //y轴标度最大最小值
  xxmin=xxmax=StateC.PrintXB[0];
  for(j=0;j<4;j++)
  {
    //取5个虚线点中的最大最小值
    xxmin=get_min(xxmin,StateC.PrintXB[j+1]);
    xxmax=get_max(xxmax,StateC.PrintXB[j+1]);
  }
  
  //把打印的2个点zv1,zv2转换为y值,比例关系(xxmin,xxmax)/(33,369)
  y1=GetZvY(xxmin,xxmax,33,369,zv1);
  y2=GetZvY(xxmin,xxmax,33,369,zv2);
  ybc=y1;//前一个单位点=第一个单位点
  
  xspt=0;
  xsfg=0;
  xjpt=0;
  
  for(k=0;k<12;k++)//12位时间点各数字,共96行
  {
        for(i=0;i<8;i++)
        {
          //每一行曲线汇总
          //曲线条数-----------------------------------------------------------
          
          if(((zv1==SENIOR_NULL)||(zv1==HAND_STOP))||((zv2==SENIOR_NULL)||(zv2==HAND_STOP)))//非正常数据不打印实时曲线
          {
            ynn=0;
          }
          else
          {
              //求2点连线间任意一单位点的y值
              xjpt++;
              ft1=(((float)y2-y1)/96)*xjpt+y1;
              yy=(u16)(ft1*10);
              if((yy%10)>4){yy+=10;}
              yy=yy/10;
              
              if(yy>ybc)//求2单位点间最佳曲线条数
                ynn=yy-ybc;
              else
                ynn=ybc-yy;
              if(ynn==0)
                ynn=1;
          }
          
          qxnn=ynn;//曲线条数

          //时间-条数
          if(timeEn)
          {
            potnn=GetOneHangForm16X8(A16x8[timeBit[k]][i],potbuf);//取一行,即字符(1/8)
          }
          else
          {
            potnn=0;
          }
          qxnn+=potnn;

          //x轴
          qxnn+=1;
          
          //虚线-条数
          if(xspt%4==0)//每4个点变换一次有无-效果就是虚线
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
          
          //最后一个点,显示x轴时间上和曲线上竖线，每条竖线4个点
          if(k==11&&i==7)
          {
            if(timeEn)
              qxnn+=8;
            else
              qxnn+=4;
          }
          
          //打印1行曲线------------------------------------------------------------
          U1SendByte(0x1b);
          U1SendByte(0x27);
          U1SendByte(GetL8(qxnn));//一行需打印的点数
          U1SendByte(GetH8(qxnn)); 
          
          //2点数据间连线
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
              U1SendByte(GetL8(ytm-2));//实时曲线中竖线
              U1SendByte(GetH8(ytm-2));
              U1SendByte(GetL8(ytm-1));
              U1SendByte(GetH8(ytm-1));
              U1SendByte(GetL8(ytm+1));
              U1SendByte(GetH8(ytm+1));
              U1SendByte(GetL8(ytm+2));
              U1SendByte(GetH8(ytm+2));
            }
            
            ybc=yy;//前一个单位点
          }

          //时间
          for(j=0;j<potnn;j++)
          {
            U1SendByte(GetL8(potbuf[j]));
            U1SendByte(GetH8(potbuf[j]));
          }
          
          //x轴
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
          
          //虚线
          if(xsfg)
          {
            for(j=0;j<5;j++)
            {
              ytm=GetZvY(xxmin,xxmax,33,369,StateC.PrintXB[j]);//虚线点位
              U1SendByte(GetL8(ytm));
              U1SendByte(GetH8(ytm));
            }
          }
          //执行本行打印--------------------------------------------------------
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

      //取当前记录的数据，包括符号
      for(j=0;j<CH_NUM;j++)
      {
          PrintZv[j]=StateC.Value[j];
          if(0x01&(StateC.FuhaoBit>>j))//负数
          {
            PrintZv[j]=StateC.Value[j]|0x8000;
          }
      }
      //手动停机或传感器无信号时不打印
      
     #if CH_NUM==1 
     if((PrintZv[0]==SENIOR_NULL)||(PrintZv[0]==HAND_STOP))
        return;
     #endif
      
      #if BSQ_ENABLE==1
      buf[i++]=0xff&Rtc.SB;
      buf[i++]=0xff&(Rtc.SB>>8);
      buf[i++]=0xff&(Rtc.SB>>16);
      buf[i++]=0xff&(Rtc.SB>>24);//时间占4个
      #else
      buf[i++]=0xff&StateA1.RecordNum;
      buf[i++]=0xff&(StateA1.RecordNum>>8);
      buf[i++]=0xff&(StateA1.RecordNum>>16);
      buf[i++]=0xff&(StateA1.RecordNum>>24);//时间占4个
      #endif
      
      for(j=0;j<CH_NUM;j++)
      {
        buf[i++]=0xff&PrintZv[j];
        buf[i++]=0xff&(PrintZv[j]>>8);//数据占2个字节 FF FF
      }
  
      //记录地址从PRINT_CH1_ADDR开始
      EE_16Pt=Print.NewPt;
      EE_16Pt=EE_16Pt*i;
      EE_16Pt=EE_16Pt+PRINT_CH1_ADDR;
            
      //记录一条
     I2C_EE_BufferWrite(buf,EE_16Pt,i,0);
            
     //记录缓冲
     Print.NewPt++;
     if(Print.NewPt>=PRINT_NEW_CAP)//缓冲溢出
       Print.NewPt=0;
     
     //记录条数
     if(Print.NewNum<PRINT_NEW_CAP)
       Print.NewNum++;
     else
       Print.NewNum=PRINT_NEW_CAP;
            
     PrintNewToEE();//保存参数到EE
     
     NOP;
}

#if PRINT_MIN_MAX_EN==1

//通道号, 求和的个数,最大最小平均
void Print_Max_Min_Avg(u8 ch,u16 cc)
{
                      
                      
                      U1SendString("通道");U1SendByte(0x30+ch+1);U1SendByte(0x0a);
                      /*
                      u16 buf[2];
                      GetCurveRange(ch,buf);//获取通道报警下限上限，曲线下限上限
                      U1SendString("设定范围：（");
                      PrintValue(buf[0]);//报警下限
                      U1SendString(",");
                      PrintValue(buf[1]);//报警上限
                      U1SendString("）");
                      U1SendByte(0x0a);
                      */
                      
                      u16 avg;
                      
                      U1SendString("最大");
                      PrintValue(StateC.print_max[ch]);//数值
                      U1SendByte(',');
                      U1SendString("最小");
                      PrintValue(StateC.print_min[ch]);//数值
                      U1SendByte(',');
                      
                      avg=get_avg(StateC.print_sum[ch],cc);
                      U1SendString("平均");
                      PrintValue(avg);//数值
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
        u8 PrintCV[200];//最多100条数据
        #endif
        
        
        //打印开始提示------------------------------------------------------
        PrintStart();
        SetIni();
        delay_ms(2);
        SetCnPrint(1);//汉字模式
        delay_ms(2);
        LcdNN(PRINTING);
        
      #if PRINT_INFOR_EN==1
        U1SendString("*********打印开始***************\n");
      #else
        //只打印用户名和标题
        GoOneHang(1);
        U1SendString("用 户 名：");
        i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
        U1SendByte(0x0a);
        U1SendString("标    题：");
        i2c_ee_read_to_pc(TITLE_ADDR,20,0);
        U1SendByte(0x0a);
        GoOneHang(1);
        
      #endif
        
        //取下载信息
        if(Print.NewNum>PRINT_NEW_CAP)
          Print.NewNum=PRINT_NEW_CAP;
        
        BagMaxRecN=(u16)70/PRINT_CH_BYTE;//每包最大条数 zz
        DownN=Print.NewNum;//下载总条数
        if(DownN>=PRINT_NEW_CAP)//记录已满
          DownN=PRINT_NEW_CAP;
        BagN=DownN/BagMaxRecN;//总包数
        if(DownN%BagMaxRecN!=0)
          BagN++;
        
        if(DownN>0)//有数据
        {
          #if PRINT_ALL_CH_EN==1//打印全部通道
          for(j=0;j<CH_NUM;j++)
          {
              u8 ChT;//通道类型
              ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//读取该通道类型
              
                //通道号.......................................................
              U1SendString("通道");
              U1SendByte(0x31+j);
              U1SendByte(':');
              //通道类型......................................................
              if(ChT==_T_T)//Temp/温度
              {
                 U1SendString("温度(℃)");
              }
              else if(ChT==_T_H)//Humi/湿度
              {
                 U1SendString("湿度(%RH)");
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
              u8 ChT;//通道类型
              ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//读取该通道类型
              
                //通道号.......................................................
              U1SendString("通道");
              U1SendByte(0x31+j);
              U1SendByte(':');
              //通道类型......................................................
              if(ChT==_T_T)//Temp/温度
              {
                 U1SendString("温度(℃)");
              }
              else if(ChT==_T_H)//Humi/湿度
              {
                 U1SendString("湿度(%RH)");
              }
              if(j%2==1)
              {
                //U1SendByte(0x0d);
                U1SendByte(0x0a);
              }
          }
          U1SendByte(0x0a);
          
          #elif PRINT_1_3_EN==1
            U1SendString("日期    时间   温度1(℃) 温度3(℃)\n");
          
          #elif PRINT_2WD_EN==1
           U1SendString("日期    时间   温度1(℃) 温度2(℃)\n");
          #elif PRINT_1W1S_EN==1
           U1SendString("日期    时间   温度℃ 湿度%RH\n");
          
          #else

                   U1SendString("日期    时间     温度（℃）\n");

           

          #endif
        }
        else
        {
          U1SendString("没有数据\n");
          GoOneHang(6);
          
          //打印结束------------------------------------------------------
          PrintEnd();
          return;
        }
        
        //开始下载包
        #if PRINT_NEW_CURVE_EN==1
        cpt=0;
        #endif
        for(j=0;j<BagN;j++)
        {
             //打印前每包间隔1.2s, 否则炜煌打印会缓冲乱掉
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
             
            //取每包数据
            if((j+1==BagN)&&(DownN%BagMaxRecN!=0))//最后一包并且不足一包
            {
              BagRecN=DownN%BagMaxRecN;
            }
            else
            {
              BagRecN=BagMaxRecN;//满包
            }
            BagDataBytes=BagRecN*PRINT_CH_BYTE;
            
            i=0;
            if(DownN<PRINT_NEW_CAP)//记录未满
            {
                 EE_32Pt=(u32)j*(BagMaxRecN*PRINT_CH_BYTE);
                 EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                 I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,BagDataBytes,0);
            }
            else//记录已满
            {
                if(j==0)
                  Current32Pt=(u32)Print.NewPt*PRINT_CH_BYTE;//第1条位置
                EE_32Pt=Current32Pt+(u32)j*(BagMaxRecN*PRINT_CH_BYTE);
                EE_32Pt=EE_32Pt%((u16)PRINT_NEW_CAP*PRINT_CH_BYTE);
                
                if(EE_32Pt+BagDataBytes<=(u32)PRINT_NEW_CAP*PRINT_CH_BYTE)
                {
                  EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                  I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,BagDataBytes,0);
                }
                else//溢出
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
<=2通道
131203 13:52:40   11.7  53.6
>2通道
131203 13:52:40
11.7  53.6 11.7  53.6
11.7  53.6 11.7  53.6
********************************/
            
            //对1包数据打印,每包数据条数BagRecN
            for(k=0;k<BagRecN;k++)
            {
                delay_ms(2);//放慢
                WDT_CLR;
                
                tt=U8_TO_U32(Uart_1.RxBuf[k*PRINT_CH_BYTE+3],Uart_1.RxBuf[k*PRINT_CH_BYTE+2],Uart_1.RxBuf[k*PRINT_CH_BYTE+1],Uart_1.RxBuf[k*PRINT_CH_BYTE]);
                PrintTime(tt);//时间
                
                #if PRINT_INFOR_EN==1
                if((j==0)&&(k==0))
                  time_begin=tt;//第1条
                if((j==BagN-1)&&(k==BagRecN-1))
                  time_end=tt;//最后1条
                #endif
                
                #if PRINT_ALL_CH_EN==1//打印全部通道-------------------------------------
                if(CH_NUM>2)
                {
                  U1SendByte(0x0a);//换行
                }
                
                for(m=0;m<CH_NUM;m++)
                {
                  U1SendByte(' ');
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//数值
                  if((CH_NUM>4)&&(m==3))//1行最多显示4个通道
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
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//必须先排除NULL
                    cc[ch]++;
                  
                  #endif
                  
                  
                }
                
                
                #elif PRINT_4_EN==1
                if(CH_NUM>2)
                {
                  U1SendByte(0x0a);//换行
                }
                
                for(m=0;m<4;m++)
                {
                  U1SendByte(' ');
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//数值

                  
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
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//必须先排除NULL
                    cc[ch]++;
                  
                  #endif
                  
                  
                }
                
                #elif PRINT_2WD_EN==1//G95-4P,打印温度1和温度2，对应通道1和通道3-------------------------------
                  U1SendByte(' ');
                  m=0;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//数值
                  
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
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//必须先排除NULL
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
                  PrintValue(dd);//数值
                  
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
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//必须先排除NULL
                    cc[ch]++;
                  
                  #endif
                  
                  
                #elif PRINT_1W1S_EN==1//对应通道1温度和通道2湿度------------------------------------------
                  U1SendByte(' ');
                  m=0;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//数值
                  
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
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//必须先排除NULL
                    cc[ch]++;
                  
                  #endif
                  
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  U1SendByte(' ');
                  
                  m=1;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//数值
                  
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
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//必须先排除NULL
                    cc[ch]++;
                  
                  #endif
                  
                
                
                #else//只打印通道1----------------------------------------------------------------------
                  U1SendByte(' ');
                  m=0;
                  dd=U8_TO_U16(Uart_1.RxBuf[k*PRINT_CH_BYTE+5+m*2],Uart_1.RxBuf[k*PRINT_CH_BYTE+4+m*2]);
                  PrintValue(dd);//数值
                  
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
                 
                  if(!((StateC.print_sum[ch]==SENIOR_NULL)||(dd==SENIOR_NULL)))//必须先排除NULL
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
        PrintCurve(PrintCV,cpt/2,0);//打印通道1曲线
        #endif
        
        
        //zz
#if PRINT_INFOR_EN==1
  U1SendString("*********打印信息统计***********\n");
  U1SendString("用 户 名：");
  i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("标    题：");
  i2c_ee_read_to_pc(TITLE_ADDR,20,0);
  U1SendByte(0x0a);
  U1SendString("S      N：");
  i2c_ee_read_to_pc(SERIAL_NO_ADDR,10,0);
  U1SendByte(0x0a);
  U1SendString("开始时间：");
  PrintTime(time_begin);
  U1SendByte(0x0a);
  U1SendString("结束时间：");
  PrintTime(time_end);
  U1SendByte(0x0a);
  U1SendString("记录间隔：");
  PrintJianGe(PRINT_JIANGE);
  U1SendByte(0x0a);
  U1SendString("记录条数：");
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
                  
          #elif PRINT_2WD_EN==1//G95-4P,打印温度1和温度2，对应通道1和通道3         
                      ch=0;
                      Print_Max_Min_Avg(ch,cc[ch]);
                      
                      #if ((JLY_MODEL==_3_NTC)||(JLY_MODEL==_2_NTC)||(JLY_MODEL==_4_NTC))
                        ch=1;
                      #else
                        ch=2;                    
                      #endif
                      
                      
                      Print_Max_Min_Avg(ch,cc[ch]);
                  
          #elif PRINT_1W1S_EN==1//对应通道1温度和通道2湿度         
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
        U1SendString("有效开始时间：                \n\n");
        GoOneHang(1);
        U1SendString("有效结束时间：                \n\n");
        GoOneHang(1);
        U1SendString("送  货  方  ：                \n\n");
        GoOneHang(1);
        U1SendString("收  货  方  ：                \n\n");
        GoOneHang(6);
        
     #elif RPINT_A_EN==1
        U1SendString("收货方：                \n\n");
        GoOneHang(1);
        U1SendString("运单号：                \n\n");
        GoOneHang(1);
        U1SendString("车牌号：                \n\n");
        GoOneHang(1);
        U1SendString("收货人：                \n\n");
        GoOneHang(1);
        U1SendString("交货人：                \n\n");
        GoOneHang(1);
        U1SendString("时  间：                \n\n");
        GoOneHang(6);
        
      #elif PRINT_D_EN==1  
        U1SendString("收货方：                \n\n");
        GoOneHang(1);
        U1SendString("收货人：                \n\n");
        GoOneHang(1);
        U1SendString("交货人：                \n\n");
        GoOneHang(6);
        
                    #elif PRINT_E_EN==1   
                        
                        U1SendString("企业名称：              \n\n");
                        GoOneHang(1);
                        U1SendString("设备名称：              \n\n");
                        GoOneHang(1);
                        U1SendString("客户名称：              \n\n");
                        GoOneHang(1);
                        U1SendString("品    名：              \n\n");
                        GoOneHang(1);
                        U1SendString("车 牌 名：              \n\n");
                        GoOneHang(1);
                        U1SendString("驾 驶 员：              \n\n");
                        GoOneHang(1);
                        U1SendString("收 货 人：              \n\n");
                        GoOneHang(6);
        
      #elif PRINT_C_EN==1
                        U1SendString("打印日期：");
                        Rtc.SS=ReadRtcD10();
                        PrintTime(Rtc.SS);
                        
                        GoOneHang(1);
                        U1SendString("签名确认：                \n\n");
                        GoOneHang(6);

      #else
        
        U1SendString("签字：         时间：           \n\n");
        GoOneHang(1);
        U1SendString("备注：\n\n");
        GoOneHang(6);
      #endif
        
        //打印结束------------------------------------------------------
        PrintEnd();
}



#endif//PRINT_NEW_EN

//记录一个点，打印一个曲线点
#if PRINT_REAL_EN==1//-------------------------------------------------------------
void PrintOneCurve(void)
{
      u16 PrintZv;
      
      //取当前记录的数据，包括符号
      PrintZv=StateC.Value[0];
      if(0x01&(StateC.FuhaoBit>>0))//负数
      {
        PrintZv=StateC.Value[0]|0x8000;
      }
      
      //手动停机或传感器无信号时不打印实时曲线
      if((PrintZv==SENIOR_NULL)||(PrintZv==HAND_STOP))
        return;
      
      PrintStart();//打印准备------------------------------------------------------
      SetIni();
      delay_ms(2);
      SetCnPrint(1);
      delay_ms(2);

      if(StateA1.RecordNum==1)
        StateC.HaveBc=0;
      
      if(StateC.HaveBc==0)
      {
        StateC.ValueBc=PrintZv;
        PrintY5Zv();//打印5个y标度和y轴
      }
  
      PrintCC(StateC.ValueBc,PrintZv,StateA1.RecordNum,((StateA1.RecordNum%5)==1));//每5个点显示一次时间
      
      StateC.ValueBc=PrintZv;//保留上一个点
      StateC.HaveBc=1;
      
      PrintEnd();//打印结束-------------------------------------------------------  
}
#endif

//记录一个，打印一个列表
#if PRINT_RT_LIST_EN==1//-------------------------------------------------------
void PrintOneList(void)
{
      
      #if PRINT_RT_AL_EN==1
      if(ChAlarm()==0)//没有通道超标，不打印
        return;
      #endif
  
      u16 j,PrintZv[CH_NUM];
      
      //取当前记录的数据，包括符号
      for(j=0;j<CH_NUM;j++)
      {
        PrintZv[j]=StateC.Value[j];
        if(0x01&(StateC.FuhaoBit>>j))//负数
        {
          PrintZv[j]=StateC.Value[j]|0x8000;
        }
      }
      
      //手动停机或传感器无信号时不打印实时曲线
      if((PrintZv[0]==SENIOR_NULL)||(PrintZv[0]==HAND_STOP))
        return;
      
      PrintStart();//打印准备------------------------------------------------------
      SetIni();
      delay_ms(2);
      SetCnPrint(1);
      delay_ms(2);
      
      #if BSQ_ENABLE==0
      if(StateA1.RecordNum==1)
      {
            U1SendString("用户名：");
            i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
            U1SendByte(0x0a);
            U1SendString("标  题：");
            i2c_ee_read_to_pc(TITLE_ADDR,20,0);
            U1SendByte(0x0a);
            GoOneHang(1);
            #if PRINT_RT_LIST_BK_SD==1
            U1SendString("日期    时间     温度（℃） 湿度（%RH）\n");
            #elif (PRINT_RT_LIST_BK_2WD==1)
            U1SendString("日期    时间     温度1（℃） 温度2（℃）\n");
            #elif PRINT_RT_ALL==1
              for(j=0;j<CH_NUM;j++)
              {
                  u8 ChT;//通道类型
                  ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//读取该通道类型
                  
                    //通道号.......................................................
                  U1SendString("通道");
                  U1SendByte(0x31+j);
                  U1SendByte(':');
                  //通道类型......................................................
                  if(ChT==_T_T)//Temp/温度
                  {
                     U1SendString("温度(℃)");
                  }
                  else if(ChT==_T_H)//Humi/湿度
                  {
                     U1SendString("湿度(%RH)");
                  }
                  if(j%2==1)
                  {
                    //U1SendByte(0x0d);
                    U1SendByte(0x0a);
                  }
              }
            
            #else
              

                   U1SendString("日期    时间     温度（℃）\n");

     
            #endif
      }
      PrintTime(StateA1.RecordNum);//时间
      #else
      PrintTime(Rtc.SB);
      #endif

      #if ((PRINT_RT_LIST_BK_SD==1)||(PRINT_RT_LIST_BK_2WD==1))
        U1SendByte(' ');
        PrintValue(PrintZv[0]);//数值
        U1SendByte(' ');
        PrintValue(PrintZv[1]);//数值,湿度
      #elif PRINT_RT_ALL==1
        if(CH_NUM>2)
        {
           U1SendByte(0x0a);//换行
        }
                
        for(j=0;j<CH_NUM;j++)
        {
           U1SendByte(' ');
           PrintValue(PrintZv[j]);//数值
           if((CH_NUM>4)&&(j==3))//1行最多显示4个通道
           {
              U1SendByte(0x0a);
           }
        }
      
      #else
      U1SendByte(' ');
      PrintValue(PrintZv[0]);//数值
      #endif
      U1SendByte(0x0a);
      
      PrintEnd();//打印结束-------------------------------------------------------
}
#endif

//===================================================================================================================

#if GPRS_TC_EN==1

#if PRINT_MIN_MAX_EN==1
//最大和最小
void Print_MaxMin(u8 ch)
{

                      U1SendString("通道");U1SendByte(0x30+ch+1);U1SendByte(0x0a);
                      
                      U1SendString("最大");
                      PrintValue(StateC.print_max[ch]);//数值
                      U1SendByte(',');
                      U1SendString("最小");
                      PrintValue(StateC.print_min[ch]);//数值


                      U1SendByte(0x0a);  
}
#endif

//buf: RTC(4)+BAT(1)+GPS(8)+2*CH_NUM
//从buf中取出数据并且转换为打印格式的数据，发送到串口U1
/*************************************
1、2通道：
131203 13:52:40   11.7  53.6
>2通道：
131203 13:52:40
11.7  53.6 11.7  53.6
11.7  53.6 11.7  53.6
********************************/
void Buf_To_Print(u8* buf)
{
    u32 tt;
    u16 j,vv;
    u8 cc=0;//开关门标记判断
    
    
    #if ((ON_OFF_BJ_EN==1)||(DOOR1_BJ_EN==1)||(DOOR2_BJ_EN==1)||(PRINT_NO_NULL_EN==1))
    
        #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
        vv=U8_TO_U16(buf[13],buf[14]);
        #else
        vv=U8_TO_U16(buf[5],buf[6]);
        #endif
        
   #endif
    
    
    #if ON_OFF_BJ_EN==1
        if((vv==KJ_BJ)||(vv==GJ_BJ)||(vv==SD_BJ)||(vv==DD_BJ))//开机、关机、上电、断电标记不打印
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
          cc=1;//需要打印
        }  
        #else

        if((vv==DOOR1_OPEN)||(vv==DOOR1_CLOSE)||(vv==DOOR2_OPEN)||(vv==DOOR2_CLOSE))//门开关不打印
        {
          return;
        }   
       #endif
        
        
    #endif
    
    
        
    tt=U8_TO_U32(buf[0],buf[1],buf[2],buf[3]);

    #if GPRS_PRT_JG_EN==1
    if(PcBsq.RecTime<=60)//只有记录间隔<=1分钟时，才判断
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
        
        if(alarm)//有数据超标
        {
           if(((tt%120)!=0)&&(cc==0))//2分钟整点才打印
              return;
        }
        else
        {
           if(((tt%PRINT_SF_TIME)!=0)&&(cc==0))//5分钟整点才打印
              return;
        }
        
      
       #else
           if(((tt%PRINT_SF_TIME)!=0)&&(cc==0))//5分钟整点才打印
              return;
       #endif
    }
    #endif
    
      
    PrintTime(tt);//时间
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
            
            PrintValue(vv);//数值
            
            
            //如果是开关门标记，不计算入最大最小值，并且其他通道不进入
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//其他通道不进入
            #endif

            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
            
            
            #if (JLY_MODEL==_8_134678NTC_25HIH)
            if((CH_NUM>4)&&(j==5))//1行最多显示4个通道
            {
              U1SendByte(0x0a);
              U1SendString("         ");
            }
            #endif
  
        }
        
    #elif PRINT_FZ_EN==1//分组打印 
        
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
            
            PrintValue(vv);//数值
            
            
            //如果是开关门标记，不计算入最大最小值，并且其他通道不进入
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//其他通道不进入
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
    
        //全部通道打印
        if(CH_NUM>2)
        {
            U1SendByte(0x0a);//换行
        }
        
        for(j=0;j<CH_NUM;j++)
        {
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//数值
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//其他通道不进入
            #endif
                
            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
            
            
            if((CH_NUM>4)&&(j==3))//1行最多显示4个通道
            {
              U1SendByte(0x0a);
            }
    
        }
        
    #elif PRINT_1W1S_EN==1
        
        for(j=0;j<2;j++)//只打印前面的温度1和湿度1
        {
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//数值
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//其他通道不进入
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
        for(j=0;j<=2;j=j+2)//只打印前面的温度1和湿度1
        #endif
        {
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);
            #endif
            
            PrintValue(vv);//数值
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//其他通道不进入
            #endif
            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
            
        }
        
    
    #else//只打印温度1
        
        for(j=0;j<1;j++)
        {
            j=0;
        
            U1SendByte(' ');
            
            #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1))
            vv=U8_TO_U16(buf[13+2*j],buf[14+2*j]);
            #else
            vv=U8_TO_U16(buf[5+2*j],buf[6+2*j]);        
            #endif
            
            PrintValue(vv);//数值
            
            #if DOOR_PRINT_EN==1
            if(cc==1)
              break;//其他通道不进入
            #endif
            
            #if PRINT_MIN_MAX_EN==1
            StateC.print_min[j]=get_min(StateC.print_min[j],vv);
            StateC.print_max[j]=get_max(StateC.print_max[j],vv);
            #endif
        }
        
    #endif
    
    
    U1SendByte(0x0a);
    
}


//主循环中，1s打印一包数据
u8 PrintDeel(void)
{ 
        //2秒打印一次
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
                  U1_TO_PC;//打印前，先把U1切换到PC
                  delay_ms(50);  
                  
                #endif

                //每包打印条数
                u8 nn,aa;
                #if ((RPTINT_ONLY_WD_EN==1)||(PRINT_FZ_EN==1))
                    //CH_NUM  G95-4.6.8   ->1行  
                    
                    nn=1;
                    
                    #if CH_NUM==8
                       aa=8/nn;               
                    #else
                       aa=9/nn;//2秒最多打印9行
                    #endif
     
                #elif PRINT_ALL_CH_EN==1
                    //全部通道
                    //CH_NUM  1.2 ->1行  3.4->2行  5.6.7.8->3行
                    
                    if(CH_NUM<3)
                      nn=1;
                    else if(CH_NUM<5)
                      nn=2;
                    else
                      nn=3;
                    
                    aa=9/nn;//2秒最多打印9行
                    
                #else
                    
                    
                    nn=1;
                    
                    #if CH_NUM==8
                       aa=8/nn;               
                    #else
                       aa=9/nn;//2秒最多打印9行
                    #endif 
                    
                #endif
                
                
                
                
                //记录间隔<=1分钟时，每x个点才打印一个数据故，aa 需 x倍
                //如果记录间隔>1分钟时，不起作用
                #if GPRS_PRT_JG_EN==1
                if(PcBsq.RecTime<=60)
                {
                  aa=aa*(PRINT_SF_TIME/PcBsq.RecTime);
                }
                #endif
                
                
                if(Pt.PrintThisDueout>aa)//2秒打印3条，即9行
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
                         
                         #elif PRINT_FZ_EN==1//分组打印
                         
                         
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
                         
                         #else//打印通道1

                         u8 ch=0;
                         Print_MaxMin(ch);
                         #endif
            
                      #endif
                  
                  
                  
                      GoOneHang(1);
                      U1SendString("********************************\n");
                  
                      #if RPINT_B_EN==1
                        U1SendString("有效开始时间：                \n\n");
                        GoOneHang(1);
                        U1SendString("有效结束时间：                \n\n");
                        GoOneHang(1);
                        U1SendString("送  货  方  ：                \n\n");
                        GoOneHang(1);
                        U1SendString("收  货  方  ：                \n\n");
                        GoOneHang(6);
                        
                     #elif RPINT_A_EN==1
                        U1SendString("收货方：                \n\n");
                        GoOneHang(1);
                        U1SendString("运单号：                \n\n");
                        GoOneHang(1);
                        U1SendString("车牌号：                \n\n");
                        GoOneHang(1);
                        U1SendString("收货人：                \n\n");
                        GoOneHang(1);
                        U1SendString("交货人：                \n\n");
                        GoOneHang(1);
                        U1SendString("时  间：                \n\n");
                        GoOneHang(6);
                        
                     #elif PRINT_D_EN==1
                        
                        U1SendString("收货方：                \n\n");
                        GoOneHang(1);
                        U1SendString("收货人：                \n\n");
                        GoOneHang(1);
                        U1SendString("交货人：                \n\n");
                        GoOneHang(6);
                        
                    #elif PRINT_E_EN==1   
                        
                        U1SendString("企业名称：              \n\n");
                        GoOneHang(1);
                        U1SendString("设备名称：              \n\n");
                        GoOneHang(1);
                        U1SendString("客户名称：              \n\n");
                        GoOneHang(1);
                        U1SendString("品    名：              \n\n");
                        GoOneHang(1);
                        U1SendString("车 牌 名：              \n\n");
                        GoOneHang(1);
                        U1SendString("驾 驶 员：              \n\n");
                        GoOneHang(1);
                        U1SendString("收 货 人：              \n\n");
                        GoOneHang(6);
                        
                     #elif PRINT_C_EN==1
                        U1SendString("打印日期：");
                        Rtc.SS=ReadRtcD10();
                        PrintTime(Rtc.SS);
                        
                        GoOneHang(1);
                        U1SendString("签名确认：                \n\n");
                        GoOneHang(6);
                        
                     #else
                
                                    U1SendString("签字：         时间：           \n\n");
                                    GoOneHang(1);
                                    U1SendString("备注：\n\n");
                                    GoOneHang(6);
                                    
                     #endif
                    
                    //打印结束------------------------------------------------------
                    PrintEnd();
                    
                    #if RF_U1_EN==1
                    U1ToRf();//打印完成后，切换到RF
                    #endif
                    
                }
                

            
            }
            

            
            return 1;
        }

        

        
        return 0;
}


//开始打印
void Print_SF_Data(void)
{
       //如果正在打印时，再次按下打印键，则退出打印
       if(Pt.PrintThisDueout>0)//zz
       {
         Pt.PrintThisDueout=0;
         return;
       }
  

        //打印开始提示------------------------------------------------------
        PrintStart();
        SetIni();
        delay_ms(2);
        SetCnPrint(1);//汉字模式
        delay_ms(2);
        
        #if PRINT_FZ_EN==0
        LcdNN(PRINTING);
        #endif
        
        //只打印用户名和标题
        GoOneHang(1);
        U1SendString("用 户 名：");
        i2c_ee_read_to_pc(USER_NAME_ADDR,20,0);
        U1SendByte(0x0a);
        U1SendString("标    题：");
        i2c_ee_read_to_pc(TITLE_ADDR,20,0);
        U1SendByte(0x0a);
        GoOneHang(1);
        
        //打印历史数据----------------------------------
        
        //当前储存状态, 必须先赋值，否则Gprs_SF_Send_Data 会出错
        Pt.GprsThisRec16Num=PcBsq.Rec16Num;
        Pt.GprsThisRecPt=PcBsq.RecPt;
  
        Pt.PrintThisDueout=Pt.PrintDueout;//需打印的条数  
        
        if(Pt.PrintThisDueout==0)
        {
          U1SendString("没有记录数据");
          U1SendByte(0x0a);
          GoOneHang(5);
          //打印结束
          PrintEnd();
        }
        else
        {
            #if PRINT_ALL_CH_EN==1//打印全部通道
          
            u8 j;
            for(j=0;j<CH_NUM;j++)
            {
                u8 ChT;//通道类型
                ChT=*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);//读取该通道类型
                
                  //通道号.......................................................
                U1SendString("通道");
                U1SendByte(0x31+j);
                U1SendByte(':');
                //通道类型......................................................
                if(ChT==_T_T)//Temp/温度
                {
                   U1SendString("温度(℃)");
                }
                else if(ChT==_T_H)//Humi/湿度
                {
                   U1SendString("湿度(%RH)");
                }
                if(j%2==1)
                {
                  //U1SendByte(0x0d);
                  U1SendByte(0x0a);
                }
            }
            U1SendByte(0x0a);
            
            #elif RPTINT_ONLY_WD_EN==1
                  //温度
                  #if ((JLY_MODEL==_2_NTC)||(JLY_MODEL==_3_2NTC_1HIH)||(JLY_MODEL==_3_2_NTC_1HIH))
                    U1SendString("时间   温度1 温度2 \n");
                  #elif ((JLY_MODEL==_3_NTC)||(JLY_MODEL==_3_PT_F100_200)||(JLY_MODEL==_4_3NTC_1HIH))
                    U1SendString("时间   温度1 温度2 温度3 \n");
                  #elif ((JLY_MODEL==_4_NTC)||(JLY_MODEL==_5_4NTC_1HIH))
                    U1SendString("时间   温度1 温度2 温度3 温度4\n");
                    
                  #elif (JLY_MODEL==_8_134678NTC_25HIH)
                    U1SendString("时间   温度1 温度2 温度3 温度4\n");
                    U1SendString("       温度5 温度6 \n");

                    
                
                  //温湿度
                  #elif CH_NUM==2
                    U1SendString("时间   温度1  \n");
                  #elif CH_NUM==4
                    U1SendString("时间   温度1 温度2 \n");
                  #elif CH_NUM==6
                    U1SendString("时间   温度1 温度2 温度3 \n");
                  #elif CH_NUM==8
                    U1SendString("时间   温度1 温度2 温度3 温度4\n");
                  #endif
                    
                    
            #elif PRINT_1_3_EN==1//必须在 PRINT_2WD_EN 上面
            U1SendString("日期    时间   温度1(℃) 温度3(℃)\n");         
                    
            #elif PRINT_2WD_EN==1
             U1SendString("日期    时间     温度1  温度2\n");

             
            #elif PRINT_1W1S_EN==1
             U1SendString("日期    时间   温度℃ 湿度%RH\n");
             
            #elif PRINT_FZ_EN==1//G95-4  分组打印
             
             
                 U1SendString("组");U1SendValue(Pt.prt_x);U1SendString("\n");
             
             
                 #if PRINT_FZ_WD_EN==1
                 U1SendString("日期    时间   温度℃ \n");
                 #else
                 
                    #if(JLY_MODEL==_3_2NTC_1HIH)
                    
                    if(Pt.prt_x==2)
                      U1SendString("日期    时间   温度℃ \n");
                    else
                      U1SendString("日期    时间   温度℃ 湿度%RH\n");
                    
                    #else
                      U1SendString("日期    时间   温度℃ 湿度%RH\n");
                    
                    #endif
                    
                    
                 #endif
             
                      
            #else

                   U1SendString("日期    时间     温度（℃）\n");

                
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
                            U1_TO_PC;//打印前，先把U1切换到PC
                            delay_ms(50);  
                            
                            #if GPRS_TC_EN==1
                                Print_SF_Data();//zz
                            #else
                                PrintNewData();
                            #endif
                            
                            U1ToRf();//打印完成后，切换到RF
                          
        
                        #else
                          
                            #if GPRS_TC_EN==1
                                Print_SF_Data();//zz
                            #else
                                PrintNewData();
                            #endif
                                
                        #endif
                          
                        //如果是关机状态，要显示关机
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
                Print_Begin();//开始打印
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
                  
                  
                  //清除完成后切换到RF
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


