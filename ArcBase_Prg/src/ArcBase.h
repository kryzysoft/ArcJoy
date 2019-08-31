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
#include "IHal/IHalRtc.h"

#include "stdint.h"

typedef struct
{
  IHalEsbRadioPrx *esbPrx;
  IHalUsbJoysticks *joysticks;
  IHalGpioOutput *ledJoy1;
  IHalGpioOutput *ledJoy2;
  IHalGpioOutput *ledNoJoys;
  IHalRtc *rtcClock;
} ArcBaseHardwareConfig;

class ArcBase: public IRtcAlarmHandler
{
  private:
    ArcBaseHardwareConfig *m_pHwConfig;
    volatile uint8_t m_joy1TimeOut;
    volatile uint8_t m_joy2TimeOut;
  public:
    ArcBase(ArcBaseHardwareConfig *hwConfig);
    void Run();
    void RtcAlarmHandler();
};

#endif
