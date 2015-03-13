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

}
