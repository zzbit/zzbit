#ifndef __PRINT_DATA_H
#define __PRINT_DATA_H

void PutMaxMinNull(void);
void GetHisInfo(void);
u16 GetHisXpt(u16 Xpt,u16* BufM,u8* RBF,u8 PC);

StrcutRtc GetXuhaoTimeBaseStartTimeBcd(u32 Xuhao);

u32 GetRtcSS(void);

void PrintTest(void);
void PrintList(u16* Xuhao,u8* BufData,u16 Num,u8 ChNo);
void PrintAll(void);
u16 GetPrintData(u16* BufM,u8* BufD);
void PrintMessageInfo(void);
void PrintTime(u32 Xuhao);
void PrintValue(u16 Value);
void PrintJianGe(u32 DD);
void PrintInfo(void);

void GetCurveRange(u8 ChNo,u16* Buf);

void PrintYz(void);
void PrintCurve(u8* BufData,u16 DataN,u8 ChNo);
void GoOneHang(u8 cc);

u8 GetL8(u16 cc);
u8 GetH8(u16 cc);

u8 GetPrintMode(void);
u8 GetPtMod(void);
void SetPtMod(u8 Md);

void PrintChInfo(u8 ChNo);
u16 GetBagMaxMinFromEE(u16 ReadAddr, u16 Len,u16* MinMaxBuf,u8* DataBuf,u8 PC,u8 No);

float GetU16ToFloat(u16 cc);
void PrintStart(void);
void PrintEnd(void);
void GetZvTobuf(u16 zv,u8* buf);
void PrintY5Zv(void);
u16 GetZvY(u16 zv1,u16 zv2,u16 ymin,u16 ymax,u16 zvv);
void PrintCC(u16 zv1,u16 zv2,u32 Xuhao,u8 timeEn);

void PrintDataSave(void);
void PrintNewData(void);
void PrintNewClr(void);

void PrintNewToEE(void);
void PrintNewFromEE(void);

void SetCnPrint(u8 cc);
void SetIni(void);

void PrintOneCurve(void);
void PrintOneList(void);

void Buf_To_Print(u8* buf);
void Print_SF_Data(void);
u8 PrintDeel(void);
void PrintDueout_Clr(void);

void Print_Begin(void);
void Print_BG_Deel(void);

void PrintKeyClr(void);



#endif
