
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

struct menuState {
  int menuIndex;
  int selectedMenuIndex;
  int isMenuSelected;
  int menuSize;
  menuPos **menu;
};

void wrapIndex(menuState *state);
void onUpClick(menuState *state);
void onDownClick(menuState *state);
void onMainClick(menuState *state);
std::string getDataByState(menuState *state);

struct menuState menu = {
  0, 0, false, ITEMS_SIZE, &menuPositions[0]
};

void onUpClick(menuState *state) {
  state->menuIndex++;
  wrapIndex(state);
}

void onDownClick(menuState *state) {
  state->menuIndex--;
  wrapIndex(state);
}

void wrapIndex(menuState *state) {
  int size;
  if(state->isMenuSelected) {
    size = state->menu[state->selectedMenuIndex]->dataSize;
  } else {
    size = state->menuSize;
  }
  if(state->menuIndex < 0 ){
    state->menuIndex = size - 1;
  }
  state->menuIndex %= size;
}

void onMainClick(menuState *state) {
    state->isMenuSelected = !state->isMenuSelected;
    if(state->isMenuSelected) {
      state->selectedMenuIndex = state->menuIndex;
      state->menuIndex = 0;
    } else {
      state->selectedMenuIndex = state->menuIndex;
    }
}

std::string getDataByState(menuState *state){
  if(state->isMenuSelected) {
    return state->menu[state->selectedMenuIndex]->dataPt[state->menuIndex];
  } else {
    return (state->menu)[state->menuIndex]->name;
  }
}

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
