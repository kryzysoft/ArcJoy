#ifndef HAL_GPIO_H
#define HAL_GPIO_H

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

class HalGpio
{
  private:
    uint32_t m_pinNumber;

  public:
    HalGpio(uint32_t pinNumber, GpioMode gpioMode, GpioPullType gpioPullType);
    void Up();
    void Down();
};

#endif