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
int16_t AcX = 0, AcY = 0, AcZ = 0, Tmp = 0;
double fXg = 0, fYg = 0, filter_valueZ = 0,  highVelocityX = 0 ,  highVelocityY = 0 ,  highVelocityZ = 0 ;
int32_t fZg = 0;
MPU6050 accelgyro;
const float alpha = 0.09;
int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, state = 0, count = 0, count1 = 0, i = 0, count3 = 0;
int ax_offset, ay_offset, az_offset;
double AcX_G = 0, AcY_G = 0, AcZ_G = 0, Abs_AcX_G = 0, Abs_AcY_G = 0, Abs_AcZ_G = 0, PGV = 0;
double velocityX = 0, velocityY = 0, velocityZ = 0, velocityXCm = 0 , velocityYCm = 0, velocityZCm = 0;
double currAccelX, currAccelY, currAccelZ, Accel_Round2X, Accel_Round2Y, Accel_Round2Z;
double prevAccelX = 0, prevAccelY = 0, prevAccelZ = 0, Abs_velocityX = 0 , Abs_velocityY = 0 , Abs_velocityZ = 0 ;
unsigned long prevTimeX = micros();
unsigned long prevTimeY = micros();
unsigned long prevTimeZ = micros();
unsigned long currTime;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  //Wire.write(1 << 8);   // set to zero (wakes up the MPU-6050)
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  TWBR = 12; // 400kHz I2C clock (200kHz if CPU is 8MHz). Leonardo measured 250kHz.

  // initialize device
  accelgyro.initialize();

  // wait for ready
  while (Serial.available() && Serial.read()); // empty buffer

  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  // delay(1000);
  // reset offsets
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  Serial.println("Setting Offset of MPU6050");
  SetOffsetOfMPU();
}
void loop() {

  for (int j = 0; j < 5 ; j++)
  {

    if (count1 == 0)
    {
      count1 = -5;
      Serial.println("Monitoring EarthQuake based on PGV");
    }
    count1++;
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
    /*Serial.print("accelerometer Readings without Low Pass Filter:  \t");
      Serial.print("AcX = "); Serial.print(AcX);
      Serial.print(" | AcY = "); Serial.print(AcY);
      Serial.print(" | AcZ = "); Serial.println(AcZ);

      Serial.println();
    */
    /* Serial.print("accelerometer Readings using Low Pass Filter:  \t");
      Serial.print("AcX = "); Serial.print(fXg);
      Serial.print(" | AcY = "); Serial.print(fYg);
      Serial.print(" | AcZ = "); Serial.println(AcZ);
    */
    AcX_G = (fXg) / 16384; //Acceleration of X-axis in g, ranging from -2G to 2G
    AcY_G = (fYg) / 16384; //Acceleration of Y-axis in g, ranging from -2G to 2G
    AcZ_G = ((filter_valueZ) / 16384) - 1; //Acceleration of Z-axis in g, ranging from -2G to 2G

    Abs_AcX_G = abs(AcX_G);
    Abs_AcY_G = abs(AcY_G);
    Abs_AcZ_G = abs(AcZ_G);

    if (((Abs_AcX_G > 0.003) && (Abs_AcY_G > 0.003)) || ((Abs_AcX_G > 0.003) && (Abs_AcZ_G > 0.003)) || ((Abs_AcY_G > 0.003) && (Abs_AcZ_G > 0.003)))
    {
      for (i = 0; i < 120; i++)
      {
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
        /*Serial.print("accelerometer Readings without Low Pass Filter:  \t");
          Serial.print("AcX = "); Serial.print(AcX);
          Serial.print(" | AcY = "); Serial.print(AcY);
          Serial.print(" | AcZ = "); Serial.println(AcZ);

          Serial.println();
        */
        /* Serial.print("accelerometer Readings using Low Pass Filter:  \t");
          Serial.print("AcX = "); Serial.print(fXg);
          Serial.print(" | AcY = "); Serial.print(fYg);
          Serial.print(" | AcZ = "); Serial.println(AcZ);
        */
        AcX_G = (fXg) / 16384; //Acceleration of X-axis in g, ranging from -2G to 2G
        AcY_G = (fYg) / 16384; //Acceleration of Y-axis in g, ranging from -2G to 2G
        AcZ_G = ((filter_valueZ) / 16384) - 1; //Acceleration of Z-axis in g, ranging from -2G to 2G

        Abs_AcX_G = abs(AcX_G);
        Abs_AcY_G = abs(AcY_G);
        Abs_AcZ_G = abs(AcZ_G);

        /*currAccelX = Abs_AcX_G * 9.81; //Converting the acceleration in m/s^2
        currAccelY = Abs_AcY_G * 9.81;
        currAccelZ = Abs_AcZ_G * 9.81;
        */    
        /*Accel_Round2X = ((int)(currAccelX * 100 + .5) / 100.0); //Round off the acceleration to 2 decimal places
        Accel_Round2Y = ((int)(currAccelY * 100 + .5) / 100.0);
        Accel_Round2Z = ((int)(currAccelZ * 100 + .5) / 100.0);
*/
        currTime = micros();
        velocityX  = AcX_G * (currTime - prevTimeX) / 1000000; //1000000 added to get the same units
        prevAccelX = Accel_Round2X;
        prevTimeX  = currTime;

        velocityY  = AcY_G * (currTime - prevTimeY) / 1000000; //1000000 added to get the same units
        prevAccelY = Accel_Round2Y;
        prevTimeY  = currTime;

        velocityZ  = AcZ_G * (currTime - prevTimeZ) / 1000000; //1000000 added to get the same units
        prevAccelZ = Accel_Round2Z;
        prevTimeZ  = currTime;

        Abs_velocityX = abs(velocityX);
        Abs_velocityY = abs(velocityY);
        Abs_velocityZ = abs(velocityZ);

        if (Abs_velocityX > highVelocityX)   // Calculating High Velocity Values in X,Y and Z Direction
        {
          highVelocityX = Abs_velocityX;
        }
        if (Abs_velocityY > highVelocityY)
        {
          highVelocityY = Abs_velocityY;
        }
        if (Abs_velocityZ > highVelocityZ)
        {
          highVelocityZ = Abs_velocityZ;
        }

        velocityX = 0; velocityY = 0; velocityZ = 0;

        /* Serial.print("Numerical accelerometer Readings in g :  \t");
          Serial.print("AcX = "); Serial.print(AcX_Numerical, 4);
          Serial.print(" | AcY = "); Serial.print(AcY_Numerical, 4);
          Serial.print(" | AcZ = "); Serial.println(AcZ_Numerical, 4);
        */
        /*  Serial.print("Velocity in cm/sec: \t");
          Serial.print(" VelocityX= "); Serial.print(velocityXCm);
          Serial.print(" | VelocityY = "); Serial.print(velocityYCm);
          Serial.print(" | VelocityZ = "); Serial.println(velocityZCm);
        */
        delay(250);
        if (count3 == 0)
        {
          Serial.println("Calculating Peak Ground Velocities in X, Y and Z direction");
          count3 = -40;
        }
        count3++;
      }
    }
    if (i == 120)
    {
      Serial.println("Peak Ground Velocities after Integration in m/sec");
      Serial.print("highX = "); Serial.print(highVelocityX);
      Serial.print(" | highY = "); Serial.print(highVelocityY);
      Serial.print(" | highZ = "); Serial.println(highVelocityZ);
    }
    highVelocityX =  highVelocityX * 100; // converting the velocity in cm/sec
    highVelocityY =  highVelocityY * 100;
    highVelocityZ =  highVelocityZ * 100;
    if (i == 120)
    {
      Serial.println("Peak ground Velocities in X,Y and Z Direction in cm/sec");
      Serial.print("highX = "); Serial.print(highVelocityX);
      Serial.print(" | highY = "); Serial.print(highVelocityY);
      Serial.print(" | highZ = "); Serial.println(highVelocityZ);
    }
    if (i == 120)
    {
      PGV = (highVelocityX + highVelocityY + highVelocityZ) / 3;
      Serial.println(PGV);
      Serial.println("Setting Offsets of MPU6050");
      SetOffsetOfMPU();
      i = 0;
       PGV = 0;
    //highAccelX = 0; highAccelY = 0; highAccelZ = 0;
    Abs_AcX_G = 0; Abs_AcY_G = 0; Abs_AcZ_G = 0;
    velocityXCm = 0; velocityYCm = 0; velocityZCm = 0;
    highVelocityX = 0; highVelocityY = 0; highVelocityZ = 0;
    velocityX = 0; velocityY = 0; velocityZ = 0;
    Abs_velocityX=0; Abs_velocityY=0; Abs_velocityZ=0;
    }
   
    delay(1000);
  }
}

