/**
 * LilyGo 4.7" E-Paper (EPD47-S3) — Property Display
 *
 * Shows owner info, day/date in English + Hindi, and a graphical moon phase.
 * Touch anywhere to see a random joke; screen reverts after 10 seconds.
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
#include "display.h"
#include "touch_mgr.h"

void setup() {
    Serial.begin(115200);
    display_init();

    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED) delay(1000);

    Wire.begin(BOARD_SDA, BOARD_SCL);
    touch_init();

    renderMain();
}

static int16_t tx, ty;

void loop() {
    if (touch_read(&tx, &ty)) {
        Serial.printf("Touched at %d,%d\n", tx, ty);
        renderTouched();
        delay(TOUCH_DISPLAY_MS);
        renderMain();
        touch_drain();
    }
    delay(10);
}
