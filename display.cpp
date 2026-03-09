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

// ── Partial-refresh state ─────────────────────────────────────────────────────
static char prevTimeStr[20] = "";

void display_init() {
    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer) {
        Serial.println("ERROR: framebuffer alloc failed!");
        while (1);
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    epd_init();
}

// ── Text helpers ──────────────────────────────────────────────────────────────

// Full-width centered bold FiraSans text.
void drawFira(const char *text, int32_t y) {
    int32_t x = 0, yt = y, x1, y1, w, h;
    get_text_bounds((GFXfont*)&FiraSans, text, &x, &yt, &x1, &y1, &w, &h, NULL);
    int32_t cx;
    cx = (EPD_WIDTH - w) / 2;     yt = y; write_string((GFXfont*)&FiraSans, text, &cx, &yt, framebuffer);
    cx = (EPD_WIDTH - w) / 2 + 1; yt = y; write_string((GFXfont*)&FiraSans, text, &cx, &yt, framebuffer);
}

// Column-centred bold FiraSans text (col_cx = desired x-centre of the string).
static void drawFiraCol(const char *text, int32_t col_cx, int32_t y) {
    int32_t x = 0, yt = y, x1, y1, w, h;
    get_text_bounds((GFXfont*)&FiraSans, text, &x, &yt, &x1, &y1, &w, &h, NULL);
    int32_t cx;
    cx = col_cx - w / 2;     yt = y; write_string((GFXfont*)&FiraSans, text, &cx, &yt, framebuffer);
    cx = col_cx - w / 2 + 1; yt = y; write_string((GFXfont*)&FiraSans, text, &cx, &yt, framebuffer);
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
            lit_l = (int)term_x; lit_r = xr;
        } else {
            double term_x = cx + half_chord * cos(M_PI * (age - SYN / 2.0) / (SYN / 2.0));
            lit_l = xl; lit_r = (int)term_x;
        }
        if (lit_l < xl) lit_l = xl;
        if (lit_r > xr) lit_r = xr;
        for (int x = lit_l; x <= lit_r; x++)
            epd_draw_pixel(x, cy + dy, 0x00, framebuffer);
    }
    epd_draw_circle(cx, cy, r, 0x00, framebuffer);
}

// ── Weather icon ──────────────────────────────────────────────────────────────

static bool condHas(const char *cond, const char *kw) {
    char buf[48]; int i = 0;
    while (cond[i] && i < 47) { buf[i] = tolower((unsigned char)cond[i]); i++; }
    buf[i] = '\0';
    return strstr(buf, kw) != NULL;
}

// Cloud silhouette: three filled circles + flat base rect.
static void drawCloud(int32_t cx, int32_t cy, int32_t r) {
    epd_fill_circle(cx,        cy,        r * 2 / 3, 0x00, framebuffer);
    epd_fill_circle(cx - r/2,  cy + r/5,  r / 2,     0x00, framebuffer);
    epd_fill_circle(cx + r/2,  cy + r/5,  r / 2,     0x00, framebuffer);
    epd_fill_rect(cx - r * 3/4, cy + r/5, r * 3/2, r * 2/5, 0x00, framebuffer);
}

