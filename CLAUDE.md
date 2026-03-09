# CLAUDE.md — AI Assistant Guide for lilygo

This file captures everything an AI assistant needs to work effectively in this repo.

---

## Project Summary

Arduino sketch for **LilyGo 4.7" EPD47-S3** (ESP32-S3, 960×540 grayscale e-paper).
Displays owner info, day/date (English + Hindi), moon phase icon, and random jokes on touch.

---

## Build Command

```bash
arduino-cli compile \
  --fqbn esp32:esp32:esp32s3:FlashSize=16M,FlashMode=qio,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,USBMode=hwcdc,CDCOnBoot=cdc \
  /home/ashish/Desktop/personal/lilygo
```

Upload:
```bash
arduino-cli upload -p /dev/ttyACM0 \
  --fqbn esp32:esp32:esp32s3:FlashSize=16M,FlashMode=qio,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,USBMode=hwcdc,CDCOnBoot=cdc \
  /home/ashish/Desktop/personal/lilygo
```

Run tests (no hardware required):
```bash
python3 tests/test_algorithms.py -v
```

---

## File Map

| File | Purpose |
|---|---|
| `lilygo.ino` | `setup()` + `loop()` only — entry point |
| `config.h` | All layout Y-positions, moon icon params, timing constants |
| `credentials.h` | WiFi SSID + password — **gitignored, never committed** |
| `credentials.h.example` | Dummy template committed to repo; copy → `credentials.h` and fill in |
| `types.h` | `DateInfo` and `MoonInfo` structs |
| `date_calc.h/.cpp` | Compile-time date parsing; Zeller's congruence for DOW |
| `moon_calc.h/.cpp` | Meeus synodic moon algorithm + Hindu tithi |
| `display.h/.cpp` | Framebuffer, all draw primitives, `renderMain()`, `renderTouched()` |
| `touch_mgr.h/.cpp` | GT911 init, `touch_read()`, `touch_drain()` |
| `jokes.h` | 8 three-line jokes |
| `devanagari.h` | NotoSansDevanagari-Bold 30pt font (generated, ~200 KB) |
| `tests/test_algorithms.py` | 20 host-side Python unit tests |

---

## Key Constraints

### Arduino struct ordering
Arduino's preprocessor does a single forward-pass to generate function prototypes.
**Structs used as function parameters/return types must be defined BEFORE any function
definitions** in `.ino` files. In this project, structs live in `types.h` which is included
at the top of every `.cpp`.

### PSRAM is mandatory
`framebuffer` is allocated with `ps_calloc()` — requires OPI PSRAM enabled in the board config.
The `#ifndef BOARD_HAS_PSRAM` guard will cause a compile-time error if it's not set.

### Compile-time date
`buildDateInfo()` parses `__DATE__` (e.g., `"Mar  8 2026"`). The displayed date is the
**compile date**, not a real-time clock. Re-flash to update the date.

### EPD drawing conventions
- Framebuffer: `uint8_t`, 4 bits per pixel, `0x00` = black, `0xFF` = white
- Size: `EPD_WIDTH * EPD_HEIGHT / 2` bytes = `960 * 540 / 2` = 259,200 bytes
- Always `memset(framebuffer, 0xFF, ...)` before drawing (white background)
- Always bracket drawing with `epd_poweron()` … `epd_poweroff()`

### Faux-bold text
`write_string()` is called twice: at `cx` then `cx+1`. This shifts pixels one pixel right
to simulate bold. `get_text_bounds()` is called first to compute centered `cx`.

### Moon icon algorithm
For each scan row `dy` from `-r` to `r`:
- `half_chord = sqrt(r² - dy²)`
- Waxing: `term_x = cx + half_chord * cos(π * age / (SYN/2))`, fill `[term_x, xr]`
- Waning: `term_x = cx + half_chord * cos(π * (age-SYN/2) / (SYN/2))`, fill `[xl, term_x]`
- Draw circle outline with `epd_draw_circle()` after filling

### Julian Day formula
The function returns the **noon** integer JDN (uses `-1524`, not `-1524.5`).
Do **NOT** add `+0.5` in `buildMoonInfo()` — doing so shifts moon age by ~0.5 days,
causing the tithi to be off by one.

### Touch double-fire fix
After `renderMain()` returns, the GT911 FIFO still holds the touch event that triggered
the joke screen. `touch_drain()` must be called after `renderMain()` in `loop()`:
```cpp
delay(150);
while (touch.getPoint(&dx, &dy)) { delay(5); }
delay(100);
```

---

## Library Patches

All patches are in `/home/ashish/Arduino/libraries/LilyGo-EPD47/src/`.
Required for **ESP32 Arduino Core 3.3.6 (IDF 5.x)** compatibility.

### 1. `ed047tc1.h` — add `esp_attr.h`
```cpp
#include <esp_attr.h>   // fixes IRAM_ATTR undefined
```

### 2. `ed047tc1.c` — add GPIO struct header
```cpp
#include <soc/gpio_struct.h>   // fixes GPIO.out_w1ts undefined
```

### 3. `libjpeg/libjpeg.c` — fix JPEG callback types
Change `uint32_t` → `UINT` and `uint8_t *` → `BYTE *` in `feed_buffer` and `tjd_output`
signatures and local variable `count` to match ROM `tjpgd.h`.

### 4. `rmt_pulse.c` — rewrite for new RMT TX API
Legacy `rmt_config_t` / `rmt_driver_install` conflicts with IDF 5 `driver_ng`.
Use `rmt_new_tx_channel` + `rmt_new_copy_encoder` + `rmt_transmit` instead.
Requires `#include <freertos/FreeRTOS.h>` for `portMAX_DELAY`.

### 5. `i2s_data_bus.c` — add `clk_src` to LCD bus config
```cpp
esp_lcd_i80_bus_config_t bus_config = {
    .clk_src = LCD_CLK_SRC_DEFAULT,   // was missing, caused "unknown clock source"
    // ...
};
```

---

## Font Notes

### FiraSans (built-in)
- 40pt @ 150dpi
- `advance_y = 50`, ascender ≈ 39
- English + emoji (U+0000–U+FFFF subset)

### NotoDevanagari (generated)
- NotoSansDevanagari-Bold, 30pt @ 150dpi
- `advance_y = 82`, ascender = 56
- Generated with `freetype-py`; script at `tools/gen_devanagari.py`
- Pre-base vowel ि (U+093F) renders slightly ahead of base consonant — acceptable
  without OpenType shaping engine

---

## WiFi Credentials

WiFi credentials are **never committed**. The pattern:

- `credentials.h` — real credentials, lives only on the device owner's machine, gitignored
- `credentials.h.example` — dummy template committed to the repo

To set up on a new machine:
```bash
cp credentials.h.example credentials.h
# edit credentials.h and fill in real SSID / password
```

`config.h` includes `credentials.h` for the `WIFI_SSID` and `WIFI_PASSWORD` defines.

---

## Known Limitations

- Date/time synced via NTP on boot; falls back to compile-time date if WiFi unavailable
- Display refreshes every 60s to update the clock (EPD is slow ~2s per refresh)
- Devanagari pre-base vowels may render out of position (no OpenType shaping)
- EPD refresh is slow (~2s); avoid rapid successive redraws
- `random()` seeding uses Arduino's default (reset-deterministic); jokes will appear
  in the same sequence after each power cycle
