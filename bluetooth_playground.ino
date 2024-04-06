
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>
#include "menu.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *mainCharactersitic;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(32, INPUT);

  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  mainCharactersitic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

int button1State = 0;
int lastButton1State = 0;
int button2State = 0;
int lastButton2State = 0;
int button3State = 0;
int lastButton3State = 0;

void loop() {
  button1State = digitalRead(34);
  button2State = digitalRead(35);
  button3State = digitalRead(32);

  if(button1State == HIGH && lastButton1State != HIGH) {
    onUpClick(&menu);
  }
  lastButton1State = button1State;

  if(button2State == HIGH && lastButton2State != HIGH) {
    onDownClick(&menu);
  }
  lastButton2State = button2State;

  if(button3State == HIGH && lastButton3State != HIGH) {
    onMainClick(&menu);
  }
  lastButton3State = button3State;

  mainCharactersitic->setValue(getDataByState(&menu));

  delay(100);
}
