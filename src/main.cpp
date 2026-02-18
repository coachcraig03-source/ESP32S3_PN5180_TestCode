#include <Arduino.h>
#include <SPI.h>
//#include <SD.h>  // Add this for SDMMC support
#include <SdFat.h>  // At top instead of SD.h
#include "pins.h"
#include "utils/RC522_Module.h"
#include "utils/VS1053_Module.h"
#include "utils/TFT_Module.h"
#include "screens/TFT_TestScreen.h"
#include <Wire.h>
#include <FT6236.h>  // Changed from FT6X36.h
#include <WiFi.h>
#include <time.h>

// WiFi credentials
const char* WIFI_SSID = "Galactia_Guest";
const char* WIFI_PASSWORD = "SantaClaus1993";

// Create modules
RC522_Module nfcModule(NFC_CS, NFC_RST);
VS1053_Module audioModule(VS1053_CS, VS1053_DCS, VS1053_DREQ, VS1053_RST);
TFT_Module tftModule(TFT_CS, TFT_DC, TFT_RST, TFT_BL, SPI2_SCK, SPI2_MOSI, SPI2_MISO);
TFT_TestScreen tftTest(&tftModule);
FT6236 touchScreen;

SdFat sd;

volatile bool touchDetected = false;

void IRAM_ATTR touchISR() {
  touchDetected = true;
}


void printMenu() {
  Serial.println();
  Serial.println("========================================");
  Serial.println("ESP32-S3 Hardware Test Menu");
  Serial.println("========================================");
  Serial.println("1: RC522 NFC test (5 reads)");
  Serial.println("2: TFT video test");
  Serial.println("3: VS1053 audio test");
  Serial.println("4: Touch screen test");
  Serial.println("5: Tone test");
  Serial.println("6: SD Card Test");
  Serial.println("7: Read an MP3 file");
  Serial.println("8: WiFi Connection Test");
  Serial.println("========================================");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(4000);
  
  Serial.println("\n=== ESP32-S3 Hardware Init ===\n");
  Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
  
  // Initialize SPI1 bus
  Serial.println("Initializing SPI1 bus...");
  SPI.begin(SPI1_SCK, SPI1_MISO, SPI1_MOSI);
  delay(100);
  
  // Hold VS1053 in reset during RC522 init
  pinMode(VS1053_RST, OUTPUT);
  digitalWrite(VS1053_RST, LOW);
  delay(100);
  
// Initialize SPI1 explicitly
Serial.println("Initializing SPI1 bus...");
SPI.begin(12, 13, 11);  // SCK, MISO, MOSI
delay(100);
pinMode(18, OUTPUT);
digitalWrite(18, LOW);
delay(100);
digitalWrite(18, HIGH);
delay(100);


  // Initialize RC522
  Serial.println("\nInitializing RC522 NFC...");
  nfcModule.begin();
  
  delay(100);
  
  // Release and initialize VS1053
  digitalWrite(VS1053_RST, HIGH);
  delay(100);
  
  Serial.println("\nInitializing VS1053 Audio...");
  audioModule.begin();
  delay(100);
  // Add after VS1053 module creation
  Serial.println("\nInitializing TFT..."); 
  tftModule.begin();
  delay(100);

    // In setup(), after TFT init:
 
Serial.println("\nInitializing Touch...");

// Reset touch controller
pinMode(TOUCH_RST, OUTPUT);
digitalWrite(TOUCH_RST, LOW);
delay(10);
digitalWrite(TOUCH_RST, HIGH);
delay(50);

Wire.begin(I2C_SDA, I2C_SCL);  // Use the defines, not hardcoded 45, 46

// Scan I2C bus
Serial.println("Scanning I2C bus...");
for (byte addr = 1; addr < 127; addr++) {
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    Serial.printf("Found I2C device at 0x%02X\n", addr);
  }
}

touchScreen.begin();
pinMode(TOUCH_INT, INPUT);
attachInterrupt(TOUCH_INT, touchISR, FALLING);  // Touch triggers on falling edge
Serial.println("Touch: ✓ Ready!");

  // Restore SPI1 after TFT corrupts it
  SPI.begin(SPI1_SCK, SPI1_MISO, SPI1_MOSI);
  delay(100);

  Serial.println("\n=== Init Complete ===");
  printMenu();
}

