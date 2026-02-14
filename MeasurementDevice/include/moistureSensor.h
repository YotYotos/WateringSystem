#ifndef MOISTURE_SENSOR_H
#define MOISTURE_SENSOR_H

#include <Arduino.h>

#include "Sensor.h"

class MoistureSensorV2p0p0 : public Sensor { // Inherit from Sensor
private:
    static const int MOISTURE_SENSOR_PIN = 34; // Define the pin for the moisture sensor. it will always be 34, but defining it as a constant makes the code more readable and easier to maintain.
    static const int DRY_THRESHOLD = 2900; // Define the threshold for dry soil. 
    static const int WET_THRESHOLD = 1070; // Define the threshold for wet soil. 
    //These values are based on the sensor's output and may need to be calibrated for different sensors or soil types.

public:

    MoistureSensorV2p0p0(){ // Constructor, can be expanded later if needed
    }

    String readinfo() override { // Override the pure virtual function from Sensor, returning a placeholder string for now
        int sensorValue = analogRead(MOISTURE_SENSOR_PIN);// Read the raw sensor value
        String moistureprecent;
        if (sensorValue >= DRY_THRESHOLD) {// If the sensor value is above the dry threshold, it's considered 0% moisture
            moistureprecent = "0%";
        } else if (sensorValue <= WET_THRESHOLD) { // If the sensor value is below the wet threshold, it's considered 100% moisture
            moistureprecent = "100%";
        } else {
            moistureprecent = String(map(sensorValue, DRY_THRESHOLD, WET_THRESHOLD, 0, 100)) + "%"; // Map the sensor value to a percentage between 0% and 100% based on the defined thresholds
        }
        return moistureprecent;
    }
};

#endif