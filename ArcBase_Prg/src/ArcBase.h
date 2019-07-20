#ifndef ARC_BASE_H
#define ARC_BASE_H

#include "IHal/IHalGpioInput.h"
#include "IHal/IHalGpioInputIrq.h"
#include "IHal/IHalGpioOutput.h"
#include "IHal/IHalEsbRadioPrx.h"
#include "IHal/IHalRtc.h"
#include "IHal/IHalLowPowerMode.h"
#include "IHal/IHalDelay.h"
#include "IHal/IHalUsbJoysticks.h"

#include "stdint.h"

typedef struct
{
//  IHalGpioInput *dipSwitch1;
//  IHalGpioInput *dipSwitch2;
//  IHalGpioInput *dipSwitch3;
//  IHalGpioInput *dipSwitch4;
//  IHalGpioInput *dipSwitch5;
//  IHalGpioInput *dipSwitch6;
//
//  IHalGpioOutput *redLed;
//  IHalGpioOutput *blueLed;
//
//  IHalGpioInputIrq *joyLeft;
//  IHalGpioInputIrq *joyRight;
//  IHalGpioInputIrq *joyUp;
//  IHalGpioInputIrq *joyDown;
//
//  IHalGpioInputIrq *joyButton1;
//  IHalGpioInputIrq *joyButton2;
//  IHalGpioInputIrq *joyButton3;
//  IHalGpioInputIrq *joyButton4;
//  IHalGpioInputIrq *joyButton5;
//  IHalGpioInputIrq *joyButton6;
//
  IHalEsbRadioPrx *esbPrx;
  IHalUsbJoysticks *joysticks;
//
//  IHalRtc *rtcClock;
//
//  IHalLowPowerMode *offMode;
//  IHalLowPowerMode *sleepMode;
//
//  IHalDelay *delay;

} ArcBaseHardwareConfig;

class ArcBase
{
  private:
    ArcBaseHardwareConfig *m_pHwConfig;
  public:
    ArcBase(ArcBaseHardwareConfig *hwConfig);
    void Run();
};

#endif
