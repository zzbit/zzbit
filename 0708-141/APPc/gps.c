#include "_config.h"
/******************************************************************************
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh 
<1> UTCʱ�䣬hhmmss.sss(ʱ����.����)��ʽ 
<2> ��λ״̬��A=��Ч��λ��V=��Ч��λ 
<3> γ��ddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������) 
<4> γ�Ȱ���N(������)��S(�ϰ���) 
<5> ����dddmm.mmmm(�ȷ�)��ʽ(ǰ���0Ҳ��������) 
<6> ���Ȱ���E(����)��W(����) 
<7> ��������(000.0~999.9�ڣ�ǰ���0Ҳ��������) 
<8> ���溽��(000.0~359.9�ȣ�������Ϊ�ο���׼��ǰ���0Ҳ��������) 
<9> UTC���ڣ�ddmmyy(������)��ʽ 
<10> ��ƫ��(000.0~180.0�ȣ�ǰ���0Ҳ��������) 
<11> ��ƫ�Ƿ���E(��)��W(��) 
<12> ģʽָʾ(��NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч)

$GNRMC,095515.111,V,,,,,0.00,0.00,160616,,,N*59


$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*xx<CR><LF>
$GPGGA����ʼ������������ʽ˵��(����ΪGPS��λ����)��
<1> UTCʱ�䣬��ʽΪhhmmss.sss��
<2> γ�ȣ���ʽΪddmm.mmmm(��һλ����Ҳ������)��
<3> γ�Ȱ���N��S(��γ����γ)
<4> ���ȣ���ʽΪdddmm.mmmm(��һλ��Ҳ������)��
<5> ���Ȱ���E��W(����������)
<6> GPS״̬�� 0δ��λ�� 1�ǲ�ֶ�λ�� 2��ֶ�λ�� 3��ЧPPS�� 6���ڹ���
<7> ʹ��������������00��12(��һ����Ҳ������)
<8> ˮƽ��ȷ�ȣ�0.5��99.9
<9> �����뺣ƽ��ĸ߶ȣ�-9999.9��9999.9��
M ָ��λ��
<10> ���ˮ׼��߶ȣ�-9999.9��9999.9��
M ָ��λ��
<11> ���GPS��������(RTCM SC-104)���������RTCM���͵����������粻�ǲ�ֶ�λ��Ϊ��
<12> ��ֲο���վ��ţ���0000��1023(��λ0Ҳ������)��
* ��������־��
xx ��$��ʼ��*֮�������ASCII������У���
<CR> �س�
<LF> ����

//$GPGGA,090854.000,3018.3243,N,12021.9969,E,1,05,4.0,56.8,M,7.1,M,,0000*53

//$GPRMC,121704.00,V,,,,,,,140514,,,N*79
//$GPRMC,112128.00,A,3018.28259,N,12022.00256,E,0.230,,200514,,,D*73
                     γ��         ���� 
���� ���� ���� ��
��γ ���� ��γ ��
γ�ȸ�ʽ:  ddmm.mmmm   3018.28259   -> 30+ (18.28259/60) = 30.304709 ���ȣ�     (6λС��)
���ȸ�ʽ��dddmm.mmmm   12022.00256  -> 120+ (22.00256/60) = 120.366709���ȣ�

//С�����4λ
$GPRMC,074110.000,V,3018.2827,N,12022.0094,E,,,081214,,,0*0E
//С�����5λ
$GPRMC,112128.00,A,3018.28259,N,12022.00256,E,0.230,,200514,,,D*73(��ģ��)

GGA:
//����ģ��                                  ������8������08
$GNGGA,020213.000,3018.3040,N,12022.0080,E,2,8,2.29,83.9,M,7.1,M,0000,0000*40
$GNRMC,020213.000,A,3018.3040,N,12022.0080,E,0.06,137.15,200515,,,D*75(����-VK2828M3G5-4λС��)

//GPSģ��                                  ������03
$GPGGA,074110.000,3018.2827,N,12022.0094,E,0,03,,34.7,M,7.1,M,,0000*7E

//SIM68               ww           jj        ������5
$GNGGA,033239.000,3018.2762,N,12021.9866,E,2,5,2.87,78.5,M,7.1,M,0000,0000*47
$GNRMC,050909.000,A,3018.3295,N,12021.9953,E,9.26,200.62,181215,,,A*71


********************************************************************************/
#if GPS_MOUSE_EN==1

