#include "NrfSleepMode.h"
#include "nrf.h"

void NrfSleepMode::Enter()
{
  NRF_POWER->SYSTEMOFF = 0x1;
  (void)NRF_POWER->SYSTEMOFF;
  while(true);
}