#include "_config.h"

#if GZ_ENABLE==1

//���� BH1750 ָ��
#define DPOWR    0X00        //�ϵ� 
#define POWER    0X01        //ͨ�� 
#define RESET    0X07        //����
#define CHMODE   0X10        //����H�ֱ��� 
#define CHMODE2  0X11        //����H�ֱ���2 
#define CLMODE   0X13        //�����ͷֱ� 
#define H1MODE   0X20        //һ��H�ֱ���,������ɺ󣬽���ϵ�ģʽ 
#define H1MODE2  0X21        //һ��H�ֱ���2 
#define L1MODE   0X23        //һ��L�ֱ���ģʽ

void I2c3Start(void)
{
  //_DINT();
  WDT_CLR;
  I2C3_PORT_INIT;
  delay_us(IIC3_DELAY*2);
  SDA3_L;
  delay_us(IIC3_DELAY*2);
  SCL3_L;
  delay_us(IIC3_DELAY);
  // _EINT();
}
void I2c3Stop(void)
{
  //_DINT();
  WDT_CLR;
  SDA3_DIR_H;
  SDA3_L;
  delay_us(IIC3_DELAY*2);
  SCL3_H;
  delay_us(IIC3_DELAY*2);
  SDA3_H;
  delay_us(IIC3_DELAY);
  //_EINT();
}
unsigned char I2c3SendByte(unsigned char cc)
{
  unsigned char BitCnt;
  unsigned int i=0;

  //_DINT();
  WDT_CLR;
  SDA3_DIR_H;
  SCL3_L;
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
    SCL3_L;
    if((cc&0x80)==0x80)
    {SDA3_H;}
    else
    {SDA3_L;}
    delay_us(IIC3_DELAY);
    SCL3_H;
    delay_us(IIC3_DELAY);
    cc=cc<<1;
  }
  SCL3_L;
  delay_us(IIC3_DELAY*2);
  SDA3_DIR_L;
  SCL3_H;
  BitCnt = 0;
  while((SDA3&SDA3_PIN)==SDA3)
  {
    i++;
    if(i>500)
    {
      BitCnt=1;
      break;
    }
  }
  SCL3_L;
  SDA3_DIR_H;
  //_EINT();
  return BitCnt;
}
unsigned char I2c3RecByte(void)
{
  vu8 retc;
  vu8 BitCnt;
  //_DINT();
  retc=0;
  WDT_CLR;
  SDA3_DIR_L;                       
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
      SCL3_H;
      delay_us(IIC3_DELAY);
      retc<<=1;
      if((SDA3&SDA3_PIN)==SDA3)
        retc=retc+1;
      SCL3_L;
      delay_us(IIC3_DELAY);
  }
  SDA3_DIR_H; 
  //_EINT();
  return(retc);
}
void I2c3Ack(unsigned char aa)
{
  //_DINT();
  WDT_CLR;
  SDA3_DIR_H;
  SCL3_L;
  if(aa==0)
  {SDA3_H;}
  else
  {SDA3_L;}
  delay_us(IIC3_DELAY);
  SCL3_H;
  delay_us(IIC3_DELAY);
  SCL3_L;
  //_EINT();
}

//...................................................................

u8 GetGzRe(u8 cc)
{
  return cc;
}

void LxStart(void)
{
  Pt.GzNoRe=0;
  
  delay_us(5);
  I2c3Start();
  //Pt.GzNoRe==1ʱ����������������Ӧ, ��ֹ���ò��Ӱ��
  Pt.GzNoRe=I2c3SendByte(0x46);//L��ַ 
  if(Pt.GzNoRe==1)
  {
    I2c3Stop();
    //U1SendStrPC("LX NO RESPOSE\r\n");//zz
    return;
  }
  Pt.GzNoRe=I2c3SendByte(POWER);//ͨ��
  if(Pt.GzNoRe==1)
  {
    I2c3Stop();
    return;
  }
  I2c3Stop();
  
  delay_us(5);
  I2c3Start();
  Pt.GzNoRe=I2c3SendByte(0x46);
  if(Pt.GzNoRe==1)
  {
    I2c3Stop();
    return;
  }
  
  #if ((JLY_MODEL==_1_GZ_100LX)||(JLY_MODEL==_1_GZ_1000LX))
  Pt.GzNoRe=I2c3SendByte(H1MODE2);//CHMODE  ����0.5LX
  #else
  Pt.GzNoRe=I2c3SendByte(H1MODE);//CHMODE  ����1LX
  #endif
  if(Pt.GzNoRe==1)
  {
    I2c3Stop();
    return;
  }
  I2c3Stop();
}
//ת��֮�󣬵ȴ�����120m���Ŷ�
u16 LxRead(void)
{
  u8 LL,HH;
  u16 UU;
  float FF;
  
  Pt.GzNoRe=0;
  
  I2c3Start();
  Pt.GzNoRe=I2c3SendByte(0x47);
  if(Pt.GzNoRe==1)
  {
    I2c3Stop();
    return 1;
  }
  HH=I2c3RecByte();
  I2c3Ack(1);
  LL=I2c3RecByte();
  I2c3Ack(0);
  I2c3Stop();
  
  #if ((JLY_MODEL==_1_GZ_100LX)||(JLY_MODEL==_1_GZ_1000LX))
  UU=HH;
  UU=(UU<<8)|LL;
  UU=0x7fff&(UU>>1);
  FF=(float)UU;
  if(0X01&LL)
    FF=FF+0.5;
  FF=FF/0.12;//�Ŵ���10������1λС��Ϊ��ʵֵ
  #else
  UU=HH;
  UU=(UU<<8)|LL;
  FF=(float)UU/1.2;//����Ϊ��ʵֵ
  #endif
  UU=(u16)FF;
  return UU;
}
//���˹⾵����ֵ������Ŵ�,�Ŵ�Ϊ2λС�� KLX ��ʽ
u16 GzEnlarge(u16 cc)
{
   u32 u32aa;
   u16 u16aa;
   
   u32aa=cc;
   u32aa=u32aa*GZ_X;
   if((u32aa%10)>4)
   {
     u32aa=(u32aa+10)/10;
   }
   else
   {
     u32aa=u32aa/10;//����ɵ�λKLX,��2λС����ֵ
   }
   u16aa=(u16)u32aa;
   return u16aa;
}