void drawWeatherIcon(const char *cond, int32_t cx, int32_t cy, int32_t r) {
    if (condHas(cond, "thunder") || condHas(cond, "storm")) {
        // Cloud + lightning bolt
        drawCloud(cx, cy - r / 4, r);
        // Zigzag bolt: three points
        int bx = cx, by = cy + r / 4;
        epd_draw_line(bx + r/4, by,        bx - r/8, by + r/3,   0x00, framebuffer);
        epd_draw_line(bx - r/8, by + r/3,  bx + r/4, by + r/3,   0x00, framebuffer);
        epd_draw_line(bx + r/4, by + r/3,  bx - r/4, by + r*2/3, 0x00, framebuffer);
        // Thicken bolt
        epd_draw_line(bx + r/4+1, by,      bx - r/8+1, by + r/3,   0x00, framebuffer);
        epd_draw_line(bx + r/4+1, by + r/3, bx - r/4+1, by + r*2/3, 0x00, framebuffer);

    } else if (condHas(cond, "snow") || condHas(cond, "sleet") || condHas(cond, "blizzard")) {
        // Cloud + snowflake dots
        drawCloud(cx, cy - r / 4, r);
        for (int i = -2; i <= 2; i++) {
            int sx = cx + i * r / 3, sy = cy + r * 2 / 3;
            epd_fill_rect(sx - 2, sy - 2, 5, 5, 0x00, framebuffer);
        }

    } else if (condHas(cond, "rain") || condHas(cond, "drizzle") || condHas(cond, "shower")) {
        // Cloud + rain drops
        drawCloud(cx, cy - r / 4, r);
        for (int i = -2; i <= 2; i++) {
            int rx2 = cx + i * r / 3, ry = cy + r / 2;
            epd_draw_vline(rx2, ry, r / 3, 0x00, framebuffer);
            epd_draw_vline(rx2 + 1, ry, r / 3, 0x00, framebuffer);
        }

    } else if (condHas(cond, "fog") || condHas(cond, "mist") || condHas(cond, "haze")) {
        // Horizontal stripes
        for (int i = -2; i <= 2; i++) {
            int fy = cy + i * r / 3;
            epd_draw_hline(cx - r, fy,     r * 2, 0x00, framebuffer);
            epd_draw_hline(cx - r, fy + 1, r * 2, 0x00, framebuffer);
        }

    } else if (condHas(cond, "partly") || condHas(cond, "scattered") || condHas(cond, "broken")) {
        // Small sun upper-left + cloud lower-right
        int sr = r * 2 / 5;
        epd_fill_circle(cx - r/3, cy - r/3, sr, 0x00, framebuffer);
        epd_draw_circle(cx - r/3, cy - r/3, sr + 2, 0x00, framebuffer);
        // Sun rays (4 cardinal)
        for (int a = 0; a < 4; a++) {
            float ang = a * M_PI / 2.0f;
            int x1 = (cx - r/3) + (int)((sr + 3) * cosf(ang));
            int y1 = (cy - r/3) + (int)((sr + 3) * sinf(ang));
            int x2 = (cx - r/3) + (int)((sr + 7) * cosf(ang));
            int y2 = (cy - r/3) + (int)((sr + 7) * sinf(ang));
            epd_draw_line(x1, y1, x2, y2, 0x00, framebuffer);
        }
        drawCloud(cx + r / 5, cy + r / 5, r * 3 / 5);

    } else if (condHas(cond, "cloud") || condHas(cond, "overcast")) {
        drawCloud(cx, cy, r);

    } else {
        // Sunny / clear / fair / unknown → sun
        epd_fill_circle(cx, cy, r * 5 / 8, 0x00, framebuffer);
        epd_draw_circle(cx, cy, r * 5 / 8 + 2, 0x00, framebuffer);
        for (int a = 0; a < 8; a++) {
            float ang = a * M_PI / 4.0f;
            int x1 = cx + (int)((r * 5/8 + 4) * cosf(ang));
            int y1 = cy + (int)((r * 5/8 + 4) * sinf(ang));
            int x2 = cx + (int)((r - 1) * cosf(ang));
            int y2 = cy + (int)((r - 1) * sinf(ang));
            epd_draw_line(x1, y1, x2, y2, 0x00, framebuffer);
        }
    }
}

