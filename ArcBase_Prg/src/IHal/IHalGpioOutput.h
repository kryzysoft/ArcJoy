#ifndef I_HAL_GPIO_OUTPUT_H
#define I_HAL_GPIO_OUTPUT_H

class IHalGpioOutput
{
  public:
    virtual void Up() = 0;
    virtual void Down() = 0;
};

#endif