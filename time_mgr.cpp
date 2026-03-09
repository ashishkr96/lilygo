#include <Arduino.h>
#include <WiFi.h>
#include "time_mgr.h"
#include "config.h"

bool time_sync() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.printf("Connecting to %s", WIFI_SSID);
    int retries = 20;  // 10 s
    while (WiFi.status() != WL_CONNECTED && retries-- > 0) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi: connect failed");
        return false;
    }
    Serial.printf("WiFi connected: %s\n", WiFi.localIP().toString().c_str());

    configTzTime(TIMEZONE_POSIX, NTP_SERVER1, NTP_SERVER2);

    // Wait up to 10 s for NTP sync
    struct tm t;
    int ntpRetries = 20;
    while (!getLocalTime(&t) && ntpRetries-- > 0) delay(500);
    if (ntpRetries <= 0) {
        Serial.println("NTP sync timed out");
        return false;
    }
    Serial.printf("NTP synced: %04d-%02d-%02d %02d:%02d:%02d\n",
                  t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                  t.tm_hour, t.tm_min, t.tm_sec);
    return true;
}

bool time_get(struct tm *out) {
    return getLocalTime(out);
}
