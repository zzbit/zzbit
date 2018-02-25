#include "_config.h"

#if HTU_ENABLE==1


void I2cHtuStart(void)
{
  WDT_CLR;
  I2C_HTU_PORT_INIT;
  
  delay_us(IIC_HTU_DELAY*2);
  SDA_HTU_L;
  delay_us(IIC_HTU_DELAY*2);
  SCL_HTU_L;
  delay_us(IIC_HTU_DELAY);
}
void I2cHtuStop(void)
{
  WDT_CLR;
  SDA_HTU_DIR_H;
  SDA_HTU_L;
  delay_us(IIC_HTU_DELAY*2);
  SCL_HTU_H;
  delay_us(IIC_HTU_DELAY*2);
  SDA_HTU_H;
  delay_us(IIC_HTU_DELAY);
}
unsigned char I2cHtuSendByte(unsigned char cc)
{
  unsigned char BitCnt;
  unsigned int i=0;

  WDT_CLR;
  SDA_HTU_DIR_H;
  SCL_HTU_L;
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
    SCL_HTU_L;
    if((cc&0x80)==0x80)
    {SDA_HTU_H;}
    else
    {SDA_HTU_L;}
    delay_us(IIC_HTU_DELAY);
    SCL_HTU_H;
    delay_us(IIC_HTU_DELAY);
    cc=cc<<1;
  }
  SCL_HTU_L;
  delay_us(IIC_HTU_DELAY*2);
  SDA_HTU_DIR_L;
  SCL_HTU_H;
  BitCnt = 0;
  while((SDA_HTU&SDA_HTU_PIN)==SDA_HTU)
  {
    i++;
    if(i>500)
    {
      BitCnt=1;
      break;
    }
  }
  SCL_HTU_L;
  SDA_HTU_DIR_H;

  return BitCnt;
}
unsigned char I2cHtuRecByte(void)
{
  vu8 retc;
  vu8 BitCnt;

  retc=0;
  WDT_CLR;
  SDA_HTU_DIR_L;                       
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {
      SCL_HTU_H;
      delay_us(IIC_HTU_DELAY);
      retc<<=1;
      if((SDA_HTU&SDA_HTU_PIN)==SDA_HTU)
        retc=retc+1;
      SCL_HTU_L;
      delay_us(IIC_HTU_DELAY);
  }
  SDA_HTU_DIR_H; 

  return(retc);
}
void I2cHtuAck(unsigned char aa)
{

  WDT_CLR;
  SDA_HTU_DIR_H;
  SCL_HTU_L;
  if(aa==0)
  {SDA_HTU_H;}
  else
  {SDA_HTU_L;}
  delay_us(IIC_HTU_DELAY);
  SCL_HTU_H;
  delay_us(IIC_HTU_DELAY);
  SCL_HTU_L;

}

u8 GetCRC8(u8* data,u8 len)
{
	u8 crc = 0;
	u8 _bit;
	u8 byteCtr;

	for (byteCtr = 0; byteCtr < len; ++byteCtr)
	{
		crc ^= (data[len-1-byteCtr]);
		for (_bit = 8; _bit > 0; --_bit)
		{ 
		    if (crc & 0x80) 
		    {
			crc = (crc << 1) ^ 0x0131;
		    }
		    else
		    {
			crc = (crc << 1);
		    }
		}
	}
        
        return crc;
}

u8 SHT3X_CalcCrc(u8 data[], u8 nbrOfBytes)
{
  u8 BIT;       
  u8 crc = 0xFF; 
  u8 byteCtr; 
  for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
  {
    crc ^= (data[byteCtr]);
    for( BIT = 8;  BIT > 0; -- BIT)
    {
      if(crc & 0x80) crc = (crc << 1) ^ 0x0131;
      else           crc = (crc << 1);
    }
  }
  return crc;
}


