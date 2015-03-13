// For Bluetooth
#include <SPI.h>
#include <Adafruit_BLE_UART.h>
// For Accelerometer
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

// Accelerometer
Adafruit_MMA8451 mma = Adafruit_MMA8451();

// Bluetooth LE
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 1
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void)
{
  Serial.begin(9600);
  while (!Serial); // Leonardo/Micro should wait for serial init

  // Setup BLE
  BTLEserial.setDeviceName("Fins"); // 7 Characters Max
  BTLEserial.begin();
  
  // Setup Accelerometer
  if (!mma.begin()) {
    Serial.println("Couldn't Start");
    while (1);
  }
  Serial.println("Accelerometer Found");
  
  mma.setRange(MMA8451_RANGE_2_G);
  Serial.print("Range = "); Serial.print(2 << mma.getRange());
}

void loop()
{
  // Read the 'raw' data in 14-bit counts
  mma.read();
  // Get a new sensor event
  sensors_event_t event; 
  mma.getEvent(&event);
  // Get acceleration in m/(s^2)
  float accel_x = event.acceleration.x;
  float accel_y = event.acceleration.y;
  float accel_z = event.acceleration.z;
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/(s^2)");
  
  // Get the orientation of the sensor
  uint8_t orientation = mma.getOrientation();
  switch (orientation) {
    case MMA8451_PL_PUF: 
      Serial.println("Portrait Up Front");
      break;
    case MMA8451_PL_PUB: 
      Serial.println("Portrait Up Back");
      break;    
    case MMA8451_PL_PDF: 
      Serial.println("Portrait Down Front");
      break;
    case MMA8451_PL_PDB: 
      Serial.println("Portrait Down Back");
      break;
    case MMA8451_PL_LRF: 
      Serial.println("Landscape Right Front");
      break;
    case MMA8451_PL_LRB: 
      Serial.println("Landscape Right Back");
      break;
    case MMA8451_PL_LLF: 
      Serial.println("Landscape Left Front");
      break;
    case MMA8451_PL_LLB: 
      Serial.println("Landscape Left Back");
      break;
   }
   Serial.println();
   delay(1000);
}
