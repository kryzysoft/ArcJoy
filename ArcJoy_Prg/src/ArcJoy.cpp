#include "ArcJoy.h"
#include "stdint.h"
#include "RttDebugLog.h"
#include "nrf_drv_clock.h"
#include "SwitchController.h"


void JOYAPP_Run();

static void joyInit(void);
static uint8_t joyReadState(void);
static void joyButtonsInit(void);
static uint8_t joyButtonsReadState(void);

#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8

#define MAX_RADIO_FAILS 40

#define HEARTBEAT_PERIOD 1

#define DEBOUNCE_TIMEOUT_MS 5

bool ArcJoy::heartbeatFlag = false;
bool ArcJoy::switchesFlag = false;

static const char* switchName[5] = {"Left","Right","Up","Down","Button"};

ArcJoy::ArcJoy(ArcJoyHardwareConfig *hwConfig):
  m_switchController(m_pHwConfig->joySwitch, 5)
{
  m_pHwConfig = hwConfig;
}

void ArcJoy::Run()
{
  uint8_t joyState = NONE;
  uint8_t dipSwitchState = 0;
  uint8_t joyButtonsState = 0;
  uint32_t fails = 0;

  uint8_t base_addr_0[4] = {'A', 'r', 'c', '1'};
  uint8_t base_addr_1[4] = {'J', 'o', 'y', '2'};
  uint8_t addr_prefix[8] = {'O', 'T', 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

  DebugInfo("Set up GPIO");

  joyInit();
  joyButtonsInit();

  DebugInfo("Set up 1ms timer");
  m_pHwConfig->debounceTimer->SetPeriodMs(1);
  m_pHwConfig->debounceTimer->SetPeriodicEventHandler(this);
  m_pHwConfig->debounceTimer->Start();

//  while(1)
//  {
//    state = !state;
//    if(state)
//    {
//      m_pHwConfig->blueLed->Up();
//    }
//    else
//    {
//      m_pHwConfig->blueLed->Down();
//    }
//
//    switchController.Tick(m_currentTime);
//    if(switchController.HasChanged())
//    {
//       
//       DebugInfo("Change: %x", switchController.GetStateAsByte(LEFT_SWITCH,DOWN_SWITCH));
//    }
//  }

  DebugInfo("Set up RTC");
  m_pHwConfig->rtcClock->SetAlarmHandler(this);
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);

  DebugInfo("Set up ESB");
  m_pHwConfig->esbPtx->SetupAddress0(base_addr_0);
  m_pHwConfig->esbPtx->SetupAddress1(base_addr_1);
  m_pHwConfig->esbPtx->SetupAddressPrefixes(addr_prefix,8);
  m_pHwConfig->esbPtx->SetRfChannel(27);

  bool led = false;


  m_pHwConfig->dipSwitch1->Enable();
  if(m_pHwConfig->dipSwitch1->IsUp())
  {
    m_joyNumber = 0;
  }
  else
  {
    m_joyNumber = 1;
  }
  m_pHwConfig->dipSwitch1->Disable();


  DebugInfo("Application run");


  while(true)
  {
    m_switchController.Tick(m_currentTime);

    switchesFlag = m_switchController.HasChanged();

    while(heartbeatFlag || switchesFlag)
    {
      if(heartbeatFlag)
      {
        DebugInfo("Heartbeat");
        heartbeatFlag = false;
        if(sendJoyState())
        {
          fails = 0;
          m_pHwConfig->blueLed->Up();
        }
        else
        {
          fails++;
          m_pHwConfig->redLed->Up();
          if(fails < MAX_RADIO_FAILS)
          {
            switchesFlag = true;
          }
        }
        m_pHwConfig->delay->DelayMs(100);
        m_pHwConfig->redLed->Down();
        m_pHwConfig->blueLed->Down();
      }
      if(switchesFlag)
      {
        DebugInfo("joyState");
        switchesFlag = false;
        if(!sendJoyState())
        {
          switchesFlag = true;
        }
      }
    }
    if(fails < MAX_RADIO_FAILS)
    {
      m_pHwConfig->sleepMode->Enter();
    }
    else
    {
      joyGpioDisable();
      m_pHwConfig->joyButtonWakeUp->EnableWakeUp();
      m_pHwConfig->offMode->Enter();
    }
  }
}

bool ArcJoy::sendJoyState()
{
  m_pHwConfig->esbPtx->On();
  uint8_t joyState = joyReadState();
  uint8_t joyButtonsState = joyButtonsReadState();
  radioSendState(joyButtonsState,joyState);

  while(m_pHwConfig->esbPtx->IsRadioBusy())
  {
  }
  m_pHwConfig->esbPtx->Off();
  
  return m_pHwConfig->esbPtx->SendSucceeded();
}


void ArcJoy::joyInit(void)
{
  m_pHwConfig->joyLeftIrq->SetHandler(this);
  m_pHwConfig->joyRightIrq->SetHandler(this);
  m_pHwConfig->joyUpIrq->SetHandler(this);
  m_pHwConfig->joyDownIrq->SetHandler(this);
}

uint8_t ArcJoy::dipSwitchReadState()
{
  uint8_t retVal = 0;

  if(m_pHwConfig->dipSwitch6->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_pHwConfig->dipSwitch5->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_pHwConfig->dipSwitch4->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_pHwConfig->dipSwitch3->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_pHwConfig->dipSwitch2->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_pHwConfig->dipSwitch1->IsDown())
  {
    retVal |= 1;
  }
  
  return retVal;
}

uint8_t ArcJoy::joyReadState(void)
{
  uint8_t joyState = m_switchController.GetStateAsByte(LEFT_SWITCH,DOWN_SWITCH);
  return joyState;
} 

void ArcJoy::GpioIrqHandler()
{
  ArcJoy::switchesFlag = true;
}

void ArcJoy::joyButtonsInit(void)
{
  m_pHwConfig->joyButtonIrq->SetHandler(this);
}

void ArcJoy::joyGpioDisable(void)
{
  m_pHwConfig->joySwitch[LEFT_SWITCH]->Disable();
  m_pHwConfig->joySwitch[RIGHT_SWITCH]->Disable();
  m_pHwConfig->joySwitch[UP_SWITCH]->Disable();
  m_pHwConfig->joySwitch[DOWN_SWITCH]->Disable();
}

uint8_t ArcJoy::joyButtonsReadState(void)
{
  return m_switchController.GetStateAsByte(BUTTON_SWITCH,BUTTON_SWITCH);
} 

void ArcJoy::radioSendState(uint8_t joyButtons, uint8_t joystick)
{
  const uint8_t dataLength = 3;
  uint8_t data[dataLength];
  data[0] = m_frameCounter;
  if(m_frameCounter<255)
  {
    m_frameCounter++;
  }
  else
  {
    m_frameCounter = 0;
  }
  data[1] = joystick;
  data[2] = joyButtons;
  m_pHwConfig->esbPtx->SendFrame(m_joyNumber,data,dataLength);
}

void ArcJoy::RtcAlarmHandler()
{
  ArcJoy::heartbeatFlag = true;
  m_pHwConfig->rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);
}

void ArcJoy::PeriodicEventHandler()
{
  m_currentTime++;
}
