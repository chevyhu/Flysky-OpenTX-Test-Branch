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

#ifndef _SCREEN_SETUP_H_
#define _SCREEN_SETUP_H_

#include "tabsgroup.h"
#include "screen_theme.h"

class Layout;
class NumberEdit;

class ScreenSetupPage: public ZoneOptionPage {
  public:
    ScreenSetupPage(uint8_t index, bool inited = true);

    void build(Window * window) override;

  protected:
    bool isChangeAllowed(const ZoneOption* option) override;
    void onZoneOptionChanged(const ZoneOption* option) override;
    void recreateWidgets();
    uint8_t index;
    bool inited;
    NumberEdit * layoutChoice = nullptr;
    void rebuild(Window * window);
};

#endif //_SCREEN_SETUP_H_
