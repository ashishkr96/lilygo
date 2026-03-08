#pragma once

struct DateInfo {
    int day, month, year;
    const char *dayName;   // "Sunday"
    const char *hindiDay;  // "रविवार"
    char dateStr[32];      // "March 8, 2026"
};

struct MoonInfo {
    float       age;       // days since last new moon (0–29.53)
    int         illum;     // illuminated percentage 0–100
    const char *phase;     // "Waning Gibbous"
    const char *paksha;    // "Krishna" / "Shukla"
    const char *tithi;     // "Panchami"
    bool        isFull;
};
