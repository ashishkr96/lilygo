#pragma once

struct WeatherInfo {
    char temp[12];      // e.g. "+24°C"
    char condition[48]; // e.g. "Partly cloudy"
    bool valid;
};

// Fetch current weather from wttr.in. Returns true on success.
bool weather_fetch(WeatherInfo *wi);
