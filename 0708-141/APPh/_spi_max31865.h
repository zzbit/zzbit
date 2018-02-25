#ifndef  __SPI_MAX31865_H
#define  __SPI_MAX31865_H


void max31865_ini(u8 ch);
int max31865_temp(u8 ch);
void max31865_start(u8 ch);
void rtd_io_all_l(void);
void rtd_io_ini(void);


#endif
