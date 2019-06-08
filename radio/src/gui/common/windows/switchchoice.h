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

#ifndef _SWITCHCHOICE_H_
#define _SWITCHCHOICE_H_

#include "window.h"
#include <map>

class Menu;
bool isSwitchAvailableInMixes(int swtch);

class SwitchChoice : public Window {
  template <class T> friend class MenuToolbar;

  public:
    SwitchChoice(Window * parent, const rect_t & rect, int vmin, int vmax, std::function<int16_t()> getValue, std::function<void(int16_t)> setValue):
      Window(parent, rect),
      vmin(vmin),
      vmax(vmax),
      getValue(std::move(getValue)),
      setValue(std::move(setValue))
    {
      menu = nullptr;
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "SwitchChoice";
    }
#endif
    void checkEvents() override;

    void paint(BitmapBuffer * dc) override;

    bool onTouchEnd(coord_t x, coord_t y) override ;

    void setAvailableHandler(std::function<bool(int)> handler)
    {
      isValueAvailable = std::move(handler);
    }

  protected:
    int16_t vmin;
    int16_t vmax;
    std::function<int16_t()> getValue;
    std::function<void(int16_t)> setValue;
    std::function<bool(int)> isValueAvailable = isSwitchAvailableInMixes;
    std::map<int, int> valueIndexMap;
    Menu * menu;
    void fillMenu(Menu * menu, std::function<bool(int16_t)> condition=nullptr);
    //used to set pointer to null after menu is closed
    void deleteMenu() {
      menu = nullptr;
    }
};

#endif // _SWITCHCHOICE_H_
