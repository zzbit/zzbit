#include "_config.h"

#if WIRELESS_ENABLE==1

void WirelessPowOff(void)
{
  WIRSLESS_POW_OFF;
  UART_U0_TXD_CLR;
}
void WirelessPowOn(void)
{
  WIRSLESS_POW_ON;
  UART_U0_TXD_SET;
}

#endif

