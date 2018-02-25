#include "_config.h"


#include "SHT3x.h"

#define	SHT_SLK		 _pc2 
#define SHT_SLK_OUT  _pcc2 
#define	SHT_SDA	     _pc3 
#define	SHT_SDA_OUT	 _pcc3 

#define SHT_SLK_LOW()   SHT_SLK_OUT=0;SHT_SLK=0
#define SHT_SLK_HIGH()  SHT_SLK_OUT=1;SHT_SLK=1
#define SHT_SDA_LOW()   SHT_SDA_OUT=0;SHT_SDA=0
#define SHT_SDA_HIGH()  SHT_SDA_OUT=1;SHT_SDA=1

#define power_on()	/*_drvcc04 = 1;*/ {_drvcc05 = 1; _pc |= 0x30; }//Դ���� & ����� = Level 1�����PC4 TO PC7  PC4 PC5 =1
#define power_off()	/*_drvcc04 = 0;*/ {_drvcc05 = 0; _pc &= ~0x30;} //Դ���� & ����� = Level 1�����PC4 TO PC7  PC4 PC5 =0//

static u8 _i2cAddress;
#define HIH_delay   600//������ʱʱ��1ms   30us  200us
//0x00   û�г���
//0x01   Ӧ�����
//0x02   У��ͳ���



//----------------------------------------------------------I2C�Ľӿڳ�ʼ��-------------------------------------------------------------------------------
void I2c_Init(void)//I2C �ĳ�ʼ��
{
  	_pcc4= 0;//1�����롢0�������PC4  ���� 
    _pcc5= 0;//1�����롢0�������PC5  ����
	_pcc2= 0;//1�����롢0�������PC2  ���� 
	_pcc3= 0;//1�����롢0�������PC3  ���� 
	_pcwu |= 0x3c; //PC2 PC3 PC4 PC5  ʹ��  0011 1100 12 10
	//Ӳ����������������
	 power_on();// ������Դ
}


//-----------------------------------------------------------------------------
void I2c_StartCondition(void)//I2C �Ŀ�ʼ�ź�
{
  SHT_SDA_HIGH();//�����߸ߵ�ƽ
  GCC_DELAY(HIH_delay);//30us
  SHT_SLK_HIGH();//�����߸ߵ�ƽ
  GCC_DELAY(HIH_delay);//30us
  
  SHT_SDA_LOW();//����������
  GCC_DELAY(HIH_delay*10);// �ӳ�300us
  SHT_SLK_LOW();//ʱ����������
  GCC_DELAY(HIH_delay*10);// �ӳ�300us
}

//-----------------------------------------------------------------------------
void I2c_StopCondition(void)//ֹͣ�ź�
{
  SHT_SLK_LOW();//ʱ����������
  GCC_DELAY(HIH_delay);//�ӳ�30us
  SHT_SDA_LOW();//����������
  GCC_DELAY(HIH_delay);//�ӳ�1us
  SHT_SLK_HIGH();//ʱ��������
  GCC_DELAY(HIH_delay*10);//�ӳ�300us
  SHT_SDA_HIGH();//����������
  GCC_DELAY(HIH_delay*10);//�ӳ�300us
}

//-----------------------------------------------------------------------------
u8 I2c_WriteByte(u8 Byte)//дһ�ֽڵ�����
{
  u8 error =0x00;
  u8    mask;
  for(mask = 0x80; mask > 0; mask >>= 1)
  {
    if((mask & Byte) == 0) 
	{
	    SHT_SDA_LOW(); 
	}
    else               
    {
    	SHT_SDA_HIGH();
    }
    GCC_DELAY(HIH_delay);//30us           
    SHT_SLK_HIGH();                         
    GCC_DELAY(HIH_delay*5);//150us 
    SHT_SLK_LOW();
    GCC_DELAY(HIH_delay);//30us            
  }
  SHT_SDA_HIGH();                        
  SHT_SLK_HIGH();                         
  GCC_DELAY(HIH_delay);//30us              
  if(SHT_SDA) error = 0x01;      
  SHT_SLK_LOW();
  GCC_DELAY(HIH_delay*20);//600us                
  return error;                       
}

