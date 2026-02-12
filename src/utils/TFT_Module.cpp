// =====================================================================
//  TFT_Module.cpp - ILI9488 Display Module Implementation
// =====================================================================

#include "TFT_Module.h"

// Define LGFX class with full configuration
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 27000000;
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 12;
      cfg.pin_mosi = 11;
      cfg.pin_miso = 13;
      cfg.pin_dc = 6;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = 10;
      cfg.pin_rst = 7;
      cfg.pin_busy = -1;
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;  // Try adding this
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);
  }
};

TFT_Module::TFT_Module(uint8_t cs, uint8_t dc, uint8_t rst, uint8_t bl,
                       uint8_t sck, uint8_t mosi, uint8_t miso)
  : _cs(cs), _dc(dc), _rst(rst), _bl(bl),
    _sck(sck), _mosi(mosi), _miso(miso), tft(nullptr)
{
}

bool TFT_Module::begin() {
  Serial.println("TFT: Initializing ILI9488...");
  
  // Setup backlight pin
  if (_bl >= 0) {
    pinMode(_bl, OUTPUT);
    digitalWrite(_bl, HIGH);
    Serial.println("TFT: Backlight ON");
  }
  
  // Create LGFX object (pins are configured in LGFX constructor)
  Serial.println("TFT: Creating display object...");
  tft = new LGFX();
  
  // Initialize display
  Serial.println("TFT: Calling init()...");
  tft->init();
  
  // Default to landscape orientation
  tft->setRotation(1);
  
  Serial.printf("TFT: Display size: %d x %d\n", tft->width(), tft->height());
  
  // Clear screen with explicit transaction
  tft->startWrite();
  tft->fillScreen(TFT_BLACK);
  tft->endWrite();
  
  Serial.println("TFT: Ready!");
  return true;
}

lgfx::LGFX_Device* TFT_Module::getTFT() {
  return tft;
}

void TFT_Module::setBacklight(bool on) {
  if (_bl >= 0) {
    digitalWrite(_bl, on ? HIGH : LOW);
  }
}

void TFT_Module::setRotation(uint8_t rotation) {
  if (tft) {
    tft->setRotation(rotation);
  }
}