void SetOffsetOfMPU ()
{
  accelgyro.setXAccelOffset(0);
  accelgyro.setYAccelOffset(0);
  accelgyro.setZAccelOffset(0);
  int state = 0;

  if (state == 0) {
    //Serial.println("\nReading sensors for first time...");
    meansensors();
    state++;
    //delay(1000);
  }

  if (state == 1) {
    //Serial.println("\nCalculating offsets...");
    calibration();
    state++;
    //delay(1000);
  }

  if (state == 2) {
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
    // prev_ax_offset = ax_offset;
    //prev_ay_offset = ay_offset;
    //prev_az_offset = az_offset;
    accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);
    state = 0;
  }
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
    int data_ready = 0;
    accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);

    meansensors();
    Serial.println("...");

    if (abs(mean_ax) <= acel_deadzone)
    {
      data_ready |= 0x01;
    }
    else ax_offset = ax_offset - mean_ax / acel_deadzone;

    if (abs(mean_ay) <= acel_deadzone)
    {
      data_ready |= 0x02;
    }
    else ay_offset = ay_offset - mean_ay / acel_deadzone;

    if (abs(16384 - mean_az) <= acel_deadzone)
    {
      data_ready |= 0x04;
    }
    else az_offset = az_offset + (16384 - mean_az) / acel_deadzone;

    if (data_ready == 0x07)
    {
      break;
    }
  }
}
