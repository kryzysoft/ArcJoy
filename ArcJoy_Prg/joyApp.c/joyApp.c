#include "joyApp.h"
#include "stdint.h"
#include "nrf.h"
#include "nrf_esb.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"

static void clocksStart(void);
static void ledInit(void);
static void dipSwitchInit(void);
static void dipSwitchDisable(void);
static uint8_t dipSwitchReadState(void);
static void joyInit(void);
static void joyDisable(void);
static uint8_t joyReadState(void);
static void joyButtonsInit(void);
static void joyButtonsDisable(void);
static uint8_t joyButtonsReadState(void);
static void radioInit(void);
static void radioSendState(uint8_t joyButtons, uint8_t joystick);
static void nrfEsbEventHandler(nrf_esb_evt_t const * p_event);
static void systemOff(void);
static void timerHeartbeatEventHandler(nrf_timer_event_t event_type, void* p_context);
static void timerInit(void);

#define LED_R 15
#define LED_B 14

#define DIPSWITCH_1 27
#define DIPSWITCH_2 28
#define DIPSWITCH_3 29
#define DIPSWITCH_4 30
#define DIPSWITCH_5  0
#define DIPSWITCH_6  1

#define JOY_LEFT     3
#define JOY_RIGHT   22
#define JOY_UP       8
#define JOY_DOWN    26

#define JOY_BUTTON_1 4
#define JOY_BUTTON_2 2
#define JOY_BUTTON_3 5
#define JOY_BUTTON_4 7
#define JOY_BUTTON_5 6
#define JOY_BUTTON_6 9

#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8

#define FRAME_HEARTBEAT 0
#define FRAME_JOYSTATE  1

static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x01, 0x00);
static nrf_esb_payload_t rx_payload;
static const nrf_drv_timer_t heartbeatTimer = NRF_DRV_TIMER_INSTANCE(0);


void JOYAPP_Run(void)
{
  uint8_t joyState = NONE;
  uint8_t dipSwitchState = 0;
  uint8_t joyButtonsState = 0;

  clocksStart();
  ledInit();
  dipSwitchInit();
  joyInit();
  joyButtonsInit();
  radioInit();
  timerInit();

  bool led = false;

  while(true)
  {
      __WFE();
      __WFE();
      nrf_delay_ms(1);
          if(led)
          {
            nrf_gpio_pin_write(LED_B,1);
            nrf_gpio_pin_write(LED_R,0);
          }
          else
          {
            nrf_gpio_pin_write(LED_B,0);
            nrf_gpio_pin_write(LED_R,1);
          }
          led = !led;


//    nrf_gpio_pin_write(LED_R,0);
//    nrf_gpio_pin_write(LED_B,0);
//    dipSwitchState = dipSwitchReadState();
//    joyState = joyReadState();
//    joyButtonsState = joyButtonsReadState();
//    radioSendState(joyButtonsState,joyState);
//    nrf_delay_ms(100);
//    nrf_gpio_pin_write(LED_R,0);
//    nrf_gpio_pin_write(LED_B,0);
//    nrf_delay_ms(300);
  }
}

