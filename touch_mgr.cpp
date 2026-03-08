#include <Arduino.h>
#include <Wire.h>
#include <TouchDrvGT911.hpp>
#include "utilities.h"
#include "epd_driver.h"
#include "touch_mgr.h"

static TouchDrvGT911 touch;

void touch_init() {
    pinMode(TOUCH_INT, OUTPUT);
    digitalWrite(TOUCH_INT, HIGH);

    // Auto-detect GT911 I2C address
    uint8_t touchAddr = 0;
    Wire.beginTransmission(0x14); if (Wire.endTransmission() == 0) touchAddr = 0x14;
    Wire.beginTransmission(0x5D); if (Wire.endTransmission() == 0) touchAddr = 0x5D;

    if (!touchAddr) {
        Serial.println("WARNING: GT911 not found");
        return;
    }

    Serial.printf("GT911 at 0x%02X\n", touchAddr);
    touch.setPins(-1, TOUCH_INT);
    if (touch.begin(Wire, touchAddr, BOARD_SDA, BOARD_SCL)) {
        touch.setMaxCoordinates(EPD_WIDTH, EPD_HEIGHT);
        touch.setSwapXY(true);
        touch.setMirrorXY(false, true);
        Serial.println("Touch ready.");
    }
}

bool touch_read(int16_t *x, int16_t *y) {
    return touch.getPoint(x, y) > 0;
}

void touch_drain() {
    delay(150);
    int16_t dx, dy;
    while (touch.getPoint(&dx, &dy)) { delay(5); }
    delay(100);
}
