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

#include "screen_theme.h"
#include "opentx.h"
#include "libwindows.h"
#include "widgets_setup.h"


ScreenThemePage::ScreenThemePage() :
  ZoneOptionPage(STR_USER_INTERFACE, ICON_THEME_SETUP)
{
}

bool ZoneOptionPage::isChangeAllowed(const ZoneOption* option) {
  return true;
}

void ZoneOptionPage::onZoneOptionChanged(const ZoneOption* option)
{

}

void ZoneOptionPage::addOption(Window * window, GridLayout& grid, const ZoneOption& option, ZoneOptionValue * value) {
  new StaticText(window, grid.getLabelSlot(), std::string(option.name));
  createOptionEdit(window, grid.getFieldSlot(), &option, value);
  grid.nextLine();
}

Window * ZoneOptionPage::createOptionEdit(Window * parent, const rect_t &rect, const ZoneOption * option, ZoneOptionValue * value)
{
  switch(option->type){
  case ZoneOption::Bool:
    return new CheckBox(parent, rect,
        [=] { return (uint8_t)value->boolValue; },
        [=](uint8_t newValue) {
          if(!isChangeAllowed(option)) return;
          value->boolValue = newValue;
          onZoneOptionChanged(option);
        });
  case ZoneOption::Integer:
    return new NumberEdit(parent, rect, option->min.signedValue, option->max.signedValue,
        [=] { return value->signedValue; },
        [=](int32_t newValue) {
          if(!isChangeAllowed(option)) return;
          value->signedValue = newValue;
          onZoneOptionChanged(option);
         });
  case ZoneOption::String:
    return new TextEdit(parent, rect, value->stringValue, sizeof(value->stringValue));
  case ZoneOption::File:
    return new FileChoice(parent, rect, BITMAPS_PATH, BITMAPS_EXT, sizeof(value->stringValue),
        [=]() { return std::string(value->stringValue, ZLEN(value->stringValue)); },
        [=](std::string newValue) {
          if(!isChangeAllowed(option)) return;
          strncpy(value->stringValue, newValue.c_str(), sizeof(value->stringValue));
          onZoneOptionChanged(option);
        }, false);
  case ZoneOption::TextSize:
    return new Choice(parent, rect, "\010StandardTiny\0   Small\0  Mid\0    Double", 0, 4,
        [=] { return (int16_t)value->unsignedValue; },
        [=](int16_t newValue) {
          if(!isChangeAllowed(option)) return;
          value->unsignedValue = newValue;
          onZoneOptionChanged(option);
        });
  case ZoneOption::Timer: {
    auto timerchoice = new Choice(parent, rect, nullptr, 0, TIMERS - 1,
        [=] { return (int16_t)value->unsignedValue; },
        [=](int16_t newValue) {
          if(!isChangeAllowed(option)) return;
          value->unsignedValue = newValue;
          onZoneOptionChanged(option);
        });
    timerchoice->setTextHandler([](int32_t value) {
      return std::string(STR_TIMER) + std::to_string(value + 1);
    });
    return timerchoice;
  }
  case ZoneOption::Source:
    return new SourceChoice(parent, rect, 1, MIXSRC_LAST_TELEM,
        [=] { return (int16_t)value->unsignedValue; },
        [=](int16_t newValue) {
          if(!isChangeAllowed(option)) return;
          value->unsignedValue = newValue;
          onZoneOptionChanged(option);
        });
  case ZoneOption::Color:
    new ColorEdit(parent, rect,
        [=] { return value->signedValue; },
        [=](int32_t newValue) {
          if(!isChangeAllowed(option)) return;
          value->signedValue = newValue;
          onZoneOptionChanged(option);
         });
  default: return nullptr;
  }

  return nullptr;
}

void ScreenThemePage::onZoneOptionChanged(const ZoneOption* option)
{
  if(option->type == ZoneOption::Color) theme->updatecolor();
  else theme->update();
  storageDirty(EE_GENERAL);
}

void ScreenThemePage::build(Window * window)
{
  GridLayout grid;
  grid.setLabelWidth(LCD_W/2);
  grid.spacer(8);

  /*
  new StaticText(window, grid.getLabelSlot(), STR_THEME);
  std::list<Theme *> themes = getRegisteredThemes();
  auto themechoice = new Choice(window, grid.getFieldSlot(), nullptr, 0,
      getRegisteredThemes().size() - 1, [=]
      {
        int index = 0;
        for (auto it = themes.begin(); it != themes.end(); ++it, index++)
        {
          if(strcmp(g_eeGeneral.themeName, (*it)->getName()) == 0) return index;
        }
        return 0;
      }, [=](int16_t newValue)
      {
        int index = 0;
        for (auto it = themes.begin(); it != themes.end(); ++it, index++)
        {
          if(index == newValue) {
            (*it)->init();
            loadTheme((*it));
            strncpy(g_eeGeneral.themeName, (*it)->getName(), sizeof(g_eeGeneral.themeName));
            storageDirty(EE_GENERAL);
            window->clear();
            build(window);
          }
        }
      });

  themechoice->setTextHandler([=](int32_t value)
  {
    int index = 0;
    for (auto it = themes.begin(); it != themes.end(); ++it, index++)
    {
      if(index == value) return std::string((*it)->getName());
    }
    return std::string("");
  });
  grid.nextLine();
  */

  // Theme options
  const ZoneOption * options = theme->getOptions();
  for (int option = 0;; option++) {
    if (!options || !options[option].name) break;
    addOption(window, grid, options[option], theme->getOptionValue(option));
  }
  // Topbar customization
  new TextButton(window, grid.getLineSlot(), STR_TOP_BAR, []() -> uint8_t {
                   new WidgetsSetupView(static_cast<WidgetsContainerInterface*>(topbar), 0, MENU_TITLE_COLOR, 2, 1);
                   return 0;
  });
  grid.nextLine();

  window->setInnerHeight(grid.getWindowHeight());
}

