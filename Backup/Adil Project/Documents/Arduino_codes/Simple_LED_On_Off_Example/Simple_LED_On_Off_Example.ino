int i=1;
void setup() {
 Serial.begin(9600);
 pinMode(13, OUTPUT);
}

void loop() {

  if (i==1)
  {
  digitalWrite(13, HIGH);
  }// turn the LED on (HIGH is the voltage level)
  i++;
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second

}