//-----------------------------------------------------------------------------
u8 I2c_ReadByte(u8 *Byte,u8 ack, u8 timeout)//��һ�ֽڵ�����
{
	
  u8 error = 0x00;
  u8 mask;
  *Byte = 0x00;
  SHT_SDA_HIGH();                          
  for(mask = 0x80; mask > 0; mask >>= 1) 
  { 
    SHT_SLK_HIGH();//ʱ��������
    GCC_DELAY(HIH_delay);//30us  
	error = I2c_WaitWhileClockStreching(timeout);
    GCC_DELAY(HIH_delay*3);//90us                
    if(SHT_SDA) *Byte |= mask;        
    SHT_SLK_LOW();//ʱ����������
    GCC_DELAY(HIH_delay);//30us               
  }
  if(ack == ACK) 
  {
	  SHT_SDA_LOW();             
  }
  else          
  {
  	  SHT_SDA_HIGH();
  }
  GCC_DELAY(HIH_delay);//30us               
  SHT_SLK_HIGH();//ʱ��������                     
  GCC_DELAY(HIH_delay*5);//150us                
  SHT_SLK_LOW();//ʱ��������
  SHT_SDA_HIGH();                          
  GCC_DELAY(HIH_delay*20);//600us              
 return error;                          
}

//-----------------------------------------------------------------------------

u8 I2c_WaitWhileClockStreching(u8 timeout)
{
u8 error = 0X00;
	
  while(SHT_SLK == 0)
  {
    if(timeout-- == 0)	return 0x04;
   GCC_DELAY(HIH_delay*20);  
  }
	return error;
}


//-----------------------------------------------------------------------------






void SHT3X_Init(u8 i2caddress)//SHT3X  �������ĳ�ʼ�� ��������������ĵ�ַ
{//
	I2c_Init();//I2c �ĳ�ʼ��
	SHT_SLK_HIGH();
	SHT_SDA_HIGH();
	STH3X_SetI2cAdr(i2caddress);
	
}//
void STH3X_SetI2cAdr(u8 i2caddress)//�贫�����ĵ�ַ   ��������������ĵ�ַ
{
	_i2cAddress = i2caddress;	
}


u8 SHT3X_GetTempAndHumi(int *Cout,etRepeatability repeatability, etMode mode,u8 timeout)//��ȡ��ʪ�ȵ�ֵ
{
  u8 error;                     
  error = SHT3X_GetTempAndHumiPolling(Cout,repeatability, timeout);
  return error;
}

void SHT3X_StopAccess(void)
{
  I2c_StopCondition();
}
u8 SHT3X_StartWriteAccess(void)//��ʼд
{
  u8 error; 
  I2c_StartCondition();//i2c��ʼ�ź�
  error = I2c_WriteByte(_i2cAddress << 1);//I2Cдһ���ֽ�  ��ַ
  return error;
}

u8 SHT3X_StartReadAccess(void)
{
  u8 error; 
  I2c_StartCondition();
  error = I2c_WriteByte(_i2cAddress << 1 | 0x01);
  return error;
}
u8 SHT3X_WriteCommand(etCommands command)//д����
{
  u8 error; 
  error  = I2c_WriteByte(command >> 8);
  error |= I2c_WriteByte(command & 0xFF);
  return error;
}

