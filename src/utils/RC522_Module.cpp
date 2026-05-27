// =====================================================================
//  RC522_Module.cpp - RC522 NFC Reader Implementation
// =====================================================================

#include "RC522_Module.h"

// Constructor - initialize rfid object in initializer list
RC522_Module::RC522_Module(uint8_t cs, uint8_t rst)
  : _cs(cs), _rst(rst), rfid(cs, rst)  // Initialize MFRC522 in initializer list
{
}

void RC522_Module::begin() {
  Serial.println("RC522: Initializing...");
  
  // Force SPI1 init right here
  SPI.begin(12, 13, 11);
  delay(50);
  
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  delay(50);

  rfid.PCD_Init();
  delay(50);
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
  delay(50);

  byte version = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.printf("RC522: Firmware version 0x%02X\n", version);
  
  byte txControl = rfid.PCD_ReadRegister(rfid.TxControlReg);
  Serial.printf("RC522: TxControl = 0x%02X\n", txControl);

  if (version == 0x00 || version == 0xFF) {
    Serial.println("RC522: ✗ Communication failure");
  } else {
    Serial.println("RC522: ✓ Ready!");
  }
}

bool RC522_Module::isCardPresent() {
  return rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial();
}

void RC522_Module::haltCard() {
  rfid.PICC_HaltA();
}

bool RC522_Module::waitForCard(uint32_t timeout_ms) {
  uint32_t start = millis();
  
  while (millis() - start < timeout_ms) {
    if (rfid.PICC_IsNewCardPresent()) {
      if (rfid.PICC_ReadCardSerial()) {
        return true;
      }
    }
    delay(50);
  }
  
  return false;
}

bool RC522_Module::readUserData(uint8_t *buffer, uint8_t numBytes) {
  uint8_t startPage = 4;
  uint8_t numPages = (numBytes + 3) / 4;  // 4 bytes per page
  
  for (uint8_t i = 0; i < numPages; i++) {
    byte readBuffer[18];
    byte size = sizeof(readBuffer);
    
    MFRC522::StatusCode status = rfid.MIFARE_Read(startPage + i, readBuffer, &size);
    if (status != MFRC522::STATUS_OK) {
      return false;
    }
    
    // Copy 4 bytes from this page
    for (uint8_t j = 0; j < 4 && (i*4 + j) < numBytes; j++) {
      buffer[i*4 + j] = readBuffer[j];
    }
  }
  
  return true;
}

int RC522_Module::extractText(const uint8_t *ndefData, char *textOut, uint8_t maxLen) {
  // NDEF structure:
  // Byte 0: 0x03 (message start)
  // Byte 5: 0x54 ('T' for text record type)
  // Byte 9+: Text data starts here
  
  if (ndefData[0] != 0x03) return 0;   // Not NDEF
  if (ndefData[5] != 0x54) return 0;   // Not text record
  
  int textStart = 9;
  int textLen = 0;
  
  for (int i = textStart; i < maxLen + textStart && ndefData[i] != 0x00 && ndefData[i] != 0xFE; i++) {
    if (ndefData[i] >= 32 && ndefData[i] <= 126) {  // Printable ASCII
      textOut[textLen++] = ndefData[i];
    }
  }
  
  textOut[textLen] = '\0';
  return textLen;
}

bool RC522_Module::readAlbumText(char *textOut, uint8_t maxLen) {
  uint8_t userData[48];
  
  if (!readUserData(userData, 48)) {
    return false;
  }
  
  int textLen = extractText(userData, textOut, maxLen);
  return textLen > 0;
}

