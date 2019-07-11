#ifndef NRF_GPIO_INPUT_IRQ_H
#define NRF_GPIO_INPUT_IRQ_H

#include "IHal/IHalGpioInputIrq.h"
#include "stdint.h"
#include "stdbool.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

class NrfGpioInputIrq: public IHalGpioInputIrq
{
  private:
    uint32_t m_pinNumber;
    static bool gpioTeEnabled;
    static void gpioEvent(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
    static IHalGpioHandler *gpioHandler;
  public:
    NrfGpioInputIrq(uint32_t pinNumber, GpioPullType gpioPullType);
    bool IsUp();
    bool IsDown();
    void SetupHandler(IHalGpioHandler *gpioHandler);
};

#endif


