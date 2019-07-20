#ifndef NRF_USB_JOYSTICKS_H
#define NRF_USB_JOYSTICKS_H

#include "nRF_Hal/NrfUsbJoysticksRaw.h"
#include "IHal/IHalUsbJoysticks.h"
#include "app_usbd.h"

class NrfUsbJoysticks: public IHalUsbJoysticks
{
  private:
    static const uint8_t JOY_COUNT = 2;
    JoyState joystickState[JOY_COUNT];
  public:
    NrfUsbJoysticks();
    void SetJoyPosition(uint8_t joyNumber, int8_t x, int8_t y);
    void SetJoyButtons(uint8_t joyNumber, uint8_t buttons);
    void ProcessJoyEvents();
};

#endif
