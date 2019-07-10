#ifndef ARC_JOY_H
#define ARC_JOY_H

#include "IHal/IHalGpio.h"
#include "IHal/IHalEsbRadioPtx.h"
#include "stdint.h"

typedef struct
{
  IHalGpio *dipSwitch1;
  IHalGpio *dipSwitch2;
  IHalGpio *dipSwitch3;
  IHalGpio *dipSwitch4;
  IHalGpio *dipSwitch5;
  IHalGpio *dipSwitch6;

  IHalGpio *redLed;
  IHalGpio *blueLed;

  IHalGpio *joyLeft;
  IHalGpio *joyRight;
  IHalGpio *joyUp;
  IHalGpio *joyDown;

  IHalGpio *joyButton1;
  IHalGpio *joyButton2;
  IHalGpio *joyButton3;
  IHalGpio *joyButton4;
  IHalGpio *joyButton5;
  IHalGpio *joyButton6;

  IHalEsbRadioPtx *esbPtx;

} ArcJoyHardwareConfig;

class ArcJoy
{
  private:
    ArcJoyHardwareConfig *m_pHwConfig;

    uint8_t dipSwitchReadState();
    void radioSendState(uint8_t joyButtons, uint8_t joystick);
  public:
    ArcJoy(ArcJoyHardwareConfig *hwConfig);
    void Run();
};

#endif
