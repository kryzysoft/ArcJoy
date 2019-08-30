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
#include "IHal/IHalDelayedEvent.h"

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

  IHalDelayedEvent *ledOffEvent;

} ArcJoyHardwareConfig;

class ArcJoy: public IRtcAlarmHandler, public IGpioIrqHandler, public IDelayedEventHandler
{
  private:
    ArcJoyHardwareConfig &m_hw;
    uint8_t m_joyNumber;
    bool m_heartbeatFlag;
    bool m_switchesFlag;
    uint8_t m_frameCounter;
    SwitchController m_switchController;
    bool m_ledOffPending;
    uint32_t m_delayMs;
    static const int32_t MAX_RADIO_FAILS = 10;
    static const int32_t HEARTBEAT_PERIOD = 1;

    uint8_t dipSwitchReadState();
    void radioSendState(uint8_t joyButtons, uint8_t joystick);
    uint8_t joyReadState();
    uint8_t joyButtonsReadState();
    void joyInit();
    void joyButtonsInit();
    void joyGpioDisable();
    bool sendJoyState();
  public:

    ArcJoy(ArcJoyHardwareConfig &hwConfig);
    void Run();
    void RtcAlarmHandler();
    void GpioIrqHandler();
    void DelayedEventHandler();
};

#endif
