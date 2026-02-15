#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor {
protected:

    const int PIN; // Pin number associated with the sensor, can be used for initialization or reading data

public:

    Sensor(int sensorPin = 3); // Default constructor, empty for now, can be expanded later if needed

    virtual ~Sensor(); // Virtual destructor for proper cleanup of derived classes

    virtual String readinfo(); // Pure virtual function to be implemented by derived classes
};

#endif