//����������С����ʽ��ת��Ϊ��γ�ȸ�������
float GpsTran(u8 dInt,u32 dDec)
{
   float ft;
   ft=0.000001*(dDec&0x7fffff)+dInt;
   if(dDec&800000)
     ft=ft*(-1);
   return ft;
}

//�ж�A��B 2��֮���Ƿ���Ե�����ֹ��ƫ�� DD  (120.380403-120.380969)*1000000
u8 IsMove(float lonA,float latA,float lonB,float latB,u16 DD)
{
  float ww,jj;
  
  ww=(latA-latB)*1000000;
  if(ww<0)
    ww=ww*(-1);
  
  jj=(lonA-lonB)*1000000;
  if(jj<0)
    jj=jj*(-1);
  
  if((ww>DD)||(jj>DD))//������DD��Χ�ڣ��ж�Ϊ���ƶ�
    return 1;
  else
    return 0;
  
}

/*
//SIM68             
                                             �ٶ�(��)��1��=1.852km/h=1852/3600 m/s��
$GNRMC,050909.000,A,3018.3295,N,12021.9953,E,9.26,200.62,181215,,,A*71
*/
u16 Get_kmh(void)
{
   float ft=0.0;
   u16 vv;
   u16 ss;

   ss=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,'.',4);//��4��.
   if(ss==10000) return 0;
   
   if(CheckNum(Uart_1.RxBuf[ss-1])&&CheckNum(Uart_1.RxBuf[ss+1])&&CheckNum(Uart_1.RxBuf[ss+2]))
   {
     if(CheckNum(Uart_1.RxBuf[ss-3])&&CheckNum(Uart_1.RxBuf[ss-2]))
     {
       ft=(Uart_1.RxBuf[ss-3]-0x30)*100+(Uart_1.RxBuf[ss-2]-0x30)*10+(Uart_1.RxBuf[ss-1]-0x30);
     }
     else if(CheckNum(Uart_1.RxBuf[ss-2]))
     {
       ft=(Uart_1.RxBuf[ss-2]-0x30)*10+(Uart_1.RxBuf[ss-1]-0x30);
     }
     else
     {
       ft=(Uart_1.RxBuf[ss-1]-0x30);
     }
     
     ft=ft+((Uart_1.RxBuf[ss+1]-0x30)*10+(Uart_1.RxBuf[ss+2]-0x30))*0.01;
     
     ft=ft*1.852;//��תΪkm/h
   }
   
   vv=(u16)(ft*10);
   
   return vv;
}

//��GGA ȡ������
u8 Get_Stn(u16 pt)
{
  u8 stn=0;//������
  u8 sct=0;//������λ��
  
  if(CheckNum(Uart_1.RxBuf[pt+5]))//5λС��
     sct=11;
  else//4λС��
     sct=10;
  
  if(CheckNum(Uart_1.RxBuf[pt+sct+1]))
     stn=(Uart_1.RxBuf[pt+sct]-0X30)*10+(Uart_1.RxBuf[pt+sct+1]-0X30);
  else
     stn=Uart_1.RxBuf[pt+sct]-0X30;
  
  return stn;
}

