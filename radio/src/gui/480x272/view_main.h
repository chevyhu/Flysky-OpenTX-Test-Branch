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

#ifndef _VIEW_MAIN_H_
#define _VIEW_MAIN_H_

#include "window.h"
enum class SlideDirection {
	Left = -1,
	None = 0,
	Right = 1
};
class ViewMain: public Window {
  public:
    ViewMain();
    ~ViewMain() override;
    bool onTouchEnd(coord_t x, coord_t y) override;
    bool onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY) override;
    bool onTouchStart(coord_t x, coord_t y) override;
    void paint(BitmapBuffer * dc) override;

    void checkEvents() override;
  protected:
    uint8_t nextView();
    uint8_t prevView();
    virtual uint8_t currentView();
    bool isTopBarVisible();
    bool isNavigationVisible();
    void drawButton(BitmapBuffer * dc, coord_t x, uint8_t icon);
    void drawMainPots();
    void drawTrims(uint8_t flightMode);
    void drawFlightMode(coord_t y);
    void showMenu();

    const int buttonHeight;
    const int buttonLeftModel;
    const int buttonLeftRadio;
    const int buttonLeftTheme;
    SlideDirection slideDirection;
};

#endif // _VIEW_MAIN_H_

