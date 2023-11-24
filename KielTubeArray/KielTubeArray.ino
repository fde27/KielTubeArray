#include <Wire.h>


void setup() {
  Serial.begin(9600);
}

void loop() {
  bool done = false;
  int num = 0; 

  while (!done) { 
    Serial.println(num);
    delay(1000);
    num += 1;
  }
}