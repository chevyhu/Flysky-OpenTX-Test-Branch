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

#include "view_main.h"
#include "menu_model.h"
#include "menu_radio.h"
#include "menu_screens.h"
#include "model_select.h"
#include "view_channels.h"
#include "view_statistics.h"
#include "view_about.h"
#include "screen_theme.h"
#include "opentx.h"
#include "libwindows.h"
#include "alpha_button_on.lbm"
#include "alpha_button_off.lbm"

#define TRIM_WIDTH                     121
#define TRIM_LH_X                      10
#define TRIM_LV_X                      14
#define TRIM_RV_X                      (LCD_W-25)
#define TRIM_RH_X                      (LCD_W-TRIM_LH_X-TRIM_WIDTH)
#define TRIM_V_Y                       285
#define TRIM_H_Y                       (LCD_H-37)
#define TRIM_LEN                       80
#define POTS_LINE_Y                    (LCD_H-20)

Layout * customScreens[MAX_CUSTOM_SCREENS] = { 0, 0, 0, 0, 0 };
Topbar * topbar;

ViewMain::ViewMain():
  Window(&mainWindow, { 0, 0, LCD_W, LCD_H }),
  buttonHeight(NAV_BUTTONS_HEIGHT),
  buttonLeftModel(50 - buttonHeight/2),
  buttonLeftRadio(LCD_W / 2 - buttonHeight/2),
  buttonLeftTheme(LCD_W - 50 - buttonHeight/2)
{
  slideDirection = SlideDirection::None;
}

ViewMain::~ViewMain()
{
  deleteChildren();
}

