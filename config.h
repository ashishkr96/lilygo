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
#define Y_PHASE     510   // moon phase text, left column
#define Y_TITHI     534   // moon tithi text, left column

// ── 7-Segment time display ────────────────────────────────────────────────────
#define SEG_TOP_Y    298   // top of digit block
#define SEG_DH        88   // digit height px
#define SEG_DW        58   // digit width px
#define SEG_T         10   // segment thickness px
#define SEG_GAP        8   // gap between adjacent elements px
#define SEG_COL_W     18   // colon block width px

// ── Weather section ───────────────────────────────────────────────────────────
#define WEATHER_ICON_CX      810   // icon centre x (right side, mirrors moon)
#define WEATHER_ICON_CY      480   // icon centre y
#define WEATHER_ICON_R        22   // icon radius
#define MOON_TEXT_CX         220   // x-centre for moon phase/tithi column
#define WEATHER_TEXT_CX      760   // x-centre for weather temp/condition column
#define WEATHER_INTERVAL_MS  (30UL * 60UL * 1000UL)   // re-fetch every 30 min
