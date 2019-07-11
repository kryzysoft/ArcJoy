#ifndef I_HAL_GPIO_INPUT_IRQ_H
#define I_HAL_GPIO_INPUT_IRQ_H

class IHalGpioHandler
{
  public:
    virtual void GpioHandler() = 0;
};

class IHalGpioInputIrq
{
  public:
    virtual bool IsUp() = 0;
    virtual bool IsDown() = 0;
    virtual void SetupHandler(IHalGpioHandler *gpioHandler) = 0;
};

#endif