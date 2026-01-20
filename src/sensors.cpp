#include "sensors.h"

#include <Wire.h>
#include <math.h>
#include "mainHeader.h"

// ============ TEMP. SENSOR (THERMISTOR) ============
float tempCalibrationSlope = 0.06568;
float tempCalibrationIntercept = -21.9;

const int thermistorNomRes = 47000;
const int thermistorSerRes = 10000;
const int thermistorBeta = 2713.05;
const int thermistorNomTemp = 25;

float roundFloat(float value, int decimalPlaces) {
    value *= pow(10, decimalPlaces);
    int rounded = round(value);
    value = rounded / pow(10, decimalPlaces);
    return value;
}

float tempRead() {
    float resistance = thermistorSerRes * (4095.0 / analogRead(thermistorADCPin) - 1.0);
    float temperature;

    temperature = resistance / thermistorNomRes;
    temperature = log(temperature);
    temperature /= thermistorBeta;
    temperature += 1.0 / (thermistorNomTemp + 273.15);
    temperature = 1.0 / temperature;                    // inversion
    temperature -= 273.15;                              // conversion to celsius

    return roundFloat(temperature, 2);
}

void enableThermistor() {
    digitalWrite(thermistorEnablePin, HIGH);
}

void disableThermistor() {
    digitalWrite(thermistorEnablePin, LOW);
}
