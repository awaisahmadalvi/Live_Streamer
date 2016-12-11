#include<Wire.h>
#include<stdint.h>
uint64_t checkout;
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available())
  {
    checkout = 12345678901234567890;
    Serial.println(checkout);
  }
}
