#ifndef  __SPI_MAX31855_H
#define  __SPI_MAX31855_H

void k_io_ini(void);
void k_io_all_l(void);

void Get_K_Temp(int* out,u8 ch);

#endif
