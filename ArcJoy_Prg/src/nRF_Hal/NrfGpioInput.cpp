#include "nRF_Hal/NrfGpioInput.h"
#include "nrf_gpio.h"
#include "stdint.h"

nrf_gpio_pin_pull_t NrfGpioInput::pullTypeConvert(GpioPullType gpioPullType)
{
  if(gpioPullType == GPIO_PULL_UP)
  {
    return NRF_GPIO_PIN_PULLUP;
  }
  else if (gpioPullType == GPIO_PULL_DOWN)
  {
    return NRF_GPIO_PIN_PULLDOWN;
  }
  else
  {
    return NRF_GPIO_PIN_NOPULL;
  }
}

NrfGpioInput::NrfGpioInput(uint32_t pinNumber, GpioPullType gpioPullType)
{
  m_pinNumber = pinNumber;
  nrf_gpio_cfg_input(m_pinNumber, pullTypeConvert(gpioPullType));
}

bool NrfGpioInput::IsUp()
{
  return (nrf_gpio_pin_read(m_pinNumber) == 1);
}

bool NrfGpioInput::IsDown()
{
  return (nrf_gpio_pin_read(m_pinNumber) == 0);
}
