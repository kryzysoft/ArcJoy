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

void NrfGpioIrq::SetupHandler(IGpioIrqHandler *gpioHandler)
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
//  if(m_senseOn)
//  {
//    EnableSense(m_senseLevel);
//  }
}

//void NrfGpioInputIrq::EnableSense(SenseLevel senseLevel)
//{
//  m_senseOn = true;
//  m_senseLevel = senseLevel;
//  if(senseLevel == SENSE_HIGH)
//  {
//    nrf_gpio_cfg_sense_set(m_pinNumber, NRF_GPIO_PIN_SENSE_HIGH);
//  }
//  else
//  {
//    nrf_gpio_cfg_sense_set(m_pinNumber, NRF_GPIO_PIN_SENSE_LOW);
//  }
//}

void NrfGpioIrq::gpioteEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  for(uint32_t i=0; i<gpioEventsCount; i++)
  {
    if(NrfGpioIrq::gpioPins[i] == pin)
    {
      NrfGpioIrq::irqHandlers[i]->IrqGpioHandler();
      break;
    }
  }

}