//��������������,��γ�Ȼ򾭶ȵ�С��, Uart_1.RxBuf , ptλ�ã� nnγ�Ȼ򾭶ȼ���
u32 Get_XY_Dec(u16 pt,u8 nn)
{
   u8 dec;
   u16 zs;
   u32 xs,out,pN;
   float ft;
   
   //�ж�С��λ����5��4
   if(CheckNum(Uart_1.RxBuf[pt+5]))
     dec=5;
   else
     dec=4;

    //zs���3λ����u16 ,С�����5λ����u32
    zs=(u16)10*(Uart_1.RxBuf[pt-2]-0x30)+(Uart_1.RxBuf[pt-1]-0x30);
    
    xs=(u32)10000*(Uart_1.RxBuf[pt+1]-0x30)+(u32)1000*(Uart_1.RxBuf[pt+2]-0x30)\
        +(u32)100*(Uart_1.RxBuf[pt+3]-0x30)+ (u32)10*(Uart_1.RxBuf[pt+4]-0x30);
    
    if(dec==5)
    {
      xs=xs+(Uart_1.RxBuf[pt+5]-0x30);
    }

    ft=((0.00001*xs +zs)/60)*1000000;//����6λС��
    out=(u32)ft;
    
    ////���Ǳ�γ������γ��Ϊ��
    if(dec==5)
      pN=pt+7;
    else
      pN=pt+6;
    
    if(nn==1)//��γ��
    {
        if((Uart_1.RxBuf[pN]!='N'))
        {
          out= out|0x800000;
        }
    }
    else//����
    {
        if((Uart_1.RxBuf[pN]!='E'))
        {
          out= out|0x800000;
        }
    }
    
    return out;
}

//��RMC ȡʱ��
/*
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh 
<1> UTCʱ�䣬hhmmss.sss(ʱ����.����)��ʽ 
<9> UTC���ڣ�ddmmyy(������)��ʽ 

$GNRMC,082622.000,A,3018.3034,N,12021.9878,E,1.54,176.35,280416,,,D*76
$GNRMC,055317.000,V,,,,,0.34,277.59,150616,,,N*5A
*/
u8 Gps_UtcRtc(void)
{
  u16 a1,a9;
  StrcutRtc  Utc;
  
  a1=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,',',1);//��1��,
  if(a1==10000)return 0;
  a9=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,',',9);//��9��,
  if(a9==10000)return 0;
    
  
  if(CK_Buf_Num(&Uart_1.RxBuf[a1+1],6)&&CK_Buf_Num(&Uart_1.RxBuf[a9+1],6))//ȫ��������
  {
      //ddmmyy
      Utc.Year=(Uart_1.RxBuf[a9+5]-0x30)*10+ (Uart_1.RxBuf[a9+6]-0x30);
      Utc.Month=(Uart_1.RxBuf[a9+3]-0x30)*10+ (Uart_1.RxBuf[a9+4]-0x30);
      Utc.Day=(Uart_1.RxBuf[a9+1]-0x30)*10+ (Uart_1.RxBuf[a9+2]-0x30);
      
      //hhmmss
      Utc.Hour=(Uart_1.RxBuf[a1+1]-0x30)*10+ (Uart_1.RxBuf[a1+2]-0x30);
      Utc.Minute=(Uart_1.RxBuf[a1+3]-0x30)*10+ (Uart_1.RxBuf[a1+4]-0x30);
      Utc.Second=(Uart_1.RxBuf[a1+5]-0x30)*10+ (Uart_1.RxBuf[a1+6]-0x30);
      
      if(Rtc_Is_Right(&Utc))//ʱ���ʽ��ȷ
      {
            //UTCת��Ϊ����ʱ�䣬����ʱ��=��������ʱ��+8Сʱ
            Utc.SS=DateToSeconds(&Utc)+3600*8;
            
            //�ȶ�ȡ�豸RTC
            Rtc.SS=ReadRtcD10();
            
            //�Ƚϣ����ʱ�� ����1�������ϣ��Զ���ʱ
            if((Rtc.SS>Utc.SS+D_RTC_TIME)||(Rtc.SS<Utc.SS-D_RTC_TIME))
            {
                SecondsToDate(Utc.SS,&Rtc);
                          
                //����ʱ��
                RtcD10ToBcd(&Rtc);//ת��BCD
                RtcSetTime();
                RtcBcdToD10(&Rtc);
                          
                return 1;
            }
      }
  }
  
  return 0;
  
  
}

