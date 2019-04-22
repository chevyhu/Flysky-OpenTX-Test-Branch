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

#include "screen_setup.h"
#include "widgets_setup.h"
#include "opentx.h"
#include "libwindows.h"


ScreenSetupPage::ScreenSetupPage(uint8_t index, bool inited) :
  ZoneOptionPage("Main view " + std::to_string(index + 1), ICON_THEME_VIEW1 + index),
  index(index),
  inited(inited)
{
}

void ScreenSetupPage::rebuild(Window * window)
{
  window->clear();
  build(window);
}
void ScreenSetupPage::recreateWidgets() {
  Layout * layout = customScreens[index];
  for(unsigned int zone = 0; zone < layout->getZonesCount(); zone++) {
    Widget* widget = layout->getWidget(zone);
    if(widget) {
      Widget::PersistentData* data = new Widget::PersistentData();
      ZoneOptionValue* oldVal = widget->getOptionValue(0);
      for (unsigned int i = 0; i < sizeof(data->options)/sizeof(data->options[0]); i++) {
        memcpy((void*)&data->options[i], (const void*)&oldVal[i], sizeof(ZoneOptionValue));
      }
      Widget* widgetNew = widget->getFactory()->create(layout->getZone(zone), data, false);
      layout->setWidget(zone, widgetNew);
      delete widget;
    }
  }
}
bool ScreenSetupPage::isChangeAllowed(const ZoneOption* option)
{
  //prevent disabling navigation on first page
  return !(strcmp(option->name, Layout::Navigation) == 0 && index == 0);
}

void ScreenSetupPage::onZoneOptionChanged(const ZoneOption* option) {
  //adjust size of widgets after changes in layout
  recreateWidgets();
  customScreens[index]->update();
  storageDirty(EE_MODEL);
}

void ScreenSetupPage::build(Window * window)
{
  Layout * layout = customScreens[index];
  GridLayout grid;
  grid.setLabelWidth(LCD_W/2);
  grid.spacer(8);

  if(!inited) {
      new TextButton(window, grid.getLineSlot(), STR_ADDMAINVIEW, [=]() -> uint8_t {
    	  inited = true;
    	  if (getRegisteredLayouts().size()) {
    	      customScreens[index] = getRegisteredLayouts().front()->create(&g_model.screenData[index].layoutData);
    	  }
    	  storageDirty(EE_MODEL);
    	  rebuild(window);
    	  return 0;
      });
      return;
  }
  // Layout choice
  auto layouts = getRegisteredLayouts();
  new StaticText(window, grid.getLabelSlot(), STR_LAYOUT);
  layoutChoice = new NumberEdit(window, grid.getFieldSlot(), 0, layouts.size() - 1,
                                [=]() {
                                  int currentIndex = 0;
                                  for (auto factory: getRegisteredLayouts()) {
                                    if (factory == layout->getFactory())
                                      return currentIndex;
                                    ++currentIndex;
                                  }
                                  return 0;
                                });
  layoutChoice->setWidth(55);
  layoutChoice->setHeight(35);
  layoutChoice->setSetValueHandler([=](uint8_t newValue) {
    delete customScreens[index];
    auto it = getRegisteredLayouts().begin();
    std::advance(it, newValue);
    auto factory = *it;
    strncpy(g_model.screenData[index].layoutName, factory->getName(), LAYOUT_NAME_LEN);
    customScreens[index] = factory->create(&g_model.screenData[index].layoutData);
    storageDirty(EE_MODEL);
    rebuild(window);
    layoutChoice->setFocus();
    NumberKeyboard::instance()->setField(layoutChoice);
  });
  layoutChoice->setDisplayHandler([=](BitmapBuffer * dc, LcdFlags flags, int32_t index) {
    auto it = getRegisteredLayouts().begin();
    std::advance(it, index);
    (*it)->drawThumb(dc, 2, 2, LINE_COLOR);
  });
  grid.nextLine(35);
  // Setup widgets button

  new TextButton(window, grid.getFieldSlot(), STR_SETUP_WIDGETS, [=]() -> uint8_t { new WidgetsSetupView(static_cast<WidgetsContainerInterface*>(customScreens[index]), index); return 0; });
  grid.nextLine();
  // Layout options
  const ZoneOption * options = layout->getFactory()->getOptions();
  for (int option=0; ; option++) {
    if(!options || !options[option].name) break;
    addOption(window, grid, options[option], layout->getOptionValue(option));
  }

  // Delete screen button
  if (index > 0) {
    new TextButton(window, grid.getLineSlot(), STR_REMOVE_SCREEN, [=]() -> uint8_t {
	  delete layout;
	  memset(&g_model.screenData[MAX_CUSTOM_SCREENS - 1], 0, sizeof(CustomScreenData));
	  customScreens[index] = NULL;
	  inited = false;
	  storageDirty(EE_MODEL);
	  rebuild(window);
	  return 0;
    });
    grid.nextLine();
  }

  window->setInnerHeight(grid.getWindowHeight());
}

