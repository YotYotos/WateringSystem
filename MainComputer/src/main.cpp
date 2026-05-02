#include <Arduino.h>

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

String line = "";

void setup() {   
  Serial1.begin(115200);

  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(20);
  digitalWrite(5, HIGH);
  delay(150);

  tft.begin();
  tft.setRotation(3); 
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
}

void loop() {
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\n') {
      tft.fillRect(0, 0, tft.width(), 80, TFT_BLACK);
      tft.setCursor(0, 0);
      tft.println(line);
      line = "";
    } else {
      line += c;
    }
  }
  tft.setCursor(111, 0);
  tft.println("line");
}