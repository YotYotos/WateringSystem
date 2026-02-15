#include "Sensor.h"

#include <Arduino.h>

Sensor::Sensor(int sensorPin) : PIN(sensorPin) {
} // Default constructor, empty for now, can be expanded later if needed

Sensor::~Sensor() = default; // Virtual destructor for proper cleanup of derived classes

String Sensor::readinfo()
{
    return String(analogRead(PIN)); // Default implementation of readinfo, can be overridden by derived classes to provide specific sensor readings. For now, it simply reads the analog value from the specified pin and returns it as a string.
} 
