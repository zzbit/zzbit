#include "_config.h"

void I2c2Start(void)
{
  //_DINT();
  WDT_CLR;
  I2C2_PORT_INIT;
  delay_us(IIC2_DELAY*2);
  SDA2_L;
  delay_us(IIC2_DELAY*2);
  SCL2_L;
  delay_us(IIC2_DELAY);
  // _EINT();
}
void I2c2Stop(void)
{
  //_DINT();
  WDT_CLR;
  SDA2_DIR_H;
  SDA2_L;
  delay_us(IIC2_DELAY*2);
  SCL2_H;
  delay_us(IIC2_DELAY*2);
  SDA2_H;
  delay_us(IIC2_DELAY);
  //_EINT();
}
unsigned char I2c2SendByte(unsigned char cc)
{
  unsigned char BitCnt;
  unsigned int i=0;

  //_DINT();
  WDT_CLR;
  SDA2_DIR_H;
  SCL2_L;
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
    SCL2_L;
    if((cc&0x80)==0x80)
    {SDA2_H;}
    else
    {SDA2_L;}
    delay_us(IIC2_DELAY);
    SCL2_H;
    delay_us(IIC2_DELAY);
    cc=cc<<1;
  }
  SCL2_L;
  delay_us(IIC2_DELAY*2);
  SDA2_DIR_L;
  SCL2_H;
  BitCnt = 0;
  while((SDA2&SDA2_PIN)==SDA2)
  {
    i++;
    if(i>500)
    {
      BitCnt=1;
      break;
    }
  }
  SCL2_L;
  SDA2_DIR_H;
  //_EINT();
  return BitCnt;
}
unsigned char I2c2RecByte(void)
{
  vu8 retc;
  vu8 BitCnt;
  //_DINT();
  retc=0;
  WDT_CLR;
  SDA2_DIR_L; 
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
      SCL2_H;
      delay_us(IIC2_DELAY);
      retc<<=1;
      if((SDA2&SDA2_PIN)==SDA2)
        retc=retc+1;
      SCL2_L;
      delay_us(IIC2_DELAY);
  }
  SDA2_DIR_H; 
  //_EINT();
  return(retc);
}
void I2c2Ack(unsigned char aa)
{
  //_DINT();
  WDT_CLR;
  SDA2_DIR_H;
  SCL2_L;
  if(aa==0)
  {SDA2_H;}
  else
  {SDA2_L;}
  delay_us(IIC2_DELAY);
  SCL2_H;
  delay_us(IIC2_DELAY);
  SCL2_L;
  //_EINT();
}
//...............................................

void RtcInit(void)
{
  I2c2Start();
  I2c2SendByte(0xa2); //д����
  I2c2SendByte(0x00); //��ַ0
  I2c2SendByte(0x00); //д��ַ0,��ʱ��
  I2c2Stop();
}

void RtcSetTime(void)
{
  RtcInit();
  I2c2Start();
  I2c2SendByte(0xa2); //д����
  I2c2SendByte(0x02); //д���ַ
  I2c2SendByte(Rtc.Second); //��
  I2c2SendByte(Rtc.Minute); //��
  I2c2SendByte(Rtc.Hour); //Сʱ
  I2c2SendByte(Rtc.Day); //��
  I2c2SendByte(0x01); //����
  I2c2SendByte(Rtc.Month);//��
  I2c2SendByte(Rtc.Year); //��  
  I2c2Stop();
}
//��ȡʱ��
void RtcReadTime(void)
{
  I2c2Start();
  I2c2SendByte(0xa2);
  I2c2SendByte(0x02);
  I2c2Start();
  I2c2SendByte(0xa3);
  Rtc.Second=0x7F&I2c2RecByte();I2c2Ack(1);
  Rtc.Minute=0x7F&I2c2RecByte();I2c2Ack(1);
  Rtc.Hour=0x3F&I2c2RecByte();I2c2Ack(1);
  Rtc.Day=0x3F&I2c2RecByte();I2c2Ack(1);
  I2c2RecByte();I2c2Ack(1);
  Rtc.Month=0x9F&I2c2RecByte();I2c2Ack(1);
  Rtc.Year=0x7F&I2c2RecByte();I2c2Ack(0);
  I2c2Stop();
}

//����ǰRTCʱ�ӣ�RTCתD10����������, ����99��ʾʱ������
u32 ReadRtcD10(void)
{
   u32 SS;
   
   RtcReadTime();
   RtcBcdToD10(&Rtc);
   
   if(Rtc_Is_Right(&Rtc))
   {
     SS=DateToSeconds(&Rtc);
   }
   else
   {
           SS=0;

           /*
           ComSendH();
           U1SendString("RTC ERR\r\n");//zz
           ComSendL();
           
           DogReset();
           */

   }
   
   return SS;
}



//.........................................................
void RtcTest(void)
{
  Rtc.Second = 0x00;//��
  Rtc.Minute = 0x12;//��
  Rtc.Hour = 0x09;//ʱ
 // Rtc.Week = 0x04;//����
  Rtc.Day = 0x12;//��
  Rtc.Month = 0x03;//��,�º����ͱ�־�����λΪ0��ʾ20xx�꣬�����ʾ19xx��
  Rtc.Year = 0x09; //��
  RtcSetTime();
  RtcReadTime();
  RtcReadTime();
  RtcReadTime();
  RtcReadTime();
  RtcReadTime();
  RtcReadTime();  
  
  u32 aa;
  RtcBcdToD10(&Rtc);
  aa=DateToSeconds(&Rtc);
  SecondsToDate(aa,&Rtc);

  aa=DateToSeconds(&Rtc);
  SecondsToDate(aa,&Rtc);
  
  aa=DateToSeconds(&Rtc);
  SecondsToDate(aa,&Rtc);  
  aa=1;
}

