#pragma once
#include "types.h"

// Populate *di from the compile-time __DATE__ macro.
// Uses Zeller's congruence for day-of-week.
void buildDateInfo(DateInfo *di);
