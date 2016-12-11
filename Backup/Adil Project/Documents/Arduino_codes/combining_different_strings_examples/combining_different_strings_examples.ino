void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
String one, two, three, data;

one = "This is an intensity 8 Earthquake. Magnitude of the Earthquake is";
two = 7.96;
three = "\nDamage slight in specially designed structures,considerable in ordinary substantial buildings with partial collapse, great in poorly built structures.\n Panel walls thrown out of frame structures. Fall of chimneys, factory stacks, columns, monuments, walls. Heavy furniture overturned.\n Sand and mud ejected in small amounts. Changes in well water. Persons driving cars disturbed."; 
data = one + two + three;
Serial.println(data);
delay (500);
}
