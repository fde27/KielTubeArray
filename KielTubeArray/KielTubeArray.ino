// Kiel Tube Array code (for the teensy 4.1)
// Ferdinand Defregger
// 27/11/23


// Libraries needed
#include <Wire.h>
#include "SparkFun_LPS28DFW_Arduino_Library.h"

// Addresses of the multiplexer for each I2C bus (manually selectable with 0 ohm resistors A0, A1, A2)
#define MULTIPLEXER1_ADDR (0x70)
#define MULTIPLEXER2_ADDR (0x71)
#define MULTIPLEXER3_ADDR (0x72)
#define NUM_SENSORS (8)

uint8_t i2cAddress = LPS28DFW_I2C_ADDRESS_DEFAULT; // 0x5C

LPS28DFW pressureSensor[NUM_SENSORS];
float ref_pres[] = {0, 0, 0, 0, 0, 0, 0, 0};


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

  // Start I2C buses
  Wire.begin();

  // Check first sensor is online
  TCA9548A1(0, MULTIPLEXER1_ADDR);
  while(pressureSensor[0].begin(i2cAddress) != LPS28DFW_OK) {
    Serial.println("Error: LPS28DFW not connected, check wiring and I2C address!");
    delay(1000);
  }

  delay(10);
  for (uint8_t sensor = 0; sensor < NUM_SENSORS; sensor++) {
    TCA9548A1(sensor, MULTIPLEXER1_ADDR);
    pressureSensor[sensor].getSensorData();
    ref_pres[sensor] = pressureSensor[sensor].data.pressure.hpa;
  }

  // // Setup sensors
  // lps28dfw_md_t modeConfig =
  // {
  //     .fs  = LPS28DFW_1260hPa,    // Full scale range
  //     .odr = LPS28DFW_200Hz,      // Output data rate
  //     .avg = LPS28DFW_4_AVG,      // Average filter
  //     .lpf = LPS28DFW_LPF_DISABLE // Low-pass filter
  // };

  // lps28dfw_ref_md_t refConfig =
  // {
  //     .apply_ref = LPS28DFW_OUT_AND_INTERRUPT,
  //     .get_ref = true
  // };

  // Serial.println("Reference pressures:");
  // for (uint8_t sensor = 0; sensor < NUM_SENSORS; sensor++) {
  //   pressureSensor[sensor].setModeConfig(&modeConfig);
  //   pressureSensor[sensor].setReferenceMode(&refConfig);
  //   delay(10);
  //   // Get reference pressure
  //   int16_t refPressureRaw = 0;
  //   pressureSensor[sensor].getReferencePressure(&refPressureRaw);
  //   float refPressureHPa = (refPressureRaw / 16.0)*100; // Divide by 16 in 1260hPa range, convert to Pa
  //   Serial.print("Sensor: ");
  //   Serial.print(sensor);
  //   Serial.print(": ");
  //   Serial.print(refPressureHPa);
  //   Serial.print("   ");
  // }
  // Serial.println("");

  

  Serial.println("Done setting up");
}

void loop() {
  for (uint8_t sensor = 0; sensor < NUM_SENSORS; sensor++) {
    TCA9548A1(sensor, MULTIPLEXER1_ADDR);
    pressureSensor[sensor].getSensorData();
    float reading = pressureSensor[sensor].data.pressure.hpa;
    Serial.print("Sensor: ");
    Serial.print(sensor);
    Serial.print(": ");
    Serial.print(reading);
    Serial.print("   ");
  }
  Serial.println("");
  // Only print every second
  delay(5);
}