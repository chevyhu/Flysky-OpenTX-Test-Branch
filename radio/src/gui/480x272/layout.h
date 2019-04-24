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

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include <list>
#include "widgets_container.h"
#include "lcd.h"


#define MAX_LAYOUT_ZONES               10
#define MAX_LAYOUT_OPTIONS             10

class LayoutFactory;

class Layout: public WidgetsContainer<MAX_LAYOUT_ZONES, MAX_LAYOUT_OPTIONS>
{
  friend class LayoutFactory;

  public:
    Layout(const LayoutFactory * factory, PersistentData * persistentData, unsigned int zonesCount = 1):
      WidgetsContainer<MAX_LAYOUT_ZONES, MAX_LAYOUT_OPTIONS>(persistentData),
      factory(factory),
      zonesCount(zonesCount)
    {
    }

    void create() override;
    void refresh() override;
    Zone getZone(unsigned int index) const override;

    unsigned int getZonesCount() const override
    {
      return zonesCount;
    }

    inline const LayoutFactory * getFactory() const
    {
      return factory;
    }

    virtual void update()
    {
    }

    uint16_t topBarHeight() const;

    uint16_t navigationHeight() const;

    uint16_t flightModeHeight() const
    {
      return isOptionSet(FlightMode) ? 10 : 0;
    }

    uint16_t trimHeight() const
    {
      return isOptionSet(Trims) ? 16 : 0;
    }

    uint16_t sliderHeight() const
    {
      return isOptionSet(Sliders) ? 16 : 0;
    }

    static constexpr const char* TopBar = "Top bar";
    static constexpr const char* FlightMode = "Flight mode";
    static constexpr const char* Sliders = "Sliders";
    static constexpr const char* Trims = "Trims";
    static constexpr const char* Navigation = "Navigation";
    static constexpr const char* Panel1BG = "Panel1 background";
    static constexpr const char* Panel2BG = "Panel2 background";
    static constexpr const char* Panel1BGC = "Panel1 color";
    static constexpr const char* Panel2BGC = "Panel2 color";

  protected:
    uint16_t margin() const
    {
      return 10;
    }

    bool isOptionSet(const char* name) const;
    ZoneOptionValue* getZoneOptionValue(const char* name) const;
    const LayoutFactory * factory;
    unsigned int zonesCount;
};


void registerLayout(const LayoutFactory * factory);

class LayoutFactory
{
  public:
    LayoutFactory(const char * name):
      name(name)
    {
      registerLayout(this);
    }
    const char * getName() const { return name; }
    virtual void drawThumb(BitmapBuffer * dc, uint16_t x, uint16_t y, uint32_t flags) const = 0;
    virtual const ZoneOption * getOptions() const = 0;
    virtual Layout * create(Layout::PersistentData * persistentData) const = 0;
    virtual Layout * load(Layout::PersistentData * persistentData) const = 0;

  protected:
    const char * name;
};

template<class T>
class BaseLayoutFactory: public LayoutFactory
{
  public:
    BaseLayoutFactory(const char * name, const uint8_t * bitmap, const ZoneOption * options):
      LayoutFactory(name),
      bitmap(bitmap),
      options(options)
    {
    }

    void drawThumb(BitmapBuffer * dc, uint16_t x, uint16_t y, uint32_t flags) const override
    {
      dc->drawBitmapPattern(x, y, bitmap, flags);
    }

    virtual const ZoneOption * getOptions() const
    {
      return options;
    }

    virtual Layout * create(Layout::PersistentData * persistentData) const
    {
      Layout * layout = new T(this, persistentData);
      if (layout) {
        layout->create();
      }
      return layout;
    }

    virtual Layout * load(Layout::PersistentData * persistentData) const
    {
      Layout * layout = new T(this, persistentData);
      if (layout) {
        layout->load();
      }
      return layout;
    }

  protected:
    const uint8_t * bitmap;
    const ZoneOption * options;
};

Layout * loadLayout(const char * name, Layout::PersistentData * persistentData);
void loadCustomScreens();

std::list<const LayoutFactory *> & getRegisteredLayouts();

#endif // _LAYOUT_H_
