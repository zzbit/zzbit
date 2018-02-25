#ifndef __UART_PC_H
#define __UART_PC_H

void ComDeel(void);
void ReadLink(void);
void ReadAdc(void);
void ReadRealValue(void);
void ReadDownloadInfor(void);
void ReadDownloadBag(void);
void SetStop(void);
void SetReStart(void);
void SetStateB(void);
void SetAlarm(void);
void SetCurve(void);
void SetAddress(void);
void SetModelSerialNum(void);
void SetPhone(void);
void ReadPhone(void);
void ReadCurve(void);
void U1GprsDeel(void);

void SetGsmMontior(void);

void CmdHexDeel(void);
void CmdHexDownPag(void);

void CmdAsciiDownPag(void);

void SetFactorySetting(void);
void SetAdcMode(void);
void SetControl(void);
void ReadControl(void);

void SetInvInfor(void);
void ReadInvInfor(void);

void ReadBattTime(void);
void SetRtc(void);

#if MODBUS_ENABLE==1
void MCGS_ReadReal(void);
void RTU_ReadReal(void);
void CmdModbusDeel(void);
void PC_ReadReal(void);
void PC_MULTI_ReadReal(void);
#endif



void ReadBatteryCapacity(void);
void ReadGprsInfo(void);
void SetGprsInfo(void);

void CmdSetSdShift(void);
void CmdReadSdShift(void);

void CmdSetPrintXX(void);
void CmdReadPrintXX(void);

void PC_SetRecordCap(void);
void PC_SetRecordClr(void);
void PC_ReadState(void);
void PC_ReadAlarm(void);

void SetSmsTitle(void);
void ReadSmsTitle(void);

void PC_ReadSMSData(void);

void Read_Multi_Infor(void);

void RfPair(void);
void SetFreq(void);


void Read_GPRS_DATA_Infor(void);
void Read_GPRS_DATA_Bag(void);
u8 Buf_To_PC(u8* buf);


#if RF_MULTI_EN==1
void SetRfkp(void);
void ReadRfkp(void);
void  KpInfor_FromEE(void);
#endif

#if BSQ_YIDA_EN==1
void YIDA_Do(void);
#endif


void SetCd(void);
void ReadCd(void);




#endif

