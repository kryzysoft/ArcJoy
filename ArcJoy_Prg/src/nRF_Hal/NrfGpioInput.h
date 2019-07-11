#ifndef NRF_GPIO_INPUT_H
#define NRF_GPIO_INPUT_H

#include "IHal/IHalGpioInput.h"
#include "stdint.h"
#include "stdbool.h"
#include "nrf_gpio.h"

typedef enum
{
  GPIO_PULL_NONE = 0,
  GPIO_PULL_UP = 1,
  GPIO_PULL_DOWN = 2
} GpioPullType;

class NrfGpioInput: public IHalGpioInput
{
  private:
    uint32_t m_pinNumber;
  public:
    NrfGpioInput(uint32_t pinNumber, GpioPullType gpioPullType);
    static nrf_gpio_pin_pull_t pullTypeConvert(GpioPullType gpioPullType);
    bool IsUp();
    bool IsDown();
};

#endif


