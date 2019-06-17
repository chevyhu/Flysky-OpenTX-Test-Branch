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

#include "opentx.h"
#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif
#include "usb_dcd_int.h"
#include "usb_bsp.h"
#if defined(__cplusplus) && !defined(SIMU)
}
#endif

void watchdogInit(unsigned int duration)
{
  IWDG->KR = 0x5555;      // Unlock registers
  IWDG->PR = 3;           // Divide by 32 => 1kHz clock
  IWDG->KR = 0x5555;      // Unlock registers
  IWDG->RLR = duration;   // 1.5 seconds nominal
  IWDG->KR = 0xAAAA;      // reload
  IWDG->KR = 0xCCCC;      // start
}

// Start TIMER7 at 2000000Hz
void init2MhzTimer()
{
  TIMER_2MHz_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1; // 0.5 uS, 2 MHz
  TIMER_2MHz_TIMER->ARR = 65535;
  TIMER_2MHz_TIMER->CR2 = 0;
  TIMER_2MHz_TIMER->CR1 = TIM_CR1_CEN;
}

// Starts TIMER at 1000Hz
void init1msTimer()
{
  INTERRUPT_xMS_TIMER->ARR = 999; // 1mS in uS
  INTERRUPT_xMS_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1;  // 1uS
  INTERRUPT_xMS_TIMER->CCER = 0;
  INTERRUPT_xMS_TIMER->CCMR1 = 0;
  INTERRUPT_xMS_TIMER->EGR = 0;
  INTERRUPT_xMS_TIMER->CR1 = 5;
  INTERRUPT_xMS_TIMER->DIER |= 1;
  NVIC_EnableIRQ(INTERRUPT_xMS_IRQn);
  NVIC_SetPriority(INTERRUPT_xMS_IRQn, 7);
}

// TODO use the same than board_sky9x.cpp
void interrupt1ms()
{
  static uint8_t pre_scale;       // Used to get 10 Hz counter

  ++pre_scale;

#if defined(DEBUG) && !defined(SIMU)
  debugCounter1ms++;
#endif

#if defined(HAPTIC)
  if (pre_scale == 5 || pre_scale == 10) {
    DEBUG_TIMER_START(debugTimerHaptic);
    HAPTIC_HEARTBEAT();
    DEBUG_TIMER_STOP(debugTimerHaptic);
  }
#endif
    if (boardState == BOARD_STARTED)// && 0 == pre_scale%3)
    {
  #if defined (FLYSKY_HALL_STICKS)
            hall_stick_loop();
  #endif
    }
  if (pre_scale == 10) {
    pre_scale = 0;
#if !defined(SIMU)
    if (boardState == BOARD_STARTED) {
        TouchDriver();
    }
#endif
    DEBUG_TIMER_START(debugTimerPer10ms);
    DEBUG_TIMER_SAMPLE(debugTimerPer10msPeriod);
    per10ms();
    DEBUG_TIMER_STOP(debugTimerPer10ms);
  }

  DEBUG_TIMER_START(debugTimerRotEnc);
  DEBUG_TIMER_STOP(debugTimerRotEnc);
}

extern "C" void INTERRUPT_xMS_IRQHandler()
{
  INTERRUPT_xMS_TIMER->SR &= ~TIM_SR_UIF;
  interrupt1ms();
  DEBUG_INTERRUPT(INT_1MS);
}

#if defined(SEMIHOSTING)
extern "C" void initialise_monitor_handles();
#endif


void delay_self(int count)
{
   for (int i = 50000; i > 0; i--)
   {
       for (; count > 0; count--);
   }
}

