#pragma once

// ── Owner ────────────────────────────────────────────────────────────────────
#define OWNER_NAME   "Owner: Ashish Kumar Choubey"

// ── Timing ───────────────────────────────────────────────────────────────────
#define TOUCH_DISPLAY_MS  10000   // ms to show joke before reverting

// ── Moon icon ────────────────────────────────────────────────────────────────
#define MOON_CX  120
#define MOON_CY  484
#define MOON_R    28

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
#define Y_DAY_EN    195   // raised to make room for 7-seg time block
#define Y_DAY_HI    268   // NotoDevanagari ascender=56; bottom≈294
// 7-seg block occupies rows SEG_TOP_Y … SEG_TOP_Y+SEG_DH  (298–386)
#define Y_DATE      432
#define Y_DIV2      448
// Bottom section (y=452–540, 88px) — icons + ONE combined text row each side
// Two FiraSans rows need 50px separation; only one fits cleanly → combine
#define Y_BOTTOM_TEXT  506   // single text row, vertically centred below icons

// ── 7-Segment time display ────────────────────────────────────────────────────
#define SEG_TOP_Y    298   // top of digit block
#define SEG_DH        88   // digit height px
#define SEG_DW        58   // digit width px
#define SEG_T         10   // segment thickness px
#define SEG_GAP        8   // gap between adjacent elements px
#define SEG_COL_W     18   // colon block width px

// ── Weather / moon bottom section ─────────────────────────────────────────────
#define MOON_ICON_CX         160   // moon icon x (shifted right from edge)
#define MOON_ICON_CY         478   // moon icon y
#define MOON_ICON_R           24   // moon icon radius
#define WEATHER_ICON_CX      800   // weather icon x (right side)
#define WEATHER_ICON_CY      478   // weather icon y
#define WEATHER_ICON_R        24   // weather icon radius
#define MOON_TEXT_CX         310   // x-centre for moon text (right of icon)
#define WEATHER_TEXT_CX      670   // x-centre for weather text (left of icon)
#define WEATHER_INTERVAL_MS  (30UL * 60UL * 1000UL)   // re-fetch every 30 min
