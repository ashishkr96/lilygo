#include <Arduino.h>
#include <HTTPClient.h>
#include "weather.h"

// WEATHER_LOCATION may be defined in credentials.h (e.g. "Delhi").
// If absent, wttr.in auto-detects location from the device's IP address.
#ifndef WEATHER_LOCATION
#define WEATHER_LOCATION ""
#endif

bool weather_fetch(WeatherInfo *wi) {
    wi->valid = false;
    HTTPClient http;
    // %t = temperature (°C with &m for metric), %C = condition text
    if (!http.begin("http://wttr.in/" WEATHER_LOCATION "?format=%t|%C&m")) return false;
    http.setTimeout(8000);
    int code = http.GET();
    if (code != 200) {
        Serial.printf("Weather HTTP %d\n", code);
        http.end();
        return false;
    }
    String body = http.getString();
    http.end();
    body.trim();

    int sep = body.indexOf('|');
    if (sep < 0) return false;

    String temp = body.substring(0, sep);
    String cond = body.substring(sep + 1);
    temp.trim();
    cond.trim();
    temp.toCharArray(wi->temp, sizeof(wi->temp));
    cond.toCharArray(wi->condition, sizeof(wi->condition));
    wi->valid = true;
    Serial.printf("Weather: %s  %s\n", wi->temp, wi->condition);
    return true;
}