//ȡGPS��γ��,��������0ʧ�ܣ�1�ɹ�
/* SIM68
$GNRMC,082622.000,A,3018.3034,N,12021.9878,E,1.54,176.35,280416,,,D*76
$GNRMC,055317.000,V,,,,,0.34,277.59,150616,,,N*5A


$GPRMC,105707.000,A,3018.3199,N,12022.1622,E,0.35,34.84,271217,,,A*5B
*/

u8 Get_GpsPosition(void)
{
      u8 cc=0;
      u16 jj,ww;
      

      ww=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,'.',2);//��2��.
      jj=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,'.',3);//��3��.
      
      if((ww>5)&&(jj>5)&&(jj!=10000)&&(ww!=10000))//��Ч����
      {
          if(CK_Buf_Num(&Uart_1.RxBuf[ww-4],4)&&CK_Buf_Num(&Uart_1.RxBuf[ww+1],4)\
            &&CK_Buf_Num(&Uart_1.RxBuf[jj-5],5)&&CK_Buf_Num(&Uart_1.RxBuf[jj+1],4))
          {
              //������
              //StateC.SatelliteN=Get_Stn(jj);
              //if(StateC.SatelliteN<7)//������С��,����������
              //  return 0;
            
              #if GPS_RTC_EN==1
              Gps_UtcRtc();//ʱ��У׼
              #endif
            
              //��ʱʱ��
              StateC.GpsOutPt=GPS_TIME;
              
              
              u8 wInt,jInt;
              u32 wDec,jDec;
              
              //γ��
              wInt=(u16)10*(Uart_1.RxBuf[ww-4]-0x30)+(Uart_1.RxBuf[ww-3]-0x30);
              wDec=Get_XY_Dec(ww,1);
              
              //����
              jInt=(u16)100*(Uart_1.RxBuf[jj-5]-0x30)+(u16)10*(Uart_1.RxBuf[jj-4]-0x30)+(Uart_1.RxBuf[jj-3]-0x30);
              jDec=Get_XY_Dec(jj,0);
              
              
              //����ģʽʱ������6�����һ�Σ��ʾ�ֹǰ���µ�����Ϊ3��ǰ������
              #if GPS_STATIC_EN==1

                  StateC.Vkmh=Get_kmh();
                  
                  if((StateC.Vkmh>GPS_KMH)||(StateC.LatiInt==0)||(StateC.StaticPt>0))//�ж��ٶȴ�ʱ���ж�Ϊ�˶�,����Ϊ��̬
                  {
                        StateC.LatiInt=wInt;
                        StateC.LatiDec=wDec;
                        StateC.LongiInt=jInt;
                        StateC.LongiDec=jDec;
                        
                        if(StateC.Vkmh>GPS_KMH)//�˶�ʱ�Ÿ��£���ֹ��4����������ʵʱ����
                          StateC.StaticPt=250;
                  }
                  else
                  {
                      //�ж�Ϊ��̬����ˢ������

                  }
              
              #else
                  
                  StateC.LatiInt=wInt;
                  StateC.LatiDec=wDec;
                  StateC.LongiInt=jInt;
                  StateC.LongiDec=jDec;
                  
              #endif
              
              cc=1;
              
              //ʱ���־��1�Σ�ָʾGPS��������
              Tshan();
              
          }
      }
      

      if(cc==0)//�յ���ָ���������Ч�ľ�γ������
      {
          //ʱ���־��2�Σ�ָʾGPSָ������
          Tshan();
          delay_ms(60);
          Tshan();
        
        
        /*
          if(Pt.GprsSleep==0)//����ģʽ��ÿ����0
          {
              StateC.LongiInt=0;
              StateC.LongiDec=0;
              StateC.LatiInt=0;
              StateC.LatiDec=0;
              
              StateC.Vkmh=0;
              StateC.SatelliteN=0;
          }
        */
      }

      
      return cc;
}
#endif

