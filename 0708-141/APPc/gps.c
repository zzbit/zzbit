#include "_config.h"
/******************************************************************************
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh 
<1> UTC时间，hhmmss.sss(时分秒.毫秒)格式 
<2> 定位状态，A=有效定位，V=无效定位 
<3> 纬度ddmm.mmmm(度分)格式(前面的0也将被传输) 
<4> 纬度半球N(北半球)或S(南半球) 
<5> 经度dddmm.mmmm(度分)格式(前面的0也将被传输) 
<6> 经度半球E(东经)或W(西经) 
<7> 地面速率(000.0~999.9节，前面的0也将被传输) 
<8> 地面航向(000.0~359.9度，以正北为参考基准，前面的0也将被传输) 
<9> UTC日期，ddmmyy(日月年)格式 
<10> 磁偏角(000.0~180.0度，前面的0也将被传输) 
<11> 磁偏角方向，E(东)或W(西) 
<12> 模式指示(仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效)

$GNRMC,095515.111,V,,,,,0.00,0.00,160616,,,N*59


$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*xx<CR><LF>
$GPGGA：起始引导符及语句格式说明(本句为GPS定位数据)；
<1> UTC时间，格式为hhmmss.sss；
<2> 纬度，格式为ddmm.mmmm(第一位是零也将传送)；
<3> 纬度半球，N或S(北纬或南纬)
<4> 经度，格式为dddmm.mmmm(第一位零也将传送)；
<5> 经度半球，E或W(东经或西经)
<6> GPS状态， 0未定位， 1非差分定位， 2差分定位， 3无效PPS， 6正在估算
<7> 使用卫星数量，从00到12(第一个零也将传送)
<8> 水平精确度，0.5到99.9
<9> 天线离海平面的高度，-9999.9到9999.9米
M 指单位米
<10> 大地水准面高度，-9999.9到9999.9米
M 指单位米
<11> 差分GPS数据期限(RTCM SC-104)，最后设立RTCM传送的秒数量，如不是差分定位则为空
<12> 差分参考基站标号，从0000到1023(首位0也将传送)。
* 语句结束标志符
xx 从$开始到*之间的所有ASCII码的异或校验和
<CR> 回车
<LF> 换行

//$GPGGA,090854.000,3018.3243,N,12021.9969,E,1,05,4.0,56.8,M,7.1,M,,0000*53

//$GPRMC,121704.00,V,,,,,,,140514,,,N*79
//$GPRMC,112128.00,A,3018.28259,N,12022.00256,E,0.230,,200514,,,D*73
                     纬度         经度 
东经 正， 西经 负
北纬 正， 南纬 负
纬度格式:  ddmm.mmmm   3018.28259   -> 30+ (18.28259/60) = 30.304709 （度）     (6位小数)
经度格式：dddmm.mmmm   12022.00256  -> 120+ (22.00256/60) = 120.366709（度）

//小数点后4位
$GPRMC,074110.000,V,3018.2827,N,12022.0094,E,,,081214,,,0*0E
//小数点后5位
$GPRMC,112128.00,A,3018.28259,N,12022.00256,E,0.230,,200514,,,D*73(老模块)

GGA:
//北斗模块                                  卫星数8，不是08
$GNGGA,020213.000,3018.3040,N,12022.0080,E,2,8,2.29,83.9,M,7.1,M,0000,0000*40
$GNRMC,020213.000,A,3018.3040,N,12022.0080,E,0.06,137.15,200515,,,D*75(北斗-VK2828M3G5-4位小数)

//GPS模块                                  卫星数03
$GPGGA,074110.000,3018.2827,N,12022.0094,E,0,03,,34.7,M,7.1,M,,0000*7E

//SIM68               ww           jj        卫星数5
$GNGGA,033239.000,3018.2762,N,12021.9866,E,2,5,2.87,78.5,M,7.1,M,0000,0000*47
$GNRMC,050909.000,A,3018.3295,N,12021.9953,E,9.26,200.62,181215,,,A*71


********************************************************************************/
#if GPS_MOUSE_EN==1

//根据整数和小数格式，转换为经纬度浮点数据
float GpsTran(u8 dInt,u32 dDec)
{
   float ft;
   ft=0.000001*(dDec&0x7fffff)+dInt;
   if(dDec&800000)
     ft=ft*(-1);
   return ft;
}

