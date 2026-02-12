// =====================================================================
//  TFT_TestScreen.h - Test routines for ILI9488 display
// =====================================================================

#ifndef TFT_TESTSCREEN_H
#define TFT_TESTSCREEN_H

#include <Arduino.h>
#include "../utils/TFT_Module.h"

class TFT_TestScreen {
public:
  // Constructor
  TFT_TestScreen(TFT_Module* module);
  
  // Run full test suite
  void runTest();
  
  // Individual tests
  void testColorFills();
  void testColorBars();
  void testText();
  void testGraphics();
  
private:
  TFT_Module* _tftModule;
  lgfx::LGFX_Device* tft;  // Use base class type
};

#endif // TFT_TESTSCREEN_H