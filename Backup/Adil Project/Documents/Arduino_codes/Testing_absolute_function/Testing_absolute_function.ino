void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int32_t a = -123456;
  int32_t b;
  double c = -0.9453;
  double d;

  d = abs (c);
  Serial.println(d,4);
  delay(500);
}
