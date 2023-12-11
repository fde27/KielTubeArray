// Kiel Tube Array code (for the teensy 4.1)
// Ferdinand Defregger
// 27/11/23


// Libraries needed
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include "SparkFun_LPS28DFW_Arduino_Library.h"
#include "SparkFun_LPS28DFW_Arduino_Library_Wire1.h"
#include "SparkFun_LPS28DFW_Arduino_Library_Wire2.h"

// Addresses of the multiplexer for each I2C bus (manually selectable with 0 ohm resistors A0, A1, A2)
#define NUM_MULTI (3)
#define NUM_SENSORS (24)
#define NUM_SENSORS_BUS3 (16)

const uint8_t i2cAddress = LPS28DFW_I2C_ADDRESS_DEFAULT; // 0x5C
const int chipSelect = BUILTIN_SDCARD; // SD card
const uint8_t multi_addr[] = {0x70, 0x75, 0x72, 0x70, 0x71, 0x72, 0x70, 0x71};
const uint8_t num_sens[] = {8, 8, 6, 8, 8, 8, 8, 8};
int fileNumber = 0;
unsigned long timeSinceStart;



LPS28DFW pressureSensor[NUM_SENSORS];
LPS28DFW_W1 pressureSensor_W1[NUM_SENSORS];
LPS28DFW_W2 pressureSensor_W2[NUM_SENSORS_BUS3];

// Select desired sensor (1st I2C bus)
void TCA9548A(uint8_t bus, uint8_t multiplexer){
  Wire.beginTransmission(multiplexer);  // Multiplexer address on the I2C bus
  Wire.write(1 << bus);          // Send byte to selected multiplexer to select desired pressure sensor
  Wire.endTransmission();
}

// Select desired sensor (2nd I2C bus)
void TCA9548A_W1(uint8_t bus, uint8_t multiplexer){
  Wire1.beginTransmission(multiplexer);  
  Wire1.write(1 << bus);          
  Wire1.endTransmission();
}

// Select desired sensor (3rd I2C bus)
void TCA9548A_W2(uint8_t bus, uint8_t multiplexer){
  Wire2.beginTransmission(multiplexer);  
  Wire2.write(1 << bus);          
  Wire2.endTransmission();
}


// Set to default state
void TCA9548A_DEFAULT(uint8_t multiplexer){
  Wire.beginTransmission(multiplexer);  // Multiplexer address on the I2C bus
  Wire.write(0);         
  Wire.endTransmission();
}

// Set to default state (Wire1)
void TCA9548A_W1_DEFAULT(uint8_t multiplexer){
  Wire1.beginTransmission(multiplexer);  // Multiplexer address on the I2C bus
  Wire1.write(0);         
  Wire1.endTransmission();
}

// Set to default state (Wire2)
void TCA9548A_W2_DEFAULT(uint8_t multiplexer){
  Wire2.beginTransmission(multiplexer);  // Multiplexer address on the I2C bus
  Wire2.write(0);         
  Wire2.endTransmission();
}



void setup() {
  // Start serial
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial started");

  // Initialise SD card
  if (!SD.begin(chipSelect)) {
  Serial.println("Card failed, or not present");
  while (1) {
    // No SD card, so don't do anything more - stay stuck here
  }
}
Serial.println("card initialized.");

  // Start I2C buses
  Wire.begin();
  Wire.setClock(400000);
  Wire1.begin();
  Wire1.setClock(400000);
  Wire2.begin();
  Wire2.setClock(400000);



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

  // Setup sensors on first bus
  uint8_t offset = 0;
  int16_t refPressureRaw = 0;
  for (uint8_t multi = 0; multi < 3; multi++) {  // Cycle through multiplexers on bus
    Serial.print("Reference pressures (");
    Serial.print(multi);
    Serial.println("):");

    for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {  // Cycle through sensors on multiplexer
      // Select sensor
      TCA9548A(sensor, multi_addr[multi]);
      delay(5);

      // Set config
      pressureSensor[sensor+offset].setModeConfig(&modeConfig);
      pressureSensor[sensor+offset].setReferenceMode(&refConfig);

      // Get reference pressure
      pressureSensor[sensor+offset].getReferencePressure(&refPressureRaw);
      float refPressureHPa = (refPressureRaw / 16.0)*100; // Divide by 16 in 1260hPa range, convert to Pa

      // Print ref. pressure
      Serial.print("Sensor: ");
      Serial.print(sensor+offset);
      Serial.print(": ");
      Serial.print(refPressureHPa);
      Serial.print("   ");

      TCA9548A_DEFAULT(multi_addr[multi]);   // Deactivate all sensors on multiplexer
    }
  Serial.println();
  offset += num_sens[multi];              // Used to keep track of pressureSensor instances
  }


  // Setup sensors on second bus
  offset = 0;
  Serial.println();
  Serial.println("Second I2C Bus:");
  for (uint8_t multi = 3; multi < 6; multi++) {  // Cycle through multiplexers on bus
    Serial.print("Reference pressures (");
    Serial.print(multi);
    Serial.println("):");

    for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {  // Cycle through sensors on multiplexer
      // Select sensor
      TCA9548A_W1(sensor, multi_addr[multi]);
      delay(5);

      // Set config
      pressureSensor_W1[sensor+offset].setModeConfig(&modeConfig);
      pressureSensor_W1[sensor+offset].setReferenceMode(&refConfig);

      // Get reference pressure
      pressureSensor_W1[sensor+offset].getReferencePressure(&refPressureRaw);
      float refPressureHPa = (refPressureRaw / 16.0)*100; // Divide by 16 in 1260hPa range, convert to Pa

      // Print ref. pressure
      Serial.print("Sensor: ");
      Serial.print(sensor+offset);
      Serial.print(": ");
      Serial.print(refPressureHPa);
      Serial.print("   ");

      TCA9548A_W1_DEFAULT(multi_addr[multi]);   // Deactivate all sensors on multiplexer
    }
  Serial.println("");
  offset += num_sens[multi];              // Used to keep track of pressureSensor instances
  }

  // Setup sensors on third bus
  offset = 0;
  Serial.println();
  Serial.println("Third I2C Bus:");
  for (uint8_t multi = 6; multi < (8); multi++) {  // Cycle through multiplexers on bus
    Serial.print("Reference pressures (");
    Serial.print(multi);
    Serial.println("):");

    for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {  // Cycle through sensors on multiplexer
      // Select sensor
      TCA9548A_W2(sensor, multi_addr[multi]);
      delay(5);

      // Set config
      pressureSensor_W2[sensor+offset].setModeConfig(&modeConfig);
      pressureSensor_W2[sensor+offset].setReferenceMode(&refConfig);

      // Get reference pressure
      pressureSensor_W2[sensor+offset].getReferencePressure(&refPressureRaw);
      float refPressureHPa = (refPressureRaw / 16.0)*100; // Divide by 16 in 1260hPa range, convert to Pa

      // Print ref. pressure
      Serial.print("Sensor: ");
      Serial.print(sensor+offset);
      Serial.print(": ");
      Serial.print(refPressureHPa);
      Serial.print("   ");

      TCA9548A_W2_DEFAULT(multi_addr[multi]);   // Deactivate all sensors on multiplexer
    }
  Serial.println("");
  offset += num_sens[multi];              // Used to keep track of pressureSensor instances
  }
  Serial.println("Done setting up");
}


