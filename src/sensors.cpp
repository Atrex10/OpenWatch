#include "sensors.h"

#include <MAX30105.h>
#include <heartRate.h>
#include <Wire.h>
#include <math.h>
#include "mainHeader.h"

// ============ HEART RATE SENSOR ============
long HRLastPulseMillis = 0;
uint8_t HRHeartRate = 0;

MAX30105 HRSensor;

void HRSetup() {
    bool detected = HRSensor.begin(Wire, I2C_SPEED_STANDARD);
    if (debug) {
        Serial.print("MAX30102 module detection: ");
        Serial.println(detected); }

    HRSensor.setup(
        0xFF,    // LED brightness
        4,      // sample average
        2,      // Red + IR
        100,    // sample rate
        411,    // pulse width
        4096    // ADC range
    );

    HRSensor.setPulseAmplitudeIR(255);
    HRSensor.setPulseAmplitudeRed(0);

    HRSensor.clearFIFO();
    // HRSensor.shutDown();
}

void HRSleep() {
    HRSensor.shutDown();
}

void HRWake() {
    HRSensor.wakeUp();
}

void HRUpdatePulse() {
    if (HRSensor.available()) {
        long ir = HRSensor.getRed();
        HRSensor.getIR();
        // Serial.println(ir);
        HRSensor.nextSample();

        if (checkForBeat(ir)) {
            long nowMillis = millis();
            long diffMillis = nowMillis - HRLastPulseMillis;

            HRLastPulseMillis = nowMillis;

            HRHeartRate = 60.0f / (diffMillis / 1000.0f);  // calculating to BPM
        }
    }
}

int HRGetPulse() {
    return HRHeartRate;
}


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
