#include <I2Cdev.h>
#include <MPU6050.h>

// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include<Wire.h>

int buffersize = 1000;   //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone = 8;   //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int giro_deadzone = 1;   //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)


const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ;
int32_t fZg = 0;
MPU6050 accelgyro;
const float alpha = 0.09;
int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, state = 0, count = 0;
int ax_offset, ay_offset, az_offset;
double AcX_Numerical = 0, AcY_Numerical = 0, AcZ_Numerical = 0, filter_valueZ = 0, fXg = 0, fYg = 0;
float as = 0.004883; //Arduino analog resolution = 5Volts/1024 (10bits)
float zg = 1.65; //Sensor zero g bias = Supply Voltage/2 =3.3/2 (V)
float sR = 0.329; //Sensor resolution (V/g)
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
  accelgyro.setXAccelOffset(-520);
  accelgyro.setYAccelOffset(-516);
  accelgyro.setZAccelOffset(2907);
}
void loop() {
  /*  if (state==0){
     //Serial.println("\nReading sensors for first time...");
     meansensors();
     state++;
     //delay(1000);
    }

    if (state==1) {
     //Serial.println("\nCalculating offsets...");
     calibration();
     state++;
     //delay(1000);
    }

    if (state==2) {
     meansensors();
     //Serial.println("\nFINISHED!");
     //Serial.print("\nSensor readings with offsets:\t");
     /*Serial.print(mean_ax);
     Serial.print("\t");
     Serial.print(mean_ay);
     Serial.print("\t");
     Serial.print(mean_az);
     Serial.print("\t");

     //Serial.print("Your offsets:\t");
     Serial.print(ax_offset);
     Serial.print("\t");
     Serial.print(ay_offset);
     Serial.print("\t");
     Serial.print(az_offset);
     Serial.print("\t");

    // Serial.println("\nData is printed as: acelX acelY acelZ ");
     //Serial.println("Check that your sensor readings are close to 0 0 16384");
    }*/
  /*accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);
    state = 0;
  */
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 4, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  fXg = AcX * alpha + (fXg * (1.0 - alpha));
  fYg = AcY * alpha + (fYg * (1.0 - alpha));
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
  fZg = AcZ * alpha + (fZg * (1.0 - alpha));
/*  Serial.print("Accelerometer Readings without Low Pass Filter:  \t");
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.println(filter_valueZ);
*/
  Serial.println();

  /* Serial.print("accelerometer Readings using Low Pass Filter:  \t");
    Serial.print("AcX = "); Serial.print(fXg);
    Serial.print(" | AcY = "); Serial.print(fYg);
    Serial.print(" | AcZ = "); Serial.println(AcZ);
  */
  AcX_Numerical = (fXg) / 16384; //Acceleration of X-axis in m/s^2, ranging from -19.62 m/s^2 to +19.62 m/s^2;
  AcY_Numerical = (fYg) / 16384; //Acceleration of Y-axis in m/s^2, ranging from -19.62 m/s^2 to +19.62 m/s^2;
  AcZ_Numerical = ((filter_valueZ) / 16384) - 1; //Acceleration of Z-axis in m/s^2, ranging from -19.62 m/s^2 to +19.62 m/s^2;

  Serial.print("Numerical accelerometer Readings in m/s^2 :  \t");
  Serial.print("AcX = "); Serial.print(AcX_Numerical, 4);
  Serial.print(" | AcY = "); Serial.print(AcY_Numerical, 4);
  Serial.print(" | AcZ = "); Serial.println(AcZ_Numerical, 4);

  delay(500);
}



void meansensors() {
  long i = 0, buff_ax = 0, buff_ay = 0, buff_az = 0;

  while (i < (buffersize + 101)) {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if (i > 100 && i <= (buffersize + 100)) { //First 100 measures are discarded
      buff_ax = buff_ax + ax;
      buff_ay = buff_ay + ay;
      buff_az = buff_az + az;
    }
    if (i == (buffersize + 100)) {
      mean_ax = buff_ax / buffersize;
      mean_ay = buff_ay / buffersize;
      mean_az = buff_az / buffersize;
    }
    i++;
    delay(2); //Needed so we don't get repeated measures
  }
}

void calibration() {
  ax_offset = -mean_ax / 8;
  ay_offset = -mean_ay / 8;
  az_offset = (16384 - mean_az) / 8;


  while (1) {
    int ready = 0;
    accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);

    meansensors();
    Serial.println("...");

    if (abs(mean_ax) <= acel_deadzone) ready++;
    else ax_offset = ax_offset - mean_ax / acel_deadzone;

    if (abs(mean_ay) <= acel_deadzone) ready++;
    else ay_offset = ay_offset - mean_ay / acel_deadzone;

    if (abs(16384 - mean_az) <= acel_deadzone) ready++;
    else az_offset = az_offset + (16384 - mean_az) / acel_deadzone;

    if (ready == 3) break;
  }
}