void ViewMain::drawMainPots()
{
  // The  pots
  drawHorizontalSlider(TRIM_LH_X, POTS_LINE_Y, TRIM_WIDTH, calibratedAnalogs[CALIBRATED_POT1], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
#if defined(PCBHORUS)
  drawHorizontalSlider(LCD_W/2-20, POTS_LINE_Y, XPOTS_MULTIPOS_COUNT*5, 1 + (potsPos[1] & 0x0f), 1, XPOTS_MULTIPOS_COUNT + 1, XPOTS_MULTIPOS_COUNT, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_NUMBER_BUTTON);
  drawHorizontalSlider(TRIM_RH_X, POTS_LINE_Y, TRIM_WIDTH, calibratedAnalogs[CALIBRATED_POT3], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
#else
  drawHorizontalSlider(TRIM_RH_X, POTS_LINE_Y, TRIM_WIDTH, calibratedAnalogs[CALIBRATED_POT2], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
#endif

#if defined(PCBHORUS)
  // The 2 rear sliders
  drawVerticalSlider(6, TRIM_V_Y, 160, calibratedAnalogs[CALIBRATED_SLIDER_REAR_LEFT], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
  drawVerticalSlider(LCD_W-18, TRIM_V_Y, 160, calibratedAnalogs[CALIBRATED_SLIDER_REAR_RIGHT], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
#endif
}

void ViewMain::drawTrims(uint8_t flightMode)
{
  for (uint8_t i=0; i<4; i++) {
    static const coord_t x[4] = { TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X };
    static uint8_t vert[4] = {0, 1, 1, 0};
    unsigned int stickIndex = CONVERT_MODE(i);
    coord_t xm = x[stickIndex];
    int32_t trim = getTrimValue(flightMode, i);

    if (vert[i]) {
      if (g_model.extendedTrims == 1) {
        drawVerticalSlider(xm, TRIM_V_Y, 120, trim, TRIM_EXTENDED_MIN, TRIM_EXTENDED_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      else {
        drawVerticalSlider(xm, TRIM_V_Y, 120, trim, TRIM_MIN, TRIM_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          uint16_t y = TRIM_V_Y + TRIM_LEN + (trim<0 ? -TRIM_LEN/2 : TRIM_LEN/2);
          lcdDrawNumber(xm+2, y, trim, TINSIZE | CENTERED | VERTICAL);
        }
      }
    }
    else {
      if (g_model.extendedTrims == 1) {
        drawHorizontalSlider(xm, TRIM_H_Y, 120, trim, TRIM_EXTENDED_MIN, TRIM_EXTENDED_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      else {
        drawHorizontalSlider(xm, TRIM_H_Y, 120, trim, TRIM_MIN, TRIM_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          uint16_t x = xm + TRIM_LEN + (trim>0 ? -TRIM_LEN/2 : TRIM_LEN/2);
          lcdDrawNumber(x, TRIM_H_Y+2, trim, TINSIZE | CENTERED);
        }
      }
    }
  }
}

void ViewMain::drawButton(BitmapBuffer * dc, coord_t x, uint8_t icon) {
  coord_t y = isTopBarVisible() ? NAV_BUTTONS_MARGIN_TOP : NAV_BUTTONS_MARGIN;
  dc->drawBitmap(x, y, &ALPHA_BUTTON_OFF);
  const BitmapBuffer * mask = theme->getIconMask(icon);
  if(mask) dc->drawMask(x+((buttonHeight-mask->getWidth())/2), y+((buttonHeight-mask->getHeight())/2), mask, TEXT_BGCOLOR);
}

void ViewMain::drawFlightMode(coord_t y) {
  char* name = g_model.flightModeData[mixerCurrentFlightMode].name;
  coord_t textW = getTextWidth(name,  sizeof(name), ZCHAR | SMLSIZE);
  lcdDrawSizedText((LCD_W - textW) / 2,  y,  name,  sizeof(name), ZCHAR | SMLSIZE);
}

bool ViewMain::onTouchStart(coord_t x, coord_t y)
{
	//reset slide direction
	slideDirection = SlideDirection::None;
	return Window::onTouchStart(x,y);
}
bool ViewMain::onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY)
{
  if(Window::onTouchSlide(x, y, startX, startY, slideX, slideY)) return true;
  //if slide was already detected ignore it
  if(slideDirection == SlideDirection::None) {
	  if (startX < x && (x - startX > LCD_W / 4)){
		  slideDirection = SlideDirection::Right;
		  g_model.view = prevView();
		  invalidate();
		  return true;
	  }
	  else if(startX > x && (startX - x > LCD_W / 4)){
		  slideDirection = SlideDirection::Left;
		  g_model.view = nextView();
		  invalidate();
		  return true;
	  }
  }
  return false;
}
void ViewMain::showMenu()
{
  Menu * menu = new Menu ();
  menu->addLine (STR_MODEL_SELECT, [=]() { new ModelselectMenu(); });
  if (modelHasNotes ()) menu->addLine (STR_VIEW_NOTES, [=]() { /* TODO*/ });
  menu->addLine (STR_MONITOR_SCREENS, [=]() { new ChannelsMonitorMenu(); });
  menu->addLine (STR_RESET_SUBMENU, [=]() {
      Menu * menu2 = new Menu();
      menu2->addLine(STR_RESET_FLIGHT, [=]() { flightReset(); });
      menu2->addLine(STR_RESET_TIMER1, [=]() { timerReset(0); });
      menu2->addLine(STR_RESET_TIMER2, [=]() { timerReset(1); });
      menu2->addLine(STR_RESET_TIMER3, [=]() { timerReset(2); });
      menu2->addLine(STR_RESET_TELEMETRY, [=]() { telemetryReset(); });
  });
  menu->addLine (STR_STATISTICS, [=]() { new StatisticsMenu(); });
  menu->addLine (STR_ABOUT_US, [=]() { new AboutMenu(); });
}

bool ViewMain::onTouchEnd(coord_t x, coord_t y)
{
  if (Window::onTouchEnd(x, y)) return true;
  if(isNavigationVisible()) {
    coord_t buttonTop = isTopBarVisible() ? NAV_BUTTONS_MARGIN_TOP : NAV_BUTTONS_MARGIN;
	  if (y >= buttonTop && y <= buttonTop + buttonHeight) {
        if(x >= buttonLeftModel && x <= buttonLeftModel + buttonHeight){
	      	new ModelMenu();
	      	return true;
        }
        if(x >= buttonLeftRadio && x <= buttonLeftRadio + buttonHeight){
	      	new RadioMenu();
	      	return true;
        }
        if(x >= buttonLeftTheme && x <= buttonLeftTheme + buttonHeight){
	      	new ScreensMenu();
	      	return true;
        }
    }
  }

  if (isTopBarVisible() && (x < TOPBAR_BUTTON_WIDTH && y < MENU_HEADER_HEIGHT)) {
    AUDIO_KEY_PRESS();
    showMenu();
    return true;
  }

  return false;
}


void ViewMain::checkEvents()
{
  // temporary for refreshing the trims
  invalidate();
}
uint8_t ViewMain::currentView() {
  if (!customScreens[g_model.view]) {
	  return 0;
  }
  return g_model.view;
}
uint8_t ViewMain::nextView()
{
  for (uint8_t index = currentView() + 1; index < sizeof(customScreens)/sizeof(customScreens[0]); index++) {
    if (customScreens[index]) return index;
  }
  return g_model.view;
}
uint8_t ViewMain::prevView()
{
  if(g_model.view!=0) {
    for (uint8_t index = currentView() -1; index > 0; index--) {
      if (customScreens[index]) return index;
	}
  }
  return 0;
}

bool ViewMain::isTopBarVisible() {
	Layout* layout = customScreens[currentView()];
	return layout && layout->topBarHeight() > 0;
}
bool ViewMain::isNavigationVisible() {
  Layout* layout = customScreens[currentView()];
  //if configured or first view
  return layout->navigationHeight() > 0 || currentView() == 0;
}

void ViewMain::paint(BitmapBuffer * dc)
{
  uint8_t view = currentView();
  Layout* layout = customScreens[view];
  theme->drawBackground();

  for (uint8_t i=0; i<MAX_CUSTOM_SCREENS; i++) {
    if (i != view && customScreens[i]) customScreens[i]->background();
  }
  if(layout) {
    int32_t y = 0;
    if (layout->topBarHeight()) drawTopBar();
    y += layout->topBarHeight();

    if(isNavigationVisible()){
        drawButton(dc, buttonLeftModel, ICON_MODEL);
        drawButton(dc, buttonLeftRadio, ICON_RADIO);
        drawButton(dc, buttonLeftTheme, ICON_THEME);
    }
    y += layout->navigationHeight();
    if (layout->flightModeHeight()) drawFlightMode(y);
    y += layout->flightModeHeight();
    if(layout->trimHeight()) drawTrims(mixerCurrentFlightMode);
    if(layout->sliderHeight()) drawMainPots();
    customScreens[view]->refresh();
  }


}
