#ifndef I_HAL_GPIO_WAKE_UP_H
#define I_HAL_GPIO_WAKE_UP_H

class IHalGpioWakeUp
{
  public:
    virtual void EnableWakeUp() = 0;
};

#endif