// ── 7-Segment time display ────────────────────────────────────────────────────
// Bitmask: bit0=a(top) bit1=b(top-right) bit2=c(bot-right)
//          bit3=d(bot) bit4=e(bot-left)  bit5=f(top-left)  bit6=g(middle)
static const uint8_t SEG_MASK[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static void drawSeg(int d, int32_t x, int32_t y) {
    if (d < 0 || d > 9) return;
    uint8_t m = SEG_MASK[d];
    const int32_t dw = SEG_DW, dh = SEG_DH, t = SEG_T, hw = dh / 2, g = 2;
    if (m & 0x01) epd_fill_rect(x + t,      y,          dw - 2*t, t,          0x00, framebuffer);
    if (m & 0x02) epd_fill_rect(x + dw - t, y + t + g,  t,        hw - t - g, 0x00, framebuffer);
    if (m & 0x04) epd_fill_rect(x + dw - t, y + hw + g, t,        hw - t - g, 0x00, framebuffer);
    if (m & 0x08) epd_fill_rect(x + t,      y + dh - t, dw - 2*t, t,          0x00, framebuffer);
    if (m & 0x10) epd_fill_rect(x,          y + hw + g, t,        hw - t - g, 0x00, framebuffer);
    if (m & 0x20) epd_fill_rect(x,          y + t + g,  t,        hw - t - g, 0x00, framebuffer);
    if (m & 0x40) epd_fill_rect(x + t,      y + hw - t/2, dw - 2*t, t,        0x00, framebuffer);
}

static void drawSegColon(int32_t x, int32_t y) {
    const int32_t t = SEG_T, dh = SEG_DH, cw = SEG_COL_W;
    int32_t dx = x + (cw - t) / 2;
    epd_fill_rect(dx, y + dh / 4,      t, t, 0x00, framebuffer);
    epd_fill_rect(dx, y + 3*dh/4 - t,  t, t, 0x00, framebuffer);
}

struct SegLayout {
    int32_t xd[4];      // digit x-positions: [0..n_h-1]=hour, [n_h..n_h+1]=minute
    int32_t x_col;      // colon x
    int32_t x_ampm;     // AM/PM text x
    int32_t y_ampm;     // AM/PM text baseline
    int     n_h;        // 1 or 2 hour digits
};

static SegLayout computeSegLayout(int h, const char *ampm) {
    SegLayout L = {};
    L.n_h = (h >= 10) ? 2 : 1;

    int32_t ax = 0, ay = 0, ax1, ay1, ampm_w, ampm_h;
    get_text_bounds((GFXfont*)&FiraSans, ampm, &ax, &ay, &ax1, &ay1, &ampm_w, &ampm_h, NULL);

    // total_w = hour_digits + gap + colon + gap + m_tens + gap + m_units + gap + ampm
    int32_t total_w = (int32_t)L.n_h * SEG_DW + (int32_t)(L.n_h - 1) * SEG_GAP
                    + SEG_GAP + SEG_COL_W + SEG_GAP
                    + SEG_DW + SEG_GAP + SEG_DW + SEG_GAP
                    + ampm_w;
    int32_t xp = ((EPD_WIDTH - total_w) / 2) & ~1;  // even-align

    for (int i = 0; i < L.n_h; i++) {
        L.xd[i] = xp; xp += SEG_DW;
        if (i < L.n_h - 1) xp += SEG_GAP;
    }
    xp += SEG_GAP;
    L.x_col = xp; xp += SEG_COL_W + SEG_GAP;
    L.xd[L.n_h]   = xp; xp += SEG_DW + SEG_GAP;
    L.xd[L.n_h+1] = xp; xp += SEG_DW + SEG_GAP;
    L.x_ampm  = xp;
    L.y_ampm  = SEG_TOP_Y + SEG_DH / 2 + 20;   // vertically centred in digit block
    return L;
}

static void drawTimeSevenSeg(const char *timeStr) {
    int h, m; char ampm[4] = "";
    sscanf(timeStr, "%d : %d %3s", &h, &m, ampm);
    SegLayout L = computeSegLayout(h, ampm);

    if (L.n_h == 2) {
        drawSeg(h / 10, L.xd[0], SEG_TOP_Y);
        drawSeg(h % 10, L.xd[1], SEG_TOP_Y);
    } else {
        drawSeg(h % 10, L.xd[0], SEG_TOP_Y);
    }
    drawSegColon(L.x_col, SEG_TOP_Y);
    drawSeg(m / 10, L.xd[L.n_h],   SEG_TOP_Y);
    drawSeg(m % 10, L.xd[L.n_h+1], SEG_TOP_Y);

    int32_t cx, yt;
    cx = L.x_ampm;     yt = L.y_ampm; write_string((GFXfont*)&FiraSans, ampm, &cx, &yt, framebuffer);
    cx = L.x_ampm + 1; yt = L.y_ampm; write_string((GFXfont*)&FiraSans, ampm, &cx, &yt, framebuffer);
}

// ── Screen renderers ──────────────────────────────────────────────────────────

/*
 * Layout (960 × 540 px):
 *
 *  y= 65   "Owner: Ashish Kumar Choubey"
 *  y=115   "Do not touch this 😤"
 *          ─── divider y=140 ───
 *  y=184   "Sunday"             (top≈145, bottom≈196)
 *  y=264   "रविवार"             (top≈208, 12px gap, bottom≈290, 30pt Deva)
 *          [7-seg digits rows 298–368]
 *  y=416   "March 9, 2026"
 *          ─── divider y=434 ───
 *  left:   moon cx=100 cy=484 r=20        │  right: weather cx=860 cy=484 r=20
 *  y=502   "Waning Gibbous  Panchami"     │         "+24°C  Partly cloudy"
 */
void renderMain(const DateInfo *di, const MoonInfo *mi, const WeatherInfo *wi) {
    epd_poweron();
    epd_clear();
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    drawFira(OWNER_NAME,                            Y_OWNER);
    drawFira("Do not touch this \xF0\x9F\x98\xA4", Y_NOTTOUCH);
    drawRule(Y_DIV1);

    drawFira(di->dayName,  Y_DAY_EN);
    drawDeva(di->hindiDay, Y_DAY_HI);
    drawTimeSevenSeg(di->timeStr);
    drawFira(di->dateStr,  Y_DATE);
    drawRule(Y_DIV2);

    // Bottom: moon left, weather right — one inline row each
    drawMoonIcon(MOON_ICON_CX, MOON_ICON_CY, MOON_ICON_R, mi->age);
    char moon_line[80];
    snprintf(moon_line, sizeof(moon_line), "%s  %s", mi->phase, mi->tithi);
    drawFiraCol(moon_line, MOON_TEXT_CX, Y_BOTTOM_TEXT);

    if (wi && wi->valid) {
        drawWeatherIcon(wi->condition, WEATHER_ICON_CX, WEATHER_ICON_CY, WEATHER_ICON_R);
        char wx_line[64];
        snprintf(wx_line, sizeof(wx_line), "%s  %s", wi->temp, wi->condition);
        drawFiraCol(wx_line, WEATHER_TEXT_CX, Y_BOTTOM_TEXT);
    }

    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    epd_poweroff();

    strcpy(prevTimeStr, di->timeStr);

    Serial.printf("Main: %s | %s | Moon %.1fd %s | Wx: %s %s\n",
                  di->dayName, di->dateStr, mi->age, mi->phase,
                  wi && wi->valid ? wi->temp : "n/a",
                  wi && wi->valid ? wi->condition : "");
}

void renderTimeRegion(const DateInfo *di) {
    // Full-width strip covering the 7-seg block — avoids per-digit boundary artifacts.
    int32_t ry = SEG_TOP_Y - 2, rh = SEG_DH + 4;

    // Clear the strip in framebuffer then redraw digits
    for (int r = ry; r < ry + rh; r++)
        memset(framebuffer + r * EPD_WIDTH / 2, 0xFF, EPD_WIDTH / 2);
    drawTimeSevenSeg(di->timeStr);

    epd_poweron();
    Rect_t rgn = {.x = 0, .y = ry, .width = EPD_WIDTH, .height = rh};
    epd_clear_area(rgn);
    epd_draw_grayscale_image(rgn, framebuffer + ry * EPD_WIDTH / 2);
    epd_poweroff();

    strcpy(prevTimeStr, di->timeStr);
    Serial.printf("Partial: %s\n", di->timeStr);
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