static void timerInit(void)
{
    uint32_t time_ms = 5000;
    nrf_drv_timer_config_t timerConfig = NRF_DRV_TIMER_DEFAULT_CONFIG;
    nrf_drv_timer_init(&heartbeatTimer, &timerConfig, timerHeartbeatEventHandler);

    uint32_t time_ticks = nrf_drv_timer_ms_to_ticks(&heartbeatTimer, time_ms);
    nrf_drv_timer_extended_compare(
         &heartbeatTimer, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    nrf_drv_timer_enable(&heartbeatTimer);

}

static void clocksStart(void)
{
    // Start HFCLK and wait for it to start.
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

static void ledInit(void)
{
    nrf_gpio_cfg_output(LED_R);
    nrf_gpio_cfg_output(LED_B);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
}

static void dipSwitchInit(void)
{
    nrf_gpio_cfg_input(DIPSWITCH_1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(DIPSWITCH_2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(DIPSWITCH_3, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(DIPSWITCH_4, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(DIPSWITCH_5, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(DIPSWITCH_6, NRF_GPIO_PIN_PULLUP);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
}

static void dipSwitchDisable(void)
{
    nrf_gpio_cfg_default(DIPSWITCH_1);
    nrf_gpio_cfg_default(DIPSWITCH_2);
    nrf_gpio_cfg_default(DIPSWITCH_3);
    nrf_gpio_cfg_default(DIPSWITCH_4);
    nrf_gpio_cfg_default(DIPSWITCH_5);
    nrf_gpio_cfg_default(DIPSWITCH_6);
}

static uint8_t dipSwitchReadState(void)
{
  uint8_t retVal = 0;
  retVal |= nrf_gpio_pin_read(DIPSWITCH_6);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(DIPSWITCH_5);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(DIPSWITCH_4);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(DIPSWITCH_3);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(DIPSWITCH_2);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(DIPSWITCH_1);
  
  retVal ^= 0x3F;

  return retVal;
}

static void joyInit(void)
{
    nrf_gpio_cfg_input(JOY_LEFT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_RIGHT, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_UP, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(JOY_DOWN, NRF_GPIO_PIN_NOPULL);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
}

static void joyDisable(void)
{
    nrf_gpio_cfg_default(JOY_LEFT);
    nrf_gpio_cfg_default(JOY_RIGHT);
    nrf_gpio_cfg_default(JOY_UP);
    nrf_gpio_cfg_default(JOY_DOWN);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
}

static uint8_t joyReadState(void)
{
  uint8_t joyState = 0x0F;

  if(nrf_gpio_pin_read(JOY_LEFT)) joyState &= (~LEFT);
  if(nrf_gpio_pin_read(JOY_RIGHT)) joyState &= (~RIGHT);
  if(nrf_gpio_pin_read(JOY_UP)) joyState &= (~UP);
  if(nrf_gpio_pin_read(JOY_DOWN)) joyState &= (~DOWN);

  return joyState ;
} 

static void joyButtonsInit(void)
{
    nrf_gpio_cfg_sense_input(JOY_BUTTON_1, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(JOY_BUTTON_2, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(JOY_BUTTON_3, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(JOY_BUTTON_4, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(JOY_BUTTON_5, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(JOY_BUTTON_6, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
}

static void joyButtonsDisable(void)
{
    nrf_gpio_cfg_default(JOY_BUTTON_2);
    nrf_gpio_cfg_default(JOY_BUTTON_3);
    nrf_gpio_cfg_default(JOY_BUTTON_4);
    nrf_gpio_cfg_default(JOY_BUTTON_5);
    nrf_gpio_cfg_default(JOY_BUTTON_6);
    // Workaround for PAN_028 rev1.1 anomaly 22 - System: Issues with disable System OFF mechanism
    nrf_delay_ms(1);
}

static uint8_t joyButtonsReadState(void)
{
  uint8_t retVal = 0;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_1);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_2);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_3);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_4);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_5);
  retVal = retVal << 1;
  retVal |= nrf_gpio_pin_read(JOY_BUTTON_6);

  retVal ^= 0x3F;
  
  return retVal;
} 

static void radioInit(void)
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

    nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
    nrf_esb_config.retransmit_count         = 6;
    nrf_esb_config.selective_auto_ack       = false;
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
    nrf_esb_config.event_handler            = nrfEsbEventHandler;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;

    nrf_esb_init(&nrf_esb_config);
    nrf_esb_set_base_address_0(base_addr_0);
    nrf_esb_set_base_address_1(base_addr_1);
    nrf_esb_set_prefixes(addr_prefix, 8);

    tx_payload.length  = 2;
    tx_payload.pipe    = 0;
    tx_payload.data[0] = 0x00;
}

static void radioSendState(uint8_t joyButtons, uint8_t joystick)
{
    tx_payload.length  = 3;
    tx_payload.pipe    = 0;
    tx_payload.data[0] = FRAME_JOYSTATE;
    tx_payload.data[1] = joystick;
    tx_payload.data[2] = joyButtons;
    tx_payload.noack = false;

    nrf_esb_write_payload(&tx_payload);
}

static void nrfEsbEventHandler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            nrf_gpio_pin_write(LED_B,1);
            nrf_gpio_pin_write(LED_R,0);
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            nrf_gpio_pin_write(LED_B,0);
            nrf_gpio_pin_write(LED_R,1);
            nrf_esb_flush_tx();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            while (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS);
            break;
    }
}

//static bool led = false;

void timerHeartbeatEventHandler(nrf_timer_event_t event_type, void* p_context)
{

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
          break;
        default:
            break;
    }
}

static void systemOff( void )
{
    NRF_POWER->SYSTEMOFF = 0x1;
    (void) NRF_POWER->SYSTEMOFF;
    while (true);
}
