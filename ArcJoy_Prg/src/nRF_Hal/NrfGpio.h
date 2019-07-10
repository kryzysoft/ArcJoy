#ifndef NRF_GPIO_H
#define NRF_GPIO_H

#include "IHal/IHalGpio.h"
#include "stdint.h"
#include "stdbool.h"
#include "nrf_gpio.h"

typedef enum
{
  GPIO_INPUT = 0,
  GPIO_OUTPUT = 1
} GpioMode;

typedef enum
{
  GPIO_PULL_NONE = 0,
  GPIO_PULL_UP = 1,
  GPIO_PULL_DOWN = 2
} GpioPullType;

class NrfGpio: public IHalGpio
{
  private:
    uint32_t m_pinNumber;
    static nrf_gpio_pin_pull_t pullTypeConvert(GpioPullType gpioPullType);
  public:
    NrfGpio(uint32_t pinNumber, GpioMode gpioMode, GpioPullType gpioPullType);
    void Up();
    void Down();
    bool IsUp();
    bool IsDown();
};

#endif


