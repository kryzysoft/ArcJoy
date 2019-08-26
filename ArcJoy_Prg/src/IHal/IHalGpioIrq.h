#ifndef I_HAL_GPIO_IRQ_H
#define I_HAL_GPIO_IRQ_H

class IGpioIrqHandler
{
  public:
    virtual void GpioIrqHandler() = 0;
};

class IHalGpioIrq
{
  public:
    virtual void SetHandler(IGpioIrqHandler *irqGpioHandler) = 0;
};

#endif