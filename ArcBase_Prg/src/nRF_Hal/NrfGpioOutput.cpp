#include "nRF_Hal/NrfGpioOutput.h"
#include "nrf_gpio.h"
#include "stdint.h"

NrfGpioOutput::NrfGpioOutput(uint32_t pinNumber)
{
  m_pinNumber = pinNumber;
  nrf_gpio_cfg_output(m_pinNumber);
}

void NrfGpioOutput::Up()
{
  nrf_gpio_pin_write(m_pinNumber, 1);
}

void NrfGpioOutput::Down()
{
  nrf_gpio_pin_write(m_pinNumber, 0);
}
