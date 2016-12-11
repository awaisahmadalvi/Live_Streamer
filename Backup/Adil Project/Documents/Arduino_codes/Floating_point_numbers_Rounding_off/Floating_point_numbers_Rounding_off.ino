#include <Wire.h>
float val = 37.77779;
void setup() {
// put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:


float rounded_down = floorf(val * 100) / 100;   /* Result: 37.77 */
float rounded = ((int)(val * 100 + .5) / 100); /* Result: 37.78 */
float rounded_up = ceilf(val * 100) / 100;      /* Result: 37.78*/

 
 //Serial.println(rounded_down);
 Serial.println(rounded);
 //Serial.println(rounded_up);
 
 delay(500);
}
