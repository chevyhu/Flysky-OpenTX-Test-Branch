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

#include "widgets_setup.h"

WidgetConfigPage::WidgetConfigPage(Widget* widget) :
ZoneOptionPage(STR_WIDGET_SETTINGS, ICON_THEME_SETUP),
widget(widget)
{
}
void WidgetConfigPage::onZoneOptionChanged(const ZoneOption* option) {
  widget->update();
  storageDirty(EE_MODEL);
}
void WidgetConfigPage::build(Window * window) {
  GridLayout grid;
  grid.spacer(8);
  grid.setLabelWidth(LCD_W/2);
  const char* error = widget->getErrorMessage();
  if(error) {
    new StaticText(window, grid.getLineSlot(), std::string(error), WARNING_COLOR | CENTERED);
    return;
  }

  const ZoneOption * options = widget->getOptions();
  for (int option=0; ; option++) {
    if(!options || !options[option].name) break;
    addOption(window, grid, options[option], widget->getOptionValue(option));
  }
}



WidgetsSetupView::WidgetsSetupView(WidgetsContainerInterface* container, uint8_t index, LcdFlags color, int padding, int thickness):
  ViewMain(),
  container(container),
  index(index),
  color(color),
  padding(padding),
  thickness(thickness)
{
}

void WidgetsSetupView::createWidget(unsigned int zone, const char* name){
  Widget* old = container->getWidget(zone);
  if(old) delete old;
  const WidgetFactory * factory = NULL;
  if(name) {
    for (auto it = getRegisteredWidgets().cbegin(); it != getRegisteredWidgets().cend(); ++it) {
       if(strcmp(name, (*it)->getName()) == 0){
         factory = (*it);
         break;
       }
    }
  }
  container->createWidget(zone, factory);
  storageDirty(EE_MODEL);
}
void WidgetsSetupView::displayWidgetConfig(Widget* widget)
{
  if(widget && widget->getFactory()->getOptions()) {
    TabsGroup* tabsGroup = new TabsGroup();
    tabsGroup->addTab(new WidgetConfigPage(widget));
  }
}

void WidgetsSetupView::showSelectWidgetMenu(unsigned int zone)
{
  Menu* menu = new Menu ();
  menu->addLine (TR_NONE, [=]() { createWidget(zone, NULL); });
  for (auto it = getRegisteredWidgets().cbegin(); it != getRegisteredWidgets().cend(); ++it) {
    const char * name = (*it)->getName();
    menu->addLine (name, [=]() {
      createWidget(zone, name);
      Widget* widget = container->getWidget(zone);
      displayWidgetConfig(widget);
    });
  }
}
void WidgetsSetupView::showWidgetMenu(unsigned int zone)
{
  Widget* widget = container->getWidget(zone);
  if(widget) {
    Menu * menu = new Menu ();
    menu->addLine (STR_SELECT_WIDGET, [=]() { showSelectWidgetMenu(zone); });
    if (widget->getFactory()->getOptions()) menu->addLine (STR_WIDGET_SETTINGS, [=]() { displayWidgetConfig (widget); });
    menu->addLine (STR_REMOVE_WIDGET, [=]() { createWidget(zone, NULL); });
  }
  else showSelectWidgetMenu(zone);
}

bool WidgetsSetupView::onTouchEnd(coord_t x, coord_t y)
{
  for (unsigned int index=0; index < container->getZonesCount(); index++) {
    Zone zone = container->getZone(index);
    if(x >= zone.x && x <= (zone.x + zone.w) && y >= zone.y && y <= (zone.y + zone.h)){
      showWidgetMenu(index);
    }
  }
  return true;
}

bool WidgetsSetupView::onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY)
{
  if(slideDirection == SlideDirection::None) {
    if (startX < x && (x - startX > LCD_W / 3)){
      slideDirection = SlideDirection::Right;
      this->detach();
      return true;
    }
  }
  return true;
}

uint8_t WidgetsSetupView::currentView() {
  return this->index;
}

void WidgetsSetupView::paint(BitmapBuffer * dc)
{
  ViewMain::paint(dc);
  for (int i=container->getZonesCount()-1; i>=0; i--) {
    Zone zone = container->getZone(i);
    lcdDrawRect(zone.x-padding, zone.y-padding, zone.w+2*padding, zone.h+2*padding, thickness, 0x3F, color);
  }
}

