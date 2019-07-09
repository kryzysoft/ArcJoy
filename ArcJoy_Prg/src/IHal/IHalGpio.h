#ifndef I_HAL_GPIO_H
#define I_HAL_GPIO_H

class IHalGpio
{
  public:
    virtual void Up() = 0;
    virtual void Down() = 0;
    virtual bool IsUp() = 0;
    virtual bool IsDown() = 0;
};

#endif