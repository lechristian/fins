// For Bluetooth
#include <SPI.h>
#include <Adafruit_BLE_UART.h>
// For Accelerometer
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

// Accelerometer
Adafruit_MMA8451 accelerometer = Adafruit_MMA8451();

// Bluetooth LE
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 1
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

void setup(void)
{
  Serial.begin(9600);
  while (!Serial); // Leonardo/Micro should wait for serial init

  // Setup BLE
  BLEserial.setDeviceName("Fins"); // 7 Characters Max
  BLEserial.begin();

  // Setup Accelerometer
  if (!accelerometer.begin()) {
    Serial.println("Couldn't Start");
    while (1);
  }
  Serial.println("Accelerometer Found");

  accelerometer.setRange(MMA8451_RANGE_2_G);
  Serial.print("Range = "); Serial.print(2 << accelerometer.getRange());
}

aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop()
{
  // Tell the nRF8001 to do whatever it should be working on.
  BLEserial.pollACI();

  // Ask what is our current status
  aci_evt_opcode_t status = BLEserial.getState();

  // If the status changed.
  if (status != laststatus) {
    if (status == ACI_EVT_DEVICE_STARTED) {
      Serial.println(F("* Advertising started"));
    }

    if (status == ACI_EVT_CONNECTED) {
      Serial.println(F("* Connected!"));
    }

    if (status == ACI_EVT_DISCONNECTED) {
      Serial.println(F("* Disconnected or advertising timed out"));
    }

    laststatus = status;
  }

  if (status == ACI_EVT_CONNECTED) {
    // Check for data
    if (BLEserial.available()) {
      Serial.print("* ");
      Serial.print(BLEserial.available());
      Serial.println(F(" bytes available from BTLE"));
    }

    // While there is something to read, read the next char
    while (BLEserial.available()) {
      char c = BLEserial.read();
      Serial.print(c);
    }

    Serial.setTimeout(100);

    // Get acceleration data
    String accel_data = getAcceleration();

    // Get buffer length
    uint8_t sendbuffer[20];
    accel_data.getBytes(sendbuffer, 20);
    char sendbuffersize = min(20, accel_data.length());

    // Print for Debugging
    // Serial.print(F("\n* Sending -> \""));
    // Serial.print((char *)sendbuffer);
    // Serial.println("\"");

    // Write to BLE buffer
    BLEserial.write(sendbuffer, sendbuffersize);
  }

  delay(1000);
}

// Get Accelerometer Acceleration
String getAcceleration()
{
  // Read the 'raw' data in 14-bit counts
  accelerometer.read();
  // Get a new sensor event
  sensors_event_t event;
  accelerometer.getEvent(&event);
  // Get acceleration in m/(s^2)
  float accel_x = event.acceleration.x;
  float accel_y = event.acceleration.y;
  float accel_z = event.acceleration.z;

  String x = String(accel_x, 2);
  String y = String(accel_y, 2);
  String z = String(accel_z, 2);

  String data = x + ", " + y + ", " + z + "\n";

  return data;
}

// Get Accelerometer Orientation
String accelerometerOrientation()
{
  uint8_t orientation = accelerometer.getOrientation();
  switch (orientation) {
    case MMA8451_PL_PUF:
      return "Portrait Up Front";
      break;
    case MMA8451_PL_PUB:
      return "Portrait Up Back";
      break;
    case MMA8451_PL_PDF:
      return "Portrait Down Front";
      break;
    case MMA8451_PL_PDB:
      return "Portrait Down Back";
      break;
    case MMA8451_PL_LRF:
      return "Landscape Right Front";
      break;
    case MMA8451_PL_LRB:
      return "Landscape Right Back";
      break;
    case MMA8451_PL_LLF:
      return "Landscape Left Front";
      break;
    case MMA8451_PL_LLB:
      return "Landscape Left Back";
      break;
  }
}