void RC522_Module::runDiagnostic() {
    //SPI.begin(SPI2_SCK, SPI2_MISO, SPI2_MOSI);
  Serial.println("\n=== RC522 DIAGNOSTIC ===");
  
 pinMode(_rst, OUTPUT);
digitalWrite(_rst, LOW);
delay(100);
digitalWrite(_rst, HIGH);
delay(200);
Serial.printf("RST pin state: %d\n", digitalRead(_rst));


  delay(50);
  Serial.println("\n=== RC522 DIAGNOSTIC ===");
  // Add this to diagnostic BEFORE switching SPI buses
// So remove the SPI2 begin and go back to SPI1

Serial.println("\n=== WRITE/READ VERIFY ===");
byte original = rfid.PCD_ReadRegister(rfid.ModWidthReg);
Serial.printf("ModWidth original: 0x%02X\n", original);

rfid.PCD_WriteRegister(rfid.ModWidthReg, 0x12);
delay(10);

byte readback = rfid.PCD_ReadRegister(rfid.ModWidthReg);
Serial.printf("ModWidth after write 0x12: 0x%02X\n", readback);

if (readback == 0x12) {
    Serial.println("✓ WRITES WORKING - MOSI OK");
} else if (readback == original) {
    Serial.println("✗ WRITE FAILED - MOSI NOT REACHING CHIP");
} else {
    Serial.printf("? UNEXPECTED: 0x%02X\n", readback);
}
Serial.println("=== END VERIFY ===\n");
  // Switch to SPI2 for RC522
  SPI.begin(SPI2_SCK, SPI2_MISO, SPI2_MOSI);
  delay(50);
  
  // Re-init RC522 on SPI2
  rfid.PCD_Init();
  delay(50);
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
  delay(50);

  // 1. Check firmware version
  byte version = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.printf("Firmware: 0x%02X %s\n", version, 
    (version == 0x91 || version == 0x92) ? "✓" : "✗ UNEXPECTED");
  
  // 2. Check antenna
  byte txControl = rfid.PCD_ReadRegister(rfid.TxControlReg);
  Serial.printf("TxControl: 0x%02X - Antenna %s\n", txControl,
    (txControl & 0x03) == 0x03 ? "ON ✓" : "OFF ✗");
  
  // 3. Check gain
  byte gain = rfid.PCD_GetAntennaGain();
  Serial.printf("Antenna gain: 0x%02X %s\n", gain,
    gain == 0x70 ? "MAX ✓" : "NOT MAX ✗");

  // 4. ComIrq
  byte comIrq = rfid.PCD_ReadRegister(rfid.ComIrqReg);
  Serial.printf("ComIrq: 0x%02X\n", comIrq);

  // 5. ErrorReg
  byte error = rfid.PCD_ReadRegister(rfid.ErrorReg);
  Serial.printf("ErrorReg: 0x%02X %s\n", error,
    error == 0x00 ? "✓ No errors" : "✗ ERRORS PRESENT");

  // 6. Force antenna on
  Serial.println("Forcing antenna on...");
  rfid.PCD_AntennaOn();
  delay(50);
  txControl = rfid.PCD_ReadRegister(rfid.TxControlReg);
  Serial.printf("TxControl after force: 0x%02X\n", txControl);

  // 7. REQA test
  Serial.println("\nSending REQA - present card now...");
  for(int i = 0; i < 20; i++) {
    rfid.PCD_WriteRegister(rfid.ComIrqReg, 0x7F);
    
    bool present = rfid.PICC_IsNewCardPresent();
    byte irq = rfid.PCD_ReadRegister(rfid.ComIrqReg);
    byte err = rfid.PCD_ReadRegister(rfid.ErrorReg);
    
    Serial.printf("Attempt %2d: present=%d ComIrq=0x%02X Error=0x%02X\n", 
      i+1, present, irq, err);
    delay(200);
  }
  
  Serial.println("=== END DIAGNOSTIC ===\n");
}

void RC522_Module::runTest(int count) {
  runDiagnostic();
  for (int i = 0; i < count; i++) {
    Serial.printf("\n--- RC522 Read %d/%d ---\n", i + 1, count);
    Serial.println("Waiting for card...");
    
    if (!waitForCard(5000)) {
      Serial.println("No card found (timeout)");
      continue;
    }
    
    // Print UID
    Serial.print("UID: ");
    for (byte j = 0; j < rfid.uid.size; j++) {
      Serial.printf("%02X ", rfid.uid.uidByte[j]);
    }
    Serial.println();
    
    // Read album text
    char cleanText[40];
    if (readAlbumText(cleanText, sizeof(cleanText) - 1)) {
      Serial.print("Album: \"");
      Serial.print(cleanText);
      Serial.println("\"");
    } else {
      // Fallback to raw data
      uint8_t userData[48];
      if (readUserData(userData, 48)) {
        Serial.print("Raw: ");
        for (int k = 0; k < 20; k++) {
          char c = userData[k];
          Serial.print((c >= 32 && c <= 126) ? c : '.');
        }
        Serial.println();
      } else {
        Serial.println("Failed to read user data");
      }
    }
    
    haltCard();
    delay(500);
  }
}