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

#include "switchchoice.h"
#include "menutoolbar.h"
#include "menu.h"
#include "draw_functions.h"
#include "strhelpers.h"
#include "dataconstants.h"

class SwitchChoiceMenuToolbar : public MenuToolbar<SwitchChoice> {
  public:
    SwitchChoiceMenuToolbar(SwitchChoice * choice, Menu * menu):
      MenuToolbar<SwitchChoice>(choice, menu)
    {
      addButton(char('\312'), SWSRC_FIRST_SWITCH, SWSRC_LAST_SWITCH);
      addButton(char('\313'), SWSRC_FIRST_TRIM, SWSRC_LAST_TRIM);
      addButton(char('\312'), SWSRC_FIRST_LOGICAL_SWITCH, SWSRC_LAST_LOGICAL_SWITCH);
    }
};

void SwitchChoice::checkEvents()
{
  Window::checkEvents();
  if (menu != nullptr)
  {
    unsigned value = getValue();
    swsrc_t val = static_cast<swsrc_t>(value);
    swsrc_t swtch = getMovedSwitch();
    if (!swtch) return;
#if defined(PCBTARANIS) || defined(PCBHORUS) || defined(PCBNV14)
    div_t info = switchInfo(swtch);
    if (IS_CONFIG_TOGGLE(info.quot))
    {
      if (info.rem != 0)
      {
        val = (val == swtch ? swtch-2 : swtch);
      }
    }
    else
    {
      val = swtch;
    }
#else
    if (IS_CONFIG_TOGGLE(swtch) && swtch == val)
    {
      val = -val;
    }
    else
    {
      val = swtch;
    }
#endif

    if (static_cast<swsrc_t>(value) != val)
    {
      if (isValueAvailable && !isValueAvailable(val))
        return;
      std::map<int, int>::iterator it = valueIndexMap.find(static_cast<int>(val));
      if (it != valueIndexMap.end() && it->second >= 0)
      {
        TRACE("EVENTS CHECK - change detected");
        setValue(static_cast<int16_t>(val));
        menu->select(it->second);
      }
    }
  }
}

void SwitchChoice::paint(BitmapBuffer * dc)
{
  bool hasFocus = this->hasFocus();
  unsigned value = getValue();
  LcdFlags textColor = (value == 0 ? CURVE_AXIS_COLOR : 0);
  LcdFlags lineColor = CURVE_AXIS_COLOR;
  if (hasFocus) {
    textColor = TEXT_INVERTED_BGCOLOR;
    lineColor = TEXT_INVERTED_BGCOLOR;
  }
  drawSwitch(dc, 3, 2, value, textColor);
  drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, lineColor);
}

void SwitchChoice::fillMenu(Menu * menu, std::function<bool(int16_t)> filter)
{
  auto value = getValue();
  int count = 0;
  int current = -1;

  menu->removeLines();
  valueIndexMap.clear();
  for (int i = vmin; i <= vmax; ++i) {
    if (filter && !filter(i))
      continue;
    if (isValueAvailable && !isValueAvailable(i))
      continue;
    valueIndexMap[i] = count;
    menu->addLine(getSwitchString(i), [=]() {
      setValue(i);
    });
    if (value == i) {
      current = count;
    }
    ++count;
  }

  if (current >= 0) {
    menu->select(current);
  }
}

bool SwitchChoice::onTouchEnd(coord_t, coord_t)
{
  AUDIO_KEY_PRESS();
  menu = new Menu();
  fillMenu(menu);
  menu->setToolbar(new SwitchChoiceMenuToolbar(this, menu));
  menu->setCloseHandler(std::bind(&SwitchChoice::deleteMenu, this));
  setFocus();
  return true;
}
