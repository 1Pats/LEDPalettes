/*
 *  LED palettes
 *
 * Class CLedPalettes selects a palette, maps it to the LED strip and shows it.
 * A palette is located in the bPalettes array (Palettes.h) by its name (enum class PaletteName).
 * Each palette is enclosed in "brackets":
 *    0xff, 0xff, 0xff,   - start of the palette
 *    R, G, B, ...        - palette colors
 *    0x00, 0x00, 0x00,   - end of the palette
 * One more 0x00, 0x00, 0x00 triplet, placed where the next palette would start, ends the list of palettes.
 * Palettes are numbered from 1, in the same order as in enum class PaletteName.
 *
 * Usage:
 *    CWs2812bRmt  oLeds(LED_PIN, NUMBER_OF_LEDS);
 *    CLedPalettes oPalettes(oLeds, NUMBER_OF_LEDS);
 *    ...
 *    if (oPalettes.pbGetPalette(PaletteName::FIRE) != nullptr) oPalettes.vShowActivePalette();
 *
 * Hungarian notation prefixes:
 *   b  - byte / uint8_t          p  - pointer (pb - pointer to byte)
 *   bo - bool                    sz - size_t
 *   en - enum                    o  - object
 *   ul - unsigned long           m_ - class member
 *   v  - void (function returns nothing)
 *   C  - class name
 */

#ifndef  _LED_PALETTES
#define  _LED_PALETTES

#include <Arduino.h>
#include <WS2812B_RMT.h>
#include "Palettes.h"

class CLedPalettes {
public:
  CLedPalettes(CWs2812bRmt &oLeds, size_t szNumLeds);
  ~CLedPalettes();
  CLedPalettes(const CLedPalettes &) = delete;   // owns a malloc'ed buffer: no copies
  CLedPalettes &operator=(const CLedPalettes &) = delete;

  size_t      szSelectPalette(PaletteName enName);
  const byte *pbMapToLedStrip(size_t szTargetSize);
  const byte *pbGetPalette(PaletteName enName);
  void        vSetPixelFromPalette(size_t szLed, size_t szColor);
  void        vShowActivePalette();
  void        vShowAllPalettes(unsigned long ulDelayMs);

  // mapped palette, 3 bytes (RGB) per color
  const byte *pbGetLedPalette() const { return m_pbLedPalette; }
  // number of colors in the mapped palette
  size_t      szGetLedPaletteSize() const { return m_szLedPalette; }

private:
  static bool boIsTriplet(const byte *pbTriplet, byte bValue);

  CWs2812bRmt &m_oLeds;                // LED strip the palette is shown on
  const size_t m_szNumLeds;            // number of LEDs on the strip
  const byte  *m_pbPalette    = nullptr;         // first color (triplet) of the selected palette
  size_t       m_szPalette    = 0;     // number of colors in the selected palette
  byte        *m_pbLedPalette = nullptr;         // palette mapped to the LED strip (pbMapToLedStrip)
  size_t       m_szLedPalette = 0;     // number of colors in m_pbLedPalette
};

#endif
