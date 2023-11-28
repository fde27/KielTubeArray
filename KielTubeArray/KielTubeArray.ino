// Kiel Tube Array code (for the teensy 4.1)
// Ferdinand Defregger
// 27/11/23


// Libraries needed
#include <Wire.h>
#include "SparkFun_LPS28DFW_Arduino_Library.h"

LPS28DFW pressureSensor;

// Addresses of the multiplexer for each I2C bus (manually selectable with 0 ohm resistors A0, A1, A2)
#define MULTIPLEXER1_ADDR (0x70)
#define MULTIPLEXER2_ADDR (0x71)
#define MULTIPLEXER3_ADDR (0x72)

uint8_t i2cAddress = LPS28DFW_I2C_ADDRESS_DEFAULT; // 0x5C


// Select desired sensor (1st I2C bus)
void TCA9548A1(uint8_t bus, uint8_t multiplexer){
  Wire.beginTransmission(multiplexer);  // Multiplexer address on the I2C bus
  Wire.write(1 << bus);          // Send byte to selected multiplexer to select desired pressure sensor
  Wire.endTransmission();
}

// // Select desired sensor (2nd I2C bus)
// void TCA9548A2(uint8_t bus, uint8_t multiplexer){
//   Wire1.beginTransmission(multiplexer);  
//   Wire1.write(1 << bus);          
//   Wire1.endTransmission();
// }

// // Select desired sensor (3rd I2C bus)
// void TCA9548A3(uint8_t bus, uint8_t multiplexer){
//   Wire2.beginTransmission(multiplexer);  
//   Wire2.write(1 << bus);          
//   Wire2.endTransmission();
// }




void setup() {
  Serial.begin(115200);
  Serial.println("Starting Serial");

  Wire.begin();

  TCA9548A1(0, MULTIPLEXER1_ADDR);
  while(pressureSensor.begin(i2cAddress) != LPS28DFW_OK)
    {
        // Not connected, inform user
        Serial.println("Error: LPS28DFW not connected, check wiring and I2C address!");

        // Wait a bit to see if connection is established
        delay(1000);
    }
  Serial.println("Done setting up");
}

void loop() {
    // Get measurements from the sensor. This must be called before accessing
    // the pressure data, otherwise it will never update
    pressureSensor.getSensorData();

    // Print temperature and pressure
    Serial.print("Temperature (C): ");
    Serial.print(pressureSensor.data.heat.deg_c);
    Serial.print("\t\t");
    Serial.print("Pressure (hPa): ");
    Serial.println(pressureSensor.data.pressure.hpa);

    // Only print every second
    delay(1000);
}