/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 * https://github.com/adafruit/Adafruit-SSD1351-library.git
 */
#include "Arduino_SSD1351.h"

Arduino_SSD1351::Arduino_SSD1351(
    Arduino_DataBus *bus, int8_t rst, uint8_t r, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, false, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

void Arduino_SSD1351::begin(uint32_t speed)
{
#if defined(ESP8266) || defined(ESP32)
  if (speed == 0)
  {
    speed = 16000000;
  }
#endif
  Arduino_TFT::begin(speed);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Arduino_SSD1351::tftInit()
{
  _bus->setDataMode(SPI_MODE0);

  _bus->writeCommand(SSD1351_COMMANDLOCK); // set command lock
  _bus->writeData(0x12);
  _bus->writeCommand(SSD1351_COMMANDLOCK); // set command lock
  _bus->writeData(0xB1);
  _bus->writeCommand(SSD1351_DISPLAYOFF); // Display off
  _bus->writeCommand(SSD1351_DISPLAYOFFSET); // 0xA2
  _bus->writeData(0x0);
  _bus->writeCommand(SSD1351_NORMALDISPLAY); // 0xA6
  _bus->writeCommand(SSD1351_DISPLAYON); // Main screen turn on
}

void Arduino_SSD1351::writeAddrColumn(uint16_t x, uint16_t w)
{
#if defined(ESP8266) || defined(ESP32)
  uint16_t x_range = ((x + _xStart) << 8) | (x + w - 1 + _xStart);

  _bus->writeCommandCore(SSD1351_SETCOLUMN); // Column addr set
  _bus->write16(x_range);
#else
  uint8_t x_start = x + _xStart, x_end = x + w - 1 + _xStart;

  _bus->writeCommandCore(SSD1351_SETCOLUMN); // Column addr set
  _bus->write(x_start);                      // XSTART
  _bus->write(x_end);                        // XEND
#endif
}

void Arduino_SSD1351::writeAddrRow(uint16_t y, uint16_t h)
{
#if defined(ESP8266) || defined(ESP32)
  uint16_t y_range = ((y + _yStart) << 8) | (y + h - 1 + _yStart);

  _bus->writeCommandCore(SSD1351_SETROW); // Row addr set
  _bus->write16(y_range);
#else
  uint8_t y_start = y + _yStart, y_end = y + h - 1 + _yStart;

  _bus->writeCommandCore(SSD1351_SETROW); // Row addr set
  _bus->write(y_start);                   // YSTART
  _bus->write(y_end);                     // YEND
#endif
}

void Arduino_SSD1351::writeAddrMemWrite()
{
  _bus->writeCommandCore(SSD1351_WRITERAM); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_SSD1351::setRotation(uint8_t r)
{
  Arduino_TFT::setRotation(r);
  uint8_t startline = (_rotation < 2) ? HEIGHT : 0;
  switch (_rotation)
  {
  case 0:
    r = 0b01110100;
    break;
  case 1:
    r = 0b01110111;
    break;
  case 2:
    r = 0b01100110;
    break;
  case 3:
    r = 0b01100101;
    break;
  }

  _bus->beginWrite();
  _bus->writeCommandCore(SSD1351_SETREMAP);
  _bus->write(r);
  _bus->writeCommandCore(SSD1351_STARTLINE);
  _bus->write(startline);
  _bus->endWrite();
}

void Arduino_SSD1351::invertDisplay(bool i)
{
  _bus->writeCommand(i ? SSD1351_INVERTDISPLAY : SSD1351_NORMALDISPLAY);
}

void Arduino_SSD1351::displayOn(void)
{
  _bus->writeCommand(SSD1351_DISPLAYALLON);
}

void Arduino_SSD1351::displayOff(void)
{
  _bus->writeCommand(SSD1351_DISPLAYALLOFF);
}