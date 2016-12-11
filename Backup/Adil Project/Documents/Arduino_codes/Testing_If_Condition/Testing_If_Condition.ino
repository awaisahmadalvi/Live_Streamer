void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 double a= 0.009;
 double b=0.09;
 double c=0.01;
if (((a>0.009) && (b>0.009)) || ((b>0.009) && (c>0.009)) || ((c>0.009) && (a>0.009)))
{
  Serial.println("I'm in if condition");
  }
else
  Serial.println("I'm outside if condition");
  delay(500);


}
