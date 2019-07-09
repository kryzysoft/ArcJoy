extern "C"
{
  #include "nrf_gpio.h"
}

#include "HalGpio.h"

#include "DefFix.h"

HalGpio::HalGpio(uint32_t pinNumber, GpioMode gpioMode, GpioPullType gpioPullType)
{
  m_pinNumber = pinNumber;

  if(gpioMode == GPIO_INPUT)
  {
    nrf_gpio_pin_pull_t nrfGpioPinPull;
    if(gpioPullType == GPIO_PULL_UP)
    {
      nrfGpioPinPull = NRF_GPIO_PIN_PULLUP;
    }
    else if(gpioPullType == GPIO_PULL_DOWN)
    {
      nrfGpioPinPull = NRF_GPIO_PIN_PULLDOWN;
    }
    else
    {
      nrfGpioPinPull = NRF_GPIO_PIN_NOPULL;
    }
    nrf_gpio_cfg_input(m_pinNumber,nrfGpioPinPull);
  }
  else if (gpioMode == GPIO_OUTPUT)
  {
    nrf_gpio_cfg_output(m_pinNumber);
  }
}

void HalGpio::Up()
{
  nrf_gpio_pin_write(m_pinNumber,1);
}

void HalGpio::Down()
{
  nrf_gpio_pin_write(m_pinNumber,0);
}

