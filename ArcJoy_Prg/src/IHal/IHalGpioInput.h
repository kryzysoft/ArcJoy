#ifndef I_HAL_GPIO_INPUT_H
#define I_HAL_GPIO_INPUT_H

class IHalGpioInput
{
  public:
    virtual bool IsUp() = 0;
    virtual bool IsDown() = 0;
};

#endif