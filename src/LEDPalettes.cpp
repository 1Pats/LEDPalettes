/*
 *  LED palettes - class CLedPalettes implementation (see LEDPalettes.h)
 */

#include "LEDPalettes.h"

CLedPalettes::CLedPalettes(CWs2812bRmt &oLeds, size_t szNumLeds)
  : m_oLeds(oLeds), m_szNumLeds(szNumLeds) {}

CLedPalettes::~CLedPalettes() {
  free(m_pbLedPalette);
}

bool CLedPalettes::boIsTriplet(const byte *pbTriplet, byte bValue) {
  return pbTriplet[0] == bValue && pbTriplet[1] == bValue && pbTriplet[2] == bValue;
}

/*
 * Selects palette enName from bPalettes.
 * Sets m_pbPalette and m_szPalette and returns the number of colors in the palette.
 * Returns 0 if the palette is not found (m_pbPalette = nullptr, m_szPalette = 0).
 */
size_t CLedPalettes::szSelectPalette(PaletteName enName) {
  const size_t  szLen    = sizeof(bPalettes);
  const uint8_t bNumber  = static_cast<uint8_t>(enName);
  size_t        szIdx    = 0;
  uint8_t       bCurrent = 0;

  m_pbPalette = nullptr;
  m_szPalette = 0;
  if (bNumber == 0) return 0;          // palettes are numbered from 1

  while (szIdx + 3 <= szLen) {
    if (!boIsTriplet(&bPalettes[szIdx], 0xFF)) return 0;   // no start mark: end of all palettes
    szIdx += 3;                        // skip the start mark
    bCurrent++;                        // next palette
    const size_t szStart = szIdx;      // first color of the palette

    while (szIdx + 3 <= szLen && !boIsTriplet(&bPalettes[szIdx], 0x00)) {
      szIdx += 3;                      // seek the end mark
    }
    if (szIdx + 3 > szLen) return 0;   // end mark is missing

    if (bCurrent == bNumber) {         // palette found
      m_pbPalette = &bPalettes[szStart];
      m_szPalette = (szIdx - szStart) / 3;
      return m_szPalette;
    }
    szIdx += 3;                        // skip the end mark
  }
  return 0;
}

/*
 * Expands or squeezes the selected palette (m_pbPalette, m_szPalette) to szTargetSize colors.
 * The result is stored in m_pbLedPalette (m_szLedPalette colors).
 * The buffer is reused if its size has not changed, otherwise it is reallocated.
 * Expanding: colors are linearly interpolated, the first and the last colors are kept.
 * Squeezing: each new color is the average of the source colors it replaces.
 * If the palette is not selected or memory allocation fails, returns nullptr
 * (m_pbLedPalette = nullptr, m_szLedPalette = 0).
 */
const byte *CLedPalettes::pbMapToLedStrip(size_t szTargetSize) {
  if (m_pbPalette == nullptr || m_szPalette == 0) szTargetSize = 0;

  if (szTargetSize != m_szLedPalette) {
    free(m_pbLedPalette);              // free(nullptr) does nothing: safe on the first call
    m_pbLedPalette = (szTargetSize == 0) ? nullptr : static_cast<byte *>(malloc(szTargetSize * 3));
    m_szLedPalette = (m_pbLedPalette == nullptr) ? 0 : szTargetSize;
  }
  if (m_pbLedPalette == nullptr) return nullptr; // nothing to map or out of memory

  for (size_t szColor = 0; szColor < szTargetSize; szColor++) {
    byte *pbTarget = &m_pbLedPalette[szColor * 3];

    if (m_szPalette == 1) {            // single color: nothing to interpolate
      memcpy(pbTarget, m_pbPalette, 3);
    } else if (szTargetSize >= m_szPalette) {    // expand: interpolate between neighbors
      const size_t szDen   = szTargetSize - 1;
      const size_t szPos   = szColor * (m_szPalette - 1);  // position in the source, scaled by szDen
      const size_t szIdx   = szPos / szDen;      // source color on the left
      const size_t szFrac  = szPos % szDen;      // distance from it, scaled by szDen
      const byte  *pbLeft  = &m_pbPalette[szIdx * 3];
      const byte  *pbRight = (szFrac == 0) ? pbLeft : pbLeft + 3;
      for (uint8_t bRgb = 0; bRgb < 3; bRgb++) {
        pbTarget[bRgb] = (pbLeft[bRgb] * (szDen - szFrac) + pbRight[bRgb] * szFrac + szDen / 2) / szDen;
      }
    } else {                           // squeeze: average a group of source colors
      // source colors szFirst .. szLast - 1
      const size_t szFirst = szColor * m_szPalette / szTargetSize;
      const size_t szLast  = (szColor + 1) * m_szPalette / szTargetSize;
      const size_t szCount = szLast - szFirst;
      for (uint8_t bRgb = 0; bRgb < 3; bRgb++) {
        size_t szSum = 0;
        for (size_t szIdx = szFirst; szIdx < szLast; szIdx++) szSum += m_pbPalette[szIdx * 3 + bRgb];
        pbTarget[bRgb] = (szSum + szCount / 2) / szCount;
      }
    }
  }
  return m_pbLedPalette;
}

/*
 * Selects palette enName and maps it to the LED strip (m_szNumLeds colors).
 * Returns the mapped palette, or nullptr if the palette is not found or memory allocation fails.
 */
const byte *CLedPalettes::pbGetPalette(PaletteName enName) {
  if (szSelectPalette(enName) == 0) return nullptr;
  return pbMapToLedStrip(m_szNumLeds);
}

/*
 * Sets LED szLed to color szColor of the mapped palette.
 * Does nothing if the palette is not mapped or szColor is out of range.
 * Call vShowActivePalette() or the strip's vShow() to display the change.
 */
void CLedPalettes::vSetPixelFromPalette(size_t szLed, size_t szColor) {
  if (m_pbLedPalette == nullptr || szColor >= m_szLedPalette) return;
  const byte *pbColor = &m_pbLedPalette[szColor * 3];
  m_oLeds.vSetPixel(szLed, pbColor[0], pbColor[1], pbColor[2]);
}

/*
 * Shows the mapped palette on the LED strip.
 */
void CLedPalettes::vShowActivePalette() {
  if (m_pbLedPalette == nullptr) return;
  for (size_t szLed = 0; szLed < m_szLedPalette; szLed++) {
    vSetPixelFromPalette(szLed, szLed);
  }
  m_oLeds.vShow();
}

/*
 * Shows all palettes one by one, each for ulDelayMs ms,
 * then keeps the strip dark for ulDelayMs * 3 ms.
 */
void CLedPalettes::vShowAllPalettes(unsigned long ulDelayMs) {
  const uint8_t bLast = static_cast<uint8_t>(PaletteName::LAST);

  for (uint8_t bPalette = 1; bPalette <= bLast; bPalette++) {
    if (pbGetPalette(static_cast<PaletteName>(bPalette)) == nullptr) {
      continue;                        // palette not found or out of memory
    }
    vShowActivePalette();
    delay(ulDelayMs);
  }
  m_oLeds.vClear();
  m_oLeds.vShow();                     // vClear() clears the buffer only
  delay(ulDelayMs * 3);
}
