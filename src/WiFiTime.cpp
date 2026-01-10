#include "WiFiTime.h"

#include <WiFi.h>
#include <time.h>
#include <rtcFunc.h>
#include <WiFiUdp.h>

#include "config.h"
#include "mainHeader.h"

WiFiUDP udp;

// const char* ntpServer = "pool.ntp.org";
IPAddress ntpServer(216, 239, 35, 0); // google public ntp
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

const unsigned int connectionTimeout = 8000; // number of ms the esp tries to connect to each network before timeout
const unsigned int timeSyncTimeout = 4000;

bool connectAnyWiFi() {
    Serial.println("connectAnyWiFi() start");
    Serial.printf("WiFi mode: %d\n", WiFi.getMode());

    WiFi.mode(WIFI_STA);
    delay(50);

    for (auto &net : Config::WiFiNetworks) {
        WiFi.begin(net.ssid, net.psswd); // trying to connect
        delay(50);
        // WiFi.setTxPower(WIFI_POWER_8_5dBm); // this is the workoround for esp32 c3 super mini broken antenna
        if (debug) {
            Serial.println(net.ssid);
            Serial.println(net.psswd); }

        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < connectionTimeout) {
            delay(50);
        }

        if (WiFi.status() == WL_CONNECTED) {
            if (debug) {
                Serial.println(WiFi.localIP()); }
            return true; // succes
        }

        WiFi.disconnect(true); // connection unsuccessful
    }

    return false; // all connections unsuccessful
}

uint32_t getNTPtime() {
    udp.begin(0);
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011; // LI, Version, Mode

    if (udp.beginPacket(ntpServer, 123) == 0) {
        if (debug) { Serial.println("Error: Problem with server address or port"); }
    }
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    if (udp.endPacket()) {
        if (debug) { Serial.println("Error: Failed to send packet"); }
    }

    unsigned long start = millis();
    while (udp.parsePacket() == 0) {
        if (millis() - start > timeSyncTimeout) { 
            Serial.println("Error: Response Waiting timeout");
            return 0; } // timeout
        delay(10);
    }

    udp.read(packetBuffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears; // UTC

    udp.stop();
    return epoch;
}

bool syncWiFiTime() {
    // configTime(0, 0, "pool.ntp.org", "time.google.com");

    // struct tm timeinfo;
    // if (!getLocalTime(&timeinfo, timeSyncTimeout)) { // trying to get local time
    //     return false; // unsuccesful
    // }

    time_t t = getNTPtime();
    if (t == 0) return false; // failed

    
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();

    struct tm timeinfo;
    localtime_r(&t, &timeinfo);

    if (debug) {
        Serial.println("Time synced"); }

    timeSct syncedTime;
    dateSct syncedDate;
    // synchronizing time
    syncedTime.second = timeinfo.tm_sec;
    syncedTime.minute = timeinfo.tm_min;
    syncedTime.hour = timeinfo.tm_hour;
    setCurTime(syncedTime);
    // synchronizing date
    syncedDate.day = timeinfo.tm_mday;
    syncedDate.month = timeinfo.tm_mon + 1;
    syncedDate.year = timeinfo.tm_year - 100;
    setCurDate(syncedDate);
    
    return true; // everything succesful
}

void disableWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}
