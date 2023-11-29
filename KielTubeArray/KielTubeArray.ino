// Kiel Tube Array code (for the teensy 4.1)
// Ferdinand Defregger
// 27/11/23


// Libraries needed
#include <Wire.h>
#include "SparkFun_LPS28DFW_Arduino_Library.h"

// Addresses of the multiplexer for each I2C bus (manually selectable with 0 ohm resistors A0, A1, A2)
#define NUM_MULTI (3)
#define NUM_SENSORS (24)

uint8_t i2cAddress = LPS28DFW_I2C_ADDRESS_DEFAULT; // 0x5C
uint8_t multi_addr[] = {0x70, 0x75, 0x72, 0x70, 0x71, 0x72, 0x70, 0x71};
uint8_t num_sens[] = {8, 8, 6};

LPS28DFW pressureSensor[NUM_SENSORS];

// Select desired sensor (1st I2C bus)
void TCA9548A1(uint8_t bus, uint8_t multiplexer){
  Wire.beginTransmission(multiplexer);  // Multiplexer address on the I2C bus
  Wire.write(1 << bus);          // Send byte to selected multiplexer to select desired pressure sensor
  Wire.endTransmission();
}

// Set to default state
void TCA9548A1_DEFAULT(uint8_t multiplexer){
  Wire.beginTransmission(multiplexer);  // Multiplexer address on the I2C bus
  Wire.write(0);         
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial started");

  // Start I2C buses
  Wire.begin();
  Wire.setClock(400000);

  Wire1.begin();
  Wire1.setClock(400000);

  Wire2.begin();
  Wire2.setClock(400000);

  uint8_t initWireSelect = 0; // Set the common value to be passed

  // // Initialize each instance with the same value
  // for (int i = 0; i < NUM_SENSORS; i++) {
  //     pressureSensor[i] = LPS28DFW(initWireSelect); // Pass the same value to all instances
  // }

  // for (uint8_t multi = 0; multi < NUM_MULTI; multi++) {
  //   TCA9548A1_DEFAULT(multi_addr[multi]);   // Deactivate all sensors on multiplexer
  // }
  scanI2C();


  // Setup sensors
  lps28dfw_md_t modeConfig =
  {
      .fs  = LPS28DFW_1260hPa,    // Full scale range
      .odr = LPS28DFW_25Hz,       // Output data rate
      .avg = LPS28DFW_512_AVG,    // Average filter
      .lpf = LPS28DFW_LPF_DISABLE // Low-pass filter
  };

  lps28dfw_ref_md_t refConfig =
  {
      .apply_ref = LPS28DFW_RST_REFS,
      .get_ref = true
  };


  uint8_t offset = 0;
  for (uint8_t multi = 0; multi < NUM_MULTI; multi++) {  // Cycle through multiplexers on bus
    Serial.print("Reference pressures (");
    Serial.print(multi);
    Serial.println("):");

    for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {  // Cycle through sensors on multiplexer
      // Select sensor
      TCA9548A1(sensor, multi_addr[multi]);
      delay(5);

      // Set config
      pressureSensor[sensor+offset].setModeConfig(&modeConfig);
      pressureSensor[sensor+offset].setReferenceMode(&refConfig);

      // Get reference pressure
      int16_t refPressureRaw = 0;
      pressureSensor[sensor+offset].getReferencePressure(&refPressureRaw);
      float refPressureHPa = (refPressureRaw / 16.0)*100; // Divide by 16 in 1260hPa range, convert to Pa

      // Print ref. pressure
      Serial.print("Sensor: ");
      Serial.print(sensor+offset);
      Serial.print(": ");
      Serial.print(refPressureHPa);
      Serial.print("   ");

      TCA9548A1_DEFAULT(multi_addr[multi]);   // Deactivate all sensors on multiplexer
    }
  Serial.println("");
  offset += num_sens[multi];              // Used to keep track of pressureSensor instances
  }

  

  Serial.println("Done setting up");
}


void loop() {
  // for (uint8_t sensor = 0; sensor < num_sens[1]; sensor++) {
  //   TCA9548A1(sensor, multi_addr[1]);
  //   pressureSensor[sensor].getSensorData();
  //   float reading = pressureSensor[sensor].data.pressure.hpa;
  //   int reading_int = reading*100;
  //   Serial.print(reading_int);
  //   Serial.print("\t");
  // }
  // Serial.println();
  // delay(1000);
}












void scanI2C() {
    byte error, address;
    int deviceCount = 0;

    Serial.println("Scanning...");

    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");
            deviceCount++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    if (deviceCount == 0) {
        Serial.println("No I2C devices found.");
    } else {
        Serial.println("Scan complete.");
    }
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