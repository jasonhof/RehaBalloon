/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.

   Extended to include air pressure reading and distribution through BLE by Sebastian Feger.
*/

#include <Adafruit_NeoPixel.h>
/*#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
# include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

//BLEServer* pServer = NULL;
//BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define PIN_NEO_PIXEL 6   // Arduino pin that connects to NeoPixel
#define NUM_PIXELS     12  // The number of LEDs (pixels) on NeoPixel

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

float initial_pressure_value;

/*class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};*/

void setup() {

  Serial.begin(115200);

  pinMode(PIN_NEO_PIXEL, OUTPUT);
  NeoPixel.begin();
  NeoPixel.setBrightness(20);

  NeoPixel.clear();

  //NeoPixel.setPixelColor(0, NeoPixel.Color(0, 0, 255)); // it only takes effect if pixels.show() is called##
  //NeoPixel.setPixelColor(1, NeoPixel.Color(255, 0, 255)); // it only takes effect if pixels.show() is called##

 // NeoPixel.show();   // send the updated pixel colors to the NeoPixel hardware.

  //delay(120000); //gives the users two minutes after programming or reset to move the board into the ballon, blow it up, close it, and leave it rest until initial pressure capturing

  for (int pixel = 0; pixel < NUM_PIXELS; pixel++) { // for each pixel
    NeoPixel.setPixelColor(pixel, NeoPixel.Color(255, 0, 0)); // it only takes effect if pixels.show() is called##
        
      //NeoPixel.show();   // send the updated pixel colors to the NeoPixel hardware.
    //delay(1000);
  }
  NeoPixel.show();
  
  bool status;
  status = bmp.begin(0x76);  
  if (!status) {
    Serial.println(bmp.sensorID(), 16);
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  delay(1000);
  //initial_pressure_value = bmp.readPressure() / 100.0F;
  baseLine();

 /* 
  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");*/
}
void baseLine(){
  initial_pressure_value = bmp.readPressure() / 100.0F;
}
void loop() {

    float pressure = bmp.readPressure() / 100.0F;
    //Serial.println(pressure);
    float change = pressure - initial_pressure_value;

    if ((change <= 1) && (change >= -1)) {
        change = 0;
        Serial.println(change);
     }else{
       Serial.println(change);
     }   
    if(change <0){
      Serial.print("Pressure is decreasing");
      baseLine();
    }
    if(change >= 0){
      if(change < 7){ //BLUE 
        float bright=change*4;
        NeoPixel.setBrightness(bright);
        //Serial.print(bright);       
        for (int pixel = 0; pixel < NUM_PIXELS; pixel++) { // for each pixel
          NeoPixel.setPixelColor(pixel, NeoPixel.Color(0, 0, 255)); // it only takes effect if pixels.show() is called
        }  
      }      
     if ((change > 6) && (change < 8)) { //GREEN   
       //rotation_loop(NeoPixel.Color(255, 96, 149));
        // NeoPixel.setBrightness(255);
         for (int pixel = 0; pixel < NUM_PIXELS; pixel++) { // for each pixel
          NeoPixel.setPixelColor(pixel, NeoPixel.Color(0, 255, 0)); // it only takes effect if pixels.show() is called
         }      
    }
    if(change >= 8){ //RED   
        NeoPixel.setBrightness(255);       
        for (int pixel = 0; pixel < NUM_PIXELS; pixel++) { // for each pixel
        NeoPixel.setPixelColor(pixel, NeoPixel.Color(255, 0, 0)); // it only takes effect if pixels.show() is called
        }
      }
}
    NeoPixel.show();   // send the updated pixel colors to the NeoPixel hardware.
/*
    // notify changed value
    if (deviceConnected) {
        char txString[8];
        dtostrf(pressure, 1, 2, txString);
        pCharacteristic->setValue(txString);
        pCharacteristic->notify();

        delay(100); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
*/

    delay(100);
}
void rotation_loop(uint32_t color) {
  for (int p = 0; p <= 11; p++) {
    NeoPixel.setPixelColor(p, NeoPixel.Color(255, 100, 0));
    NeoPixel.setPixelColor(p + 1, NeoPixel.Color(255, 100, 0));
    // strip.setPixelColor(p + 2, strip.Color(255, 0, 0));
    NeoPixel.show();
    delay(150);
    p++;
    NeoPixel.clear();
  }
}
