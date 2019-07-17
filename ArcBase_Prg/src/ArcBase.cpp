#include "ArcBase.h"
#include "ArcBase_c.h"

ArcBase::ArcBase(ArcBaseHardwareConfig *hwConfig)
{
  m_pHwConfig = hwConfig;
}

static JoyState joyState[2] = {{false,false,false,false,false},{false,false,false,false,false}};

#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8


void ArcBase::Run()
{
  arcBase_init();
  uint8_t base_addr_0[4] = {'A', 'r', 'c', '1'};
  uint8_t base_addr_1[4] = {'J', 'o', 'y', '2'};
  uint8_t addr_prefix[8] = {'O', 'T', 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

  m_pHwConfig->esbPrx->SetupAddress0(base_addr_0);
  m_pHwConfig->esbPrx->SetupAddress1(base_addr_1);
  m_pHwConfig->esbPrx->SetupAddressPrefixes(addr_prefix,8);

  m_pHwConfig->esbPrx->On();

  while(true)
  {
    uint8_t data[256];
    uint8_t dataLength;
    uint8_t pipe;

    while(m_pHwConfig->esbPrx->GetNextFrame(&pipe,data,&dataLength))
    {
      uint8_t joyIndex = pipe;
      if(joyIndex>1) joyIndex = 1;
      if((data[1] & LEFT) != 0) joyState[joyIndex].left = true;
      else joyState[joyIndex].left = false;
      if((data[1] & RIGHT) != 0) joyState[joyIndex].right = true;
      else joyState[joyIndex].right = false;
      if((data[1] & UP) != 0) joyState[joyIndex].up = true;
      else joyState[joyIndex].up = false;
      if((data[1] & DOWN) != 0) joyState[joyIndex].down = true;
      else joyState[joyIndex].down = false;
      if(data[2] >0) joyState[joyIndex].fire = true;
      else joyState[joyIndex].fire = false;
    }
    arcBase_whileLoop(joyState[0], joyState[1]);
  }
}


