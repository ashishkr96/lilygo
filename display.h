#pragma once
#include <stdint.h>
#include "types.h"

// Shared framebuffer — allocated in display_init(), used by all draw calls.
extern uint8_t *framebuffer;

// Must be called once in setup() before any draw/render functions.
void display_init();

// Centered bold text helpers
void drawFira(const char *text, int32_t y);
void drawDeva(const char *text, int32_t y);   // NotoDevanagari font
void drawRule(int32_t y);                      // full-width horizontal rule

// Moon phase icon: lit side right (waxing) or left (waning).
// cx/cy = circle centre, r = radius, age = days since new moon.
void drawMoonIcon(int32_t cx, int32_t cy, int32_t r, float age);

// Full-screen render passes
void renderMain();     // property header + date + moon section
void renderTouched();  // random joke screen