//ȡ�¶ȡ�ʪ�ȣ����� SENIOR_NULL, ������������
u16 GetSHT30(int* out)
{
  u8 HH,LL,CRC,aa,buf[2],re,adr=0x44;
  u16 u16PP,timeout=100;
  

  I2cHtuStart();
  re=I2cHtuSendByte(adr<<1);
  if(re==0)//�д�������Ӧ
  {

      I2cHtuSendByte(0x24);//CMD_MEAS_POLLING_H
      I2cHtuSendByte(0x00);

      //�ȴ���������
      do{
         delay_ms(1);
         I2cHtuStart();
         aa=I2cHtuSendByte(adr<<1|0x01);
      }while((aa!=0)&&(--timeout));

      
      //�¶�------------------------------------
      HH=I2cHtuRecByte();//LSB16
      I2cHtuAck(1);
      LL=I2cHtuRecByte();//LSB8
      I2cHtuAck(1);
      CRC=I2cHtuRecByte();//LSB16
      I2cHtuAck(1);
      

      buf[0]=HH;
      buf[1]=LL;
      
      if(SHT3X_CalcCrc(buf,2)==CRC)//CRCУ����ȷ
      {
          u16PP=HH;
          u16PP=(u16PP<<8)|LL;
          out[0]=(1750*(u32)u16PP) / 65535 - 450;// T = -45 + 175 * rawValue / (2^16-1)
          
      }
      else
      {
          u16PP=SENIOR_NULL;
      }

      
      
      //ʪ��----------------------------------------
      HH=I2cHtuRecByte();//LSB16
      I2cHtuAck(1);
      LL=I2cHtuRecByte();//LSB8
      I2cHtuAck(1);
      CRC=I2cHtuRecByte();//LSB16
      I2cHtuAck(0);
      
      I2cHtuStop();//stop
      
      buf[0]=HH;
      buf[1]=LL;
      if(SHT3X_CalcCrc(buf,2)==CRC)//CRCУ����ȷ
      {
          u16PP=HH;
          u16PP=(u16PP<<8)|LL;
          out[1]=((u32)u16PP*1000) / 65535;// RH = rawValue / (2^16-1) * 100
          
          
      }
      else
      {
          u16PP=SENIOR_NULL;
      }
      
  
  }
  else
  {
    u16PP=SENIOR_NULL;
    I2cHtuStop();
  }
  
  return u16PP;

}


/*
//SHT20 �� HDU21
//cc=0,�¶ȣ�cc=1,ʪ��
u16 HtuMeasure(u8 cc,u8 fh)
{
  u8 HH,LL,CRC,aa,buf[2],re;
  u16 u16PP,timeout=100;
  float fTRH;
  
  re=0;
  
  delay_us(10);
  I2cHtuStart();
  re=I2cHtuSendByte(0x80);
  if(re==0)//�д�������Ӧ
  {
      if(cc==0)
      {
        I2cHtuSendByte(0xf3);//�¶�
      }
      else
      {
        I2cHtuSendByte(0xf5);//ʪ��
      }
      

      //�ȴ���������
      do{
         delay_ms(20);
         I2cHtuStart();
         aa=I2cHtuSendByte(0x81);
      }while((aa!=0)&&(--timeout));

      
      HH=I2cHtuRecByte();//LSB16
      I2cHtuAck(1);
      LL=I2cHtuRecByte();//LSB8
      I2cHtuAck(1);
      CRC=I2cHtuRecByte();//LSB16
      I2cHtuAck(0);
      I2cHtuStop();
      
      buf[0]=LL;
      buf[1]=HH;
      if(GetCRC8(buf,2)==CRC)//CRCУ����ȷ
      {
          u16PP=HH;
          u16PP=(u16PP<<8)|LL;
          
          u16PP&=~0x0003;
          if(cc==0)//�¶�
          {
            fTRH= -46.85 + 175.72/65536 *(float)u16PP;
          }
          else if(cc==1)//ʪ��
          {
            fTRH= -6.0 + 125.0/65536 * (float)u16PP;
          }
          
          fh=0;
          if(fTRH<0)
          {
            fTRH=fTRH*(-1);
            fh=1;
          }
          
          fTRH=fTRH*10;
          u16PP=(u16)fTRH;
      }
      else
      {
          u16PP=SENIOR_NULL;
      }
  }
  else
  {
    u16PP=SENIOR_NULL;
  }
  
  return u16PP;

}
*/


/*

//ȡ�¶ȡ�ʪ�ȣ����� SENIOR_NULL, ������������
u16 GetHIH613X(int* out)
{
  long int TEMP,HUMI;
  u8 TH,TL,HH,HL,CC,re;
  u16 u16PP;
  
  re=0;
  
  delay_us(10);
  I2cHtuStart();
  re=I2cHtuSendByte(0x4e);
  if(re==0)//����������Ӧ
  {
      u16PP=0;
    
      I2cHtuStop();
      
      //��������ʱ�䣬����ֵ50�����60
      delay_ms(65);
      
      I2cHtuStart();
      I2cHtuSendByte(0x4f);
      
      HH=I2cHtuRecByte();
      I2cHtuAck(1);
      HL=I2cHtuRecByte();
      I2cHtuAck(1);
      TH=I2cHtuRecByte();
      I2cHtuAck(1);
      TL=I2cHtuRecByte();
      I2cHtuAck(0);
      I2cHtuStop();
      
      CC=HH&0xc0;
      if(CC==0)//��Ч����
      {
        //�¶�
        TEMP=(TH*256+TL)>>2;
        TEMP=(TEMP*10*165)/16383-40*10;
        out[0]=TEMP;
        
        //ʪ��
        HUMI=(HH&0x3f)*256+HL;
        HUMI=(HUMI*100*10)/16383;
        out[1]=HUMI;
        
        NOP;
        
      }
      else
      {
         u16PP=SENIOR_NULL;//���ݴ���
      }
      
  }
  else
  {
    u16PP=SENIOR_NULL;//������û��Ӧ
  }
  
  return u16PP;
  
}
*/




#endif


