#pragma once
#include <stdint.h>

// Probe I2C bus for GT911 at 0x14 or 0x5D, configure axes and interrupt pin.
// Must be called after Wire.begin().
void touch_init();

// Read the oldest pending touch point into *x/*y.
// Returns true if a touch is available.
bool touch_read(int16_t *x, int16_t *y);

// Consume all buffered touch events from the GT911 FIFO.
// Call after renderMain() to prevent double-fire.
void touch_drain();
