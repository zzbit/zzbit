#ifndef __SHT3X_H
#define __SHT3X_H



typedef enum
{
  ACK  = 0,
  NACK = 1,
}etI2cAck;

typedef enum
{
  CMD_READ_SERIALNBR  = 0x3780, // read serial number
  CMD_READ_STATUS     = 0xF32D, // read status register
  CMD_CLEAR_STATUS    = 0x3041, // clear status register
  CMD_HEATER_ENABLE   = 0x306D, // enabled heater
  CMD_HEATER_DISABLE  = 0x3066, // disable heater
  CMD_SOFT_RESET      = 0x30A2, // soft reset
  CMD_MEAS_CLOCKSTR_H = 0x2C06, // measurement: clock stretching, high repeatability
  CMD_MEAS_CLOCKSTR_M = 0x2C0D, // measurement: clock stretching, medium repeatability
  CMD_MEAS_CLOCKSTR_L = 0x2C10, // measurement: clock stretching, low repeatability
  CMD_MEAS_POLLING_H  = 0x2400, // measurement: polling, high repeatability
  CMD_MEAS_POLLING_M  = 0x240B, // measurement: polling, medium repeatability
  CMD_MEAS_POLLING_L  = 0x2416, // measurement: polling, low repeatability
  CMD_MEAS_PERI_05_H  = 0x2032, // measurement: periodic 0.5 mps, high repeatability
  CMD_MEAS_PERI_05_M  = 0x2024, // measurement: periodic 0.5 mps, medium repeatability
  CMD_MEAS_PERI_05_L  = 0x202F, // measurement: periodic 0.5 mps, low repeatability
  CMD_MEAS_PERI_1_H   = 0x2130, // measurement: periodic 1 mps, high repeatability
  CMD_MEAS_PERI_1_M   = 0x2126, // measurement: periodic 1 mps, medium repeatability
  CMD_MEAS_PERI_1_L   = 0x212D, // measurement: periodic 1 mps, low repeatability
  CMD_MEAS_PERI_2_H   = 0x2236, // measurement: periodic 2 mps, high repeatability
  CMD_MEAS_PERI_2_M   = 0x2220, // measurement: periodic 2 mps, medium repeatability
  CMD_MEAS_PERI_2_L   = 0x222B, // measurement: periodic 2 mps, low repeatability
  CMD_MEAS_PERI_4_H   = 0x2334, // measurement: periodic 4 mps, high repeatability
  CMD_MEAS_PERI_4_M   = 0x2322, // measurement: periodic 4 mps, medium repeatability
  CMD_MEAS_PERI_4_L   = 0x2329, // measurement: periodic 4 mps, low repeatability
  CMD_MEAS_PERI_10_H  = 0x2737, // measurement: periodic 10 mps, high repeatability
  CMD_MEAS_PERI_10_M  = 0x2721, // measurement: periodic 10 mps, medium repeatability
  CMD_MEAS_PERI_10_L  = 0x272A, // measurement: periodic 10 mps, low repeatability
  CMD_FETCH_DATA      = 0xE000, // readout measurements for periodic mode
  CMD_R_AL_LIM_LS     = 0xE102, // read alert limits, low set
  CMD_R_AL_LIM_LC     = 0xE109, // read alert limits, low clear
  CMD_R_AL_LIM_HS     = 0xE11F, // read alert limits, high set
  CMD_R_AL_LIM_HC     = 0xE114, // read alert limits, high clear
  CMD_W_AL_LIM_HS     = 0x611D, // write alert limits, high set
  CMD_W_AL_LIM_HC     = 0x6116, // write alert limits, high clear
  CMD_W_AL_LIM_LC     = 0x610B, // write alert limits, low clear
  CMD_W_AL_LIM_LS     = 0x6100, // write alert limits, low set
  CMD_NO_SLEEP        = 0x303E,
}etCommands;

typedef enum
{
  REPEATAB_HIGH,   // high repeatability
  REPEATAB_MEDIUM, // medium repeatability
  REPEATAB_LOW,    // low repeatability
}etRepeatability;

typedef enum
{
  MODE_CLKSTRETCH, // clock stretching
  MODE_POLLING,    // polling
}etMode;

typedef enum
{
  FREQUENCY_HZ5,  //  0.5 measurements per seconds
  FREQUENCY_1HZ,  //  1.0 measurements per seconds
  FREQUENCY_2HZ,  //  2.0 measurements per seconds
  FREQUENCY_4HZ,  //  4.0 measurements per seconds
  FREQUENCY_10HZ, // 10.0 measurements per seconds
}etFrequency;
typedef union {
  struct
  {
    #ifdef LITTLE_ENDIAN  // bit-order is little endian
    u16 CrcStatus     : 1; // write data checksum status
    u16 CmdStatus     : 1; // command status
    u16 Reserve0      : 2; // reserved
    u16 ResetDetected : 1; // system reset detected
    u16 Reserve1      : 5; // reserved
    u16 T_Alert       : 1; // temperature tracking alert
    u16 RH_Alert      : 1; // humidity tracking alert
    u16 Reserve2      : 1; // reserved
    u16 HeaterStatus  : 1; // heater status
    u16 Reserve3      : 1; // reserved
    u16 AlertPending  : 1; // alert pending status 
    #else                 // bit-order is big endian
    u16 AlertPending  : 1;
    u16 Reserve3      : 1;
    u16 HeaterStatus  : 1;
    u16 Reserve2      : 1;
    u16 RH_Alert      : 1;
    u16 T_Alert       : 1;
    u16 Reserve1      : 5;
    u16 ResetDetected : 1;
    u16 Reserve0      : 2;
    u16 CmdStatus     : 1;
    u16 CrcStatus     : 1;
    #endif
  } BIT;
} regStatus;

#define POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

//---------------------------------------------------------------------------------------------------------------------------------------------------------
void SHT3X_Init(u8 i2caddress);//
void STH3X_SetI2cAdr(u8 i2caddress);
u8 SHT3X_GetTempAndHumi(int *Cout,etRepeatability repeatability, etMode mode,u8 timeout);
void SHT3X_StopAccess(void);
u8 SHT3X_StartWriteAccess(void);
u8 SHT3X_StartReadAccess(void);
u8 SHT3X_WriteCommand(etCommands command);
u8 SHT3X_Read2BytesAndCrc(u16* data, etI2cAck finaleAckNack,u8 timeout);
u8 SHT3X_CheckCrc(u8 data[], u8 nbrOfBytes, u8 checksum);
int SHT3X_CalcTemperature(u16 rawValue);
int SHT3X_CalcHumidity(u16 rawValue);
u8 SHT3X_GetTempAndHumiPolling(int *Cout,etRepeatability repeatability,u8 timeout);
u8 SHT3X_CalcCrc(u8 data[], u8 nbrOfBytes);
u8 SHT3X_StartPeriodicMeasurment(void);
u8 SHT3X_ReadMeasurementBuffer(int *Cout);


void I2c_Init(void);
void I2c_StartCondition(void);
void I2c_StopCondition(void);
u8 I2c_WriteByte(u8 Byte);
u8 I2c_ReadByte(u8 *Byte,u8 ack, u8 timeout);
u8 I2c_WaitWhileClockStreching(u8 timeout);


void delayms(int a);


#endif


