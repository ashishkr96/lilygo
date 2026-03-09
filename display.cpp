#include <Arduino.h>
#include <math.h>
#include <string.h>
#include "epd_driver.h"
#include "firasans.h"
#include "devanagari.h"
#include "display.h"
#include "config.h"
#include "jokes.h"

// ── Framebuffer ───────────────────────────────────────────────────────────────
uint8_t *framebuffer = NULL;

void display_init() {
    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer) {
        Serial.println("ERROR: framebuffer alloc failed!");
        while (1);
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    epd_init();
}

// ── Text helpers ─────────────────────────────────────────────────────────────

// Render text centered horizontally, drawn twice (x, x+1) for faux-bold.
void drawFira(const char *text, int32_t y) {
    int32_t x = 0, yt = y, x1, y1, w, h;
    get_text_bounds((GFXfont*)&FiraSans, text, &x, &yt, &x1, &y1, &w, &h, NULL);
    int32_t cx;
    cx = (EPD_WIDTH - w) / 2;     yt = y; write_string((GFXfont*)&FiraSans, text, &cx, &yt, framebuffer);
    cx = (EPD_WIDTH - w) / 2 + 1; yt = y; write_string((GFXfont*)&FiraSans, text, &cx, &yt, framebuffer);
}

void drawDeva(const char *text, int32_t y) {
    int32_t x = 0, yt = y, x1, y1, w, h;
    get_text_bounds((GFXfont*)&NotoDevanagari, text, &x, &yt, &x1, &y1, &w, &h, NULL);
    int32_t cx;
    cx = (EPD_WIDTH - w) / 2;     yt = y; write_string((GFXfont*)&NotoDevanagari, text, &cx, &yt, framebuffer);
    cx = (EPD_WIDTH - w) / 2 + 1; yt = y; write_string((GFXfont*)&NotoDevanagari, text, &cx, &yt, framebuffer);
}

void drawRule(int32_t y) {
    epd_fill_rect(60, y, EPD_WIDTH - 120, 4, 0x00, framebuffer);
}

// ── Moon icon ─────────────────────────────────────────────────────────────────
// For each scan row, compute the lit x-range via a cosine terminator ellipse,
// then fill those pixels black.  Circle outline drawn last.
//
//   Waxing (age 0→SYN/2): lit on right.
//     term_x = cx + half_chord * cos(π * age / (SYN/2))
//     age=0  → term_x = xr  → nothing lit   (new moon)
//     age=SYN/4 → term_x = cx → right half  (first quarter)
//     age=SYN/2 → term_x = xl → full disk   (full moon)
//
//   Waning (age SYN/2→SYN): lit on left.
//     term_x = cx + half_chord * cos(π * (age-SYN/2) / (SYN/2))
//     age=SYN/2 → term_x = xr → full disk   (full moon)
//     age=3*SYN/4 → term_x = cx → left half (last quarter)
//     age=SYN  → term_x = xl → nothing lit  (new moon)
void drawMoonIcon(int32_t cx, int32_t cy, int32_t r, float age) {
    const double SYN = 29.530588853;
    for (int dy = -r; dy <= r; dy++) {
        double hc2 = (double)(r * r - dy * dy);
        if (hc2 < 0) continue;
        double half_chord = sqrt(hc2);
        int xl = cx - (int)half_chord;
        int xr = cx + (int)half_chord;
        int lit_l, lit_r;
        if (age <= SYN / 2.0) {
            double term_x = cx + half_chord * cos(M_PI * age / (SYN / 2.0));
            lit_l = (int)term_x;
            lit_r = xr;
        } else {
            double term_x = cx + half_chord * cos(M_PI * (age - SYN / 2.0) / (SYN / 2.0));
            lit_l = xl;
            lit_r = (int)term_x;
        }
        if (lit_l < xl) lit_l = xl;
        if (lit_r > xr) lit_r = xr;
        for (int x = lit_l; x <= lit_r; x++)
            epd_draw_pixel(x, cy + dy, 0x00, framebuffer);
    }
    epd_draw_circle(cx, cy, r, 0x00, framebuffer);
}

// ── Screen renderers ──────────────────────────────────────────────────────────

/*
 * Layout (960 × 540 px) — Y-baselines:
 *
 *  y= 65  "Owner: Ashish Kumar Choubey"   FiraSans (ascender=39)
 *  y=115  "Do not touch this 😤"
 *         ─── divider y=140 ───
 *  y=228  "Sunday"
 *  y=316  "रविवार"                         NotoDevanagari (ascender=56)
 *  y=400  "March 8, 2026"
 *         ─── divider y=428 ───
 *  moon icon  cx=120, cy=484, r=28
 *  y=490  "Waning Gibbous"
 *  y=534  "Krishna Panchami"
 */
void renderMain(const DateInfo *di, const MoonInfo *mi) {
    epd_poweron();
    epd_clear();
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    drawFira(OWNER_NAME,                            Y_OWNER);
    drawFira("Do not touch this \xF0\x9F\x98\xA4", Y_NOTTOUCH);
    drawRule(Y_DIV1);

    drawFira(di->dayName,  Y_DAY_EN);
    drawDeva(di->hindiDay, Y_DAY_HI);
    drawFira(di->timeStr,  Y_TIME);
    drawFira(di->dateStr,  Y_DATE);
    drawRule(Y_DIV2);

    drawMoonIcon(MOON_CX, MOON_CY, MOON_R, mi->age);
    char tithi_line[64];
    snprintf(tithi_line, sizeof(tithi_line), "%s %s", mi->paksha, mi->tithi);
    drawFira(mi->phase,   Y_PHASE);
    drawFira(tithi_line, Y_TITHI);

    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    epd_poweroff();

    Serial.printf("Main: %s | %s | Moon %.1fd %d%% %s | %s %s\n",
                  di->dayName, di->dateStr,
                  mi->age, mi->illum, mi->phase,
                  mi->paksha, mi->tithi);
}

void renderTouched() {
    int idx = random(NUM_JOKES);
    const char **j = JOKES[idx];

    epd_poweron();
    epd_clear();
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    drawFira("You touched it, didn't you... \xF0\x9F\x98\x8F", 120);
    drawFira(j[0], 220);
    drawFira(j[1], 310);
    drawFira(j[2], 400);

    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    epd_poweroff();
    Serial.printf("Joke #%d shown\n", idx);
}
