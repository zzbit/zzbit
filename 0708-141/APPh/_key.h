#ifndef __KEY_H
#define __KEY_H

void KeyPortInit(void);
void KeyDeel(void);
void Key_1_Do(void);
void Key_2_Do(void);
void Key_3_Do(void);
void Key_4_Do(void);
void KeyIsrDo(void);
void KeyIntSet(void);
void KeyIntClr(void);

void KeyTimeDe(void);
u8 KeyDoing(void);

u8 KeyXiaoDou(void);
u8 WakeUp(void);

void KeyView(void);
void KeyHand(void);
u8 KeyMinMaxDoing(void);
void KeyAlarm(void);
void KeyDoStart(void);
void HandStart(void);

void KeyShutDown(void);

#if KEY_SET_ENABLE==1
void Loop1sDeel(void);
u8 KeySetAddrQuit(void);
u8 KeySetIsBusy(void);

void KeyAccDec(u8 Kt,u8 Dec);
void LcdKeyMode(u8 cc,u8 Kt);
u16 KeySetOne(u8 Kt,u8 Dec,u16 vch);
void GetAlarmExt(u16* buf);
void KeySetEnd(void);
void KeySetStart(void);
void Mt2Kt(void);
void KeyDogDeel(void);
void KeyBsqAddrAcc(void);
void KeySetModeDeel(void);
#endif

void Loop2Deel(void);
void KeyDecDeel(void);




void OFF6KeySet(void);
void OFF6KeyRead(void);
void OFF6toOFF(u8 cc);

#if ((RF_REC_EN==1)||(RF_RP_EN==1))
void KeyXzAcc(void);
#endif

#if RF_U1_EN==1
void U1_RF_PC_X(void);
#endif


void U1ToRf(void);

#endif
