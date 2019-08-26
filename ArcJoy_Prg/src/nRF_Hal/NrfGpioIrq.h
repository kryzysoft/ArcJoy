#ifndef NRF_GPIO_IRQ_H
#define NRF_GPIO_IRQ_H

#include "IHal/IHalGpioIrq.h"
#include "stdint.h"
#include "stdbool.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#define MAX_GPIO_EVENTS 8

class NrfGpioIrq: public IHalGpioIrq
{
  private:
    static void gpioteEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
    static IGpioIrqHandler *irqHandlers[MAX_GPIO_EVENTS];
    static uint32_t gpioPins[MAX_GPIO_EVENTS];
    static int32_t gpioEventsCount;
    static bool gpioTeEnabled;
    nrfx_gpiote_pin_t m_pinNumber;
  public:
    NrfGpioIrq(uint32_t pinNumber);
    virtual void SetHandler(IGpioIrqHandler *iqrHandler);
};


#endif


