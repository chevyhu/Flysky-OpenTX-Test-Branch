/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "board.h"
#include "pwr.h"


uint32_t powerupReason
    __NOINIT;  // Stores power up reason beyond initialization for emergency
               // mode activation
uint32_t boardState __NOINIT;

void pwrInit() {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PWR_ON_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);

  // TODO move this elsewhere!
  // Init Module PWR
  // GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN);
  // GPIO_InitStructure.GPIO_Pin = INTMODULE_PWR_GPIO_PIN;
  // GPIO_Init(INTMODULE_PWR_GPIO, &GPIO_InitStructure);

  // TODO move this elsewhere!
  GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PWR_GPIO_PIN;
  GPIO_Init(EXTMODULE_PWR_GPIO, &GPIO_InitStructure);

  // Bluetooth
  GPIO_SetBits(BLUETOOTH_ON_GPIO, BLUETOOTH_ON_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = BLUETOOTH_ON_GPIO_PIN;
  GPIO_Init(BLUETOOTH_ON_GPIO, &GPIO_InitStructure);

  // Init PWR SWITCH PIN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pin = PWR_SWITCH_GPIO_PIN;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);

  // Init SD-DETECT PIN
  GPIO_ResetBits(SD_PRESENT_GPIO, SD_PRESENT_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = SD_PRESENT_GPIO_PIN;
  GPIO_Init(SD_PRESENT_GPIO, &GPIO_InitStructure);

  // TODO move this elsewhere! (plus SD DETECT on other boards)
  // Init TRAINER DETECT PIN
  // GPIO_InitStructure.GPIO_Pin = TRAINER_DETECT_GPIO_PIN;
  // GPIO_Init(TRAINER_DETECT_GPIO, &GPIO_InitStructure);

  boardState = BOARD_POWER_OFF;
}

void pwrOn() {
  GPIO_SetBits(PWR_GPIO, PWR_ON_GPIO_PIN);
  boardState = BOARD_POWER_ON;
}

void pwrSoftReboot() {
  boardState = BOARD_REBOOT;
  NVIC_SystemReset();
}

void pwrOff() {
  // Shutdown the Haptic
  hapticDone();
  boardState = BOARD_POWER_OFF;
  GPIO_ResetBits(PWR_GPIO, PWR_ON_GPIO_PIN);
}

uint32_t pwrPressed() {
  return GPIO_ReadInputDataBit(PWR_GPIO, PWR_SWITCH_GPIO_PIN) == Bit_RESET;
}

void pwrResetHandler() {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;

  // these two NOPs are needed (see STM32F errata sheet) before the peripheral
  // register can be written after the peripheral clock was enabled
  __ASM volatile("nop");
  __ASM volatile("nop");

  if (boardState != BOARD_POWER_OFF) {
    powerupReason =
        boardState != BOARD_REBOOT && WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()
            ? DIRTY_SHUTDOWN
            : ~DIRTY_SHUTDOWN;
    RCC->CSR |= RCC_CSR_RMVF;  // clear all flags
  } else {
    powerupReason = ~DIRTY_SHUTDOWN;
  }
}