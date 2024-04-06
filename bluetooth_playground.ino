
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string.h>

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
int menuIndex = 0;
int selectedMenu = 0;
bool isMenuSelected = false;
const int ITEMS_SIZE = 3;

struct menuPos {
  std::string name;
  std::string *dataPt;
  int dataSize;
};

const int MENU1_SIZE = 2;
std::string menu1Data[MENU1_SIZE] = {
  "Data1", "Data2"
};
menuPos menu1 = {"A", &menu1Data[0], MENU1_SIZE};

const int MENU2_SIZE = 4;
std::string menu2Data[MENU2_SIZE] = {
  "ABC", "XYZ", "YUI", "QWER"
};
menuPos menu2 = {"B", &menu2Data[0], MENU2_SIZE};

const int MENU3_SIZE = 2;
std::string menu3Data[MENU3_SIZE] = {
  "453", "98372"
};
menuPos menu3 = {"C", &menu3Data[0], MENU3_SIZE};

menuPos *menuPositions[ITEMS_SIZE] = {
  &menu1,
  &menu2,
  &menu3
};

void loop() {
  button1State = digitalRead(34);
  button2State = digitalRead(35);
  button3State = digitalRead(32);

  if(button1State == HIGH && lastButton1State != HIGH) {
    menuIndex++;
  }
  lastButton1State = button1State;

  if(button2State == HIGH && lastButton2State != HIGH) {
    menuIndex--;
  }
  lastButton2State = button2State;

  wrapIndex();
  if(button3State == HIGH && lastButton3State != HIGH) {
    isMenuSelected = !isMenuSelected;
    selectedMenu = menuIndex;
    menuIndex = 0;
  }
  lastButton3State = button3State;

  if(isMenuSelected) {
    mainCharactersitic->setValue(menuPositions[selectedMenu]->dataPt[menuIndex]);
  } else {
    mainCharactersitic->setValue(menuPositions[menuIndex]->name);
  }
  delay(100);
}

void wrapIndex() {
  int size;
  if(isMenuSelected) {
    size = menuPositions[selectedMenu]->dataSize;
  } else {
    size = ITEMS_SIZE;
  }
  if(menuIndex < 0 ){
    menuIndex = size - 1;
  }
  menuIndex %= size;
}