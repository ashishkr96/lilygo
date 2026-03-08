# LilyGo E-Paper Property Display

A property-label display for the **LilyGo 4.7" EPD47-S3** (960×540 px, 16-level grayscale).

**What it shows:**
- Owner label and "do not touch" warning
- Current day of week in English + Hindi (Devanagari)
- Full date in English
- Moon phase icon + phase name + Hindu tithi (paksha)

**On touch:** a random joke appears for 10 seconds, then the main screen returns.

---

## Hardware

| Component | Details |
|---|---|
| Display | LilyGo T5 4.7" EPD47 (EPD_WIDTH=960, EPD_HEIGHT=540) |
| MCU | ESP32-S3-WROOM-1 |
| Touch | GT911 via I2C (auto-detected at 0x14 or 0x5D) |
| PSRAM | 8 MB OPI (mandatory) |
| Flash | 16 MB |

Pin assignments (from `utilities.h`):

| Signal | GPIO |
|---|---|
| SDA | 18 |
| SCL | 17 |
| TOUCH_INT | 47 |

---

## Dependencies

Install via **Arduino Library Manager** or manually into `~/Arduino/libraries/`:

| Library | Source |
|---|---|
| `LilyGo-EPD47` | https://github.com/Xinyuan-LilyGO/LilyGo-EPD47 |
| `SensorLib` | bundled with the above (GT911 driver) |

The Devanagari font (`devanagari.h`) is pre-generated and committed — no extra tools needed at build time.

---

## Arduino IDE Settings

| Setting | Value |
|---|---|
| Board | `ESP32S3 Dev Module` |
| **PSRAM** | **`OPI PSRAM`** ← mandatory |
| Flash Size | `16MB (128Mb)` |
| Partition Scheme | `16M Flash (3M APP/9.9MB FATFS)` |
| USB CDC On Boot | `Enable` |
| USB Mode | `Hardware CDC and JTAG` |
| Upload Mode | `UART0/Hardware CDC` |

---

## Build & Upload (arduino-cli)

```bash
# One-time: install ESP32 core
arduino-cli core install esp32:esp32

# Compile
arduino-cli compile \
  --fqbn esp32:esp32:esp32s3:FlashSize=16M,FlashMode=qio,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,USBMode=hwcdc,CDCOnBoot=cdc \
  /path/to/lilygo

# Find the port
arduino-cli board list

# Upload (replace /dev/ttyACM0 with your port)
arduino-cli upload -p /dev/ttyACM0 \
  --fqbn esp32:esp32:esp32s3:FlashSize=16M,FlashMode=qio,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,USBMode=hwcdc,CDCOnBoot=cdc \
  /path/to/lilygo

# Monitor serial output (115200 baud)
arduino-cli monitor -p /dev/ttyACM0 --config baudrate=115200
```

---

## Run Tests

Tests cover the pure algorithms (date, moon phase, moon icon) on the host — no hardware needed.

```bash
python3 tests/test_algorithms.py -v
```

Requires Python 3.6+, no external packages.

---

## Regenerating the Devanagari Font

The font generator requires `freetype-py`:

```bash
pip3 install freetype-py
python3 tools/gen_devanagari.py > devanagari.h 2>/tmp/metrics.txt
```

> **Important:** use `2>` not `2>&1` — mixing stderr into stdout corrupts the header file.

---

## Library Patches

The `LilyGo-EPD47` library needs five patches for ESP32 Arduino Core 3.x (IDF 5.x).
See [`CLAUDE.md`](CLAUDE.md#library-patches) for the exact changes.

---

## Project Structure

```
lilygo/
├── lilygo.ino          Main sketch — setup() + loop() only
├── config.h            Layout constants, timing, owner name
├── types.h             DateInfo / MoonInfo structs
├── date_calc.h/.cpp    Compile-time date parsing + Zeller's congruence
├── moon_calc.h/.cpp    Meeus moon phase + Hindu tithi
├── display.h/.cpp      Framebuffer, draw helpers, renderMain/renderTouched
├── touch_mgr.h/.cpp    GT911 init, read, drain
├── jokes.h             8-joke pool
├── devanagari.h        NotoSansDevanagari-Bold font data (generated)
└── tests/
    └── test_algorithms.py  Host-side unit tests (20 tests)
```
