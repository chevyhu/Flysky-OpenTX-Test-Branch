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

#ifndef _SCREEN_THEME_H_
#define _SCREEN_THEME_H_

#include "tabsgroup.h"
#include "libwindows.h"
#include "opentx.h"


class ZoneOptionPage : public PageTab {
  public:
    ZoneOptionPage(std::string title, unsigned icon) : PageTab(title, icon) {}
  protected:
    void addOption(Window * window, GridLayout& grid, const ZoneOption& option, ZoneOptionValue * value);
    Window* createOptionEdit(Window * parent, const rect_t &rect, const ZoneOption * option, ZoneOptionValue * value);
    virtual bool isChangeAllowed(const ZoneOption* option);
    virtual void onZoneOptionChanged(const ZoneOption* option);
};

class ScreenThemePage: public ZoneOptionPage {
  public:
    ScreenThemePage();
    void build(Window * window) override;
  protected:
    void onZoneOptionChanged(const ZoneOption* option) override;
};

#endif //_SCREEN_THEME_H_