void loop() {
  if (!Serial.available()) return;
  
  char c = Serial.read();
  
switch (c) {
  case '1':
    Serial.println("\n>>> Running RC522 test (5 reads)...");
    nfcModule.runTest(5);
    printMenu();
    break;

  case '2':
    Serial.println("\n>>> Running TFT display test...");
    tftTest.runTest();
    
    // Restore SPI1 after TFT test
    SPI.begin(SPI1_SCK, SPI1_MISO, SPI1_MOSI);
    delay(100);
    
    printMenu();
    break;  // THIS WAS MISSING!
    
  case '3':
    Serial.println("\n>>> Running VS1053 test...");
    if (audioModule.isAlive()) {
      Serial.println("✓ VS1053 is responding!");
      audioModule.getChipInfo();
    } else {
      Serial.println("✗ VS1053 not responding - check wiring");
    }
    printMenu();
    break;  // THIS WAS MISSING!

  case '4':
  {
    Serial.println("\n>>> Touch test - tap screen 10 times...");
    int touchCount = 0;
    while (touchCount < 10) {
      if (touchDetected) {
        touchDetected = false;
        TS_Point p = touchScreen.getPoint();
        
        if (p.x != 0 && p.y != 0) {  // Filter phantom touches
          touchCount++;
          Serial.printf("Touch %d: X=%d Y=%d\n", touchCount, p.x, p.y);
        }
        delay(300);
      }
      delay(10);
    }
    Serial.println("Touch test complete!");
    printMenu();
    break;
  }


  case '5':  // VS1053 Tone Test
  {
  Serial.println("\n=== VS1053 Tone Test ===");
  Serial.println("Initializing VS1053...");
  
  SPI.begin(12, 13, 11);
  delay(10);
  
  Serial.println("Starting 440Hz tone...");
  
  // Enable sine test
  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
  digitalWrite(VS1053_CS, LOW);
  SPI.transfer(0x02); SPI.transfer(0x00);
  SPI.transfer(0x08); SPI.transfer(0x24);
  digitalWrite(VS1053_CS, HIGH);
  SPI.endTransaction();
  delay(10);
  
  // Start 440Hz tone
  while (digitalRead(VS1053_DREQ) == LOW) delay(1);
  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
  digitalWrite(VS1053_DCS, LOW);
  SPI.transfer(0x53); SPI.transfer(0xEF); SPI.transfer(0x6E);
  SPI.transfer(0xB8); SPI.transfer(0x01);
  SPI.transfer(0x00); SPI.transfer(0x00); SPI.transfer(0x00);
  digitalWrite(VS1053_DCS, HIGH);
  SPI.endTransaction();
  
  Serial.println("Tone playing... Press any key to stop");
  while (!Serial.available()) delay(100);
  Serial.read();
  
  // Stop tone
  SPI.begin(12, 13, 11);
  while (digitalRead(VS1053_DREQ) == LOW) delay(1);
  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
  digitalWrite(VS1053_DCS, LOW);
  SPI.transfer(0x45); SPI.transfer(0x78); SPI.transfer(0x69); SPI.transfer(0x74);
  SPI.transfer(0x00); SPI.transfer(0x00); SPI.transfer(0x00); SPI.transfer(0x00);
  digitalWrite(VS1053_DCS, HIGH);
  SPI.endTransaction();
  
  Serial.println("Tone stopped");
      printMenu();
  break;
  }
    
 case '6':  // SD Card Test with SdFat
  Serial.println("\n=== SD Card Test (SdFat) ===");
  
  SPI.begin(SPI1_SCK, SPI1_MISO, SPI1_MOSI);
  
  if (!sd.begin(SD_CS, SD_SCK_MHZ(25))) {
    Serial.println("SD init failed!");
    sd.initErrorHalt(&Serial);
  } else {
    Serial.println("SD Card initialized!");
    
    // Get actual card info
    Serial.printf("Card type: %d\n", sd.card()->type());
    Serial.printf("Sectors: %lu\n", sd.card()->sectorCount());
    Serial.printf("Size: %.2f MB\n", sd.card()->sectorCount() * 512.0 / 1048576.0);
    
    // List files
    Serial.println("\nRoot directory:");
    FsFile root;
    root.open("/");
    FsFile file;
    while (file.openNext(&root, O_RDONLY)) {
      char name[64];
      file.getName(name, sizeof(name));
      if (!file.isDirectory()) {
        Serial.printf("  %s (%lu bytes)\n", name, file.fileSize());
      } else {
        Serial.printf("  [%s/]\n", name);
      }
      file.close();
    }
    root.close();
  }
       printMenu();
  break;
case '7':  // Test MP3 file read
  Serial.println("\n=== Test MP3 Read ===");
  
  SPI.begin(SPI1_SCK, SPI1_MISO, SPI1_MOSI);
  
  if (!sd.begin(SD_CS, SD_SCK_MHZ(25))) {
    Serial.println("SD init failed!");
  } else {
    // Open first album
    FsFile albumDir;
    albumDir.open("/Bon Jovi - Have a Nice Day");
    
    // Find first MP3
    FsFile file;
    while (file.openNext(&albumDir, O_RDONLY)) {
      char name[64];
      file.getName(name, sizeof(name));
      
      if (strstr(name, ".mp3") || strstr(name, ".MP3")) {
        Serial.printf("Found MP3: %s (%lu bytes)\n", name, file.fileSize());
        
        // Read first 512 bytes
        uint8_t buffer[512];
        int bytesRead = file.read(buffer, 512);
        Serial.printf("Read %d bytes from MP3\n", bytesRead);
        
        file.close();
        break;
      }
      file.close();
    }
    albumDir.close();
  }
         printMenu();
  break;
  case '8':  // WiFi Test
{
  Serial.println("\n=== WiFi Connection Test ===");
  Serial.printf("Free heap before: %d bytes\n", ESP.getFreeHeap());
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  // Test NTP time
  Serial.println("\nGetting time from NTP...");
  configTime(-28800, 3600, "pool.ntp.org");

  // Wait for time to sync (can take 2-10 seconds)
  Serial.print("Syncing time");
  int timeouts = 0;
  while (time(nullptr) < 100000 && timeouts < 20) {  // Wait up to 10 seconds
      delay(500);
      Serial.print(".");
      timeouts++;
  }
  Serial.println();

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
      char timeStr[64];
      strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
      Serial.printf("✓ Current time: %s\n", timeStr);
  } else {
      Serial.println("✗ Failed to get time from NTP");
  }

  WiFi.disconnect();
  Serial.println("WiFi disconnected");
  
  printMenu();
  break;
}


  default:
    break;
}
}