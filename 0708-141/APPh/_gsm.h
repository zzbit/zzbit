#ifndef __GSM_H
#define __GSM_H

void ErrorToRst(void);

void GsmWakeUp(void);
void GsmSlp(void);


u8 CheckBuf(u8* BufA,u8* BufB,u16 len);
u8 CompBuf(u8* BufA,u8 LenA,u8* BufB,u8 LenB);
u8 GetPosC(u8* BufIn,u8 StartPt,u8 EndPt,u8 CC);

int hex_2_ascii(unsigned char *data, unsigned char *buffer, int len);
int ascii_2_hex(unsigned char *O_data, unsigned char *N_data, int len);
unsigned int decode(unsigned char in[],unsigned char out[],unsigned int nLen);


void U0Deel(void);
void U0SmsLen(u16 Len);
void U0SmsPdu(u8* Phone,u8* BufUni,u8 LenUni);
u16 U0PduAnalyze(u8 *phone,u8 *BufUni);

void GsmLoop(void);
void U0Deel(void);
void U0Clear(void);

void EM310_Rst(void);
void EM310_PWon(void);


u8 SmsRecDeel(void);
u8 GsmIsFree(u8 cc);
void DogDeel(void);
void GsmInitDeel(void);
void SmsDeel(void);
void CsqDeel(void);

void SmsAlarmDeel(void);
void SmsAlarmOneOk(void);
void SmsAlarmStart(void);
void SmsAlarmEnd(void);
void SmsSendOk(void);

void PowInDeel(void);
void SmsPowStart(void);

void SmsPowOneOk(void);
void SmsPowDeel(void);
u16 GetPowUni(u8* UniBf,u8 PowFg);
void GsmOnErrDeel(void);


u8 SmsPowReady(u8 PhonePt);
u8 SmsAlarmReady(u8 PhonePt);




void GsmAT2Off(void);

void GsmInvReadOne(u8* Buf);
void GsmInvSendOk(void);
void GsmInvDeel(void);
void GsmInvRecord(u32 SS,u8 ipt);
u8 SmsInvReady(u8 PhonePt);
void GsmInvSet(u32* bufin);
void GsmInvRead(void);
void GsmInvInit(void);
void GsmInvShutDeel(void);


u16 GetOnePotData(u8* BufA,u32 SS);
void InvRecordAcc(u8* buf,u16 Len);
u16 GsmDayPotsToUni(u8* Buf,u8* UniBf);


void SmsKeySet(void);
void SmsKeyRead(void);
void DogClear(void);


u8 GetPhoneError(u8 PhonePt);

void GsmToPowOn(void);
void GsmToPowOff(void);

void ClrBuf(u8* buf,u16 len);

void GsmTest(void);
u8 GetInternationalPhone(u8* Phone,u8* out);

void SmsTestDeel(void);
u8 SmsTestReady(u8 PhonePt);
u16 GetTestUni(u8* UniBf);
void SmsTestOneOk(void);
void SmsTestStart(void);

u8 InterNSeek(u8* buf);

void SmsBattDeel(void);
u8 SmsBattReady(u8 PhonePt);
u16 GetBattUni(u8* UniBf,u8 AL);
void SmsBattOneOk(void);
u8 SmsBattStart(void);

void SmsSendPhone(u8* phone);

u16 GetBattTxt(u8* ss,u8 AL);
u16 GetSmsTxt(u8* ss,u8 Alarm);
u16 GetTestTxt(u8* Txt);
u16 GetPowTxt(u8* Txt,u8 PowFg);

void GprsDeel(void);

void GprsGetRealData(u16 mm);



void GprsPowToRam(u8 PowIn);
void GprsPowTcpSend(void);

void GsmReTime(u8 cc);

u8 PhoneIsRight(void);

void SmsTxtAnalyze(u8* phone,u8* txt);
u8 SmsTxtDeel(void);

void SmsKeyOff(void);
void SmsKeyOn(void);

u8 Get_LAC_CID(void);

void GprsSleepFromEE(void);
void GprsSleepToEE(void);


void UtcStart(void);

u8 GetGsmXh(void);


void SF_Exit_Tc_Clr(void);
u8 SF_Send_Head(u8 type,u16 pt);


void Gprs_SF_Tramit_Data(void);

void Gprs_SF_Deel(void);

u8 Gprs_SF_Recv_Do(void);



u8 GsmFireDeel(void);
u8 Gprs_Exit_Close_Deel(void);


void Gprs_SF_XLH_FromEE(void);
void Gprs_SF_XLH_ToEE(void);
void PrintDueOutFromEE(void);
void PrintDueOutToEE(void);

u16 Buf_7E_Decode(u8* in,u16 inlen,u8* out);


void SF_Send_HandOn(void);
void SF_Send_HandOff(void);

void GetID_FromEE(u8* ID);
u8 CheckID(u8* buf);

u8 Is_WSD(u8* buf);

void Grps_TCP_Ok(void);

u8 GetPos2C(u8* BufIn,u8 StartPt,u8 EndPt,u8 CC,u8* out);

void Gprs_UpTime_FromEE(void);
void Gprs_PrtCAP_FromEE(void);

#if SMS_TCP_EN==1
void TcpSmsSend(void);
#endif


u16 GetRtcUni(u8* UniBf);



#endif
