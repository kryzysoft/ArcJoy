#include "NrfSystemOffMode.h"
#include "nrf.h"

void NrfSystemOffMode::Enter()
{
  NRF_POWER->SYSTEMOFF = 0x1;
  (void)NRF_POWER->SYSTEMOFF;
  while(true);
}