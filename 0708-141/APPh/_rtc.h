#ifndef __RTC_H
#define __RTC_H

u32 DateToSeconds(StrcutRtc *Rtc);
void SecondsToDate(u32 seconds,StrcutRtc *Rtc);
void RtcBcdToD10(StrcutRtc *Rtc);
void RtcD10ToBcd(StrcutRtc *Rtc);

StrcutRtc SS_To_BCD(u32 SS);
u8 Rtc_Is_Right(StrcutRtc *Rtc);

#endif


