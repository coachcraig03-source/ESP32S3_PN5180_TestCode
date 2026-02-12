// =====================================================================
//  VS1053_Module.h - VS1053B Audio Decoder Module
//  Tests and controls VS1053B MP3/audio decoder
// =====================================================================

#ifndef VS1053_MODULE_H
#define VS1053_MODULE_H

#include <Arduino.h>

class VS1053_Module {
public:
  // Constructor
  VS1053_Module(uint8_t cs, uint8_t dcs, uint8_t dreq, uint8_t rst);
  
  // Initialize the VS1053
  bool begin();
  
  // Test if VS1053 is alive and responding
  bool isAlive();
  
  // Read SCI register
  uint16_t readRegister(uint8_t reg);
  
  // Write SCI register
  void writeRegister(uint8_t reg, uint16_t value);
  
  // Get chip version/status
  void getChipInfo();
  
private:
  uint8_t _cs;      // Command chip select (X-CS)
  uint8_t _dcs;     // Data chip select (X-DCS)
  uint8_t _dreq;    // Data request
  uint8_t _rst;     // Reset
  
  // Wait for DREQ to go high (ready for data)
  bool waitForDREQ(uint32_t timeout_ms = 100);
};

// VS1053 SCI (Serial Command Interface) Registers
#define VS1053_REG_MODE       0x00
#define VS1053_REG_STATUS     0x01
#define VS1053_REG_BASS       0x02
#define VS1053_REG_CLOCKF     0x03
#define VS1053_REG_DECODE_TIME 0x04
#define VS1053_REG_AUDATA     0x05
#define VS1053_REG_WRAM       0x06
#define VS1053_REG_WRAMADDR   0x07
#define VS1053_REG_HDAT0      0x08
#define VS1053_REG_HDAT1      0x09
#define VS1053_REG_AIADDR     0x0A
#define VS1053_REG_VOL        0x0B
#define VS1053_REG_AICTRL0    0x0C
#define VS1053_REG_AICTRL1    0x0D
#define VS1053_REG_AICTRL2    0x0E
#define VS1053_REG_AICTRL3    0x0F

#endif // VS1053_MODULE_H