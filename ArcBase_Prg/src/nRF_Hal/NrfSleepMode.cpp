#include "NrfSleepMode.h"
#include "nrf.h"

void NrfSleepMode::Enter()
{
  __SEV();
  __WFE();
  __WFE();
}