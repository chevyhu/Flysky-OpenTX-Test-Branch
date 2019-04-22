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

#define  __BATTERY_DRIVER_C__

#define BATTERY_W 70
#define BATTERY_H 160
#define BATTERY_CONNECTOR_W 16
#define BATTERY_CONNECTOR_H 4
#define BATTERY_BORDER 2
#define BATTERY_W_INNER (BATTERY_W - 2*BATTERY_BORDER)
#define BATTERY_H_INNER (BATTERY_H - 2*BATTERY_BORDER)

void battery_charge_init()
{
	srand (1);  // for charge animation
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PWR_CHARGE_FINISHED_GPIO_PIN | PWR_CHARGING_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_CHARGING_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(PWR_CHARGING_GPIO, PWR_CHARGE_FINISHED_GPIO_PIN | PWR_CHARGING_GPIO_PIN);
}

uint16_t get_battery_charge_state()
{
#if !defined(SIMU)
   static unsigned int finishedTime = 0;
   static unsigned int chargingTime = 0;
   static unsigned int noneTime = 0;
   unsigned short DelayTime = 0;
   if( BOARD_POWER_OFF == boardState )
   {
       DelayTime = 150;
   }
   else
   {
       DelayTime = 3;
   }

  if (!READ_CHARGE_FINISHED_STATE())
  {
      finishedTime++;
      if(finishedTime > 1000)
          finishedTime = 1000;
      if(chargingTime)
          chargingTime--;
  }
  else if (!READ_CHARGING_STATE())
  {
      chargingTime++;
      if(chargingTime > 1000)
          chargingTime = 1000;
      if(finishedTime)
          finishedTime--;
  }
  else
  {
      noneTime++;
      //if(noneTime>2)
      {   if(finishedTime>200)
            finishedTime -= 50;
          else
            finishedTime = 0;
          if(chargingTime>200)
            chargingTime -= 50;
          else
            chargingTime = 0;
      }
  }
  if(finishedTime>DelayTime)
  {
     noneTime = 0;
     chargingTime = 0;
     return CHARGE_FINISHED;
  }
  if(chargingTime>DelayTime)
  {
      noneTime = 0;
     finishedTime = 0;
     return CHARGE_STARTED;
  }
#endif
  return CHARGE_NONE;
}

void drawChargingInfo(uint16_t chargeState){
	static int progress = 0;
	static int battery_top=(LCD_H - BATTERY_H)/2, battery_left=(LCD_W - BATTERY_W)/2;
	const char* text = chargeState == CHARGE_STARTED ? STR_BATTERYCHARGING : STR_BATTERYFULL;
    int h = 0;
    LcdFlags color = 0;
    if(CHARGE_STARTED == chargeState)
    {
        if(progress >= 100)
        {
            progress = 0;
            battery_top  = rand()%(LCD_H - BATTERY_H - BATTERY_CONNECTOR_H) + BATTERY_CONNECTOR_H;
            battery_left = rand()%(LCD_W - BATTERY_W);
        }
        else
        {
            progress+=25;
        }
        text = STR_BATTERYCHARGING;
        h = ((BATTERY_H_INNER * progress) / 100);
        color = ROUND|BATTERY_CHARGE_COLOR;
    }
    else if(CHARGE_FINISHED == chargeState)
    {
        text = STR_BATTERYFULL;
        h = BATTERY_H_INNER;
        color = ROUND|BATTERY_CHARGE_COLOR;
    }
    else
    {
        text = STR_BATTERYNONE;
        h = BATTERY_H_INNER;
        color = ROUND|TEXT_COLOR;
    }
  
  int text_top;
  if (battery_top < (LCD_H-BATTERY_H)/2)
  {
  		text_top = battery_top + BATTERY_H + 50;
  }
  else
  {
  	  text_top = battery_top - BATTERY_CONNECTOR_H - 50; 
  }
	lcd->drawSizedText(LCD_W/2, text_top , text, strlen(text), CENTERED|TEXT_BGCOLOR);

	lcd->drawFilledRect(battery_left, battery_top, BATTERY_W, BATTERY_H, SOLID, ROUND|TEXT_BGCOLOR);
	lcd->drawFilledRect(battery_left + BATTERY_BORDER, battery_top + BATTERY_BORDER, BATTERY_W_INNER, BATTERY_H_INNER, SOLID, ROUND|TEXT_COLOR);

  lcd->drawFilledRect(battery_left + BATTERY_BORDER, battery_top + BATTERY_BORDER + BATTERY_H_INNER - h , BATTERY_W_INNER, h, SOLID, color);
	lcd->drawFilledRect(battery_left + (BATTERY_W - BATTERY_CONNECTOR_W)/2, battery_top - BATTERY_CONNECTOR_H , BATTERY_CONNECTOR_W, BATTERY_CONNECTOR_H, SOLID, TEXT_BGCOLOR);
}

//this method should be called by timer interrupt or by GPIO interrupt
void handle_battery_charge()
{
#if !defined(SIMU)
  static uint16_t chargeState = CHARGE_NONE;
  static uint32_t updateTime = 0;
  if(boardState != BOARD_POWER_OFF) return;

  static uint32_t checkTime = 0;
  if(checkTime == 0 || ((get_tmr10ms() - checkTime) >= 1))
  {
    checkTime = get_tmr10ms();
    chargeState = get_battery_charge_state();
  }
  if(updateTime == 0 || ((get_tmr10ms() - updateTime) >= 50))
  {
      updateTime = get_tmr10ms();     
      lcdNextLayer();
      lcd->clear();
      drawChargingInfo(chargeState);
      lcdRefresh();
      BACKLIGHT_ENABLE();
   }
#endif
}

uint16_t getBatteryVoltage()
{
  int32_t instant_vbat = anaIn(TX_VOLTAGE);  // using filtered ADC value on purpose
  return (uint16_t)((instant_vbat * (1000 + g_eeGeneral.txVoltageCalibration)) / 2942);
}
