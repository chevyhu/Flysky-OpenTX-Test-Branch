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

#ifndef _WIDGETS_SETUP_H_
#define _WIDGETS_SETUP_H_

#include "view_main.h"
#include "widgets_container.h"
#include "opentx.h"
#include "libwindows.h"
#include "view_main.h"
#include "tabsgroup.h"
#include "screen_theme.h"

class WidgetConfigPage: public ZoneOptionPage {
  public:
    WidgetConfigPage(Widget* widget);
    void build(Window * window) override;
  protected:
    void onZoneOptionChanged(const ZoneOption* option) override;
    Widget* widget;
};

class WidgetsSetupView: public ViewMain {
  public:
    WidgetsSetupView(WidgetsContainerInterface* container, uint8_t index, LcdFlags color = TEXT_INVERTED_BGCOLOR, int padding = 4, int thickness = 2);

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "WidgetsSetupView";
    }
#endif
    uint8_t currentView() override;
    void createWidget(unsigned int zone, const char* name);
    void displayWidgetConfig(Widget* widget);
    void showWidgetMenu(unsigned int zone);
    void showSelectWidgetMenu(unsigned int zone);

    bool onTouchStart(coord_t x, coord_t y) override
    {
      ViewMain::onTouchStart(x, y);
      return true;
    }

    bool onTouchEnd(coord_t x, coord_t y) override;

    bool onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY) override;

    void paint(BitmapBuffer * dc) override;

  protected:
    WidgetsContainerInterface* container;
    uint8_t index;
    LcdFlags color;
    int padding;
    int thickness;
};

#endif // _WIDGETS_SETUP_H_
