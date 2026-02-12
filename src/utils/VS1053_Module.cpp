// =====================================================================
//  VS1053_Module.cpp - VS1053B Audio Decoder Implementation
// =====================================================================

#include "VS1053_Module.h"
#include <SPI.h>

VS1053_Module::VS1053_Module(uint8_t cs, uint8_t dcs, uint8_t dreq, uint8_t rst)
  : _cs(cs), _dcs(dcs), _dreq(dreq), _rst(rst)
{
}

bool VS1053_Module::begin() {
  Serial.println("VS1053: Initializing...");
  
  // Setup pins
  pinMode(_cs, OUTPUT);
  pinMode(_dcs, OUTPUT);
  pinMode(_dreq, INPUT);
  
  // Deselect both interfaces
  digitalWrite(_cs, HIGH);
  digitalWrite(_dcs, HIGH);
  
  // Hardware reset if RST pin is defined
  if (_rst >= 0) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
    delay(20);
    digitalWrite(_rst, HIGH);
    delay(100);
    Serial.println("VS1053: Hardware reset complete");
  }
  
  // Wait for DREQ to go high (chip ready)
  Serial.println("VS1053: Waiting for DREQ...");
  if (!waitForDREQ(500)) {
    Serial.println("VS1053: ✗ DREQ timeout - chip not responding");
    return false;
  }
  
  Serial.println("VS1053: ✓ DREQ is HIGH - chip is ready");
  
  // Software reset
  Serial.println("VS1053: Sending software reset...");
  writeRegister(VS1053_REG_MODE, 0x0804); // SM_SDINEW + SM_RESET
  delay(100);
  
  // Wait for chip to be ready again
  if (!waitForDREQ(500)) {
    Serial.println("VS1053: ✗ Failed after software reset");
    return false;
  }
  
  // Set clock multiplier for higher performance
  writeRegister(VS1053_REG_CLOCKF, 0x6000); // SC_MULT = 3.0x, SC_ADD = 0x00
  delay(10);
  
  Serial.println("VS1053: ✓ Initialization complete");
  return true;
}

bool VS1053_Module::isAlive() {
  // Try to read STATUS register
  uint16_t status = readRegister(VS1053_REG_STATUS);
  
  // VS1053 STATUS register should have sensible values
  // Bit 15-4 are version info, should not be all 0s or all 1s
  if (status == 0x0000 || status == 0xFFFF) {
    Serial.printf("VS1053: ✗ Bad STATUS register: 0x%04X\n", status);
    return false;
  }
  
  Serial.printf("VS1053: ✓ STATUS register: 0x%04X\n", status);
  return true;
}

uint16_t VS1053_Module::readRegister(uint8_t reg) {
  uint16_t result = 0;
  
  waitForDREQ(100);
  
  // VS1053 needs SPI Mode 0, slower speed
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  digitalWrite(_cs, LOW);
  delayMicroseconds(1);
  
  SPI.transfer(0x03);  // Read command
  SPI.transfer(reg);
  result = SPI.transfer(0x00) << 8;
  result |= SPI.transfer(0x00);
  
  delayMicroseconds(1);
  digitalWrite(_cs, HIGH);
  
  SPI.endTransaction();
  
  return result;
}

void VS1053_Module::writeRegister(uint8_t reg, uint16_t value) {
  waitForDREQ(100);
  
  // VS1053 needs SPI Mode 0, slower speed
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  digitalWrite(_cs, LOW);
  delayMicroseconds(1);
  
  SPI.transfer(0x02);  // Write command
  SPI.transfer(reg);
  SPI.transfer(value >> 8);
  SPI.transfer(value & 0xFF);
  
  delayMicroseconds(1);
  digitalWrite(_cs, HIGH);
  
  SPI.endTransaction();
}

void VS1053_Module::getChipInfo() {
  Serial.println("\n=== VS1053 Chip Information ===");
  
  uint16_t mode = readRegister(VS1053_REG_MODE);
  Serial.printf("MODE:        0x%04X\n", mode);
  
  uint16_t status = readRegister(VS1053_REG_STATUS);
  Serial.printf("STATUS:      0x%04X\n", status);
  
  uint16_t clockf = readRegister(VS1053_REG_CLOCKF);
  Serial.printf("CLOCKF:      0x%04X\n", clockf);
  
  uint16_t vol = readRegister(VS1053_REG_VOL);
  Serial.printf("VOLUME:      0x%04X\n", vol);
  
  // Decode version from STATUS register
  uint16_t version = (status >> 4) & 0x0F;
  Serial.printf("Chip version: %d\n", version);
  
  Serial.println("================================\n");
}

bool VS1053_Module::waitForDREQ(uint32_t timeout_ms) {
  uint32_t start = millis();
  while (digitalRead(_dreq) == LOW) {
    if (millis() - start > timeout_ms) {
      return false;
    }
    delayMicroseconds(10);
  }
  return true;
}