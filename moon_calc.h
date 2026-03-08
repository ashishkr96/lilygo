#pragma once
#include "types.h"

// Populate *mi using the Meeus synodic month algorithm.
// Reference new moon: JDE 2451550.09765 (Jan 6 2000)
// Synodic period: 29.530588853 days
void buildMoonInfo(MoonInfo *mi, int year, int month, int day);
