# Release Notes — LEDPalettes

## V1.0.0

Initial release.

- Class `CLedPalettes`: select a palette by name and map it to the length of the LED strip
- Expanding uses linear interpolation; squeezing averages the replaced colors
- 13 built-in palettes in `Palettes.h`, selected via `enum class PaletteName`
- Example: `AllPalettes`

## Notes

- Requires the WS2812B_RMT library and Arduino-ESP32 core 3.x.
- The mapped palette uses a heap buffer of `LEDs × 3` bytes.
