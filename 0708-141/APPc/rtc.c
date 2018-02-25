//�ݲ�����2100������ hello word !
//��  ��  ��  ��  ��  ���ú��������2012��1��1��0ʱ0��0���𵽵�ǰʱ��������������
#include "_config.h"

#define xMINUTE          (u32)(60                 )//1�ֵ�����
#define xHOUR            (u32)(60*xMINUTE         )//1Сʱ������
#define xDAY             (u32)(24*xHOUR           )//1�������
#define xYEAR            (u32)(365*xDAY           )//1�������

#define jzYEAR           12//��׼�꣬����������

u32 DateToSeconds(StrcutRtc *Rtc)
{
       static u32 month[12]={
       /*01��*/xDAY*(0),
       /*02��*/xDAY*(31),
       /*03��*/xDAY*(31+28),
       /*04��*/xDAY*(31+28+31),
       /*05��*/xDAY*(31+28+31+30),
       /*06��*/xDAY*(31+28+31+30+31),
       /*07��*/xDAY*(31+28+31+30+31+30),
       /*08��*/xDAY*(31+28+31+30+31+30+31),
       /*09��*/xDAY*(31+28+31+30+31+30+31+31),
       /*10��*/xDAY*(31+28+31+30+31+30+31+31+30),
       /*11��*/xDAY*(31+28+31+30+31+30+31+31+30+31),
       /*12��*/xDAY*(31+28+31+30+31+30+31+31+30+31+30)  };
       u32 seconds;
       u8 year;
       
       year = Rtc->Year-jzYEAR;               //������2100��ǧ�������
       seconds = xYEAR*year + xDAY*((year/4)+1);//ǰ�����ȥ������
       if((Rtc->Month < 3) && ((Rtc->Year%4)==0))//����û��2�·�ʱ��û�ж��һ��
         seconds -= xDAY;

       seconds += month[Rtc->Month-1];       //���Ͻ��걾�¹�ȥ������
       seconds += xDAY*(Rtc->Day-1);         //���ϱ����ȥ������
       seconds += xHOUR*Rtc->Hour;           //���ϱ�Сʱ��ȥ������
       seconds += xMINUTE*Rtc->Minute;       //���ϱ����ӹ�ȥ������
       seconds += Rtc->Second;               //���ϵ�ǰ����
       
       return seconds;
}
void SecondsToDate(u32 seconds,StrcutRtc *Rtc)
{
    static u8 month[12]={
    /*01��*/31,
    /*02��*/28,
    /*03��*/31,
    /*04��*/30,
    /*05��*/31,
    /*06��*/30,
    /*07��*/31,
    /*08��*/31,
    /*09��*/30,
    /*10��*/31,
    /*11��*/30,
    /*12��*/31  };

    u32 days;
    u16 today, leap;//������
    
    Rtc->Second = seconds % 60;//�����
    seconds    /= 60;
    Rtc->Minute =  seconds % 60;//��÷�
    seconds    /= 60;
    Rtc->Hour   = seconds % 24;//���ʱ
    
    days        = seconds / 24;//���������

    today = days%1461;//����ĵڼ���
    
    leap = days/1461 + 1;
    if(today<60)
      leap--;
    days -= leap;//��ȥ��ȥ�Ķ��ٸ�����2��29�յ�����

    Rtc->Year = jzYEAR + (days / 365); //�����
    days %= 365;                       //����ĵڼ���
    days = 1 + days;                   //1�տ�ʼ
    if( (Rtc->Year % 4) == 0 )
    {
        if(days == 60)//����2��29��
        {
            Rtc->Month = 2;
            Rtc->Day = 29;
            
            if(today<60)//today==60 ��ʾ��3��1�գ�������2��29��
              return;
        }
    }
    
    for(Rtc->Month=0; month[Rtc->Month]<days; Rtc->Month++)
    {
        days -= month[Rtc->Month];
    }
    
    Rtc->Month++;               //������
    Rtc->Day = days;            //�����
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

//����SS������Ӧʱ��bcd
StrcutRtc SS_To_BCD(u32 SS)
{
  StrcutRtc time_bcd;
  
  SecondsToDate(SS,&time_bcd);
  RtcD10ToBcd(&time_bcd);
  return time_bcd;
}

//�ж�ʱ���ʽ�ǶԵ�
u8 Rtc_Is_Right(StrcutRtc *Rtc)
{
  if((Rtc->Year>13)&&(Rtc->Month>0)&&(Rtc->Month<13)&&(Rtc->Day>0)&&(Rtc->Day<32)&&(Rtc->Hour<24)&&(Rtc->Minute<60)&&(Rtc->Second<60))
    return 1;

  else
    return 0;
}

