#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <Arduino.h>

#include "Sensor.h"

class FlowSensorFS300A : public Sensor { // Inherit from Sensor
private:
    volatile uint32_t pulseCount; // Use volatile for variables modified in ISR to prevent compiler optimizations that could lead to incorrect behavior
    unsigned long lastTime; // Store the last time to ensure accurate flow rate calculation
    String lastFlowRate; // Store the last calculated flow rate to return when readinfo is called before the next calculation

    const float CalibrationFactor; // Calibration factor for the flow sensor

    static void pulseCounterISR(void* arg);// Static ISR function to handle pulse counting, takes a void pointer to allow passing the 'this' pointer
public:
    FlowSensorFS300A(int sensorPin = 35, float CFactor = 7.5); // Constructor with default calibration factor

    void begin();// Initialize the sensor pin and attach the interrupt

    String readinfo() override;// Override the readinfo method to provide flow rate information
};

#endif