#include "flowSensor.h"

#include <Arduino.h>

void FlowSensorFS300A::pulseCounterISR(void* arg) {// Static ISR function to handle pulse counting, takes a void pointer to allow passing the 'this' pointer
    FlowSensorFS300A* self = (FlowSensorFS300A*)arg;// Cast the void pointer back to the class instance
    self->pulseCount++;// Increment the pulse count each time the ISR is triggered (HIGH)
}

FlowSensorFS300A::FlowSensorFS300A(int sensorPin, float CFactor) : Sensor(sensorPin), CalibrationFactor(CFactor), pulseCount(0), lastTime(0) {}

void FlowSensorFS300A::begin() {// Initialize the sensor pin and attach the interrupt
    pinMode(PIN, INPUT_PULLUP); // making sure that input is either HIGH or LOW, not floating
    attachInterruptArg(digitalPinToInterrupt(PIN), pulseCounterISR, this, RISING);//attach the interrupt to the sensor pin, trigger on rising edge(HIGH or LOW), and pass 'this' pointer to the ISR(interrupt service routine)
    lastTime = millis();// Initialize lastTime to current time
    lastFlowRate = "0 L/min"; // Initialize lastFlowRate to a default value
}

String FlowSensorFS300A::readinfo() {// Override the readinfo method to provide flow rate information
    unsigned long currentTime = millis();// Get the current time
    unsigned long timeDiff = currentTime - lastTime;// Calculate the time difference since the last flow rate calculation

    // calculate flow rate every 1 second
    if (timeDiff >= 1000) {
        float flowRate = (pulseCount / (timeDiff / 1000.0)) / CalibrationFactor; // L/min
        pulseCount = 0;// Reset pulse count after calculating flow rate
        lastTime = currentTime;// Update lastTime to current time for the next calculation
        lastFlowRate = String(flowRate, 2) + " L/min"; // Store the last calculated flow rate as a string with no decimal places
    }
    return lastFlowRate; // Return the last calculated flow rate
}