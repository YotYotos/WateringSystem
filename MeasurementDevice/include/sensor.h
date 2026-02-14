#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor {
public:

    Sensor() {} // Default constructor, empty for now, can be expanded later if needed
    virtual ~Sensor() = default; // Virtual destructor for proper cleanup of derived classes

    virtual String readinfo() = 0; // Pure virtual function to be implemented by derived classes
};

#endif