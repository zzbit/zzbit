#include "_config.h"

#if SHR1000_ENABLE==1

void I2c4Start(void)
{
  //_DINT();
  WDT_CLR;
  I2C4_PORT_INIT;
  
  delay_us(IIC4_DELAY*2);
  SDA4_L;
  delay_us(IIC4_DELAY*2);
  SCL4_L;
  delay_us(IIC4_DELAY);
  // _EINT();
}
void I2c4Stop(void)
{
  //_DINT();
  WDT_CLR;
  SDA4_DIR_H;
  SDA4_L;
  delay_us(IIC4_DELAY*2);
  SCL4_H;
  delay_us(IIC4_DELAY*2);
  SDA4_H;
  delay_us(IIC4_DELAY);
  //_EINT();
}
unsigned char I2c4SendByte(unsigned char cc)
{
  unsigned char BitCnt;
  unsigned int i=0;

  //_DINT();
  WDT_CLR;
  SDA4_DIR_H;
  SCL4_L;
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
    SCL4_L;
    if((cc&0x80)==0x80)
    {SDA4_H;}
    else
    {SDA4_L;}
    delay_us(IIC4_DELAY);
    SCL4_H;
    delay_us(IIC4_DELAY);
    cc=cc<<1;
  }
  SCL4_L;
  delay_us(IIC4_DELAY*2);
  SDA4_DIR_L;
  SCL4_H;
  BitCnt = 0;
  while((SDA4&SDA4_PIN)==SDA4)
  {
    i++;
    if(i>500)
    {
      BitCnt=1;
      break;
    }
  }
  SCL4_L;
  SDA4_DIR_H;
  //_EINT();
  return BitCnt;
}
unsigned char I2c4RecByte(void)
{
  vu8 retc;
  vu8 BitCnt;
  //_DINT();
  retc=0;
  WDT_CLR;
  SDA4_DIR_L;                       
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
      SCL4_H;
      delay_us(IIC4_DELAY);
      retc<<=1;
      if((SDA4&SDA4_PIN)==SDA4)
        retc=retc+1;
      SCL4_L;
      delay_us(IIC4_DELAY);
  }
  SDA4_DIR_H; 
  //_EINT();
  return(retc);
}
void I2c4Ack(unsigned char aa)
{
  //_DINT();
  WDT_CLR;
  SDA4_DIR_H;
  SCL4_L;
  if(aa==0)
  {SDA4_H;}
  else
  {SDA4_L;}
  delay_us(IIC4_DELAY);
  SCL4_H;
  delay_us(IIC4_DELAY);
  SCL4_L;
  //_EINT();
}

//...................................................................
//寄存器
#define REG_OPERATION    0x03//操作寄存器
#define REG_DATARD8      0x7f//压力输出 MSB
#define REG_DATARD16     0x80//压力输出 LSB
#define REG_TEMP         0x81//温度输出 14bit

//设置值
#define High_resolution  0x0a//高分辨率 17bit
#define High_speed       0x09//高速     15bit
#define ultra_LowPower   0x0b//超低功耗 15bit
#define low_power        0x0c//低功耗   17或15bit

//设置超低功耗模式后，传感器采样周期性进行，频率为1Hz
void shr1000Write(void)
{
  //delay_ms(100);//上电至少等待100ms
  I2c4Start();
  I2c4SendByte(0x22);
  I2c4SendByte(REG_OPERATION);
  I2c4SendByte(High_speed);
  //I2c4SendByte(ultra_LowPower);
  I2c4Stop();  
}


u16 shr1000ReadTemp(void)
{
  u8 HH,LL;
  u16 u16PP;
  
  
  delay_us(10);
  I2c4Start();
  I2c4SendByte(0x22);
  I2c4SendByte(REG_TEMP);
  I2c4Start();
  I2c4SendByte(0x23);
  HH=I2c4RecByte();//LSB16
  I2c4Ack(1);
  LL=I2c4RecByte();//LSB8
  I2c4Ack(0);
  I2c4Stop();

  u16PP=HH;
  u16PP=(u16PP<<8)|LL;
  
  if(u16PP&0x2000)//第13位=1 为 负数，取补码
  {
    u16PP=(~u16PP);
    u16PP=u16PP&0x3fff;
    u16PP=u16PP+1;
  }
  else
  {
    u16PP=u16PP*5;
    if(u16PP%10>4)
      u16PP=u16PP+10;
    u16PP=u16PP/10;
  }

  return u16PP;//最后返回1位小数
}

u16 shr1000ReadPressure(void)
{
  u8 MM,HH,LL;
  u32 u32UU;
  u16 u16PP;
  
  
  delay_us(10);
  I2c4Start();
  if(I2c4SendByte(0x22))//没响应
  {
    I2c4Stop();
    return SENIOR_NULL;
  }
  if(I2c4SendByte(REG_DATARD8))
  {
    I2c4Stop();
    return SENIOR_NULL;
  }
  I2c4Start();
  if(I2c4SendByte(0x23))
  {
    I2c4Stop();
    return SENIOR_NULL;
  }
  MM=I2c4RecByte();//MSB
  I2c4Ack(0);
  I2c4Stop();

  delay_us(10);
  I2c4Start();
  if(I2c4SendByte(0x22))
  {
    I2c4Stop();
    return SENIOR_NULL;
  }
  if(I2c4SendByte(REG_DATARD16))
  {
    I2c4Stop();
    return SENIOR_NULL;
  }
  I2c4Start();
  if(I2c4SendByte(0x23))
  {
    I2c4Stop();
    return SENIOR_NULL;
  }
  HH=I2c4RecByte();//LSB16
  I2c4Ack(1);
  LL=I2c4RecByte();//LSB8
  I2c4Ack(0);
  I2c4Stop();

  
  u32UU=MM;
  u32UU=(u32UU<<8)|HH;
  u32UU=(u32UU<<8)|LL;
  u32UU=u32UU/4;//单位为Pa
  
  if((u32UU%10)>4)
    u32UU=u32UU+10;
  u16PP=u32UU/10;//转换为Kpa 2位小数  0.01kpa
  
  
  return u16PP;
}


#endif