void boardInit()
{
#if defined(SEMIHOSTING)
  initialise_monitor_handles();
#endif

#if !defined(SIMU)
  RCC_AHB1PeriphClockCmd(PWR_RCC_AHB1Periph |
                         LCD_RCC_AHB1Periph |
                         BACKLIGHT_RCC_AHB1Periph |
                         SD_RCC_AHB1Periph |
                         SDRAM_RCC_AHB1Periph |
                         AUDIO_RCC_AHB1Periph |
                         MONITOR_RCC_AHB1Periph |
                         KEYS_RCC_AHB1Periph |
                         ADC_RCC_AHB1Periph |
                         AUX_SERIAL_RCC_AHB1Periph |
                         TELEMETRY_RCC_AHB1Periph |
                         TRAINER_RCC_AHB1Periph |
                         AUDIO_RCC_AHB1Periph |
                         HAPTIC_RCC_AHB1Periph |
                         INTMODULE_RCC_AHB1Periph |
                         INTMODULE_RCC_AHB1Periph|
                         EXTMODULE_RCC_AHB1Periph,
                         ENABLE);

  RCC_AHB3PeriphClockCmd(SDRAM_RCC_AHB3Periph,
                         ENABLE);

  RCC_APB1PeriphClockCmd(INTERRUPT_xMS_RCC_APB1Periph |
                         TIMER_2MHz_RCC_APB1Periph |
                         TELEMETRY_RCC_APB1Periph |
                         TRAINER_RCC_APB1Periph |
                         INTMODULE_RCC_APB1Periph |
                         HALL_RCC_APB1Periph |
                         EXTMODULE_RCC_APB1Periph |
                         INTMODULE_RCC_APB1Periph_TIM3 |
                         BACKLIGHT_RCC_APB1Periph,
                         ENABLE);

  RCC_APB2PeriphClockCmd(LCD_RCC_APB2Periph |
                         ADC_RCC_APB2Periph |
                         HAPTIC_RCC_APB2Periph |
                         AUX_SERIAL_RCC_APB2Periph |
                         //INTMODULE_RCC_APB2Periph |
                         AUDIO_RCC_APB2Periph |
                         EXTMODULE_RCC_APB2Periph,
                         ENABLE);

  __enable_irq();

#if defined(DEBUG)
   auxSerialInit(0, 0); // default serial mode (None if DEBUG nm  ot defined)
#endif
  TRACE("\nNV14 board started :)");
  delay_ms(10);
  TRACE("RCC->CSR = %08x", RCC->CSR);

  pwrInit();
  battery_charge_init();
  init2MhzTimer();
  init1msTimer();
  uint32_t pwr_press_time = 0;
  if(UNEXPECTED_SHUTDOWN()) pwrOn();
  backlightInit();
  lcdInit();
  while (boardState == BOARD_POWER_OFF)
  {
    if (pwrPressed())
    {
      if (pwr_press_time == 0)
      {
         pwr_press_time = get_tmr10ms();
      }
      if ((get_tmr10ms() - pwr_press_time) > POWER_ON_DELAY)
      {
          pwrOn();
      }
    }
    else
    {
       pwr_press_time = 0;
       handle_battery_charge();
    }   
  }

  keysInit();
  audioInit();
  // we need to initialize g_FATFS_Obj here, because it is in .ram section (because of DMA access)
  // and this section is un-initialized
  memset(&g_FATFS_Obj, 0, sizeof(g_FATFS_Obj));
  monitorInit();
  adcInit();
  backlightInit();
  lcdInit();
#if defined(FLYSKY_HALL_STICKS)
  hall_stick_init(FLYSKY_HALL_BAUDRATE);
#endif
  usbInit();
  hapticInit();
  TouchInit();
  boardState = BOARD_STARTED;
#if defined(DEBUG)
  DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP|DBGMCU_TIM1_STOP|DBGMCU_TIM2_STOP|DBGMCU_TIM3_STOP|DBGMCU_TIM4_STOP|DBGMCU_TIM5_STOP|DBGMCU_TIM6_STOP|DBGMCU_TIM7_STOP|DBGMCU_TIM8_STOP|DBGMCU_TIM9_STOP|DBGMCU_TIM10_STOP|DBGMCU_TIM11_STOP|DBGMCU_TIM12_STOP|DBGMCU_TIM13_STOP|DBGMCU_TIM14_STOP, ENABLE);
#endif
#endif
}


void boardOff()
{
  BACKLIGHT_DISABLE();

  while (pwrPressed()) {
    wdt_reset();
  }
  lcd->drawFilledRect(0, 0, LCD_WIDTH, LCD_HEIGHT, SOLID, HEADER_BGCOLOR);
  SysTick->CTRL = 0; // turn off systick
  pwrOff();
#if defined(PCBFLYSKY) && !defined (SIMU)
  haptic.event( AU_ERROR );
  delay_ms(50);
  while(1)
  {
    NVIC_SystemReset();
  }
#endif
}

uint8_t currentTrainerMode = 0xff;

void checkTrainerSettings()
{
  uint8_t requiredTrainerMode = g_model.trainerMode;
  if (requiredTrainerMode != currentTrainerMode) {
    switch (currentTrainerMode) {
      case TRAINER_MODE_MASTER_TRAINER_JACK:
        stop_trainer_capture();
        break;
      case TRAINER_MODE_SLAVE:
        stop_trainer_ppm();
        break;
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        auxSerialStop();
    }

    currentTrainerMode = requiredTrainerMode;
    switch (requiredTrainerMode) {
      case TRAINER_MODE_SLAVE:
        init_trainer_ppm();
        break;
      case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
        if (g_eeGeneral.auxSerialMode == UART_MODE_SBUS_TRAINER) {
          auxSerialSbusInit();
          break;
        }
        // no break
      default:
        // master is default
        init_trainer_capture();
        break;
    }
  }
}
