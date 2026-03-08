# Program Flow

## Boot Sequence

```
Power on / reset
      │
      ▼
setup()
  ├─ Serial.begin(115200)
  ├─ display_init()
  │     ├─ ps_calloc(EPD_WIDTH * EPD_HEIGHT / 2)   → framebuffer in PSRAM
  │     └─ epd_init()                               → EPD hardware init
  │
  ├─ (if waking from deep sleep) delay(1000)
  │
  ├─ Wire.begin(BOARD_SDA=18, BOARD_SCL=17)
  ├─ touch_init()
  │     ├─ TOUCH_INT pin → HIGH
  │     ├─ probe I2C 0x14 then 0x5D
  │     └─ touch.begin() + setMaxCoordinates + setSwapXY + setMirrorXY
  │
  └─ renderMain()                                   → first screen draw
```

---

## Main Loop

```
loop() [called repeatedly]
      │
      ▼
touch_read(&tx, &ty)
      │
   touch?
   ├── NO  → delay(10ms) → repeat
   │
   └── YES
         │
         ▼
    renderTouched()
      ├─ pick random joke index (0–7)
      ├─ epd_poweron() → epd_clear() → memset framebuffer 0xFF
      ├─ drawFira("You touched it…", y=120)
      ├─ drawFira(joke line 1,        y=220)
      ├─ drawFira(joke line 2,        y=310)
      ├─ drawFira(joke line 3,        y=400)
      ├─ epd_draw_grayscale_image()
      └─ epd_poweroff()
         │
         ▼
    delay(10000 ms)
         │
         ▼
    renderMain()   (see below)
         │
         ▼
    touch_drain()
      ├─ delay(150ms)
      ├─ poll touch.getPoint() until empty
      └─ delay(100ms)
         │
         ▼
    delay(10ms) → repeat loop
```

---

## renderMain() Detail

```
renderMain()
  │
  ├─ buildDateInfo()
  │     ├─ parse __DATE__ macro  ("Mar  8 2026")
  │     ├─ Zeller's congruence   → dayName, hindiDay
  │     └─ format dateStr        ("March 8, 2026")
  │
  ├─ buildMoonInfo()
  │     ├─ julianDay(y,m,d)      → noon JDN
  │     ├─ age = (JDN - REF) mod SYN
  │     ├─ illum = (1 - cos(2π·age/SYN)) / 2
  │     ├─ phase name            (New Moon … Waning Crescent)
  │     └─ tithi index           → paksha + tithi name
  │
  ├─ epd_poweron() → epd_clear() → memset 0xFF
  │
  ├─ drawFira("Owner: Ashish Kumar Choubey",  y=65)
  ├─ drawFira("Do not touch this 😤",          y=115)
  ├─ drawRule(y=140)
  │
  ├─ drawFira(dayName,   y=228)   — e.g. "Sunday"
  ├─ drawDeva(hindiDay,  y=316)   — e.g. "रविवार"  (NotoDevanagari)
  ├─ drawFira(dateStr,   y=400)   — e.g. "March 8, 2026"
  ├─ drawRule(y=428)
  │
  ├─ drawMoonIcon(cx=120, cy=484, r=28, age)
  │     ├─ for each scan row dy in [-r, r]:
  │     │     half_chord = √(r²-dy²)
  │     │     if waxing: term_x = cx + half_chord·cos(π·age / (SYN/2))
  │     │                lit region = [term_x, xr]
  │     │     if waning: term_x = cx + half_chord·cos(π·(age-SYN/2) / (SYN/2))
  │     │                lit region = [xl, term_x]
  │     │     → fill lit pixels black
  │     └─ draw circle outline
  │
  ├─ drawFira(phase,       y=490)  — e.g. "Waning Gibbous"
  ├─ drawFira(tithi_line,  y=534)  — e.g. "Krishna Panchami"
  │
  ├─ epd_draw_grayscale_image(epd_full_screen(), framebuffer)
  └─ epd_poweroff()
```

---

## Screen Layout (960 × 540 px)

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │ y=0
│          Owner: Ashish Kumar Choubey          (y=65)        │
│               Do not touch this 😤            (y=115)       │
├─────────────────────────────────────────────────────────────┤ y=140
│                                                             │
│                       Sunday                  (y=228)       │
│                       रविवार                  (y=316)       │
│                    March 8, 2026              (y=400)       │
│                                                             │
├─────────────────────────────────────────────────────────────┤ y=428
│                                                             │
│   🌘   Waning Gibbous                         (y=490)       │
│        Krishna Panchami                       (y=534)       │
└─────────────────────────────────────────────────────────────┘ y=540
```

---

## Module Dependency Graph

```
lilygo.ino
  ├── display.h / display.cpp
  │     ├── config.h
  │     ├── types.h
  │     ├── date_calc.h / date_calc.cpp
  │     ├── moon_calc.h / moon_calc.cpp
  │     ├── jokes.h
  │     ├── firasans.h          (LilyGo-EPD47)
  │     ├── devanagari.h        (generated font)
  │     └── epd_driver.h        (LilyGo-EPD47)
  │
  ├── touch_mgr.h / touch_mgr.cpp
  │     ├── TouchDrvGT911.hpp   (SensorLib)
  │     ├── utilities.h         (LilyGo-EPD47 pin defs)
  │     └── epd_driver.h
  │
  └── utilities.h               (BOARD_SDA, BOARD_SCL)
```

---

## Date Calculation: Zeller's Congruence

```
Input: year, month, day
  if month < 3: month += 12, year -= 1
  k = year % 100
  j = year / 100
  h = (day + 13*(month+1)/5 + k + k/4 + j/4 - 2*j) % 7

h: 0=Sat 1=Sun 2=Mon 3=Tue 4=Wed 5=Thu 6=Fri
```

## Moon Age Calculation

```
JDN = julianDay(year, month, day)      # noon integer Julian Day Number
age = (JDN - 2451550.09765) mod 29.530588853   # days since ref new moon
illum% = (1 - cos(2π·age/SYN)) / 2 * 100

Tithi index = floor(age / (SYN/30))
  0–14 → Shukla paksha (TITHI_S[index])
  15–29 → Krishna paksha (TITHI_K[index-15])
```
