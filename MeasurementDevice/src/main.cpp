#include <Arduino.h>

#include <ESPNowW.h>
#include <WiFi.h>
uint8_t BROADCAST[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t MainCompAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};// initialize as broadcast, and will change after address is found
uint8_t MyMacAddress[6];
bool isMCAddressFound = true;// change to true when a measurement device acts as the main computer

#define PKT_CLEAR   'c' // c = clear; meaning a new packet can be create into newpacket
#define PKT_REQ     'r' // r = data request
#define PKT_MAC     'm' // m = send macc adrr to requester
#define PKT_SENSOR  's' // s = send data of sensors
typedef struct // the struct of any packet i recive
{
  uint8_t packetData[3];
  uint8_t addrForPacket[6];
  uint8_t whatToDo; // act like a flag that tells what to do; either to ask for data either to send data to mc or send mac adrr to a measurment device
}Packet;

volatile Packet newPacket;

#include "moistureSensor.h"
MoistureSensorV2p0p0 moist_sensor = MoistureSensorV2p0p0(35); // Initialize with the correct pin number

#include "flowSensor.h"
FlowSensorFS300A flow_sensor = FlowSensorFS300A(34); // Initialize with the correct pin number

unsigned long lastSend = 0;

void findMCAddress(const uint8_t *mac_addr, const uint8_t *data, int data_len)// find the address of the main computer or the closest measurement device, which will be used as the destination address for sending data
{
  if (data_len == 2)
    {
      if (data[0] == 'M' && data[1] == 'C') 
      { 
        memcpy(MainCompAddr, mac_addr, 6);
        isMCAddressFound = true;
      }
    }
}

void onRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) 
{
  if(data_len == 2 || data_len == 3)
  {
    if (newPacket.whatToDo != PKT_CLEAR) return;
    memcpy((void*)newPacket.addrForPacket, mac_addr, 6); 
    if (data_len == 3)
    {
      memcpy((void*)newPacket.packetData, data, 3);
      newPacket.whatToDo = PKT_REQ;
    }else if (data_len == 2){
      memset((void*)newPacket.packetData, 0, 3);
      memcpy((void*)newPacket.packetData, data, 2);
      if(newPacket.packetData[0] == 'N' && newPacket.packetData[1] == 'M'){
        newPacket.whatToDo = PKT_MAC;
      }else{
        newPacket.whatToDo = PKT_SENSOR;
      }
    }
  }
  return;
}

void setup() {
  Serial.begin(9600);

  pinMode(2, OUTPUT);// blue LED on the board
  
  newPacket.whatToDo = PKT_CLEAR;
  
  WiFi.mode(WIFI_STA);// Set WiFi to station mode to use ESP-NOW
  WiFi.disconnect(); // Disconnect from any WiFi network to ensure ESP-NOW works properly
  ESPNow.init();
  WiFi.macAddress(MyMacAddress);// Get the MAC address of the device
  
  ESPNow.unreg_recv_cb();
  ESPNow.add_peer(BROADCAST, 1); 
  ESPNow.reg_recv_cb(findMCAddress);
  while (!isMCAddressFound) // wait until the main computer or the closest measurement device is found
  {
    ESPNow.send_message(BROADCAST,(uint8_t*)"NM", 2); // NM = new measurement device, for the main computer or the closest measurement device 
    digitalWrite(2,LOW);
    delay(200);
    digitalWrite(2,HIGH);
    delay(300);
  }
  ESPNow.unreg_recv_cb();
  
  ESPNow.add_peer(BROADCAST, 1); 
  ESPNow.add_peer(MainCompAddr, 1); 

  ESPNow.reg_recv_cb(onRecv);

  flow_sensor.begin();

  digitalWrite(2,HIGH);
}

void loop() {
  Packet localPacket;
  localPacket.whatToDo = PKT_CLEAR;

  noInterrupts();
  if (newPacket.whatToDo != PKT_CLEAR) {
    memcpy(&localPacket, (const void*)&newPacket, sizeof(Packet));
    newPacket.whatToDo = PKT_CLEAR;
  }
  interrupts();

  if (localPacket.whatToDo == PKT_MAC)// when someone requests for the address of the main computer or the closest measurement device, send it to them
  {
    ESPNow.send_message(BROADCAST, (uint8_t*)"MC", 2);
  }
  else if (localPacket.whatToDo == PKT_REQ)// when the main computer or the closest measurement device requests for data, ask the corresponding device for data
  { 
    if (memcmp(localPacket.addrForPacket, MainCompAddr, 6) == 0){
      uint8_t lastDigMac[3];
      lastDigMac[0] = MyMacAddress[3];
      lastDigMac[1] = MyMacAddress[4];
      lastDigMac[2] = MyMacAddress[5];
      if (memcmp(localPacket.packetData, lastDigMac,3) == 0){
        uint8_t dataTosend[2];
        dataTosend[0] = moist_sensor.readinfo().toInt();
        dataTosend[1] = flow_sensor.readinfo().toInt();
        ESPNow.send_message(MainCompAddr, dataTosend, 2);
      }
      else{
        delay(random(10, 50));
        ESPNow.send_message(BROADCAST, lastDigMac, 3);
      }
    }
  }
  else if (localPacket.whatToDo == PKT_SENSOR) // send data of another device to the main computer
  {
    uint8_t data[2];
    memcpy(data ,localPacket.packetData, 2);
    ESPNow.send_message(MainCompAddr, data, 2);
  }
  else{

  }
}
