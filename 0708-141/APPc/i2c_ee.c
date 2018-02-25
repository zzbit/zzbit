#include "_config.h"

void I2cStart(void)
{
  //_DINT();
  WDT_CLR;
  I2C_PORT_INIT;
  delay_us(IIC_DELAY*2);
  SDA_L;
  delay_us(IIC_DELAY*2);
  SCL_L;
  delay_us(IIC_DELAY);
  // _EINT();
}
void I2cStop(void)
{
  //_DINT();
  WDT_CLR;
  SDA_DIR_H;
  SDA_L;
  delay_us(IIC_DELAY*2);
  SCL_H;
  delay_us(IIC_DELAY*2);
  SDA_H;
  delay_us(IIC_DELAY);
  //_EINT();
}
unsigned char I2cSendByte(unsigned char cc)
{
  unsigned char BitCnt;
  unsigned int i=0;

  //_DINT();
  WDT_CLR;
  SDA_DIR_H;
  SCL_L;
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
    SCL_L;
    if((cc&0x80)==0x80)
    {SDA_H;}
    else
    {SDA_L;}
    delay_us(IIC_DELAY);
    SCL_H;
    delay_us(IIC_DELAY);
    cc=cc<<1;
  }
  SCL_L;
  delay_us(IIC_DELAY*2);
  SDA_DIR_L;
  SCL_H;
  BitCnt = 0;
  while((SDA&SDA_PIN)==SDA)
  {
    i++;
    if(i>500)
    {
      BitCnt=1;
      break;
    }
  }
  SCL_L;
  SDA_DIR_H;
  //_EINT();
  return BitCnt;
}
unsigned char I2cRecByte(void)
{
  vu8 retc;
  vu8 BitCnt;
  //_DINT();
  retc=0;
  WDT_CLR;
  SDA_DIR_L;                       
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
      SCL_H;
      delay_us(IIC_DELAY);
      retc<<=1;
      if((SDA&SDA_PIN)==SDA)
        retc=retc+1;
      SCL_L;
      delay_us(IIC_DELAY);
  }
  SDA_DIR_H; 
  //_EINT();
  return(retc);
}
void I2cAck(unsigned char aa)
{
  //_DINT();
  WDT_CLR;
  SDA_DIR_H;
  SCL_L;
  if(aa==0)
  {SDA_H;}
  else
  {SDA_L;}
  delay_us(IIC_DELAY);
  SCL_H;
  delay_us(IIC_DELAY);
  SCL_L;
  //_EINT();
}
//...............................................................................
//搜索I2C总线上挂有几片EE
u8 I2cSeekEE(void)
{
  u8 No,MeiYou;
  u8 Num=0;

  WP_H;
  _DINT();
  
  for(No=0;No<8;No++)
  {
    I2cStart();
    MeiYou=I2cSendByte(0xa0+(No<<1));
    I2cStop();
    _EINT();
    
    if(MeiYou)//不存在
    {
      continue;
    }
    else
    {
      Num++;//存在，检测下一片
    }
  }
  NOP;
  NOP;
  return Num;
}

//.....................................................................
void I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite,u8 No)
{
  NOP;
  NOP;
  NOP;
  WP_L;
  if(NumByteToWrite>2000)return;
  _DINT();
  I2cStart();
  #if EEPROM>=256
  I2cSendByte(0xa0+(No<<1));
  I2cSendByte(WriteAddr/256);
  I2cSendByte(WriteAddr%256);
  #else
  #endif
  for(;NumByteToWrite>0;NumByteToWrite--)
  {
    WDT_CLR;//zz
    I2cSendByte(*pBuffer);

    if((WriteAddr&EE_PAGE_MAX)==EE_PAGE_MAX)
    {
       I2cStop();
       delay_us(IIC_DELAY*250);
       I2cStart();
       #if EEPROM>=256
       I2cSendByte(0xa0+(No<<1));
       I2cSendByte((WriteAddr+1)/256);
       I2cSendByte((WriteAddr+1)%256);
       #else
       #endif
    }
    WriteAddr++;
    pBuffer++;
  }
  I2cStop();
  delay_us((u16)600*IIC_DELAY);//延时必须足够长 IIC_DELAY 10时为250，IIC_DELAY 40时为70
  WP_H;
  _EINT();
  NOP;
  NOP;
  NOP;
}

