// =====================================================================
//  ESP32-S3 NFC Music Player - Pin Definitions
//  Hardware: ESP32-S3-DevKitC-1-N8R8
// =====================================================================

#ifndef PINS_H
#define PINS_H

// =========================
//   SPI BUS 1 (VSPI) - Audio & NFC
//   PN5180 NFC + VS1053B Audio Decoder
// =========================
#define SPI1_SCK         12      // Shared clock
#define SPI1_MOSI        11      // Shared MOSI
#define SPI1_MISO        13      // Shared MISO

// =========================
//   PN5180 NFC READER
//   ** WORKING - DO NOT CHANGE **
// =========================
#define NFC_CS          16      // PN5180 Chip Select (NSS)
#define NFC_RST         18      // PN5180 Reset
#define NFC_BUSY        14      // PN5180 Busy signal
#define NFC_IRQ         15      // PN5180 IRQ (may not be needed)
#define NFC_AUX         -1      // PN5180 AUX (not used)

// =========================
//   VS1053B AUDIO DECODER
//   Uses SPI1 bus (11, 12, 13)
// =========================
#define VS1053_CS       9       // VS1053 SPI Chip Select
#define VS1053_DCS      7       // VS1053 Data Chip Select (XDCS)
#define VS1053_DREQ     2       // VS1053 Data Request (must be interrupt-capable)
#define VS1053_RST      8       // VS1053 Reset (XRST)

// =========================
//   SPI BUS 2 (HSPI) - Display & SD Card
//   4.0" ST7796S TFT with SD card
// =========================
#define SPI2_SCK        36      // HSPI Clock
#define SPI2_MOSI       35      // HSPI MOSI
#define SPI2_MISO       37      // HSPI MISO

// =========================
//   TFT DISPLAY (ST7796S 4.0")
//   320x480 resolution
// =========================
#define TFT_CS          33      // LCD Chip Select (LCD_CS)
#define TFT_RST         34      // LCD Reset (LCD_RST) - may not be exposed, use -1 if not available
#define TFT_DC          38      // LCD Data/Command (LCD_RS)
#define TFT_BL          39      // LCD Backlight (optional)

// =========================
//   SD CARD (on TFT module)
//   Shares SPI2 bus with TFT
// =========================
#define SD_CS           40      // SD Card Chip Select

// =========================
//   CAPACITIVE TOUCH (I2C)
//   Separate I2C bus - no conflicts
// =========================
#define I2C_SDA         41      // Touch IIC Data (CTP_SDA)
#define I2C_SCL         42      // Touch IIC Clock (CTP_SCL)
#define TOUCH_RST       45      // Touch Reset (CTP_RST)
#define TOUCH_INT       46      // Touch Interrupt (CTP_INT)

// =========================
//   USER INTERFACE
// =========================
#define LED_STATUS      48      // Built-in RGB LED
#define BUTTON_MODE     0       // Boot button (built-in)

// =========================
//   NOTES
// =========================
// SPI1 (11,12,13): PN5180 NFC + VS1053B Audio
// SPI2 (35,36,37): ST7796S TFT + SD Card
// I2C  (41,42):    Capacitive Touch
//
// GPIO 34 may not be exposed on ESP32-S3-DevKitC-1
// If TFT_RST not available, use -1 and tie RST high on display

#endif // PINS_H