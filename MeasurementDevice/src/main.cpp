#include <Arduino.h>

#include <ESPNowW.h>
#include <WiFi.h>
uint8_t BROADCAST[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t MainCompAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};// initialize as broadcast, and will change after address is found
uint8_t MyMacAddress[6];
bool isMCAddressFound = false;// change to true when a measurement device acts as the main computer

bool sendMacAddress = false;// flag to indicate whether to send a MC packet or not
bool askForData = false;// flag to indicate whether to ask for data from the requested device or not
bool sendPacketToMC = false;// flag to indicate whether to send data packet to the main computer or not
String PacketData = "";
uint8_t addrForPacket[6];

// #include <TFT_eSPI.h>
// TFT_eSPI tft = TFT_eSPI();

#include "moistureSensor.h"
MoistureSensorV2p0p0 moist_sensor = MoistureSensorV2p0p0(35); // Initialize with the correct pin number

#include "flowSensor.h"
FlowSensorFS300A flow_sensor = FlowSensorFS300A(34); // Initialize with the correct pin number

void findMCAddress(const uint8_t *mac_addr, const uint8_t *data, int data_len)// find the address of the main computer or the closest measurement device, which will be used as the destination address for sending data
{
  if (data_len == 2)
    {
      String receivedData = "";
      receivedData += (char)data[0];
      receivedData += (char)data[1];
      if (receivedData == "MC"){ // check message confirmation
          for (int i = 0; i < 6; i++)
              MainCompAddr[i] = mac_addr[i];
          isMCAddressFound = true;
      }
    }
}

void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) 
{
  if (data_len == 2) {
    String receivedData = "";
    receivedData += (char)data[0];
    receivedData += (char)data[1];
    for (int i = 0; i < 6; i++) 
      {
        addrForPacket[i] = mac_addr[i];
      }
    if (receivedData == "NM") {// NM = new measurement device
      sendMacAddress = true;
      ESPNow.add_peer((uint8_t*)mac_addr, 1); // Add the sender as a peer to send data back
    }
    else if (receivedData == "SD")// SD = send data
    {
      askForData = true;
    }
    else{// if the message is not NM or SD, it must be a data packet
      PacketData = receivedData;
      sendPacketToMC = true;
    }
  }
}

void setup() {

  WiFi.mode(WIFI_STA);// Set WiFi to station mode to use ESP-NOW
  WiFi.disconnect(); // Disconnect from any WiFi network to ensure ESP-NOW works properly
  ESPNow.init();

  WiFi.macAddress(MyMacAddress);// Get the MAC address of the device

  ESPNow.add_peer(BROADCAST, 1); 
  ESPNow.reg_recv_cb(findMCAddress);
  while (!isMCAddressFound) // wait until the main computer or the closest measurement device is found
  {
    ESPNow.send_message(BROADCAST,(uint8_t*)"NM", 2); // NM = new measurement device, for the main computer or the closest measurement device 
    delay(500);
  }
  ESPNow.unreg_recv_cb();
  ESPNow.add_peer(MainCompAddr, 1); 

  ESPNow.reg_recv_cb(onRecv);

  pinMode(5, OUTPUT);// reset tft through pin 5
  digitalWrite(5, LOW);
  delay(20);
  digitalWrite(5, HIGH);
  delay(2000);

  // tft.begin();// simple setup for tft display 
  // delay(1000);
  // tft.setRotation(3); 
  // tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_WHITE);
  // tft.setTextSize(3);

  flow_sensor.begin();

  pinMode(2, OUTPUT);// blue LED on the board
}

void loop() { 

  if (sendMacAddress)// when someone requests for the address of the main computer or the closest measurement device, send it to them
  {
    ESPNow.send_message(addrForPacket, (uint8_t*)"MC", 2);
    sendMacAddress = false;
  }

  if (askForData)// when the main computer or the closest measurement device requests for data, ask the corresponding device for data
  {
    if (memcmp(addrForPacket, MyMacAddress, 6) == 0){
      uint8_t dataTosend[2];
      dataTosend[0] = moist_sensor.readinfo().toInt();
      dataTosend[1] = flow_sensor.readinfo().toInt();
      ESPNow.send_message(MainCompAddr, (uint8_t*)dataTosend, 2);
    }
    else{
      String requestData = "SD";
      ESPNow.send_message(addrForPacket, (uint8_t*)requestData.c_str(), requestData.length());
    }
    askForData = false;
  }

  if (sendPacketToMC) // send data of another device to the main computer
  {
    String data = PacketData;
    ESPNow.set_mac(addrForPacket);// set mac address as the source of the data
    ESPNow.send_message(MainCompAddr, (uint8_t*)data.c_str(), data.length());
    ESPNow.set_mac(MyMacAddress); 
    sendPacketToMC = false; 

    // tft.fillScreen(TFT_BLACK);// print data to tft display for debugging, can be removed later
    // tft.setCursor(10, 10);
    // tft.print("Data from:");
    // tft.print(addrForPacket[5], HEX);
    // tft.print(addrForPacket[4], HEX);
    // tft.print(addrForPacket[3], HEX);
    // tft.print(addrForPacket[2], HEX);
    // tft.print(addrForPacket[1], HEX);
    // tft.println(addrForPacket[0], HEX);
    // tft.print("Data:");
    // uint8_t incoming[2];
    // memcpy(incoming, (uint8_t*)data.c_str(), 2);
    // int moisture = incoming[0];
    // int flow = incoming[1];
    // tft.print("Moisture: ");
    // tft.println(moisture);
    // tft.print("Flow: ");
    // tft.println(flow);
  }

  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(2, LOW);
  delay(1800);
}
