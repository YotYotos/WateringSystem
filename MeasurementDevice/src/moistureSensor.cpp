#include "moistureSensor.h"

#include <Arduino.h>

const int MoistureSensorV2p0p0::DRY_THRESHOLD = 2900; // Define the threshold for dry soil. 
const int MoistureSensorV2p0p0::WET_THRESHOLD = 1070; // Define the threshold for wet soil. 
//These values are based on the sensor's output and may need to be calibrated for different sensors or soil types.



MoistureSensorV2p0p0::MoistureSensorV2p0p0(int sensorPin) : Sensor(sensorPin){ // Constructor, can be expanded later if needed
}

String MoistureSensorV2p0p0::readinfo() { // Override the pure virtual function from Sensor, returning a placeholder string for now
    int sensorValue = analogRead(PIN);// Read the raw sensor value
    String moistureprecent;
    if (sensorValue >= DRY_THRESHOLD) {// If the sensor value is above the dry threshold, it's considered 0% moisture
        moistureprecent = "0";
    } else if (sensorValue <= WET_THRESHOLD) { // If the sensor value is below the wet threshold, it's considered 100% moisture
        moistureprecent = "100";
    } else {
        moistureprecent = String(map(sensorValue, WET_THRESHOLD, DRY_THRESHOLD, 100, 0)); // Map the sensor value to a percentage between 0% and 100% based on the defined thresholds
    }
    return moistureprecent;
}