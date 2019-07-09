#ifndef ARC_JOY_H
#define ARC_JOY_H

#include "IHal/IHalGpio.h"
#include "stdint.h"

class ArcJoy
{
  private:
    IHalGpio *m_pRedLed;
    IHalGpio *m_pBlueLed;
    IHalGpio *m_pDipSwitch1;
    IHalGpio *m_pDipSwitch2;
    IHalGpio *m_pDipSwitch3;
    IHalGpio *m_pDipSwitch4;
    IHalGpio *m_pDipSwitch5;
    IHalGpio *m_pDipSwitch6;

    uint8_t dipSwitchReadState();
  public:
    ArcJoy(IHalGpio *redLed, IHalGpio *blueLed, IHalGpio *dipSwitch1, IHalGpio *dipSwitch2, IHalGpio *dipSwitch3, IHalGpio *dipSwitch4, IHalGpio *dipSwitch5, IHalGpio *dipSwitch6);
    void Run();
};

#endif
