#include "ArcBase.h"
#include "NrfEsbRadioPrx.h"
#include "NrfUsbJoysticks.h"
#include "NrfGpioOutput.h"
#include "NrfGpioInput.h"
#include "NrfHighVoltageMode.h"
#include "NrfRtc.h"
#include "NrfDelay.h"
#include "RttDebugLog.h"

int main()
{
  DebugInit();
  NrfHighVoltageMode highVoltageMode;
  highVoltageMode.SetupOutputVoltage3V();

  NrfEsbRadioPrx esbPrx(true); 
  NrfUsbJoysticks joysticks;
  NrfGpioOutput ledJoy1(6);
  NrfGpioOutput ledJoy2(12);
  NrfGpioOutput ledNoJoys(8);
  NrfRtc rtcClock;

  ArcBaseHardwareConfig hwConfig;

  hwConfig.esbPrx = &esbPrx;
  hwConfig.joysticks = &joysticks;
  hwConfig.ledJoy1 = &ledJoy1;
  hwConfig.ledJoy2 = &ledJoy2;
  hwConfig.ledNoJoys = &ledNoJoys;
  hwConfig.rtcClock = &rtcClock;

  ArcBase arcBase(&hwConfig);

  arcBase.Run();
}