#ifndef __LOGGER_H
#define __LOGGER_H

void LoggerLoop(void);
void LoggerSample(void);
void LoggerRecord(void);
void LoggerInit(void);
void EEToStateA1(void);
void StateA1ToEE(void);
void EEToStateA2(void);
void StateA2ToEE(void);
void EEToStateB(void);
void StateBToEE(void);
void LoggerConfigFirst(void);
void LoggerAlarmAllSeek(void);

void channel_to_flash_fisrt(void);

void bsq_init(void);
void LoggerWorkDeel(void);
void DownTimeOver(void);
void StateASaveDeel(void);
void DisplayToLow(void);
void LcdExtreDeel(void);

void LoggerAlarmOneSeek(u8 ChNo);
void CurveFlashToBuf(StructCurve* Curve,u8 ChNo);
void alarm_buf_to_flash(u8* buf,u8 ch_no,u8 CC);
void CurveBufToFlash(StructCurve* Curve,u8 ch_no);

u8 LoggerCalculateValue(void);
u8 ChAlarm(void);
u8 HaveALEnable(void);
StructCurve curve_shift(StructCurve dd);

void SaveMinMaxAvgToEE(void);
void GetMinMaxAvgFromEE(void);

void SaveHandFlagToEE(void);
void GetHandFlagFromEE(void);
u8 SeeChAlarm(u8 Ch);
u8 HavePow(void);

void PowPtDeel(void);
void PowTimeClr(void);
void PowTimeStart(void);
u8 PowTimeReady(void);

void LoggerAlarmDeel(void);
void LoggerLcdDeel(void);
void SampleRecordDeel(void);

void LcdAlarmEn(void);
void LcdMode(void);

void Bell_Shan(void);
void Bell_Off(void);

void GetChFilterAd(u8 ChNo);
void FilterChClr(u8 ChNo);
void FilterClr(void);


u8 AlarmXSeek(u16 dd,u8 ChNo,u8 CC);
void SampleRealDo(void);

#if ((ALARM_BELL_TIME_EN==1)||(ALARM_TIME_ENABLE==1))
void AlarmTimeDeel(void);
#endif

#if ALARM_TIME_ENABLE==1
void AlarmedClr(void);
void AlarmedSet(void);
void AlarmTimeDo(void);
void AlarmedToEE(void);
void AlarmedFormEE(void);
u8 HaveChAlarmed(void);
#endif

#if ALARM_BELL_TIME_EN==1
void AlarmBellTimeDo(void);
#endif

void RestartClr(void);
u8 PowShutReady(void);

void RyControlDeel(void);
void RyFirst(void);

u8 ChALEnable(u8 ChNo);

void LoggerExtInt(void);
u16 GetHumiValue(u16 humiAd,u8 TchNo);


void AlarmPointBc(void);
u8 AlarmPointSeek(void);
void AlarmPointStart(void);
void AlarmPointDeel(void);

u8 CompABcc(u16 aa,u16 bb,u16 cc);


void PCBSQ_Record(void);
void PCBSQ_RecNumPt_ToEE(void);
void PCBSQ_CapFromEE(void);
void PCBSQ_CapToEE(void);
void PCBSQ_RecClr(void);

u16 GetBatt(void);

void RyGsmDeel(void);

void ChAEnToFlash(u8 ch,u8 EN);

#if LCD08_SD_SHIFT_EN==1
void GetSdShiftEnFromEE(void);
void SetSdShiftEnToEE(u8 cc);
#endif

void GetPrintXX(void);
void SetPrintXX(void);

void WifiLinkDeel(void);
void Wifi_NET_Reset(void);

void bsq_allwaysSet(void);
void LoggerStop(void);

u8 RecAlarm(void);

u8 GetPhoneNumFromEE(void);
void GetPhoneHaoFromEE(u8* BufA,u8 Pt);
void SetPhoneToEE(u8 *BufA);
void ClrPhoneToEE(void);


u16 GetSmsUni(u8* UniBf,u8 Alarm);
u16 GetSmsTitleUni(u8* buf);

#if RF_REC_EN==1
void Rf_Rec_Yq(u8* buf,u16 len);

void RfDataNULL(void);
void RfDataMN(void);
void RfDataToChDeel(void);
void RfTimeoutDeel(void);
void RfTimeoutClr(void);

void LoggerRfRec(void);


#endif
void GetRfSnFromEE(void);



#if RF_RP_EN==1
void Rf_Rec_Rp(u8 cc);
void Rf_N_Rp_deel(void);
void Rf_NULL_Rp(void);

#endif

void LcdRpSt(void);

#if ((GPS_MOUSE_EN==1))
void GpsTimeOutDeel(void);
#endif

#if (SEN_UART_EN==1)
u16 Get_SenTTL_Value(u8* buf,u16 len);
#endif


float GetDewpoint(float T,float RH);
u8 get_add(u8* buf,u16 len);

#if ((RF_REC_EN==1)||(RF_KP_EN==1)||(RF_RP_EN==1)||(RF_TXRX_470M_EN==1))
void RfSetF(u8 cc);
void SetRfFzToEE(void);
void GetRfFzFromEE(void);
void SendSnToSenior(void);
void RfU_SendBuf(u8* BufIn,u16 Len);
#endif

void TimeDecDeel(void);

void SIM68_to_sleep(void);
void SIM68_to_wake(void);


void RecRpSend(void);
void RecRPData(u8* buf,u16 len);


void MultiBufEE(u8 cc);


void RfReset(void);
void RfU_Init(u32 Baudrate,u8 UART_CLK);


void PCBSQ_RecTime_ToEE(void);
void PCBSQ_RecOne(u8 cc);

void DogReset(void);

u8 Buf_To_Gprs(u8* buf);
u8 BufTx(u8* buf,u8 cc);
u8 GetJY(u8 bbc,u8 cc,u8 xor);
u8 Gprs_SF_Send_Data(u16 nn,u16 mm,u8 cc);

u8 TCP_LG31_Send_Head(u16 mm);


u8 GprsDataHav(void);
void Gprs_SF_Tramit_Data(void);
u8 UxSendByte(u8 cc);


void GprsSendOk(void);

u8 WifiRcvDo(void);


void GprsDueOutFromEE(void);
void GprsDueOutToEE(void);


void Read_EE_LastOne_To_RfBuf(void);


u8 TcpRtcAuto(u8* buf,u16 len,u8 zx);

void RF_First(void);


u8 ServerCmdSet(u8* buf);


void DoorDeel(void);

#if SEN_MODBUS_485_EN==1
void Get485VV(u8* buf,u16 len);
#endif



void ZX_Rtc_Send(void);
void LogSynRtcDeel(void);

u8 GprsUpSend(u8* buf,u8 cmd);


void LG37_ReCmd(u8 bType);


void SaveBcToEE(void);
void GetBcFromEE(void);


void JDledDeel(void);


#endif
