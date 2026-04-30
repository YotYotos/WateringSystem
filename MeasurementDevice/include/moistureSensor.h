#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#include <Arduino.h>

#include "Sensor.h"

class MoistureSensorV2p0p0 : public Sensor { // Inherit from Sensor
private:
    static const int DRY_THRESHOLD; // Define the threshold for dry soil. 
    static const int WET_THRESHOLD; // Define the threshold for wet soil. 
    //These values are based on the sensor's output and may need to be calibrated for different sensors or soil types.

public:

    MoistureSensorV2p0p0(int sensorPin = 34); // Constructor, can be expanded later if needed

    String readinfo() override; // Override the pure virtual function from Sensor, returning a placeholder string for now

};

#endif