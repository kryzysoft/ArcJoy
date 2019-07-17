#ifndef I_HAL_GPIO_INPUT_H
#define I_HAL_GPIO_INPUT_H

class IHalGpioInput
{
  public:
    virtual bool IsUp() = 0;
    virtual bool IsDown() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;
};

#endif