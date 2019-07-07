#include "joyApp.h"
#include "stdint.h"
#include "nrf.h"
#include "nrf_esb.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrfx_gpiote.h"

static void pinToggle(uint32_t pinNumber);
static void allPinInit(void);

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

#define JOY_BUTTON_1 9
#define JOY_BUTTON_2 6
#define JOY_BUTTON_3 7
#define JOY_BUTTON_4 5
#define JOY_BUTTON_5 2
#define JOY_BUTTON_6 4

#define NONE  0
#define LEFT  1
#define RIGHT 2
#define UP    4
#define DOWN  8

#define FRAME_HEARTBEAT 0
#define FRAME_JOYSTATE  1

#define MAX_RADIO_FAILS 5

static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x01, 0x00);
static nrf_esb_payload_t rx_payload;
static const nrf_drv_timer_t heartbeatTimer = NRF_DRV_TIMER_INSTANCE(0);

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
#define COMPARE_COUNTERTIME  (4UL)  

static volatile bool radioSuccess = false;
static volatile bool radioDone = false;

static volatile bool sendHeartbeat = false;
static volatile bool sendJoyState = false;


static void joyEvent(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

void JOYAPP_Run(void)
{
  uint8_t joyState = NONE;
  uint8_t dipSwitchState = 0;
  uint8_t joyButtonsState = 0;
  uint32_t fails = 0;

  allPinInit();
  while(true)
  {
    pinToggle(LED_R);
    pinToggle(LED_B);
    pinToggle(DIPSWITCH_1);
    pinToggle(DIPSWITCH_2);
    pinToggle(DIPSWITCH_3);
    pinToggle(DIPSWITCH_4);
    pinToggle(DIPSWITCH_5);
    pinToggle(DIPSWITCH_6);
    pinToggle(JOY_LEFT);
    pinToggle(JOY_RIGHT);
    pinToggle(JOY_UP);
    pinToggle(JOY_DOWN);
    pinToggle(JOY_BUTTON_1);
    pinToggle(JOY_BUTTON_2);
    pinToggle(JOY_BUTTON_3);
    pinToggle(JOY_BUTTON_4);
    pinToggle(JOY_BUTTON_5);
    pinToggle(JOY_BUTTON_6);
  }
}

static void pinToggle(uint32_t pinNumber)
{
  nrf_gpio_pin_write(pinNumber,1);
  nrf_delay_ms(5);
  nrf_gpio_pin_write(pinNumber,0);
}

static void allPinInit(void)
{
    nrf_gpio_cfg_output(LED_R);
    nrf_gpio_cfg_output(LED_B);
    nrf_gpio_cfg_output(DIPSWITCH_1);
    nrf_gpio_cfg_output(DIPSWITCH_2);
    nrf_gpio_cfg_output(DIPSWITCH_3);
    nrf_gpio_cfg_output(DIPSWITCH_4);
    nrf_gpio_cfg_output(DIPSWITCH_5);
    nrf_gpio_cfg_output(DIPSWITCH_6);
    nrf_gpio_cfg_output(JOY_LEFT);
    nrf_gpio_cfg_output(JOY_RIGHT);
    nrf_gpio_cfg_output(JOY_UP);
    nrf_gpio_cfg_output(JOY_DOWN);
    nrf_gpio_cfg_output(JOY_BUTTON_1);
    nrf_gpio_cfg_output(JOY_BUTTON_2);
    nrf_gpio_cfg_output(JOY_BUTTON_3);
    nrf_gpio_cfg_output(JOY_BUTTON_4);
    nrf_gpio_cfg_output(JOY_BUTTON_5);
    nrf_gpio_cfg_output(JOY_BUTTON_6);
}
