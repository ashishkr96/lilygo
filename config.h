#pragma once

// ── Owner ────────────────────────────────────────────────────────────────────
#define OWNER_NAME   "Owner: Ashish Kumar Choubey"

// ── Timing ───────────────────────────────────────────────────────────────────
#define TOUCH_DISPLAY_MS  10000   // ms to show joke before reverting

// ── WiFi credentials ─────────────────────────────────────────────────────────
#include "credentials.h"   // gitignored — copy credentials.h.example and fill in

// ── NTP / Timezone ───────────────────────────────────────────────────────────
#define NTP_SERVER1         "pool.ntp.org"
#define NTP_SERVER2         "time.nist.gov"
#define TIMEZONE_POSIX      "IST-5:30:00"   // India Standard Time
#define REDRAW_INTERVAL_MS  60000UL          // poll interval; re-render only on day change

// ── Layout: Y-baselines (FiraSans ascender=39, advance_y=50) ─────────────────
#define Y_OWNER      65
#define Y_NOTTOUCH  115
#define Y_DIV1      140
#define Y_DAY_EN    195   // English day; top≈156, bottom≈207
// 7-seg block: 214–278 (7px gap after day name, smaller digits)
#define Y_DATE      324   // date baseline; top≈285
#define Y_DIV2      340
// Bottom section (y=340–540, 200px) — moon centre, weather right
#define Y_BOTTOM_TEXT  496

// ── 7-Segment time display ────────────────────────────────────────────────────
#define SEG_TOP_Y    214   // top of digit block
#define SEG_DH        64   // digit height px (smaller)
#define SEG_DW        44   // digit width px
#define SEG_T          8   // segment thickness px
#define SEG_GAP        7   // gap between adjacent elements px
#define SEG_COL_W     13   // colon block width px

// ── Weather / moon bottom section ─────────────────────────────────────────────
#define MOON_ICON_CX          80   // moon icon x (left edge indicator)
#define MOON_ICON_CY         426   // moon icon y
#define MOON_ICON_R           26   // moon icon radius
#define WEATHER_ICON_CX      880   // weather icon x (right edge)
#define WEATHER_ICON_CY      426   // weather icon y
#define WEATHER_ICON_R        26   // weather icon radius
#define WEATHER_TEXT_CX      800   // x-centre for weather text (right side)
#define WEATHER_INTERVAL_MS  (30UL * 60UL * 1000UL)   // re-fetch every 30 min
