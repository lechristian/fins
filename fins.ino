#include <SPI.h>
// For Bluetooth
#include <Adafruit_BLE_UART.h>
// For Accelerometer
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
// For OLED Screen
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>

// Accelerometer
Adafruit_MMA8451 accelerometer = Adafruit_MMA8451();

// Bluetooth LE
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 1
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

// OLED Screen
#define OLED_SCLK A0
#define OLED_MOSI A1
#define OLED_CS   8
#define OLED_RST  7
#define OLED_DC   6

// Color definitions
#define BLACK           0x0000
#define RED             0xF800
#define CYAN            0x07FF
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

Adafruit_SSD1331 display = Adafruit_SSD1331(OLED_CS, OLED_DC, OLED_MOSI, OLED_SCLK, OLED_RST);

void setup(void)
{
  Serial.begin(9600);
  
  // Setup OLED
  display.begin();

  drawFlip();
  
  // Setup BLE
  BLEserial.setDeviceName("Flip"); // 7 Characters Max
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
int counter = 0;
String lastMovement = "";
uint16_t time = millis();
String accel_buffer[10];
boolean sent = false;

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
    
    if (!sent) {
      bleSend();
    }

    // Print for Debugging
    // Serial.print(F("\n* Sending -> \""));
    // Serial.print((char *)sendbuffer);
    // Serial.println("\"");
  } else {
    int index = counter % 10;
    accel_buffer[index] = getAcceleration();
    counter += 1;
    
    if (counter == 10) {
      counter = 0;
    }
    
    if (isMoving()) {
      if (lastMovement != "m") {
        drawFish();
        lastMovement = "m";
        time = millis();
      }
    } else {
      if (lastMovement != "s" && (millis() - time > 2500)) {
        drawStarfish();
        lastMovement = "s";
      }
    }
  }

  delay(1000);
}

void bleSend()
{
  int k;
  for (k = 0; k < 10; k += 1) {
    // Get buffer length
    uint8_t sendbuffer[20];
    accel_buffer[k].getBytes(sendbuffer, 20);
    char sendbuffersize = min(20, accel_buffer[k].length());
    
    // Write to BLE buffer
    BLEserial.write(sendbuffer, sendbuffersize);
    delay(200);
  }
  
  sent = true;
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

  String data = x + "," + y + "," + z + "\n";
  return data;
}

boolean isMoving()
{
  accelerometer.read();
  // Get a new sensor event
  sensors_event_t event;
  accelerometer.getEvent(&event);
  
  float accel_x = event.acceleration.x;
  float accel_y = event.acceleration.y;
  
  if (abs(accel_x) > 0.15 || abs(accel_y) > 0.15) {
    return true;
  } else {
    return false;
  }
}

void drawFlip(void) {
  display.fillScreen(BLACK);
  
  display.drawFastVLine(21, 17, 4, CYAN);
  display.drawFastVLine(21, 40, 3, CYAN);
  display.drawFastVLine(22, 17, 4, CYAN);
  display.drawFastVLine(22, 40, 3, CYAN);
  display.drawFastVLine(23, 18, 5, CYAN);
  display.drawFastVLine(23, 36, 6, CYAN);
  display.drawFastVLine(24, 20, 8, CYAN);
  display.drawFastVLine(24, 31, 9, CYAN);
  display.drawFastVLine(25, 20, 20, CYAN);
  display.drawFastVLine(26, 21, 17, CYAN);
  display.drawFastVLine(27, 25, 8, CYAN);
  display.fillRect(28, 28, 14, 4, CYAN);
  display.fillRect(32, 20, 5, 22, CYAN);
  display.fillRect(34, 18, 9, 2, CYAN); 
  display.drawFastHLine(35, 17, 8, CYAN);
  display.drawFastHLine(37, 16, 5, CYAN);
  display.fillRect(46, 17, 4, 25, CYAN);
  display.fillRect(53, 17, 4, 5, CYAN);
  display.fillRect(53, 25, 4, 17, CYAN);
  display.fillRect(61, 27, 4, 26, CYAN);
  display.fillRect(65, 28, 2, 14, CYAN);
  display.fillRect(67, 27, 2, 14, CYAN);
  display.fillRect(67, 27, 7, 4, CYAN);
  display.fillRect(67, 39, 7, 4, CYAN);
  display.fillRect(73, 30, 3, 10, CYAN);
  display.fillRect(72, 28, 3, 4, CYAN);
  display.drawFastVLine(76, 32, 6, CYAN);
  
  delay(2000);
}

