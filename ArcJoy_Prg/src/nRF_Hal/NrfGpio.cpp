#include "nRF_Hal/NrfGpio.h"
#include "nrf_gpio.h"
#include "stdint.h"

nrf_gpio_pin_pull_t NrfGpio::pullTypeConvert(GpioPullType gpioPullType)
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

NrfGpio::NrfGpio(uint32_t pinNumber, GpioMode gpioMode, GpioPullType gpioPullType)
{
  m_pinNumber = pinNumber;
  if(gpioMode == GPIO_OUTPUT)
  {
    nrf_gpio_cfg_output(m_pinNumber);
  }
  else if(gpioMode == GPIO_INPUT)
  {
    nrf_gpio_cfg_input(m_pinNumber, pullTypeConvert(gpioPullType));
  }
}

void NrfGpio::Up()
{
  nrf_gpio_pin_write(m_pinNumber, 1);
}

void NrfGpio::Down()
{
  nrf_gpio_pin_write(m_pinNumber, 0);
}

bool NrfGpio::IsUp()
{
  return (nrf_gpio_pin_read(m_pinNumber) == 1);
}

bool NrfGpio::IsDown()
{
  return (nrf_gpio_pin_read(m_pinNumber) == 0);
}
