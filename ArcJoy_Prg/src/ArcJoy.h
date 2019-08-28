#ifndef ARC_JOY_H
#define ARC_JOY_H

#include "IHal/IHalGpioInput.h"
#include "IHal/IHalGpioIrq.h"
#include "IHal/IHalGpioOutput.h"
#include "IHal/IHalEsbRadioPtx.h"
#include "IHal/IHalRtc.h"
#include "IHal/IHalLowPowerMode.h"
#include "IHal/IHalDelay.h"
#include "IHal/IHalGpioIrq.h"
#include "IHal/IHalGpioWakeUp.h"
#include "IHal/IHalPeriodicEvent.h"

#include "SwitchController.h"

#include "stdint.h"

enum
{
  LEFT_SWITCH = 0,
  RIGHT_SWITCH = 1,
  UP_SWITCH = 2,
  DOWN_SWITCH = 3,
  BUTTON_SWITCH = 4
};

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

  IHalGpioInput *joySwitch[5];

  IHalGpioInput *joyButton;

  IHalGpioWakeUp *joyButtonWakeUp;

  IHalGpioIrq *joyLeftIrq;
  IHalGpioIrq *joyRightIrq;
  IHalGpioIrq *joyUpIrq;
  IHalGpioIrq *joyDownIrq;

  IHalGpioIrq *joyButtonIrq;
  IHalEsbRadioPtx *esbPtx;

  IHalRtc *rtcClock;

  IHalLowPowerMode *offMode;
  IHalLowPowerMode *sleepMode;

  IHalDelay *delay;

  IHalPeriodicEvent *debounceTimer;

} ArcJoyHardwareConfig;

class ArcJoy: public IRtcAlarmHandler, public IGpioIrqHandler, public IPeriodicEventHandler
{
  private:
    ArcJoyHardwareConfig *m_pHwConfig;
    uint8_t m_joyNumber;
    static bool heartbeatFlag;
    static bool switchesFlag;
    uint8_t m_frameCounter;
    volatile uint32_t m_currentTime;
    SwitchController m_switchController;

    uint8_t dipSwitchReadState();
    void radioSendState(uint8_t joyButtons, uint8_t joystick);
    uint8_t joyReadState();
    uint8_t joyButtonsReadState();
    void joyInit();
    void joyButtonsInit();
    void joyGpioDisable();
    bool sendJoyState();
  public:

    ArcJoy(ArcJoyHardwareConfig *hwConfig);
    void Run();
    void RtcAlarmHandler();
    void GpioIrqHandler();
    void PeriodicEventHandler();
};

#endif
