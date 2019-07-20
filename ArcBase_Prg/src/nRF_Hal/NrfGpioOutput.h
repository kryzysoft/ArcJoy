#ifndef NRF_GPIO_OUTPUT_H
#define NRF_GPIO_OUTPUT_H

#include "IHal/IHalGpioOutput.h"
#include "stdint.h"
#include "stdbool.h"
#include "nrf_gpio.h"

class NrfGpioOutput: public IHalGpioOutput
{
  private:
    uint32_t m_pinNumber;
  public:
    NrfGpioOutput(uint32_t pinNumber);
    void Up();
    void Down();
};

#endif


