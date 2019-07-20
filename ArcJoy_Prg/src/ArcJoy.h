#ifndef ARC_JOY_H
#define ARC_JOY_H

#include "IHal/IHalGpioInput.h"
#include "IHal/IHalGpioInputIrq.h"
#include "IHal/IHalGpioOutput.h"
#include "IHal/IHalEsbRadioPtx.h"
#include "IHal/IHalRtc.h"
#include "IHal/IHalLowPowerMode.h"
#include "IHal/IHalDelay.h"

#include "stdint.h"

typedef struct
{
  IHalGpioInput *dipSwitch1;
  IHalGpioInput *dipSwitch2;
  IHalGpioInput *dipSwitch3;
  IHalGpioInput *dipSwitch4;
  IHalGpioInput *dipSwitch5;
  IHalGpioInput *dipSwitch6;

  IHalGpioOutput *redLed;
  IHalGpioOutput *blueLed;

  IHalGpioInputIrq *joyLeft;
  IHalGpioInputIrq *joyRight;
  IHalGpioInputIrq *joyUp;
  IHalGpioInputIrq *joyDown;

  IHalGpioInputIrq *joyButton1;
  IHalGpioInputIrq *joyButton2;
  IHalGpioInputIrq *joyButton3;
  IHalGpioInputIrq *joyButton4;
  IHalGpioInputIrq *joyButton5;
  IHalGpioInputIrq *joyButton6;

  IHalEsbRadioPtx *esbPtx;

  IHalRtc *rtcClock;

  IHalLowPowerMode *offMode;
  IHalLowPowerMode *sleepMode;

  IHalDelay *delay;

} ArcJoyHardwareConfig;

class ArcJoy: public IHalRtcAlarmHandler, public IHalGpioHandler
{
  private:
    ArcJoyHardwareConfig *m_pHwConfig;
    uint8_t m_joyNumber;
    static bool sendHeartbeat;
    uint8_t m_frameCounter;

    uint8_t dipSwitchReadState();
    void radioSendState(uint8_t joyButtons, uint8_t joystick);
    uint8_t joyReadState();
    uint8_t joyButtonsReadState();
    void joyInit();
    void joyButtonsInit();
    void joyGpioDisable();
  public:
    static bool sendJoyState;

    ArcJoy(ArcJoyHardwareConfig *hwConfig);
    void Run();
    void RtcAlarmHandler();
    void GpioHandler();
};

#endif
