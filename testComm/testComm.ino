#include <Wire.h>
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}
uint8_t inputsA = 0;
uint8_t inputsB = 0;
void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(0x20);
  Wire.write(0x13);
  Wire.endTransmission();
  Wire.requestFrom(0x20, 1);
  inputsB = Wire.read();
  Wire.beginTransmission(0x20);
  Wire.write(0x12);
  Wire.endTransmission();
  Wire.requestFrom(0x20, 1);
  inputsA = Wire.read();
  Serial.print("A: ");
  Serial.println(inputsA, BIN);
  Serial.print("B: ");
  Serial.println(inputsB, BIN);
  delay(200);
}
