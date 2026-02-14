#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <Arduino.h>

#include "Sensor.h"

class FlowSensorFS300A : public Sensor { // Inherit from Sensor
private:
    const uint8_t pin;
    volatile uint32_t pulseCount;
    unsigned long lastTime;

    // Calibration factor from datasheet
    const float k = 7.5; // adjust if needed

    static void pulseCounterISR(void* arg) {
        FlowSensorFS300A* self = (FlowSensorFS300A*)arg;
        self->pulseCount++;
    }

public:
    FlowSensorFS300A(uint8_t sensorPin) : pin(sensorPin), pulseCount(0), lastTime(0) {}

    void begin() {
        pinMode(pin, INPUT_PULLUP);
        attachInterruptArg(digitalPinToInterrupt(pin), pulseCounterISR, this, RISING);
        lastTime = millis();
    }

    String readinfo() override {
        unsigned long currentTime = millis();
        unsigned long dt = currentTime - lastTime;

        // compute flow rate every 1 second
        if (dt >= 1000) {
            float flowRate = (pulseCount / (dt / 1000.0)) / k; // L/min
            pulseCount = 0;
            lastTime = currentTime;
            return String(flowRate, 2) + " L/min";
        }
        return String("Measuring...");
    }
};

#endif