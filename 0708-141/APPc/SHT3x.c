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

#define power_on()	/*_drvcc04 = 1;*/ {_drvcc05 = 1; _pc |= 0x30; }//源电流 & 灌电流 = Level 1（最大）PC4 TO PC7  PC4 PC5 =1
#define power_off()	/*_drvcc04 = 0;*/ {_drvcc05 = 0; _pc &= ~0x30;} //源电流 & 灌电流 = Level 1（最大）PC4 TO PC7  PC4 PC5 =0//

static u8 _i2cAddress;
#define HIH_delay   600//定义延时时间1ms   30us  200us
//0x00   没有出错
//0x01   应答出错
//0x02   校验和出错



//----------------------------------------------------------I2C的接口初始化-------------------------------------------------------------------------------
void I2c_Init(void)//I2C 的初始化
{
  	_pcc4= 0;//1是输入、0是输出。PC4  输入 
    _pcc5= 0;//1是输入、0是输出。PC5  输入
	_pcc2= 0;//1是输入、0是输出。PC2  输入 
	_pcc3= 0;//1是输入、0是输出。PC3  输入 
	_pcwu |= 0x3c; //PC2 PC3 PC4 PC5  使能  0011 1100 12 10
	//硬件设置了上拉电阻
	 power_on();// 打来电源
}


//-----------------------------------------------------------------------------
void I2c_StartCondition(void)//I2C 的开始信号
{
  SHT_SDA_HIGH();//数据线高电平
  GCC_DELAY(HIH_delay);//30us
  SHT_SLK_HIGH();//数据线高电平
  GCC_DELAY(HIH_delay);//30us
  
  SHT_SDA_LOW();//数据线拉低
  GCC_DELAY(HIH_delay*10);// 延迟300us
  SHT_SLK_LOW();//时钟线线拉低
  GCC_DELAY(HIH_delay*10);// 延迟300us
}

//-----------------------------------------------------------------------------
void I2c_StopCondition(void)//停止信号
{
  SHT_SLK_LOW();//时钟线线拉低
  GCC_DELAY(HIH_delay);//延迟30us
  SHT_SDA_LOW();//数据线拉低
  GCC_DELAY(HIH_delay);//延迟1us
  SHT_SLK_HIGH();//时钟线拉高
  GCC_DELAY(HIH_delay*10);//延迟300us
  SHT_SDA_HIGH();//数据线拉高
  GCC_DELAY(HIH_delay*10);//延迟300us
}

//-----------------------------------------------------------------------------
u8 I2c_WriteByte(u8 Byte)//写一字节的数据
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
u8 I2c_ReadByte(u8 *Byte,u8 ack, u8 timeout)//读一字节的数据
{
	
  u8 error = 0x00;
  u8 mask;
  *Byte = 0x00;
  SHT_SDA_HIGH();                          
  for(mask = 0x80; mask > 0; mask >>= 1) 
  { 
    SHT_SLK_HIGH();//时钟线拉高
    GCC_DELAY(HIH_delay);//30us  
	error = I2c_WaitWhileClockStreching(timeout);
    GCC_DELAY(HIH_delay*3);//90us                
    if(SHT_SDA) *Byte |= mask;        
    SHT_SLK_LOW();//时钟线线拉低
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
  SHT_SLK_HIGH();//时钟线拉高                     
  GCC_DELAY(HIH_delay*5);//150us                
  SHT_SLK_LOW();//时钟线拉低
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






void SHT3X_Init(u8 i2caddress)//SHT3X  传感器的初始化 输入参数传感器的地址
{//
	I2c_Init();//I2c 的初始化
	SHT_SLK_HIGH();
	SHT_SDA_HIGH();
	STH3X_SetI2cAdr(i2caddress);
	
}//
void STH3X_SetI2cAdr(u8 i2caddress)//设传感器的地址   输入参数传感器的地址
{
	_i2cAddress = i2caddress;	
}


u8 SHT3X_GetTempAndHumi(int *Cout,etRepeatability repeatability, etMode mode,u8 timeout)//获取温湿度的值
{
  u8 error;                     
  error = SHT3X_GetTempAndHumiPolling(Cout,repeatability, timeout);
  return error;
}

void SHT3X_StopAccess(void)
{
  I2c_StopCondition();
}
u8 SHT3X_StartWriteAccess(void)//开始写
{
  u8 error; 
  I2c_StartCondition();//i2c开始信号
  error = I2c_WriteByte(_i2cAddress << 1);//I2C写一个字节  地址
  return error;
}

u8 SHT3X_StartReadAccess(void)
{
  u8 error; 
  I2c_StartCondition();
  error = I2c_WriteByte(_i2cAddress << 1 | 0x01);
  return error;
}
u8 SHT3X_WriteCommand(etCommands command)//写命令
{
  u8 error; 
  error  = I2c_WriteByte(command >> 8);
  error |= I2c_WriteByte(command & 0xFF);
  return error;
}

u8 SHT3X_Read2BytesAndCrc(u16* data, etI2cAck finaleAckNack,u8 timeout)//读取两字节进行CRC校验
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

int SHT3X_CalcTemperature(u16 rawValue)//计算温度值  x10
{
  return 1750 * (u32)rawValue / 65535 - 450;// T = -45 + 175 * rawValue / (2^16-1)
}

int SHT3X_CalcHumidity(u16 rawValue)//计算湿度值  x10
{
  return 1000 * (u32)rawValue / 65535;  // RH = rawValue / (2^16-1) * 100
}


u8 SHT3X_GetTempAndHumiPolling(int *Cout,etRepeatability repeatability,u8 timeout)
{
  u8 error;           
  u16    temprawValue;   
  u16    humirawValue;
  error  = SHT3X_StartWriteAccess();///写开始  开始信号+地址 +写  0x44  0100 0100  0x88
  if(error == 0x00)//如果没有报错
  {
    error = SHT3X_WriteCommand(CMD_MEAS_POLLING_H);//发送命令     0x24  0x16
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
    error= SHT3X_Read2BytesAndCrc(&temprawValue, ACK, 0);//读数据   
    error= SHT3X_Read2BytesAndCrc(&humirawValue, NACK, 0);//读数据   
  }
  
  SHT3X_StopAccess();//停止信号
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


void delayms(int a)//12M 是平时，16M是插USB  12M  4
{
  while(a--)
  {
    GCC_DELAY(3000);
  }
}




//-----------------------------------开始周期性测量---------------------------------------
u8 SHT3X_StartPeriodicMeasurment(void)
{
   u8   error;
   error = SHT3X_StartWriteAccess();//开始信号+写+地址
   if(error==0x00)
   {
      error = SHT3X_WriteCommand(CMD_MEAS_PERI_1_H);//发命令
   }
   SHT3X_StopAccess();
   return error;                                                                   	
}
//-------------------------------读取周期性测量的BUFF----------------------------------
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