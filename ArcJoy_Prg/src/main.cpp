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
#include "nRF_Hal/NrfGpioInputIrq.h"
#include "nRF_Hal/NrfGpioOutput.h"
#include "nRF_Hal/NrfEsbRadioPtx.h"
#include "nRF_Hal/NrfRtc.h"



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

int main(void)
{
  NrfGpioOutput redLed(LED_R);
  NrfGpioOutput blueLed(LED_B);

  NrfGpioInput dipSwitch1(DIPSWITCH_1, GPIO_PULL_UP);
  NrfGpioInput dipSwitch2(DIPSWITCH_2, GPIO_PULL_UP);
  NrfGpioInput dipSwitch3(DIPSWITCH_3, GPIO_PULL_UP);
  NrfGpioInput dipSwitch4(DIPSWITCH_4, GPIO_PULL_UP);
  NrfGpioInput dipSwitch5(DIPSWITCH_5, GPIO_PULL_UP);
  NrfGpioInput dipSwitch6(DIPSWITCH_6, GPIO_PULL_UP);
  
  NrfGpioInputIrq joyLeft(JOY_LEFT, GPIO_PULL_NONE);
  NrfGpioInputIrq joyRight(JOY_RIGHT, GPIO_PULL_NONE);
  NrfGpioInputIrq joyUp(JOY_UP, GPIO_PULL_NONE);
  NrfGpioInputIrq joyDown(JOY_DOWN, GPIO_PULL_NONE);

  NrfGpioInputIrq joyButton1(JOY_BUTTON_1, GPIO_PULL_NONE);
  NrfGpioInputIrq joyButton2(JOY_BUTTON_2, GPIO_PULL_NONE);
  NrfGpioInputIrq joyButton3(JOY_BUTTON_3, GPIO_PULL_NONE);
  NrfGpioInputIrq joyButton4(JOY_BUTTON_4, GPIO_PULL_NONE);
  NrfGpioInputIrq joyButton5(JOY_BUTTON_5, GPIO_PULL_NONE);
  NrfGpioInputIrq joyButton6(JOY_BUTTON_6, GPIO_PULL_NONE);

  NrfEsbRadioPtx ptxRadio;

  NrfRtc rtcClock;

  ArcJoyHardwareConfig hwConfig;

  hwConfig.dipSwitch1 = &dipSwitch1;
  hwConfig.dipSwitch2 = &dipSwitch2;
  hwConfig.dipSwitch3 = &dipSwitch3;
  hwConfig.dipSwitch4 = &dipSwitch4;
  hwConfig.dipSwitch5 = &dipSwitch5;
  hwConfig.dipSwitch6 = &dipSwitch6;

  hwConfig.redLed = &redLed;
  hwConfig.blueLed = &blueLed;

  hwConfig.joyLeft = &joyLeft;
  hwConfig.joyRight = &joyRight;
  hwConfig.joyUp = &joyUp;
  hwConfig.joyDown = &joyDown;

  hwConfig.joyButton1 = &joyButton1;
  hwConfig.joyButton2 = &joyButton2;
  hwConfig.joyButton3 = &joyButton3;
  hwConfig.joyButton4 = &joyButton4;
  hwConfig.joyButton5 = &joyButton5;
  hwConfig.joyButton6 = &joyButton6;

  hwConfig.esbPtx = &ptxRadio;

  hwConfig.rtcClock = &rtcClock;

  ArcJoy arcJoy(&hwConfig);

  arcJoy.Run();

}
