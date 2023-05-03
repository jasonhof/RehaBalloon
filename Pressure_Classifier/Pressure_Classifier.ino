#include <LSM6DS3.h>
#include <Wire.h>

#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "model.h"

// Pressure Sensor imports

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

// const values for training

const float pressureThreshold = 7.5;
const int numSamples = 119;
int samplesRead = numSamples;



// const float accelerationThreshold = 2.5; // threshold of significant in G's
// const int numSamples = 119;

// int samplesRead = numSamples;

LSM6DS3 myIMU(I2C_MODE, 0x6A);  

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// array to map gesture index to a name
const char* GESTURES[] = {
  "pumps",
  "quick_pumps"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);



  // Serial.begin(115200);

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
  initial_pressure_value = bmp.readPressure() / 100.0F;


  // if (myIMU.begin() != 0) {
  //   Serial.println("Device error");
  // } else {
  //   Serial.println("Device OK!");
  // }

  // Serial.println();

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

}

void loop() {
  // float aX, aY, aZ, gX, gY, gZ;

  float pressure = bmp.readPressure() / 100.0F;

  // Serial.println(pressure);

  float change = pressure - initial_pressure_value;

  // wait for significant motion
  while (samplesRead == numSamples) {
      // read the acceleration data
      // aX = myIMU.readFloatAccelX();
      // aY = myIMU.readFloatAccelY();
      // aZ = myIMU.readFloatAccelZ();

      // // sum up the absolutes
      // float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      float pressure = bmp.readPressure() / 100.0F;
      float change = pressure - initial_pressure_value;

      // check if it's above the threshold
      if (change >= pressureThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available
      // read the acceleration and gyroscope data
      // aX = myIMU.readFloatAccelX();
      // aY = myIMU.readFloatAccelY();
      // aZ = myIMU.readFloatAccelZ();

      // gX = myIMU.readFloatGyroX();
      // gY = myIMU.readFloatGyroY();
      // gZ = myIMU.readFloatGyroZ();

      float pressure = bmp.readPressure() / 100.0F;
      float change = pressure - initial_pressure_value;

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 6 + 0] = (change + 1.0) / 20.0;
      // tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = 0;//(aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = 0;//(aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = 0;//(gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = 0;//(gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = 0;//(gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return;
        }

        // Loop through the output tensor values from the model
        Serial.println("Probabilities:")
        for (int i = 0; i < NUM_GESTURES; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);
          if (tflOutputTensor->data.f[i] > 0.8) {
            Serial.print(GESTURES[i])
            Serial.print(" detected (prob: ")
            Serial.print(tflOutputTensor->data.f[i])
            Serial.println(")")
          }
        }
        delay(1000);
        Serial.println();
      }
  }
}
