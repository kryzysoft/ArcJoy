#include "nRF_Hal/NrfDelay.h"
#include "nrf_delay.h"

void NrfDelay::DelayMs(uint32_t ms)
{
  nrf_delay_ms(ms);
}
