#include "nRF_Hal/NrfGpioWakeUp.h"
#include "nrf_gpio.h"

NrfGpioWakeUp::NrfGpioWakeUp(uint32_t pinNumber)
{
  m_pinNumber = pinNumber;
}

void NrfGpioWakeUp::EnableWakeUp()
{
  nrf_gpio_cfg_sense_set(m_pinNumber, NRF_GPIO_PIN_SENSE_LOW);
}
