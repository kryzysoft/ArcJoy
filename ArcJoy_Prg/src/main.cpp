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
#include "nRF_Hal/NrfGpio.h"


#define LED_R 15
#define LED_B 14

#define DIPSWITCH_1 27
#define DIPSWITCH_2 28
#define DIPSWITCH_3 29
#define DIPSWITCH_4 30
#define DIPSWITCH_5  0
#define DIPSWITCH_6  1

int main(void)
{
  NrfGpio redLed(LED_R, GPIO_OUTPUT, GPIO_PULL_NONE);
  NrfGpio blueLed(LED_R, GPIO_OUTPUT, GPIO_PULL_NONE);

  NrfGpio dipSwitch1(DIPSWITCH_1, GPIO_INPUT, GPIO_PULL_UP);
  NrfGpio dipSwitch2(DIPSWITCH_2, GPIO_INPUT, GPIO_PULL_UP);
  NrfGpio dipSwitch3(DIPSWITCH_3, GPIO_INPUT, GPIO_PULL_UP);
  NrfGpio dipSwitch4(DIPSWITCH_4, GPIO_INPUT, GPIO_PULL_UP);
  NrfGpio dipSwitch5(DIPSWITCH_5, GPIO_INPUT, GPIO_PULL_UP);
  NrfGpio dipSwitch6(DIPSWITCH_6, GPIO_INPUT, GPIO_PULL_UP);

  ArcJoy arcJoy(&redLed, &blueLed, &dipSwitch1, &dipSwitch2, &dipSwitch3, &dipSwitch4, &dipSwitch5, &dipSwitch6);

  arcJoy.Run();

}
