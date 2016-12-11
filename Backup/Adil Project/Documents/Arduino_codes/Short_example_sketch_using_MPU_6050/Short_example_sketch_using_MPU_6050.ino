#include <MPU6050.h>
// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include<Wire.h>
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp;
int32_t fZg;
MPU6050 accelgyro;
const float alpha = 0.09;
double AcX_G=0, AcY_G=0, AcZ_G=0, filter_valueZ=0, fXg=0, fYg=0;
int count=0;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  //Wire.write(1 << 8);   // set to zero (wakes up the MPU-6050)
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Leonardo measured 250kHz.

  // initialize device
  accelgyro.initialize();

  // wait for ready
  while (Serial.available() && Serial.read()); // empty buffer

  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  // delay(1000);
  // reset offsets
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  Serial.println("Setting Offset of MPU6050");
    accelgyro.setXAccelOffset(-488);
    accelgyro.setYAccelOffset(-2022);
    accelgyro.setZAccelOffset(911);
  
}
void loop() {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 4, true); // request a total of 14 registers
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    //AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    for (int k = 0; k < 15; k++) // Applying moving average filter
    {
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x3F);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU_addr, 2, true);
      AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      fZg = fZg + AcZ;
      count++;
      AcZ = 0;
    }
    filter_valueZ = fZg / count;
    count = 0;
    fZg = 0;
    fXg = AcX * alpha + (fXg * (1.0 - alpha));
    fYg = AcY * alpha + (fYg * (1.0 - alpha));
    //fZg = AcZ * alpha + (fZg * (1.0 - alpha));
      Serial.print("accelerometer Readings without Low Pass Filter:  \t");
      Serial.print("AcX = "); Serial.print(AcX);
      Serial.print(" | AcY = "); Serial.print(AcY);
      Serial.print(" | AcZ = "); Serial.println(filter_valueZ,4);

      Serial.println();
    
      /*Serial.print("accelerometer Readings using Low Pass Filter:  \t");
      Serial.print("AcX = "); Serial.print(fXg,4);
      Serial.print(" | AcY = "); Serial.print(fYg,4);
      Serial.print(" | AcZ = "); Serial.println( filter_valueZ,4);
    */
    AcX_G = (fXg)/16384; //Acceleration of X-axis in g, ranging from -2G to 2G
    AcY_G = (fYg)/ 16384; //Acceleration of Y-axis in g, ranging from -2G to 2G
    AcZ_G = (((filter_valueZ)/16384))-1; //Acceleration of Z-axis in g, ranging from -2G to 2G
    
 /* Serial.print("accelerometer Readings using Low Pass Filter:  \t");
  Serial.print("AcX_G = "); Serial.print(AcX_G,4);
  Serial.print(" | AcY_G = "); Serial.print(AcY_G,4);
  Serial.print(" | AcZ_G = "); Serial.println(AcZ_G,4);
*/
  delay(333);
}