//判断A、B 2点之间是否可以当作静止，偏差 DD  (120.380403-120.380969)*1000000
u8 IsMove(float lonA,float latA,float lonB,float latB,u16 DD)
{
  float ww,jj;
  
  ww=(latA-latB)*1000000;
  if(ww<0)
    ww=ww*(-1);
  
  jj=(lonA-lonB)*1000000;
  if(jj<0)
    jj=jj*(-1);
  
  if((ww>DD)||(jj>DD))//超出了DD范围内，判断为在移动
    return 1;
  else
    return 0;
  
}

/*
//SIM68             
                                             速度(节)（1节=1.852km/h=1852/3600 m/s）
$GNRMC,050909.000,A,3018.3295,N,12021.9953,E,9.26,200.62,181215,,,A*71
*/
u16 Get_kmh(void)
{
   float ft=0.0;
   u16 vv;
   u16 ss;

   ss=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,'.',4);//第4个.
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
     
     ft=ft*1.852;//节转为km/h
   }
   
   vv=(u16)(ft*10);
   
   return vv;
}

//从GGA 取卫星数
u8 Get_Stn(u16 pt)
{
  u8 stn=0;//卫星数
  u8 sct=0;//卫星数位置
  
  if(CheckNum(Uart_1.RxBuf[pt+5]))//5位小数
     sct=11;
  else//4位小数
     sct=10;
  
  if(CheckNum(Uart_1.RxBuf[pt+sct+1]))
     stn=(Uart_1.RxBuf[pt+sct]-0X30)*10+(Uart_1.RxBuf[pt+sct+1]-0X30);
  else
     stn=Uart_1.RxBuf[pt+sct]-0X30;
  
  return stn;
}

//解析出具体数据,求纬度或经度的小数, Uart_1.RxBuf , pt位置， nn纬度或经度极性
u32 Get_XY_Dec(u16 pt,u8 nn)
{
   u8 dec;
   u16 zs;
   u32 xs,out,pN;
   float ft;
   
   //判断小数位数，5或4
   if(CheckNum(Uart_1.RxBuf[pt+5]))
     dec=5;
   else
     dec=4;

    //zs最多3位所以u16 ,小数最多5位所以u32
    zs=(u16)10*(Uart_1.RxBuf[pt-2]-0x30)+(Uart_1.RxBuf[pt-1]-0x30);
    
    xs=(u32)10000*(Uart_1.RxBuf[pt+1]-0x30)+(u32)1000*(Uart_1.RxBuf[pt+2]-0x30)\
        +(u32)100*(Uart_1.RxBuf[pt+3]-0x30)+ (u32)10*(Uart_1.RxBuf[pt+4]-0x30);
    
    if(dec==5)
    {
      xs=xs+(Uart_1.RxBuf[pt+5]-0x30);
    }

    ft=((0.00001*xs +zs)/60)*1000000;//保留6位小数
    out=(u32)ft;
    
    ////不是北纬，即南纬，为负
    if(dec==5)
      pN=pt+7;
    else
      pN=pt+6;
    
    if(nn==1)//对纬度
    {
        if((Uart_1.RxBuf[pN]!='N'))
        {
          out= out|0x800000;
        }
    }
    else//经度
    {
        if((Uart_1.RxBuf[pN]!='E'))
        {
          out= out|0x800000;
        }
    }
    
    return out;
}

//从RMC 取时钟
/*
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh 
<1> UTC时间，hhmmss.sss(时分秒.毫秒)格式 
<9> UTC日期，ddmmyy(日月年)格式 

$GNRMC,082622.000,A,3018.3034,N,12021.9878,E,1.54,176.35,280416,,,D*76
$GNRMC,055317.000,V,,,,,0.34,277.59,150616,,,N*5A
*/
u8 Gps_UtcRtc(void)
{
  u16 a1,a9;
  StrcutRtc  Utc;
  
  a1=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,',',1);//第1个,
  if(a1==10000)return 0;
  a9=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,',',9);//第9个,
  if(a9==10000)return 0;
    
  
  if(CK_Buf_Num(&Uart_1.RxBuf[a1+1],6)&&CK_Buf_Num(&Uart_1.RxBuf[a9+1],6))//全部是数字
  {
      //ddmmyy
      Utc.Year=(Uart_1.RxBuf[a9+5]-0x30)*10+ (Uart_1.RxBuf[a9+6]-0x30);
      Utc.Month=(Uart_1.RxBuf[a9+3]-0x30)*10+ (Uart_1.RxBuf[a9+4]-0x30);
      Utc.Day=(Uart_1.RxBuf[a9+1]-0x30)*10+ (Uart_1.RxBuf[a9+2]-0x30);
      
      //hhmmss
      Utc.Hour=(Uart_1.RxBuf[a1+1]-0x30)*10+ (Uart_1.RxBuf[a1+2]-0x30);
      Utc.Minute=(Uart_1.RxBuf[a1+3]-0x30)*10+ (Uart_1.RxBuf[a1+4]-0x30);
      Utc.Second=(Uart_1.RxBuf[a1+5]-0x30)*10+ (Uart_1.RxBuf[a1+6]-0x30);
      
      if(Rtc_Is_Right(&Utc))//时间格式正确
      {
            //UTC转换为北京时间，北京时间=格林威治时间+8小时
            Utc.SS=DateToSeconds(&Utc)+3600*8;
            
            //先读取设备RTC
            Rtc.SS=ReadRtcD10();
            
            //比较，如果时钟 差在1分钟以上，自动对时
            if((Rtc.SS>Utc.SS+D_RTC_TIME)||(Rtc.SS<Utc.SS-D_RTC_TIME))
            {
                SecondsToDate(Utc.SS,&Rtc);
                          
                //设置时钟
                RtcD10ToBcd(&Rtc);//转成BCD
                RtcSetTime();
                RtcBcdToD10(&Rtc);
                          
                return 1;
            }
      }
  }
  
  return 0;
  
  
}

