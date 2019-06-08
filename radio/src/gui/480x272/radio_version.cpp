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

#include "radio_version.h"
#include "opentx.h"
#include "libwindows.h"

enum QR_Code
{
  QR_CODE_QUICK_START,
  QR_CODE_USER_MANUAL
};

class QR_CodeBody : public Window {
  public:
    QR_CodeBody(Window * parent, const rect_t &rect, uint8_t QR_Index) :
      Window(parent, rect),
      QR_Index(QR_Index)
    {
    }

    void checkEvents() override
    {
      invalidate();
    }

    void paint(BitmapBuffer * dc) override
    {
      if (QR_Index == QR_CODE_QUICK_START){
        static BitmapBuffer * quickStartBitmap = BitmapBuffer::loadMask(getThemePath("quick_start_guide.png"));

        lcd->setOffset(0, 0);
        lcd->clearClippingRect();

        lcd->drawBitmap((LCD_W - quickStartBitmap->getWidth()) / 2, (LCD_H - quickStartBitmap->getHeight()) / 2, quickStartBitmap);
      }
      else if (QR_Index == QR_CODE_USER_MANUAL){
        static BitmapBuffer * userManulBitmap = BitmapBuffer::loadMask(getThemePath("user_manual.png"));

        lcd->setOffset(0, 0);
        lcd->clearClippingRect();

        lcd->drawBitmap((LCD_W - userManulBitmap->getWidth()) / 2, (LCD_H - userManulBitmap->getHeight()) / 2, userManulBitmap);
      }
    }

  protected:
    uint8_t QR_Index;
};

class QR_CodePage : public PageTab {
  public:
    QR_CodePage(uint8_t QR_Index) :
      PageTab(QR_Index == QR_CODE_QUICK_START ? STR_QUICK_START_GUIDE:STR_USER_MANUAL, ICON_RADIO_VERSION),
      QR_Index(QR_Index)
    {
    }

    void build(Window * window) override
    {
      new QR_CodeBody(window, {0, 0, LCD_W, window->height() - footerHeight}, QR_Index);
    }

  protected:
    static constexpr coord_t footerHeight = 30;
    uint8_t QR_Index;
};

QR_CodeMenu::QR_CodeMenu(uint8_t QR_Index) :
  TabsGroup()
{
  addTab(new QR_CodePage(QR_Index));
}

RadioVersionPage::RadioVersionPage():
  PageTab(STR_MENUVERSION, ICON_RADIO_VERSION)
{
}

void RadioVersionPage::build(Window * window)
{
  GridLayout grid;
  //grid.setLabelWidth(60);
  grid.spacer(8);

  getCPUUniqueID(reusableBuffer.version.id);

  new StaticText(window, grid.getLabelSlot(), "FW Version :");
  new StaticText(window, grid.getFieldSlot(), vers_stamp);
  //grid.nextLine(12);
  //new StaticText(window, {6, grid.getWindowHeight(), LCD_W - 6, 26}, vers_stamp);
  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(), "Data version");
  new StaticText(window, grid.getFieldSlot(), eeprom_stamp);
  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(), "Date");
  new StaticText(window, grid.getFieldSlot(), date_stamp);
  grid.nextLine();

  new StaticText(window, grid.getLabelSlot(), "Time");
  new StaticText(window, grid.getFieldSlot(), time_stamp);
  grid.nextLine();


  new StaticText(window, grid.getLabelSlot(), "CPU UID :");
  grid.nextLine(12);
  new StaticText(window, grid.getLineSlot(), reusableBuffer.version.id);
  grid.nextLine(60);

  new TextButton(window, grid.getLineSlot(), STR_QUICK_START_GUIDE, [=]() -> int8_t {
      new QR_CodeMenu(QR_CODE_QUICK_START);
      return 1;
  });
  grid.nextLine();
  new TextButton(window, grid.getLineSlot(), STR_USER_MANUAL, [=]() -> int8_t {
      new QR_CodeMenu(QR_CODE_USER_MANUAL);
      return 1;
  });
  grid.nextLine();
  new TextButton(window, grid.getLineSlot(), STR_FACTORYRESET, [=]() -> int8_t {
    auto dialog = new Dialog(WARNING_TYPE_INPUT, STR_CONFIRMRESET, STR_STORAGE_FORMAT, [=]() {
      storageEraseAll(false);
      NVIC_SystemReset();
      return 0;
    });
    dialog->runForever();
    return 0;
  });
}
