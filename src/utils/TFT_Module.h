// =====================================================================
//  TFT_Module.h - ST7796S Display Module
//  Encapsulates LovyanGFX setup for ST7796S 480x320 display
// =====================================================================

#ifndef TFT_MODULE_H
#define TFT_MODULE_H

#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// Forward declaration
class LGFX;

class TFT_Module {
public:
  // Constructor
  TFT_Module(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t bl,
             uint8_t sck, uint8_t mosi, uint8_t miso);
  
  // Initialize the display
  bool begin();
  
  // Get access to the underlying display
  lgfx::LGFX_Device* getTFT();
  
  // Control backlight
  void setBacklight(bool on);
  
  // Set rotation (0, 1, 2, 3)
  void setRotation(uint8_t rotation);
  
private:
  LGFX* tft;
  uint8_t _cs, _dc, _rst, _bl;
  uint8_t _sck, _mosi, _miso;
};

#endif // TFT_MODULE_H