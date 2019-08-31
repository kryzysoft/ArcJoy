#include "nRF_Hal/NrfGpioInput.h"
#include "nRF_Hal/NrfGpioIrq.h"
#include "nrfx_gpiote.h"
#include "nrf_gpio.h"
#include "stdint.h"
#include "DefFix.h"

#define MAX_GPIO_EVENTS 8

bool NrfGpioIrq::gpioTeEnabled = false;
int32_t NrfGpioIrq::gpioEventsCount = 0;
IGpioIrqHandler *NrfGpioIrq::irqHandlers[MAX_GPIO_EVENTS] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
uint32_t NrfGpioIrq::gpioPins[MAX_GPIO_EVENTS] = {0,0,0,0,0,0,0,0};

NrfGpioIrq::NrfGpioIrq(uint32_t pinNumber)
{
  m_pinNumber = pinNumber;
}

void NrfGpioIrq::SetHandler(IGpioIrqHandler *gpioHandler)
{
  
  NrfGpioIrq::irqHandlers[gpioEventsCount] = gpioHandler;
  NrfGpioIrq::gpioPins[gpioEventsCount] = m_pinNumber;
  gpioEventsCount++;

  if(!gpioTeEnabled)
  {
    nrfx_gpiote_init();
    gpioTeEnabled = true;
  }
  nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
  nrfx_gpiote_in_init(m_pinNumber, &config, gpioteEventHandler);
  nrfx_gpiote_in_event_enable(m_pinNumber,true);
}

void NrfGpioIrq::gpioteEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  for(uint32_t i=0; i<gpioEventsCount; i++)
  {
    if(NrfGpioIrq::gpioPins[i] == pin)
    {
      NrfGpioIrq::irqHandlers[i]->GpioIrqHandler();
      break;
    }
  }

}