u8 SHT3X_Read2BytesAndCrc(u16* data, etI2cAck finaleAckNack,u8 timeout)//��ȡ���ֽڽ���CRCУ��
{
  u8      error;    
  u8     bytes[2]; 
  u8     checksum; 
 
  error = I2c_ReadByte(&bytes[0], ACK, timeout);
  if(error == 0X00) error = I2c_ReadByte(&bytes[1], ACK, 0);
  if(error == 0X00) error = I2c_ReadByte(&checksum, finaleAckNack, 0);
  if(error == 0X00) error = SHT3X_CheckCrc(bytes, 2, checksum);
  *data = (bytes[0] << 8) | bytes[1];
  return error;
}

u8 SHT3X_CheckCrc(u8 data[], u8 nbrOfBytes, u8 checksum)
{
  u8 crc; 
  crc = SHT3X_CalcCrc(data, nbrOfBytes);
  if(crc != checksum) 
  {
  	return 0x02;
  }
  else               
  {
  	 return 0x00;
  }
}

int SHT3X_CalcTemperature(u16 rawValue)//�����¶�ֵ  x10
{
  return 1750 * (u32)rawValue / 65535 - 450;// T = -45 + 175 * rawValue / (2^16-1)
}

int SHT3X_CalcHumidity(u16 rawValue)//����ʪ��ֵ  x10
{
  return 1000 * (u32)rawValue / 65535;  // RH = rawValue / (2^16-1) * 100
}


u8 SHT3X_GetTempAndHumiPolling(int *Cout,etRepeatability repeatability,u8 timeout)
{
  u8 error;           
  u16    temprawValue;   
  u16    humirawValue;
  error  = SHT3X_StartWriteAccess();///д��ʼ  ��ʼ�ź�+��ַ +д  0x44  0100 0100  0x88
  if(error == 0x00)//���û�б���
  {
    error = SHT3X_WriteCommand(CMD_MEAS_POLLING_H);//��������     0x24  0x16
  }
  while(timeout--)
   {
      error = SHT3X_StartReadAccess();
      if(error == 0x00) break;
      GCC_DELAY(HIH_delay*20);//2ms 
   }
   if(timeout == 0) error = 0x04;
  if(error == 0x00)
  {
    error= SHT3X_Read2BytesAndCrc(&temprawValue, ACK, 0);//������   
    error= SHT3X_Read2BytesAndCrc(&humirawValue, NACK, 0);//������   
  }
  
  SHT3X_StopAccess();//ֹͣ�ź�
  if(error == 0x00)
  {
    Cout[0] = SHT3X_CalcTemperature(temprawValue);
    Cout[1] = SHT3X_CalcHumidity(humirawValue);
  }
  return error;
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
      if(crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else           crc = (crc << 1);
    }
  }
  return crc;
}


void delayms(int a)//12M ��ƽʱ��16M�ǲ�USB  12M  4
{
  while(a--)
  {
    GCC_DELAY(3000);
  }
}




//-----------------------------------��ʼ�����Բ���---------------------------------------
u8 SHT3X_StartPeriodicMeasurment(void)
{
   u8   error;
   error = SHT3X_StartWriteAccess();//��ʼ�ź�+д+��ַ
   if(error==0x00)
   {
      error = SHT3X_WriteCommand(CMD_MEAS_PERI_1_H);//������
   }
   SHT3X_StopAccess();
   return error;                                                                   	
}
//-------------------------------��ȡ�����Բ�����BUFF----------------------------------
u8 SHT3X_ReadMeasurementBuffer(int *Cout)
{
  u8  error; 
  u16    rawValue[2]; 
  error = SHT3X_StartWriteAccess();
  if(error == 0x00) error = SHT3X_WriteCommand(CMD_FETCH_DATA);
  if(error == 0x00) error = SHT3X_StartReadAccess();  
  if(error == 0x00) error = SHT3X_Read2BytesAndCrc(&rawValue, ACK, 0);
  if(error ==0x00)
  {
    Cout[0] = SHT3X_CalcTemperature(rawValue[0]);
    Cout[1] = SHT3X_CalcHumidity(rawValue[1]);
  }
  SHT3X_StopAccess();
  return error;
}