//取GPS经纬度,卫星数，0失败，1成功
/* SIM68
$GNRMC,082622.000,A,3018.3034,N,12021.9878,E,1.54,176.35,280416,,,D*76
$GNRMC,055317.000,V,,,,,0.34,277.59,150616,,,N*5A


$GPRMC,105707.000,A,3018.3199,N,12022.1622,E,0.35,34.84,271217,,,A*5B
*/

u8 Get_GpsPosition(void)
{
      u8 cc=0;
      u16 jj,ww;
      

      ww=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,'.',2);//第2个.
      jj=GetXpt(Uart_1.RxBuf,0,Uart_1.RecLen,'.',3);//第3个.
      
      if((ww>5)&&(jj>5)&&(jj!=10000)&&(ww!=10000))//有效数据
      {
          if(CK_Buf_Num(&Uart_1.RxBuf[ww-4],4)&&CK_Buf_Num(&Uart_1.RxBuf[ww+1],4)\
            &&CK_Buf_Num(&Uart_1.RxBuf[jj-5],5)&&CK_Buf_Num(&Uart_1.RxBuf[jj+1],4))
          {
              //卫星数
              //StateC.SatelliteN=Get_Stn(jj);
              //if(StateC.SatelliteN<7)//卫星数小于,不接收数据
              //  return 0;
            
              #if GPS_RTC_EN==1
              Gps_UtcRtc();//时钟校准
              #endif
            
              //超时时间
              StateC.GpsOutPt=GPS_TIME;
              
              
              u8 wInt,jInt;
              u32 wDec,jDec;
              
              //纬度
              wInt=(u16)10*(Uart_1.RxBuf[ww-4]-0x30)+(Uart_1.RxBuf[ww-3]-0x30);
              wDec=Get_XY_Dec(ww,1);
              
              //经度
              jInt=(u16)100*(Uart_1.RxBuf[jj-5]-0x30)+(u16)10*(Uart_1.RxBuf[jj-4]-0x30)+(Uart_1.RxBuf[jj-3]-0x30);
              jDec=Get_XY_Dec(jj,0);
              
              
              //正常模式时，数据6秒更新一次，故静止前最新的数据为3秒前的数据
              #if GPS_STATIC_EN==1

                  StateC.Vkmh=Get_kmh();
                  
                  if((StateC.Vkmh>GPS_KMH)||(StateC.LatiInt==0)||(StateC.StaticPt>0))//判断速度大时才判断为运动,否则为静态
                  {
                        StateC.LatiInt=wInt;
                        StateC.LatiDec=wDec;
                        StateC.LongiInt=jInt;
                        StateC.LongiDec=jDec;
                        
                        if(StateC.Vkmh>GPS_KMH)//运动时才更新，静止后4分钟内仍在实时更新
                          StateC.StaticPt=250;
                  }
                  else
                  {
                      //判断为静态，不刷新数据

                  }
              
              #else
                  
                  StateC.LatiInt=wInt;
                  StateC.LatiDec=wDec;
                  StateC.LongiInt=jInt;
                  StateC.LongiDec=jDec;
                  
              #endif
              
              cc=1;
              
              //时间标志闪1次，指示GPS数据正常
              Tshan();
              
          }
      }
      

      if(cc==0)//收到了指令，但是是无效的经纬度数据
      {
          //时间标志闪2次，指示GPS指令正常
          Tshan();
          delay_ms(60);
          Tshan();
        
        
        /*
          if(Pt.GprsSleep==0)//正常模式才每次清0
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

