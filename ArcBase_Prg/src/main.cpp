#include "ArcBase.h"
#include "NrfEsbRadioPrx.h"

int main()
{
  NrfEsbRadioPrx esbPrx(true); 
  ArcBaseHardwareConfig hwConfig;
  hwConfig.esbPrx = &esbPrx;

  ArcBase arcBase(&hwConfig);

  arcBase.Run();
}