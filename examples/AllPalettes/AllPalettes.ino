/*
 *  LED palettes demo
 *
 * Shows all palettes from Palettes.h on a WS2812B LED strip, one by one.
 * See LEDPalettes.h for class CLedPalettes 
 */
#define NUMBER_OF_LEDS 100
#define LED_PIN 18
#define DELAY_FOR_PALETTE 2000         // ms, how long each palette is shown

#include <Arduino.h>
#include <WS2812B_RMT.h>
#include "LEDPalettes.h"

CWs2812bRmt  oLeds(LED_PIN, NUMBER_OF_LEDS);     // LED strip driven by the ESP32 RMT peripheral
CLedPalettes oPalettes(oLeds, NUMBER_OF_LEDS);   // palettes shown on oLeds

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.printf("*** Example of LEDPalettes library. Show all palettes");
  if (!oLeds.bInit()) {
    Serial.printf(", WS2812B_RMT initialization error! ***\n");
    while (true) delay(1000);          // stop here
  }

  oLeds.vSetGammaCorrection(true);
  oLeds.vSetBrightness(127);           // about half brightness (0..255); full brightness is not needed
  Serial.printf(" started ***\n\n");   // success
}

void loop() {
  oPalettes.vShowAllPalettes(DELAY_FOR_PALETTE);
}