void loop() {
  // String containing data
  String dataString = "";
  timeSinceStart = millis();
  dataString += String(timeSinceStart) + ',';

  for (uint8_t multi = 0; multi < 3; multi++) {  // Cycle through multiplexers on bus
      for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {
        TCA9548A(sensor, multi_addr[multi]);
        pressureSensor[sensor].getSensorData();
        float reading = pressureSensor[sensor].data.pressure.hpa;
        int reading_int = reading*100;
        dataString += String(reading_int) + ',';
      }
      TCA9548A_DEFAULT(multi_addr[multi]); 
    }

  for (uint8_t multi = 3; multi < 6; multi++) {  // Cycle through multiplexers on bus
    for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {
      TCA9548A_W1(sensor, multi_addr[multi]);
      pressureSensor_W1[sensor].getSensorData();
      float reading = pressureSensor_W1[sensor].data.pressure.hpa;
      int reading_int = reading*100;
      dataString += String(reading_int) + ',';
      TCA9548A_W1_DEFAULT(multi_addr[multi]); 
    }
  }

  for (uint8_t multi = 6; multi < 8; multi++) {  // Cycle through multiplexers on bus
    for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {
      TCA9548A_W2(sensor, multi_addr[multi]);
      pressureSensor_W2[sensor].getSensorData();
      float reading = pressureSensor_W2[sensor].data.pressure.hpa;
      int reading_int = reading*100;
      dataString += String(reading_int);
      if (!(multi == 7 && sensor == 7)){
        dataString += ',';
      }
    TCA9548A_W2_DEFAULT(multi_addr[multi]); 
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
}



// for (uint8_t multi = 0; multi < 1; multi++) {  // Cycle through multiplexers on bus
//     for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {
//       TCA9548A(sensor, multi_addr[multi]);
//       pressureSensor[sensor].getSensorData();
//       float reading = pressureSensor[sensor].data.pressure.hpa;
//       int reading_int = reading*100;
//       dataString += String(reading_int) + ',';
//     }
//     TCA9548A_DEFAULT(multi_addr[multi]); 
//   }
//   for (uint8_t multi = 3; multi < 4; multi++) {  // Cycle through multiplexers on bus
//     for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {
//       TCA9548A_W1(sensor, multi_addr[multi]);
//       pressureSensor_W1[sensor].getSensorData();
//       float reading = pressureSensor_W1[sensor].data.pressure.hpa;
//       int reading_int = reading*100;
//       dataString += String(reading_int) + ',';
//       TCA9548A_DEFAULT(multi_addr[multi]); 
//     }
//   }
//   for (uint8_t multi = 6; multi < 7; multi++) {  // Cycle through multiplexers on bus
//     for (uint8_t sensor = 0; sensor < num_sens[multi]; sensor++) {
//       TCA9548A_W2(sensor, multi_addr[multi]);
//       pressureSensor_W2[sensor].getSensorData();
//       float reading = pressureSensor_W2[sensor].data.pressure.hpa;
//       int reading_int = reading*100;
//       dataString += String(reading_int);
//       if (!(multi == 7 && sensor == 7)){
//         dataString += ',';
//       }
//     TCA9548A_DEFAULT(multi_addr[multi]); 
//     }
//   }





