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
#define Y_DAY_EN    225   // English day; top≈186 (46px below divider), bottom≈237
// 7-seg block: 298–386 (61px gap after day name)
#define Y_DATE      432   // date baseline
#define Y_DIV2      448
// Bottom section (y=448–540, 92px) — moon icon left, weather icon right, one text row each
#define Y_BOTTOM_TEXT  506

// ── 7-Segment time display ────────────────────────────────────────────────────
#define SEG_TOP_Y    298   // top of digit block
#define SEG_DH        88   // digit height px
#define SEG_DW        58   // digit width px
#define SEG_T         10   // segment thickness px
#define SEG_GAP        8   // gap between adjacent elements px
#define SEG_COL_W     18   // colon block width px

// ── Weather / moon bottom section ─────────────────────────────────────────────
#define MOON_ICON_CX         100   // moon icon x
#define MOON_ICON_CY         490   // moon icon y
#define MOON_ICON_R           20   // moon icon radius
#define WEATHER_ICON_CX      860   // weather icon x
#define WEATHER_ICON_CY      490   // weather icon y
#define WEATHER_ICON_R        20   // weather icon radius
#define MOON_TEXT_CX         300   // x-centre for moon phase text (left half)
#define WEATHER_TEXT_CX      680   // x-centre for weather text (right half)
#define WEATHER_INTERVAL_MS  (30UL * 60UL * 1000UL)   // re-fetch every 30 min
