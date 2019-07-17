#ifndef NRF_GPIO_INPUT_IRQ_H
#define NRF_GPIO_INPUT_IRQ_H

#include "IHal/IHalGpioInputIrq.h"
#include "stdint.h"
#include "stdbool.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

typedef enum
{
  SENSE_HIGH = 0,
  SENSE_LOW = 1
} SenseLevel;

class NrfGpioInputIrq: public IHalGpioInputIrq
{
  private:
    bool m_senseOn;
    SenseLevel m_senseLevel;
    uint32_t m_pinNumber;
    GpioPullType m_gpioPullType;
    static bool gpioTeEnabled;
    static void gpioEvent(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
    static IHalGpioHandler *gpioHandler;
  public:
    NrfGpioInputIrq(uint32_t pinNumber, GpioPullType gpioPullType);
    bool IsUp();
    bool IsDown();
    void EnableSense(SenseLevel senseLevel);
    void SetupHandler(IHalGpioHandler *gpioHandler);
    void Disable();
};

#endif