void I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead,u8 No)
{
  NOP;
  NOP;
  NOP;
  WP_H;
  if(NumByteToRead>2000||NumByteToRead==0)return;
  _DINT();
  I2cStart();
  #if EEPROM>=256
  I2cSendByte(0xa0+(No<<1));
  I2cSendByte(ReadAddr/256);
  I2cSendByte(ReadAddr%256);
  #else
  #endif
  I2cStart();
  I2cSendByte(0xa0+(No<<1)+0x01);
  for(;NumByteToRead>0;NumByteToRead--)
  {
    *pBuffer=I2cRecByte();
    if(NumByteToRead==1)
      I2cAck(0);
    else
      I2cAck(1); 
    pBuffer++;
  }
  I2cStop();
  _EINT();
  NOP;
  NOP;
  NOP;
}
u8 i2c_ee_read_to_pc(u16 ReadAddr, u16 NumByteToRead,u8 No)
{
  u8 add=0;
  NOP;
  NOP;
  NOP;
  WP_H;
  if(NumByteToRead>2000)return 0;
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
  for(;NumByteToRead>0;NumByteToRead--)
  {
    add+=U1SendByte(I2cRecByte());
    if(NumByteToRead==1)
      I2cAck(0);
    else
      I2cAck(1); 
  }
  I2cStop();
  _EINT();
  NOP;
  NOP;
  NOP;
  return add;
}
//顺丰协议修改，返回的是异或码，剔除7E,,7D->7D 01,7E->7D 02
u8 i2c_SF_ee_read_to_u0(u16 ReadAddr, u16 NumByteToRead,u8 No)
{
  u8 bbc=0;
  WP_H;
  if(NumByteToRead>2000)return 0;
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
  for(;NumByteToRead>0;NumByteToRead--)
  {
    bbc^=U0_SF_SendByte(I2cRecByte());
    if(NumByteToRead==1)
      I2cAck(0);
    else
      I2cAck(1); 
  }
  I2cStop();
  _EINT();
  return bbc;
}
u8 i2c_ee_read_to_u0(u16 ReadAddr, u16 NumByteToRead,u8 No)
{
  u8 add=0;
  NOP;
  NOP;
  NOP;
  WP_H;
  if(NumByteToRead>2000)return 0;
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
  for(;NumByteToRead>0;NumByteToRead--)
  {
    #if WIFI_ENABLE==1
      add+=UxSendByte(I2cRecByte());//zz
    #else
      add+=U0SendByte(I2cRecByte());
    #endif
    
    if(NumByteToRead==1)
      I2cAck(0);
    else
      I2cAck(1); 
  }
  I2cStop();
  _EINT();
  NOP;
  NOP;
  NOP;
  return add;
}

void EETest(void)
{


  u8 BufA[16],BufB[16];
  u16 EE_Addr;
  u16 i,j;
  
  BufA[0]=I2cSeekEE();
  
  EE_Addr=0;
  
  
  for(j=0;j<((u32)EE_SIZE+1)/16;j++)//整个EE全部
  {
    for(i=0;i<16;i++)
    {
      BufA[i]=0x30+j;
      BufB[i]=0;
    }
    I2C_EE_BufferWrite(BufA,EE_Addr,16,0);
    I2C_EE_BufferRead(BufB,EE_Addr,16,0);

    if(CheckBuf(BufA,BufB,16)==1)
    {
      if(EE_Addr>=EE_SIZE)
      {
         NOP;
         break;
      }
      else
        EE_Addr+=16;
    }
    else
    {
      NOP;//错误
      NOP;
      NOP;
      break;
    }
    delay_ms(10);
  }

}



