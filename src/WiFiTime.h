#ifndef WIFITIME_H
#define WIFITIME_H

#include <WiFiUdp.h>

// wifi entry structure
struct WiFiEntry {
    const char* ssid;
    const char* psswd;
};

bool connectAnyWiFi();

bool syncWiFiTime();

void disableWiFi();

#endif
