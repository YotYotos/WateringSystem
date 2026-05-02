#include <Arduino.h>
#include <ESPNowW.h>
#include <WiFi.h>

uint8_t BROADCAST[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t addr[3] = {0xFF, 0xFF, 0xFF};
bool found = false;

void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)// manage packets from the main computer or the closest measurement device, and packets from other measurement devices
{
  for (int i = 0; i < data_len; i++) {
    char c = data[i];
    if (c == 'N' || c == 'M'){
      Serial1.print(c);
    }
    else{
      uint8_t value = c;
      int result = static_cast<int>(value);
      Serial1.print(result);
      Serial1.print(";");
    }
  }
  Serial1.println(";");
  if (!found){
    ESPNow.send_message(BROADCAST, (uint8_t*)"MC", 2);
    addr[0] = mac_addr[3];
    addr[1] = mac_addr[4];
    addr[2] = mac_addr[5];
    found = true;
  }
}

void setup() {
  Serial1.begin(115200, SERIAL_8N1, 16, 17);
  delay(2000);
  WiFi.mode(WIFI_STA);// Set WiFi to station mode to use ESP-NOW
  WiFi.disconnect(); // Disconnect from any WiFi network to ensure ESP-NOW works properly
  ESPNow.init();
  ESPNow.unreg_recv_cb();
  ESPNow.reg_recv_cb(onRecv);
  ESPNow.add_peer(BROADCAST, 1); // Add broadcast peer to send messages to all devices

}

String line = "";

void loop() {
  if (found) {
    ESPNow.send_message(BROADCAST, addr, 3); // Send a message to the main computer every loop iteration
    delay(500); // Adjust the delay as needed
  }
  while (Serial1.available())
  {
    char c = Serial1.read();

    if (c == '\n') {
      Serial.println(line);  // forward to PC
      line = "";
    } else {
      line += c;
    }
  }
  
}