# LEDPalettes

Color palettes for WS2812B LED strips driven by the [WS2812B_RMT](https://github.com/1Pats/WS2812B_RMT) library.

Select a palette by name with `pbGetPalette()`, and the library stretches or squeezes it to the length of your
LED strip. With a 100-LED strip you always get 100 colors, whatever palette you pick.

## ✅ Requirements

- ESP32 board (dual-core recommended)
- Arduino-ESP32 core **3.x**
- [WS2812B_RMT](https://github.com/1Pats/WS2812B_RMT) library

## 🚀 Quick Start

```cpp
#include <WS2812B_RMT.h>
#include <LEDPalettes.h>

CWs2812bRmt  oLeds(18, 100);           // GPIO, number of LEDs
CLedPalettes oPalettes(oLeds, 100);    // palettes shown on oLeds

void setup() {
    if (!oLeds.bInit()) while (true);
    oLeds.vSetBrightness(64);
    if (oPalettes.pbGetPalette(PaletteName::FIRE) != nullptr) oPalettes.vShowActivePalette();
}

void loop() {}
```

See `examples/AllPalettes` for a demo that cycles through every palette.

## 🎨 Built-in Palettes

| # | Name | # | Name |
|---|------|---|------|
| 1 | `RED` | 8 | `BLUESERENITY` |
| 2 | `GREEN` | 9 | `GREENHARMONY` |
| 3 | `BLUE` | 10 | `CHRISTMAS` |
| 4 | `YELLOW` | 11 | `RALPHLAURENCHRISTMAS` |
| 5 | `WATER` | 12 | `FIRE` |
| 6 | `OCEANBLUESERENITY` | 13 | `RAINBOW` |
| 7 | `CREAMY` | | |

`PaletteName::LAST` always equals the last palette.

## 📚 API

| Method | Description |
|--------|-------------|
| `CLedPalettes(CWs2812bRmt &oLeds, size_t szNumLeds)` | Create a palette object for a strip |
| `const byte *pbGetPalette(PaletteName enName)` | Select a palette and map it to the strip; returns `nullptr` if not found or out of memory |
| `size_t szSelectPalette(PaletteName enName)` | Select a palette only; returns its number of colors, 0 if not found |
| `const byte *pbMapToLedStrip(size_t szTargetSize)` | Stretch or squeeze the selected palette to `szTargetSize` colors |
| `void vSetPixelFromPalette(size_t szLed, size_t szColor)` | Set one LED to a color of the mapped palette (call `vShow()` afterwards) |
| `void vShowActivePalette()` | Show the mapped palette on the strip |
| `void vShowAllPalettes(unsigned long ulDelayMs)` | Show every palette in turn, each for `ulDelayMs` ms, then blank the strip |
| `const byte *pbGetLedPalette() const` | Mapped palette, 3 bytes per color in **RGB** order |
| `size_t szGetLedPaletteSize() const` | Number of colors in the mapped palette |

## 🔧 How Mapping Works

- **Expanding** (strip longer than the palette): colors are linearly interpolated; the first and last colors are kept.
- **Squeezing** (strip shorter than the palette): each new color is the average of the source colors it replaces.
- The mapped palette is held in one heap buffer of `LEDs × 3` bytes, reused while the size stays the same.

## ➕ Adding Your Own Palette

Palettes live in `src/Palettes.h`, in the `bPalettes` array (stored in flash). Each palette is
wrapped in "brackets":

```cpp
0xff, 0xff, 0xff,      // start of palette
R, G, B,               // any number of colors
R, G, B,
0x00, 0x00, 0x00,      // end of palette
```

One more `0x00, 0x00, 0x00` triplet after the last palette ends the list.
When you add or remove a palette, update `enum class PaletteName` so the numbers match the
order in the array, and keep `LAST` equal to the last palette.

## 📄 License

MIT License — see [LICENSE](LICENSE).
