// =====================================================================
//  TFT_TestScreen.cpp - Test routines implementation
// =====================================================================

#include "TFT_TestScreen.h"

TFT_TestScreen::TFT_TestScreen(TFT_Module* module)
  : _tftModule(module), tft(nullptr)
{
  // Don't get tft pointer here - it's not created yet!
}

void TFT_TestScreen::runTest() {
  Serial.println("\n=== Running Full TFT Test Suite ===");
  
  // Get tft pointer NOW (after begin() has been called)
  tft = _tftModule->getTFT();

    Serial.printf("TFT pointer: %p\n", tft);  // ADD THIS
  
  if (!tft) {
    Serial.println("ERROR: TFT not initialized!");
    return;
  }

    Serial.println("About to call testColorFills...");  // ADD THIS
  
  testColorFills();
  delay(5000);
  
  testColorBars();
  delay(5000);
  
  testText();
  delay(5000);
  
  testGraphics();
  delay(5000);
  
  Serial.println("=== TFT Test Complete ===\n");
}

void TFT_TestScreen::testColorFills() {
  Serial.println("Test: Color fills...");
    Serial.println("Calling fillScreen(RED)...");  // ADD THIS
  
  tft->fillScreen(TFT_RED);
    Serial.println("Red done");  // ADD THIS
  delay(1000);
  
  tft->fillScreen(TFT_GREEN);
  delay(1000);
  
  tft->fillScreen(TFT_BLUE);
  delay(500);
  
  tft->fillScreen(TFT_BLACK);
  
  Serial.println("  ✓ Color fills OK");
}

void TFT_TestScreen::testColorBars() {
  Serial.println("Test: Color bars...");
  
  tft->fillScreen(TFT_BLACK);
  
  // Draw horizontal color bars (adjusted for 480x320)
  int barHeight = 40;
  int y = 20;
  
  tft->fillRect(0, y, 480, barHeight, TFT_RED);
  y += barHeight;
  
  tft->fillRect(0, y, 480, barHeight, TFT_GREEN);
  y += barHeight;
  
  tft->fillRect(0, y, 480, barHeight, TFT_BLUE);
  y += barHeight;
  
  tft->fillRect(0, y, 480, barHeight, TFT_YELLOW);
  y += barHeight;
  
  tft->fillRect(0, y, 480, barHeight, TFT_CYAN);
  y += barHeight;
  
  tft->fillRect(0, y, 480, barHeight, TFT_MAGENTA);
  
  Serial.println("  ✓ Color bars OK");
}

void TFT_TestScreen::testText() {
  Serial.println("Test: Text rendering...");
  
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  
  // Title
  tft->setTextSize(3);
  tft->drawString("ILI9488 Display", 120, 20);
  
  // Info
  tft->setTextSize(2);
  tft->drawString("ESP32-S3", 180, 80);
  tft->drawString("480 x 320", 180, 110);
  tft->drawString("LovyanGFX", 180, 140);
  
  // Status
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->setTextSize(2);
  tft->drawString("Status: OK", 160, 220);
  
  // Box
  tft->drawRect(100, 260, 280, 40, TFT_WHITE);
  tft->setTextColor(TFT_CYAN, TFT_BLACK);
  tft->drawString("Text Test Complete", 120, 270);
  
  Serial.println("  ✓ Text rendering OK");
}

void TFT_TestScreen::testGraphics() {
  Serial.println("Test: Graphics primitives...");
  
  tft->fillScreen(TFT_BLACK);
  
  // Lines
  for (int i = 0; i < 10; i++) {
    tft->drawLine(0, i * 32, 480, i * 32, TFT_BLUE);
  }
  
  delay(500);
  
  // Rectangles
  tft->fillScreen(TFT_BLACK);
  tft->drawRect(50, 50, 100, 80, TFT_RED);
  tft->fillRect(200, 50, 100, 80, TFT_GREEN);
  tft->drawRect(350, 50, 80, 80, TFT_BLUE);
  
  // Circles
  tft->drawCircle(100, 200, 40, TFT_YELLOW);
  tft->fillCircle(250, 200, 40, TFT_CYAN);
  tft->drawCircle(400, 200, 35, TFT_MAGENTA);
  
  delay(1000);
  
  // Final message
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_GREEN, TFT_BLACK);
  tft->setTextSize(3);
  tft->drawString("All Tests Passed!", 100, 140);
  
  Serial.println("  ✓ Graphics OK");
}