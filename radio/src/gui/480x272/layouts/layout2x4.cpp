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

const uint8_t LBM_LAYOUT_2x4[] = {
#include "mask_layout2x4.lbm"
};

const ZoneOption OPTIONS_LAYOUT_2x4[] = {
  { Layout::TopBar, ZoneOption::Bool },
  { Layout::Navigation, ZoneOption::Bool },
  { Layout::FlightMode, ZoneOption::Bool },
  { Layout::Sliders, ZoneOption::Bool },
  { Layout::Trims, ZoneOption::Bool },
  { Layout::Panel1BG, ZoneOption::Bool },
  { Layout::Panel1BGC, ZoneOption::Color },
  { Layout::Panel2BG, ZoneOption::Bool },
  { Layout::Panel2BGC, ZoneOption::Color },
  { NULL, ZoneOption::Bool }
};

class Layout2x4: public Layout
{
  public:
    Layout2x4(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData, 8)
    {
    }

    void create() override
    {
      Layout::create();
      getZoneOptionValue(Panel1BGC)->unsignedValue = RGB(77, 112, 203);
      getZoneOptionValue(Panel2BGC)->unsignedValue = RGB(77, 112, 203);
      getZoneOptionValue(Panel2BG)->boolValue = false;

    }

    virtual Zone getZone(unsigned int index) const
    {
      Zone zone = Layout::getZone(index);
      zone.w -= margin(); //additional margin
      zone.w /= 2; //half of space
      zone.h -= margin()*3; //additional margin
      zone.h /= 4; //quarter of space

      if (index % 2 == 1) zone.x += zone.w + margin(); //right column
      zone.y += (index / 2) * (zone.h + margin()); //row
      return zone;
    }

    void refresh() override;
};

void Layout2x4::refresh()
{
  Zone fullScreen = Layout::getZone(0);
  fullScreen.w /=2;
  if (isOptionSet(Panel1BG)) {
    lcdSetColor(getZoneOptionValue(Panel1BGC)->unsignedValue);
    lcdDrawSolidFilledRect(fullScreen.x, fullScreen.y, fullScreen.w, fullScreen.h, CUSTOM_COLOR);
  }

  if (isOptionSet(Panel2BG)) {
    fullScreen.x += fullScreen.w;
    lcdSetColor(getZoneOptionValue(Panel2BGC)->unsignedValue);
    lcdDrawSolidFilledRect(fullScreen.x, fullScreen.y, fullScreen.w, fullScreen.h, CUSTOM_COLOR);
  }
  Layout::refresh();
}

BaseLayoutFactory<Layout2x4> layout2x4("Layout2x4", LBM_LAYOUT_2x4, OPTIONS_LAYOUT_2x4);
