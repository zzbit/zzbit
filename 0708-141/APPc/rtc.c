//暂不考虑2100的问题 hello word !
//【  功  能  描  述  】该函数计算从2012年1月1日0时0分0秒起到当前时间所经过的秒数
#include "_config.h"

#define xMINUTE          (u32)(60                 )//1分的秒数
#define xHOUR            (u32)(60*xMINUTE         )//1小时的秒数
#define xDAY             (u32)(24*xHOUR           )//1天的秒数
#define xYEAR            (u32)(365*xDAY           )//1年的秒数

#define jzYEAR           12//基准年，必须是闰年

u32 DateToSeconds(StrcutRtc *Rtc)
{
       static u32 month[12]={
       /*01月*/xDAY*(0),
       /*02月*/xDAY*(31),
       /*03月*/xDAY*(31+28),
       /*04月*/xDAY*(31+28+31),
       /*05月*/xDAY*(31+28+31+30),
       /*06月*/xDAY*(31+28+31+30+31),
       /*07月*/xDAY*(31+28+31+30+31+30),
       /*08月*/xDAY*(31+28+31+30+31+30+31),
       /*09月*/xDAY*(31+28+31+30+31+30+31+31),
       /*10月*/xDAY*(31+28+31+30+31+30+31+31+30),
       /*11月*/xDAY*(31+28+31+30+31+30+31+31+30+31),
       /*12月*/xDAY*(31+28+31+30+31+30+31+31+30+31+30)  };
       u32 seconds;
       u8 year;
       
       year = Rtc->Year-jzYEAR;               //不考虑2100年千年虫问题
       seconds = xYEAR*year + xDAY*((year/4)+1);//前几年过去的秒数
       if((Rtc->Month < 3) && ((Rtc->Year%4)==0))//闰年没过2月份时，没有多出一天
         seconds -= xDAY;

       seconds += month[Rtc->Month-1];       //加上今年本月过去的秒数
       seconds += xDAY*(Rtc->Day-1);         //加上本天过去的秒数
       seconds += xHOUR*Rtc->Hour;           //加上本小时过去的秒数
       seconds += xMINUTE*Rtc->Minute;       //加上本分钟过去的秒数
       seconds += Rtc->Second;               //加上当前秒数
       
       return seconds;
}
void SecondsToDate(u32 seconds,StrcutRtc *Rtc)
{
    static u8 month[12]={
    /*01月*/31,
    /*02月*/28,
    /*03月*/31,
    /*04月*/30,
    /*05月*/31,
    /*06月*/30,
    /*07月*/31,
    /*08月*/31,
    /*09月*/30,
    /*10月*/31,
    /*11月*/30,
    /*12月*/31  };

    u32 days;
    u16 today, leap;//闰年数
    
    Rtc->Second = seconds % 60;//获得秒
    seconds    /= 60;
    Rtc->Minute =  seconds % 60;//获得分
    seconds    /= 60;
    Rtc->Hour   = seconds % 24;//获得时
    
    days        = seconds / 24;//获得总天数

    today = days%1461;//今年的第几天
    
    leap = days/1461 + 1;
    if(today<60)
      leap--;
    days -= leap;//减去过去的多少个包含2月29日的闰年

    Rtc->Year = jzYEAR + (days / 365); //获得年
    days %= 365;                       //今年的第几天
    days = 1 + days;                   //1日开始
    if( (Rtc->Year % 4) == 0 )
    {
        if(days == 60)//闰年2月29日
        {
            Rtc->Month = 2;
            Rtc->Day = 29;
            
            if(today<60)//today==60 表示是3月1日，否则是2月29日
              return;
        }
    }
    
    for(Rtc->Month=0; month[Rtc->Month]<days; Rtc->Month++)
    {
        days -= month[Rtc->Month];
    }
    
    Rtc->Month++;               //调整月
    Rtc->Day = days;            //获得日
}

void RtcBcdToD10(StrcutRtc *Rtc)
{
  Rtc->Second=(u8)(BCD_TO_D10(Rtc->Second));
  Rtc->Minute=(u8)(BCD_TO_D10(Rtc->Minute));
  Rtc->Hour=(u8)(BCD_TO_D10(Rtc->Hour));
  Rtc->Day=(u8)(BCD_TO_D10(Rtc->Day));
  Rtc->Month=(u8)(BCD_TO_D10(Rtc->Month));
  Rtc->Year=(u8)(BCD_TO_D10(Rtc->Year));  
}

void RtcD10ToBcd(StrcutRtc *Rtc)
{
  Rtc->Second=(u8)(D10_TO_BCD(Rtc->Second));
  Rtc->Minute=(u8)(D10_TO_BCD(Rtc->Minute));
  Rtc->Hour=(u8)(D10_TO_BCD(Rtc->Hour));
  Rtc->Day=(u8)(D10_TO_BCD(Rtc->Day));
  Rtc->Month=(u8)(D10_TO_BCD(Rtc->Month));
  Rtc->Year=(u8)(D10_TO_BCD(Rtc->Year));  
}

//返回SS秒数对应时间bcd
StrcutRtc SS_To_BCD(u32 SS)
{
  StrcutRtc time_bcd;
  
  SecondsToDate(SS,&time_bcd);
  RtcD10ToBcd(&time_bcd);
  return time_bcd;
}

//判断时间格式是对的
u8 Rtc_Is_Right(StrcutRtc *Rtc)
{
  if((Rtc->Year>13)&&(Rtc->Month>0)&&(Rtc->Month<13)&&(Rtc->Day>0)&&(Rtc->Day<32)&&(Rtc->Hour<24)&&(Rtc->Minute<60)&&(Rtc->Second<60))
    return 1;

  else
    return 0;
}

