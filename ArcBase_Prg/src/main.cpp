#include "ArcBase.h"
#include "NrfEsbRadioPrx.h"
#include "NrfUsbJoysticks.h"

int main()
{
  NrfEsbRadioPrx esbPrx(true); 
  NrfUsbJoysticks joysticks;

  ArcBaseHardwareConfig hwConfig;
  hwConfig.esbPrx = &esbPrx;
  hwConfig.joysticks = &joysticks;

  ArcBase arcBase(&hwConfig);

  arcBase.Run();
}