#if SENCOND_GZ_EN==1
//��2���ն�=============================================
void I2cBHStart(void)
{
  //_DINT();
  WDT_CLR;
  I2CBH_PORT_INIT;
  delay_us(IIC3_DELAY*2);
  SDABH_L;
  delay_us(IIC3_DELAY*2);
  SCLBH_L;
  delay_us(IIC3_DELAY);
  // _EINT();
}
void I2cBHStop(void)
{
  //_DINT();
  WDT_CLR;
  SDABH_DIR_H;
  SDABH_L;
  delay_us(IIC3_DELAY*2);
  SCLBH_H;
  delay_us(IIC3_DELAY*2);
  SDABH_H;
  delay_us(IIC3_DELAY);
  //_EINT();
}
unsigned char I2cBHSendByte(unsigned char cc)
{
  unsigned char BitCnt;
  unsigned int i=0;

  //_DINT();
  WDT_CLR;
  SDABH_DIR_H;
  SCLBH_L;
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
    SCLBH_L;
    if((cc&0x80)==0x80)
    {SDABH_H;}
    else
    {SDABH_L;}
    delay_us(IIC3_DELAY);
    SCLBH_H;
    delay_us(IIC3_DELAY);
    cc=cc<<1;
  }
  SCLBH_L;
  delay_us(IIC3_DELAY*2);
  SDABH_DIR_L;
  SCLBH_H;
  BitCnt = 0;
  while((SDABH&SDABH_PIN)==SDABH)
  {
    i++;
    if(i>500)
    {
      BitCnt=1;
      break;
    }
  }
  SCLBH_L;
  SDABH_DIR_H;
  //_EINT();
  return BitCnt;
}
unsigned char I2cBHRecByte(void)
{
  vu8 retc;
  vu8 BitCnt;
  //_DINT();
  retc=0;
  WDT_CLR;
  SDABH_DIR_L;                       
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
      SCLBH_H;
      delay_us(IIC3_DELAY);
      retc<<=1;
      if((SDABH&SDABH_PIN)==SDABH)
        retc=retc+1;
      SCLBH_L;
      delay_us(IIC3_DELAY);
  }
  SDABH_DIR_H; 
  //_EINT();
  return(retc);
}
void I2cBHAck(unsigned char aa)
{
  //_DINT();
  WDT_CLR;
  SDABH_DIR_H;
  SCLBH_L;
  if(aa==0)
  {SDABH_H;}
  else
  {SDABH_L;}
  delay_us(IIC3_DELAY);
  SCLBH_H;
  delay_us(IIC3_DELAY);
  SCLBH_L;
  //_EINT();
}

//...................................................................
void LxBHStart(void)
{
  Pt.GzBHNoRe=0;
  
  delay_us(5);
  I2cBHStart();
  //Pt.GzBHNoRe==1ʱ����������������Ӧ, ��ֹ���ò��Ӱ��
  Pt.GzBHNoRe=I2cBHSendByte(0x46);//L��ַ 
  if(Pt.GzBHNoRe==1)
  {
    I2cBHStop();
    return;
  }
  Pt.GzBHNoRe=I2cBHSendByte(POWER);//ͨ��
  if(Pt.GzBHNoRe==1)
  {
    I2cBHStop();
    return;
  }
  I2cBHStop();
  
  delay_us(5);
  I2cBHStart();
  Pt.GzBHNoRe=I2cBHSendByte(0x46);
  if(Pt.GzBHNoRe==1)
  {
    I2cBHStop();
    return;
  }
  

  Pt.GzBHNoRe=I2cBHSendByte(H1MODE);//CHMODE  ����1LX
  if(Pt.GzBHNoRe==1)
  {
    I2cBHStop();
    return;
  }
  I2cBHStop();
}
//ת��֮�󣬵ȴ�����120m���Ŷ�
u16 LxBHRead(void)
{
  u8 LL,HH;
  u16 UU;
  float FF;
  
  Pt.GzBHNoRe=0;
  
  I2cBHStart();
  Pt.GzBHNoRe=I2cBHSendByte(0x47);
  if(Pt.GzBHNoRe==1)
  {
    I2cBHStop();
    return 1;
  }
  HH=I2cBHRecByte();
  I2cBHAck(1);
  LL=I2cBHRecByte();
  I2cBHAck(0);
  I2cBHStop();
  

  UU=HH;
  UU=(UU<<8)|LL;
  FF=(float)UU/1.2;//����Ϊ��ʵֵ

  UU=(u16)FF;
  return UU;
}
#endif






#endif

