#ifndef SENSORS_H
#define SENSORS_H

// ============ HEART RATE SENSOR ============
void HRUpdatePulse();  // should be called at least 100 times a second

int HRGetPulse();

void HRSetup();

void HRWake();
void HRSleep();


// ============ TEMP. SENSOR (THERMISTOR) ============
float tempRead();

void enableThermistor();
void disableThermistor();


#endif
