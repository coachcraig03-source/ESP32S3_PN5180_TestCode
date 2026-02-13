#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "pins.h"

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI3_HOST;  // HSPI on ESP32-S3
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;  // 40MHz
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = SPI2_SCK;   // GPIO 36
      cfg.pin_mosi = SPI2_MOSI;  // GPIO 35
      cfg.pin_miso = -1;         // NOT CONNECTED
      cfg.pin_dc = TFT_DC;       // GPIO 40
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = TFT_CS;       // GPIO 38
      cfg.pin_rst = TFT_RST;     // GPIO 39
      cfg.pin_busy = -1;
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }
    setPanel(&_panel_instance);
  }
};

LGFX tft;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ST7796S with LovyanGFX ===");
  
  // CRITICAL: Reset TFT immediately to clear boot state
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(50);
  digitalWrite(TFT_RST, HIGH);
  delay(150);
  
  Serial.println("TFT reset complete");
  
  // Backlight
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  Serial.println("Backlight ON");
  
  // Initialize display
  Serial.println("Initializing display...");
  tft.init();
  tft.setRotation(1);  // Landscape
  
  Serial.printf("Display size: %d x %d\n", tft.width(), tft.height());
  
  // Test pattern
  Serial.println("Drawing test pattern...");
  
  tft.fillScreen(TFT_RED);
  delay(1000);
  
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  
  tft.fillScreen(TFT_BLUE);
  delay(1000);
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("ST7796S Works!", 50, 100);
  tft.drawString("480 x 320", 100, 150);
  
  Serial.println("Test complete!");
}

void loop() {
  delay(1000);
}