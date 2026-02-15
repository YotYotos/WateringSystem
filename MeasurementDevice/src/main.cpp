#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#include "moistureSensor.h"
MoistureSensorV2p0p0 moist_sensor = MoistureSensorV2p0p0(34); // Initialize with the correct pin number

#include "flowSensor.h"
FlowSensorFS300A flow_sensor = FlowSensorFS300A(35); // Initialize with the correct pin number

void setup() {

  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(20);
  digitalWrite(5, HIGH);
  delay(150);

  tft.begin();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  flow_sensor.begin();
}

void loop() {

  tft.fillRect(0, 0, tft.width(), 80, TFT_BLACK);
  tft.setCursor(0, 0);
  tft.println(flow_sensor.readinfo());
  tft.println(moist_sensor.readinfo());

  delay(500);
}
