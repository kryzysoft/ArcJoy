#ifndef I_HAL_GPIO_IRQ_H
#define I_HAL_GPIO_IRQ_H

class IGpioIrqHandler
{
  public:
    virtual void IrqGpioHandler() = 0;
};

class IHalGpioIrq
{
  public:
    virtual void SetupHandler(IGpioIrqHandler *irqGpioHandler) = 0;
};

#endif