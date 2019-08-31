#include "nRF_Hal/NrfUsbJoysticksRaw.h"
#include "nRF_Hal/NrfUsbJoysticks.h"
#include "nrf_drv_clock.h"
#include "app_usbd_hid.h"
#include "app_usbd_hid_generic_internal.h"
#include "app_usbd_hid_generic.h"

NrfUsbJoysticks::NrfUsbJoysticks()
{
  for (uint32_t i=0; i<JOY_COUNT; i++)
  {
    joystickState[i].left = false;
    joystickState[i].right = false;
    joystickState[i].up = false;
    joystickState[i].down = false;
    joystickState[i].fire = false;
  }
  JOYSTICKS_Init();
}

void NrfUsbJoysticks::SetJoyPosition(uint8_t joyNumber, int8_t x, int8_t y)
{
  joystickState[joyNumber].left = (x<0);
  joystickState[joyNumber].right = (x>0);
  joystickState[joyNumber].up = (y<0);
  joystickState[joyNumber].down = (y>0);
}

void NrfUsbJoysticks::SetJoyButtons(uint8_t joyNumber, uint8_t buttons)
{
  joystickState[joyNumber].fire = (buttons > 0);
}

void NrfUsbJoysticks::ProcessJoyEvents()
{
  JOYSTICKS_SendReports(joystickState[0],joystickState[1]);
}
