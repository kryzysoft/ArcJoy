#ifndef I_HAL_USB_JOYSTICKS_H
#define I_HAL_USB_JOYSTICKS_H

#include "stdint.h"

class IHalUsbJoysticks
{
  public:
    virtual void SetJoyPosition(uint8_t joyNumber, int8_t x, int8_t y) = 0;
    virtual void SetJoyButtons(uint8_t joyNumber, uint8_t buttons) = 0;
    virtual void ProcessJoyEvents() = 0;
};

#endif
