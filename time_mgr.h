#pragma once
#include <time.h>

// Connect to WiFi (via saved credentials or captive-portal on first boot),
// then sync time over NTP.  Returns true on success, false on timeout/failure.
bool time_sync();

// Thin wrapper around getLocalTime().  Returns true if time is valid.
bool time_get(struct tm *out);
