#pragma once
#include <time.h>
#include "types.h"

// Populate *di from the compile-time __DATE__ macro.
// Uses Zeller's congruence for day-of-week.
void buildDateInfo(DateInfo *di);

// Populate *di from a struct tm obtained via getLocalTime().
// tm_wday 0=Sun…6=Sat is mapped to the same Zeller h index used by buildDateInfo.
void buildDateInfoFromTm(DateInfo *di, const struct tm *t);
