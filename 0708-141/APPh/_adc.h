#ifndef __ADC_H
#define __ADC_H


void AdcInit(void);
void AdcToValue(u16 Adc,u8 ChNo);
u16 GetVbat(void);

#endif

