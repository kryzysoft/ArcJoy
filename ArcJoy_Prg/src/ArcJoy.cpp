#include "ArcJoy.h"
#include "stdint.h"
#include "DebugLog.h"
#include "nrf_drv_clock.h"
#include "SwitchController.h"
#include "stdlib.h"

ArcJoy::ArcJoy(ArcJoyHardwareConfig &hwConfig):
  m_hw(hwConfig), m_switchController(hwConfig.joySwitch, 5, hwConfig.delay), m_delayMs(100), m_heartbeatFlag(false), m_switchesFlag(false)
{
}

void ArcJoy::Run()
{
  uint8_t joyState = 0;
  uint8_t dipSwitchState = 0;
  uint8_t joyButtonsState = 0;
  uint32_t fails = 0;
  m_ledOffPending = false;

  uint8_t base_addr_0[4] = {'A', 'r', 'c', '1'};
  uint8_t base_addr_1[4] = {'J', 'o', 'y', '2'};
  uint8_t addr_prefix[8] = {'O', 'T', 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

  DebugInfo("Set up GPIO");

  joyInit();
  joyButtonsInit();

  DebugInfo("Set up RTC");
  m_hw.rtcClock->SetAlarmHandler(this);
  m_hw.rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);

  DebugInfo("Set up ESB");
  m_hw.esbPtx->SetupAddress0(base_addr_0);
  m_hw.esbPtx->SetupAddress1(base_addr_1);
  m_hw.esbPtx->SetupAddressPrefixes(addr_prefix,8);
  m_hw.esbPtx->SetRfChannel(27);

  bool led = false;


  m_hw.dipSwitch1->Enable();
  if(m_hw.dipSwitch1->IsUp())
  {
    m_joyNumber = 0;
  }
  else
  {
    m_joyNumber = 1;
  }
  m_hw.dipSwitch1->Disable();


  DebugInfo("Application run");


  while(true)
  {
    m_switchesFlag =  m_switchController.HasChanged();

    while(m_heartbeatFlag || m_switchesFlag)
    {
      if(m_heartbeatFlag)
      {
      DebugInfo("Heartbeat %d", m_switchController.GetStateAsByte(LEFT_SWITCH,DOWN_SWITCH));
     //   DebugInfo("Heartbeat");
        m_heartbeatFlag = false;
        if(sendJoyState())
        {
          fails = 0;
          m_hw.blueLed->Up();
        }
        else
        {
          fails++;
          m_hw.redLed->Up();
          if(fails < MAX_RADIO_FAILS)
          {
            m_switchesFlag = true;
          }
          else
          {
            m_switchesFlag = false;
          }
        }

        if(DebugInputAvailable())
        {
          char inputText[30];
          DebugInputRead(inputText,30);
          m_delayMs = atoi(inputText);
        }

        m_hw.ledOffEvent->ScheduleEvent(this,m_delayMs);
        m_ledOffPending = true;
      }
      if(m_switchesFlag)
      {
        DebugInfo("joyState %d", m_switchController.GetStateAsByte(LEFT_SWITCH,DOWN_SWITCH));
        m_switchesFlag = false;
        if(!sendJoyState())
        {
          DebugInfo("Fail");
          m_switchesFlag = true;
        }
        if(m_switchController.HasChanged())
        {
          DebugInfo("New State");
          m_switchesFlag = true;
        }
      }
    }
    if(fails < MAX_RADIO_FAILS)
    {
      DebugInfo("Sleep");
      m_hw.sleepMode->Enter();
    }
    else
    {
      m_hw.blueLed->Down();
      m_hw.redLed->Down();
      joyGpioDisable();
      m_hw.joyButtonWakeUp->EnableWakeUp();
      m_hw.offMode->Enter();
    }
  }
}

bool ArcJoy::sendJoyState()
{
  m_hw.esbPtx->On();
  uint8_t joyState = joyReadState();
  uint8_t joyButtonsState = joyButtonsReadState();
  radioSendState(joyButtonsState,joyState);

  while(m_hw.esbPtx->IsRadioBusy())
  {
  }
  m_hw.esbPtx->Off();
  
  return m_hw.esbPtx->SendSucceeded();
}


void ArcJoy::joyInit(void)
{
  m_hw.joyLeftIrq->SetHandler(this);
  m_hw.joyRightIrq->SetHandler(this);
  m_hw.joyUpIrq->SetHandler(this);
  m_hw.joyDownIrq->SetHandler(this);
}

uint8_t ArcJoy::dipSwitchReadState()
{
  uint8_t retVal = 0;

  if(m_hw.dipSwitch6->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_hw.dipSwitch5->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_hw.dipSwitch4->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_hw.dipSwitch3->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_hw.dipSwitch2->IsDown())
  {
    retVal |= 1;
  }
  retVal = retVal << 1;
  if(m_hw.dipSwitch1->IsDown())
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
}

void ArcJoy::joyButtonsInit(void)
{
  m_hw.joyButtonIrq->SetHandler(this);
}

void ArcJoy::joyGpioDisable(void)
{
  m_hw.joySwitch[LEFT_SWITCH]->Disable();
  m_hw.joySwitch[RIGHT_SWITCH]->Disable();
  m_hw.joySwitch[UP_SWITCH]->Disable();
  m_hw.joySwitch[DOWN_SWITCH]->Disable();
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
  m_hw.esbPtx->SendFrame(m_joyNumber,data,dataLength);
}

void ArcJoy::RtcAlarmHandler()
{
  m_heartbeatFlag = true;
  m_hw.rtcClock->SetupAlarmInSeconds(HEARTBEAT_PERIOD);
}

void ArcJoy::DelayedEventHandler()
{
  m_hw.blueLed->Down();
  m_hw.redLed->Down();
}

