#include "nRF_Hal/NrfGpioInput.h"
#include "nRF_Hal/NrfGpioInputIrq.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "stdint.h"
#include "DefFix.h"

IHalGpioHandler *NrfGpioInputIrq::gpioHandler = NULL;
bool NrfGpioInputIrq::gpioTeEnabled = false;

NrfGpioInputIrq::NrfGpioInputIrq(uint32_t pinNumber, GpioPullType gpioPullType)
{
  m_pinNumber = pinNumber;
  nrf_gpio_cfg_input(m_pinNumber, NrfGpioInput::pullTypeConvert(gpioPullType));
}

bool NrfGpioInputIrq::IsUp()
{
  return (nrf_gpio_pin_read(m_pinNumber) == 1);
}

bool NrfGpioInputIrq::IsDown()
{
  return (nrf_gpio_pin_read(m_pinNumber) == 0);
}

void NrfGpioInputIrq::SetupHandler(IHalGpioHandler *gpioHandler)
{
  NrfGpioInputIrq::gpioHandler = gpioHandler;

  if(!gpioTeEnabled)
  {
    nrfx_gpiote_init();
    gpioTeEnabled = true;
  }
  nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
  nrfx_gpiote_in_init(m_pinNumber,&config,gpioEvent);
  nrfx_gpiote_in_event_enable(m_pinNumber,true);
}

void NrfGpioInputIrq::gpioEvent(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  NrfGpioInputIrq::gpioHandler->GpioHandler();
}


