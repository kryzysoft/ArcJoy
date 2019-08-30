/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "ArcJoy.h"
#include "nRF_Hal/NrfGpioInput.h"
#include "nRF_Hal/NrfGpioIrq.h"
#include "nRF_Hal/NrfGpioOutput.h"
#include "nRF_Hal/NrfGpioWakeUp.h"
#include "nRF_Hal/NrfEsbRadioPtx.h"
#include "nRF_Hal/NrfRtc.h"
#include "nRF_Hal/NrfSystemOffMode.h"
#include "nRF_Hal/NrfSleepMode.h"
#include "nRF_Hal/NrfDelay.h"
#include "nRF_Hal/NrfTimerDelayedEvent.h"

#include "DebugLog.h"

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

#define JOY_BUTTON 9

static NrfGpioInput dipSwitch1(DIPSWITCH_1, GPIO_PULL_UP);
static NrfGpioOutput redLed(LED_R);
static NrfGpioOutput blueLed(LED_B);

static NrfGpioInput dipSwitch2(DIPSWITCH_2, GPIO_PULL_UP);
static NrfGpioInput dipSwitch3(DIPSWITCH_3, GPIO_PULL_UP);
static NrfGpioInput dipSwitch4(DIPSWITCH_4, GPIO_PULL_UP);
static NrfGpioInput dipSwitch5(DIPSWITCH_5, GPIO_PULL_UP);
static NrfGpioInput dipSwitch6(DIPSWITCH_6, GPIO_PULL_UP);
  
static NrfGpioInput joyLeft(JOY_LEFT, GPIO_PULL_NONE);
static NrfGpioInput joyRight(JOY_RIGHT, GPIO_PULL_NONE);
static NrfGpioInput joyUp(JOY_UP, GPIO_PULL_NONE);
static NrfGpioInput joyDown(JOY_DOWN, GPIO_PULL_NONE);

static NrfGpioInput joyButton(JOY_BUTTON, GPIO_PULL_NONE);

static NrfGpioIrq joyLeftIrq(JOY_LEFT);
static NrfGpioIrq joyRightIrq(JOY_RIGHT);
static NrfGpioIrq joyUpIrq(JOY_UP);
static NrfGpioIrq joyDownIrq(JOY_DOWN);
static NrfGpioIrq joyButtonIrq(JOY_BUTTON);

static NrfGpioWakeUp joyButtonWakeUp(JOY_BUTTON);

static NrfEsbRadioPtx ptxRadio(true);

static NrfRtc rtcClock;
static NrfSystemOffMode systemOffMode;
static NrfSleepMode sleepMode;
static NrfDelay delay;
static NrfTimerDelayedEvent ledOffEvent;

int main(void)
{
  DebugInit();
  DebugInfo("Hardware initialization");

  ArcJoyHardwareConfig hwConfig;

  hwConfig.dipSwitch1 = &dipSwitch1;
  hwConfig.dipSwitch2 = &dipSwitch2;
  hwConfig.dipSwitch3 = &dipSwitch3;
  hwConfig.dipSwitch4 = &dipSwitch4;
  hwConfig.dipSwitch5 = &dipSwitch5;
  hwConfig.dipSwitch6 = &dipSwitch6;

  hwConfig.redLed = &redLed;
  hwConfig.blueLed = &blueLed;

  hwConfig.joySwitch[LEFT_SWITCH] = &joyLeft;
  hwConfig.joySwitch[RIGHT_SWITCH] = &joyRight;
  hwConfig.joySwitch[UP_SWITCH] = &joyUp;
  hwConfig.joySwitch[DOWN_SWITCH] = &joyDown;

  hwConfig.joySwitch[BUTTON_SWITCH] = &joyButton;

  hwConfig.joyLeftIrq = &joyLeftIrq;
  hwConfig.joyRightIrq = &joyRightIrq;
  hwConfig.joyUpIrq = &joyUpIrq;
  hwConfig.joyDownIrq = &joyDownIrq;

  hwConfig.joyButtonIrq = &joyButtonIrq;
  hwConfig.joyButtonWakeUp = &joyButtonWakeUp;

  hwConfig.esbPtx = &ptxRadio;

  hwConfig.rtcClock = &rtcClock;

  hwConfig.offMode = &systemOffMode;
  hwConfig.sleepMode = &sleepMode;
  hwConfig.delay = &delay;

  hwConfig.ledOffEvent = &ledOffEvent;

  ArcJoy arcJoy(hwConfig);

  DebugInfo("Application run");

  arcJoy.Run();

}

void assert_nrf_callback(uint16_t line_num, const uint8_t * file_name)
{
  DebugErr("Assert failed in file\r\n%s\r\nat line %d\r\n", file_name, line_num);
}
