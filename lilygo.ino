/**
 * LilyGo 4.7" E-Paper (EPD47-S3) — Property Display
 *
 * Shows owner info, day/date in English + Hindi, 7-segment clock, moon phase,
 * and live weather (temperature + condition icon) fetched from wttr.in.
 * Touch anywhere to see a random joke; screen reverts after 10 seconds.
 * Date/time synced via NTP on boot; falls back to compile-time date if unavailable.
 *
 * Board settings (Arduino IDE):
 *   Board            : ESP32S3 Dev Module
 *   PSRAM            : OPI PSRAM          ← mandatory
 *   Flash Size       : 16MB (128Mb)
 *   Partition Scheme : 16M Flash (3M APP/9.9MB FATFS)
 *   USB CDC On Boot  : Enable
 *   USB Mode         : Hardware CDC and JTAG
 */

#ifndef BOARD_HAS_PSRAM
#error "Enable PSRAM: Arduino IDE -> Tools -> PSRAM -> OPI PSRAM"
#endif

#include <Arduino.h>
#include <Wire.h>
#include "utilities.h"   // BOARD_SDA, BOARD_SCL
#include "config.h"
#include "types.h"
#include "date_calc.h"
#include "moon_calc.h"
#include "display.h"
#include "touch_mgr.h"
#include "time_mgr.h"
#include "weather.h"

static DateInfo       currentDi;
static MoonInfo       currentMi;
static WeatherInfo    currentWi   = {};
static int            lastRenderedDay = -1;
static unsigned long  lastCheckMs    = 0;
static unsigned long  lastWeatherMs  = 0;   // millis() of last weather fetch
static int16_t        tx, ty;

// Rebuild currentDi/currentMi from the current NTP time.
// Returns true if getLocalTime() succeeded.
static bool refreshData() {
    struct tm t;
    if (!time_get(&t)) return false;
    buildDateInfoFromTm(&currentDi, &t);
    buildMoonInfo(&currentMi, currentDi.year, currentDi.month, currentDi.day);
    return true;
}

void setup() {
    Serial.begin(115200);
    display_init();

    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED) delay(1000);

    Wire.begin(BOARD_SDA, BOARD_SCL);
    touch_init();

    bool ntpOk = time_sync();
    if (!ntpOk || !refreshData()) {
        Serial.println("Falling back to compile-time date");
        buildDateInfo(&currentDi);
        buildMoonInfo(&currentMi, currentDi.year, currentDi.month, currentDi.day);
    }

    // Initial weather fetch
    weather_fetch(&currentWi);
    lastWeatherMs = millis();

    lastRenderedDay = currentDi.day;
    lastCheckMs     = millis();
    renderMain(&currentDi, &currentMi, &currentWi);
}

void loop() {
    unsigned long now = millis();

    if (now - lastCheckMs >= REDRAW_INTERVAL_MS) {
        lastCheckMs = now;
        if (refreshData()) {
            bool dayChanged     = (currentDi.day != lastRenderedDay);
            bool weatherDue     = (now - lastWeatherMs >= WEATHER_INTERVAL_MS);

            if (dayChanged || weatherDue) {
                // Full render — update weather first if due
                if (weatherDue || !currentWi.valid) {
                    weather_fetch(&currentWi);
                    lastWeatherMs = now;
                }
                lastRenderedDay = currentDi.day;
                renderMain(&currentDi, &currentMi, &currentWi);
            } else {
                renderTimeRegion(&currentDi);   // only changed 7-seg digits
            }
        }
    }

    if (touch_read(&tx, &ty)) {
        Serial.printf("Touched at %d,%d\n", tx, ty);
        renderTouched();
        delay(TOUCH_DISPLAY_MS);
        renderMain(&currentDi, &currentMi, &currentWi);
        touch_drain();
    }

    delay(10);
}
