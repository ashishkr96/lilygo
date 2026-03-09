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
// NotoDevanagari 30pt: ascender=56, descender=26, advance_y=82
#define Y_OWNER      65
#define Y_NOTTOUCH  115
#define Y_DIV1      140
#define Y_DAY_EN    184   // English day; top≈145, bottom≈196
#define Y_DAY_HI    264   // Hindi day; top≈208 (12px gap), bottom≈290
// 7-seg block: 298–368 (8px gap after Hindi bottom)
#define Y_DATE      416   // date baseline; top≈377
#define Y_DIV2      434
// Bottom section (y=434–540, 106px) — icons + ONE inline row each side
#define Y_BOTTOM_TEXT  502   // single inline row, vertically centred

// ── 7-Segment time display ────────────────────────────────────────────────────
#define SEG_TOP_Y    298   // top of digit block
#define SEG_DH        70   // digit height px (reduced to free bottom space)
#define SEG_DW        52   // digit width px
#define SEG_T          9   // segment thickness px
#define SEG_GAP        8   // gap between adjacent elements px
#define SEG_COL_W     16   // colon block width px

// ── Weather / moon bottom section ─────────────────────────────────────────────
#define MOON_ICON_CX          100   // moon icon x (left edge area)
#define MOON_ICON_CY          484   // moon icon y (vertically centred in bottom section)
#define MOON_ICON_R            20   // moon icon radius
#define WEATHER_ICON_CX       860   // weather icon x (right edge area)
#define WEATHER_ICON_CY       484   // weather icon y
#define WEATHER_ICON_R         20   // weather icon radius
#define MOON_TEXT_CX          320   // x-centre for moon text (left half, right of icon)
#define WEATHER_TEXT_CX       660   // x-centre for weather text (right half, left of icon)
#define WEATHER_INTERVAL_MS  (30UL * 60UL * 1000UL)   // re-fetch every 30 min
