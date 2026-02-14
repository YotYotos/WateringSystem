#include <Arduino.h>

#include "Adafruit_ILI9341.h"
#define TFT_DC 21
#define TFT_CS 22
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#include "moistureSensor.h"
MoistureSensorV2p0p0 moist_sensor;

#include "flowSensor.h"
FlowSensorFS300A flow_sensor(35);

void setup() {
  tft.begin();
  delay(1000); // Wait for the display to initialize
  tft.setRotation(0);         
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
}

void loop() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 10);
  tft.println(flow_sensor.readinfo());
  tft.println(moist_sensor.readinfo());

  delay(500);
}
