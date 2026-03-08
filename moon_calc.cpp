#include <Arduino.h>
#include <math.h>
#include "moon_calc.h"

static const double SYN = 29.530588853;
static const double REF = 2451550.09765; // JDE of reference new moon (2000-Jan-06)

static const char *TITHI_S[] = {
    "Pratipada", "Dwitiya",    "Tritiya",     "Chaturthi", "Panchami",
    "Shashthi",  "Saptami",    "Ashtami",     "Navami",    "Dashami",
    "Ekadashi",  "Dwadashi",   "Trayodashi",  "Chaturdashi","Purnima"
};
static const char *TITHI_K[] = {
    "Pratipada", "Dwitiya",    "Tritiya",     "Chaturthi", "Panchami",
    "Shashthi",  "Saptami",    "Ashtami",     "Navami",    "Dashami",
    "Ekadashi",  "Dwadashi",   "Trayodashi",  "Chaturdashi","Amavasya"
};

// Julian Day Number at noon for the given calendar date.
// Returns integer JDN (already noon → no +0.5 needed).
static long julianDay(int y, int m, int d) {
    if (m <= 2) { y--; m += 12; }
    int A = y / 100, B = 2 - A + A/4;
    return (long)(365.25*(y+4716)) + (long)(30.6001*(m+1)) + d + B - 1524;
}

void buildMoonInfo(MoonInfo *mi, int year, int month, int day) {
    double jd  = (double)julianDay(year, month, day);
    double age = fmod(jd - REF, SYN);
    if (age < 0) age += SYN;

    mi->age   = (float)age;
    mi->illum = (int)((1.0 - cos(2.0*M_PI*age/SYN)) / 2.0 * 100.0 + 0.5);
    mi->isFull = (age >= 13.5 && age <= 16.5);

    if      (age <  1.85) mi->phase = "New Moon";
    else if (age <  7.38) mi->phase = "Waxing Crescent";
    else if (age <  9.22) mi->phase = "First Quarter";
    else if (age < 14.77) mi->phase = "Waxing Gibbous";
    else if (age < 16.61) mi->phase = "Full Moon!";
    else if (age < 22.15) mi->phase = "Waning Gibbous";
    else if (age < 24.0)  mi->phase = "Last Quarter";
    else                  mi->phase = "Waning Crescent";

    int tNum = (int)(age / (SYN / 30.0));
    if (tNum <= 14) { mi->paksha = "Shukla";  mi->tithi = TITHI_S[tNum];      }
    else            { mi->paksha = "Krishna"; mi->tithi = TITHI_K[tNum - 15]; }
}