void drawFish(void) {
  display.fillScreen(BLACK);
  
  display.fillRect(22, 31, 67, 5, YELLOW);
  
  display.fillRect(23, 28, 65, 3, YELLOW);
  display.fillRect(23, 36, 65, 3, YELLOW);
  
  display.fillRect(24, 26, 63, 2, YELLOW);
  display.fillRect(24, 39, 63, 2, YELLOW);

  display.fillRect(25, 24, 60, 2, YELLOW);
  display.fillRect(25, 41, 60, 2, YELLOW);

  display.fillRect(27, 22, 56, 2, YELLOW);
  display.fillRect(27, 43, 56, 2, YELLOW);

  display.fillRect(30, 20, 51, 2, YELLOW);
  display.fillRect(30, 45, 51, 2, YELLOW);
  
  display.drawFastHLine(33, 19, 45, YELLOW);
  display.drawFastHLine(33, 47, 45, YELLOW);
  
  display.drawFastHLine(35, 18, 41, YELLOW);
  display.drawFastHLine(35, 48, 41, YELLOW);

  display.drawFastHLine(38, 17, 35, YELLOW);
  display.drawFastHLine(38, 49, 35, YELLOW);

  display.drawFastHLine(43, 16, 25, YELLOW);
  display.drawFastHLine(43, 50, 25, YELLOW);

  display.drawFastHLine(50, 15, 11, YELLOW);
  display.drawFastHLine(50, 51, 11, YELLOW);
  
  display.fillTriangle(10, 16, 28, 33, 10, 50, YELLOW);
  delay(500);
}

void drawStarfish(void) {
  display.fillScreen(BLACK);
  
  display.fillRect(40, 24, 21, 20, RED);
  
  display.fillRect(45, 9, 4, 15, RED);
  display.drawPixel(41, 23, RED);
  display.drawFastVLine(42, 19, 5, RED);
  display.drawFastVLine(43, 15, 9, RED);
  display.drawFastVLine(44, 13, 11, RED);
  display.fillRect(46, 7, 2, 2, RED);
  display.drawFastVLine(49, 11, 13, RED);
  display.drawFastVLine(50, 15, 9, RED);
  display.drawFastVLine(51, 18, 6, RED);
  display.drawFastVLine(52, 22, 2, RED);
  
  display.drawPixel(60, 23, RED);
  display.fillRect(61, 23, 3, 10, RED);
  display.fillRect(64, 22, 3, 9, RED);
  display.drawFastVLine(67, 21, 9, RED);
  display.fillRect(68, 20, 2, 9, RED);
  display.drawFastVLine(70, 19, 9, RED);
  display.drawFastVLine(71, 19, 7, RED);
  display.drawPixel(66, 21, RED);
  display.drawFastVLine(61, 33, 2, RED);
  display.fillRect(72, 18, 2, 5, RED);
  display.drawFastVLine(74, 18, 3, RED);
  
  display.fillRect(56, 44, 14, 4, RED);
  display.drawFastVLine(61, 38, 6, RED);
  display.fillRect(62, 41, 3, 3, RED);
  display.drawFastHLine(65, 43, 3, RED);
  display.fillRect(70, 45, 2, 3, RED);
  display.drawFastVLine(72, 46, 3, RED);
  display.fillRect(60, 48, 12, 2, RED);
  display.drawFastHLine(53, 44, 3, RED);
  display.drawFastHLine(54, 45, 2, RED);
  display.drawFastHLine(57, 48, 3, RED);
  display.drawFastHLine(64, 50, 4, RED);
  
  display.fillRect(39, 44, 9, 7, RED);
  display.fillRect(37, 51, 8, 4, RED);
  display.drawFastVLine(39, 41, 3, RED);
  display.drawFastVLine(38, 45, 6, RED);
  display.drawFastVLine(48, 44, 4, RED);
  display.drawFastHLine(45, 51, 2, RED);
  display.drawFastHLine(36, 55, 8, RED);
  display.drawFastHLine(36, 56, 5, RED);
  display.drawFastHLine(37, 57, 2, RED);
  
  display.fillRect(34, 27, 6, 8, RED);
  display.fillRect(29, 26, 5, 6, RED);
  display.fillRect(25, 25, 4, 4, RED);
  display.drawFastHLine(37, 35, 3, RED);
  display.drawFastHLine(31, 32, 3, RED);
  display.drawFastHLine(26, 29, 3, RED);
  display.drawFastVLine(24, 26, 3, RED);
  display.drawPixel(39, 36, RED);
  display.drawPixel(33, 33, RED);
  display.drawPixel(28, 30, RED);
  display.drawPixel(29, 25, RED);
  display.drawPixel(39, 26, RED);
  delay(500);
}

