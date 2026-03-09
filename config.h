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
#define Y_DAY_EN    228
#define Y_DAY_HI    316   // NotoDevanagari ascender=56
#define Y_TIME      364   // "10:35 AM" — one FiraSans line below Hindi day
#define Y_DATE      412
#define Y_DIV2      440
#define Y_PHASE     490
#define Y_TITHI     534

// ── Partial-refresh region: time + date lines only ───────────────────────────
#define TIME_REGION_Y   (Y_TIME - 45)                    // = 319
#define TIME_REGION_H   (Y_DATE + 15 - TIME_REGION_Y)   // = 108
