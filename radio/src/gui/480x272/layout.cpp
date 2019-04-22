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

std::list<const LayoutFactory *> & getRegisteredLayouts()
{
  static std::list<const LayoutFactory *> layouts;
  return layouts;
}

void registerLayout(const LayoutFactory * factory)
{
  TRACE("register layout %s", factory->getName());
  getRegisteredLayouts().push_back(factory);
}

const LayoutFactory * getLayoutFactory(const char * name)
{
  std::list<const LayoutFactory *>::const_iterator it = getRegisteredLayouts().cbegin();
  for (; it != getRegisteredLayouts().cend(); ++it) {
    if (!strcmp(name, (*it)->getName())) {
      return (*it);
    }
  }
  return NULL;
}

Layout * loadLayout(const char * name, Layout::PersistentData * persistentData)
{
  const LayoutFactory * factory = getLayoutFactory(name);
  if (factory) {
    return factory->load(persistentData);
  }
  return NULL;
}

void loadCustomScreens()
{
  for (unsigned int i=0; i<MAX_CUSTOM_SCREENS; i++) {
    delete customScreens[i];
    customScreens[i] = loadLayout(g_model.screenData[i].layoutName, &g_model.screenData[i].layoutData);
  }

  if (customScreens[0] == NULL && getRegisteredLayouts().size()) {
    customScreens[0] = getRegisteredLayouts().front()->create(&g_model.screenData[0].layoutData);
  }
  topbar->load();
}


bool Layout::isOptionSet(const char* name) const
{
  ZoneOptionValue* value = getZoneOptionValue(name);
  return value && value->boolValue;
}
ZoneOptionValue* Layout::getZoneOptionValue(const char* name) const
{
  const ZoneOption * options = factory->getOptions();
  int index = 0;
  while (options[index].name)
  {
    if (strcmp(options[index].name, name) == 0)
    {
      return &persistentData->options[index];
    }
    index++;
  }
  return nullptr;
}

void Layout::create()
{
  WidgetsContainer::create();
  const ZoneOption * options = factory->getOptions();
  int index = 0;
  while (options[index].name)
  {
    if (options[index].type == ZoneOption::Bool)
    {
      persistentData->options[index].boolValue = true;
    }
    index++;
  }
}

Zone Layout::getZone(unsigned int index) const
{
  uint16_t doubleMargin =  2*margin();
  uint16_t w = (uint16_t)LCD_W;
  w -= doubleMargin;
  uint16_t h = (uint16_t)LCD_H;
  h -= doubleMargin;
  Zone zone = { margin(), margin(), w, h };

  zone.y += topBarHeight();
  zone.h -= topBarHeight();

  zone.y += navigationHeight();
  zone.h -= navigationHeight();

  zone.y += flightModeHeight();
  zone.h -= flightModeHeight();

  zone.x += trimHeight();
  zone.w -= trimHeight() * 2;
  zone.h -= trimHeight();

  zone.x += sliderHeight();
  zone.w -= sliderHeight() * 2;
  zone.h -= sliderHeight();

  return zone;
}

void Layout::refresh()
{
  WidgetsContainer::refresh();
}

uint16_t Layout::topBarHeight() const
{
  return isOptionSet(TopBar) ? MENU_HEADER_HEIGHT : 0;
}
uint16_t Layout::navigationHeight() const
{
  return isOptionSet(Navigation) ? NAV_BUTTONS_HEIGHT + (NAV_BUTTONS_MARGIN_TOP - MENU_HEADER_HEIGHT) : 0;
}
