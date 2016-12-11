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
int16_t AcX = 0, AcY = 0, AcZ = 0;//, Tmp = 0;
double fXg = 0, fYg = 0, filter_valueZ = 0;
int32_t fZg = 0;
MPU6050 accelgyro;
const float alpha = 0.09, beta = 0.4;
int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, state = 0, count = 0, i = 0, count3 = 0, count4 = 0;
int ax_offset, ay_offset, az_offset, prev_ax_offset, prev_ay_offset, prev_az_offset, OffsetDisturbed=0;
double AcX_G = 0, AcY_G = 0, AcZ_G = 0, Abs_AcX_G = 0, Abs_AcY_G = 0 , Abs_AcZ_G = 0;
double velocityX = 0, velocityY = 0, velocityZ = 0, velocityXCm = 0 , velocityYCm = 0, velocityZCm = 0, Abs_velocityXCm = 0 , Abs_velocityYCm = 0 , Abs_velocityZCm = 0;
double currAccelX, currAccelY, currAccelZ, Accel_Round2X, Accel_Round2Y, Accel_Round2Z;
double highAccelX = 0, highAccelY = 0, highAccelZ = 0, highVelocityX = 0, highVelocityY = 0, highVelocityZ = 0;
double prevAccelX = 0, prevAccelY = 0, prevAccelZ = 0;
double PGA = 0, PGV = 0, VelocityX_LPF=0, VelocityY_LPF=0, VelocityZ_LPF=0;
unsigned long currTime;
unsigned long prevTimeX = micros();
unsigned long prevTimeY = micros();
unsigned long prevTimeZ = micros();
int count1 = 0;

void setup() {
  
  Wire.begin();
  
  TWBR = 12; // 400kHz I2C clock (200kHz if CPU is 8MHz). Leonardo measured 250kHz.
  Serial.begin(9600);
  accelgyro.initialize();
  while (Serial.available() && Serial.read()); // empty buffer
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  Serial.println("Setting Offset of MPU6050");
  SetOffsetOfMPU();
  
 
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  //Wire.write(1 << 8);   // set to zero (wakes up the MPU-6050)
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);  
  
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  
  /* accelgyro.setXAccelOffset(-585);
    accelgyro.setYAccelOffset(-2206);
    accelgyro.setZAccelOffset(916);
  */
}
void loop() {

  for (int j = 0; j < 5 ; j++)
  {

    if (count1 == 0)
    {
      count1 = -5;
      Serial.println("Monitoring EarthQuake");
      Serial.print("Number of EarthQuake Tested:  ");
      Serial.println(count4);
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

    if (((Abs_AcX_G > 0.003) && (Abs_AcY_G > 0.003)) || ((Abs_AcY_G > 0.003) && (Abs_AcZ_G > 0.003)) || ((Abs_AcZ_G > 0.003) && (Abs_AcX_G > 0.003)))
    {
      for (i = 0; i < 200; i++)
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

        /*currAccelX = AcX_G * 9.81; //Converting the acceleration in m/s^2
          currAccelY = AcY_G * 9.81;
          currAccelZ = AcZ_G * 9.81;
        */
          Accel_Round2X = ((int)(AcX_G * 100 + .5) / 100.0); //Round off the acceleration to 2 decimal places
          Accel_Round2Y = ((int)(AcY_G * 100 + .5) / 100.0);
          Accel_Round2Z = ((int)(AcZ_G * 100 + .5) / 100.0);
        
        currTime = micros();
        velocityX  = (Accel_Round2X + prevAccelX) / 2 * (currTime - prevTimeX) / 1000000; //1000000 added to get the same units
        VelocityX_LPF = velocityX * beta + (VelocityX_LPF * (1.0 - beta));
        prevAccelX = Accel_Round2X;
        prevTimeX  = currTime;

        velocityY  = (Accel_Round2Y + prevAccelY) / 2 * (currTime - prevTimeY) / 1000000; //1000000 added to get the same units
        VelocityY_LPF = velocityY * beta + (VelocityY_LPF * (1.0 - beta));
        prevAccelY = Accel_Round2Y;
        prevTimeY  = currTime;

        velocityZ  = (Accel_Round2Z + prevAccelZ) / 2 * (currTime - prevTimeZ) / 1000000; //1000000 added to get the same units
        VelocityZ_LPF = velocityZ * beta + (VelocityZ_LPF * (1.0 - beta));
        prevAccelZ = Accel_Round2Z;
        prevTimeZ  = currTime;

        velocityXCm = VelocityX_LPF * 100; // converting the velocity in cm/sec
        velocityYCm = VelocityY_LPF * 100;
        velocityZCm = VelocityZ_LPF * 100;

        Abs_velocityXCm = abs(velocityXCm);
        Abs_velocityYCm = abs(velocityYCm);
        Abs_velocityZCm = abs(velocityZCm);

        if (Abs_AcX_G > highAccelX)    // Calculating high Acceleration Values in X,Y and Z Direction
        {
          highAccelX = Abs_AcX_G;
        }
        if (Abs_AcY_G > highAccelY)
        {
          highAccelY = Abs_AcY_G;
        }
        if (Abs_AcZ_G > highAccelZ)
        {
          highAccelZ = Abs_AcZ_G;
        }


        if (Abs_velocityXCm > highVelocityX)   // Calculating High Velocity Values in X,Y and Z Direction
        {
          highVelocityX = Abs_velocityXCm;
        }
        if (Abs_velocityYCm > highVelocityY)
        {
          highVelocityY = Abs_velocityYCm;
        }
        if (Abs_velocityZCm > highVelocityZ)
        {
          highVelocityZ = Abs_velocityZCm;
        }

        if (count3 == 0)
        {
          Serial.println("Calculating Peak Ground Accelerations and Peak Ground Velocities in X, Y and Z direction");
          count3 = -40;
        }
        count3++;
        delay(250);
      }
    }
    if (i == 200)
    {
      PGA = (highAccelX + highAccelY + highAccelZ) / 3;
      PGV = (highVelocityX + highVelocityY + highVelocityZ) / 3;
      Serial.println(PGA, 4);
      Serial.println(PGV);

      // EarthQuakeUsingAccel(PGA);

      if ((PGA < 0.15) && (20 <= PGV) && (PGV <= 105))
      {
        Serial.print ("if 1");
        EarthQuakeUsingAccel(PGA);
      }
      if ((PGA < 0.15) && (PGV < 20))
      {
        Serial.println("if 2");
        EarthQuakeUsingAccel(PGA);
      }
      if ((PGA < 0.15) && (PGV > 105))
      {
        Serial.println("if 3");
        EarthQuakeUsingAccel(PGA);
      }
      if ((PGA >= 0.15) && (20 <= PGV) && (PGV <= 105))
      {
        Serial.println("if 4");
        EarthQuakeUsingVelocity (PGV);
      }
      if ((PGA >= 0.15) && (PGV < 20))
      {
        Serial.println("if 5");
        EarthQuakeUsingAccel(PGA);
      }
      if ((PGA >= 0.15) && (PGV > 105))
      {
        Serial.println("if 6");
        EarthQuakeUsingVelocity(PGV);
      }

      PGA = 0;
      PGV = 0;
      highAccelX = 0; highAccelY = 0; highAccelZ = 0;
      Abs_AcX_G = 0; Abs_AcY_G = 0; Abs_AcZ_G = 0;
      velocityXCm = 0; velocityYCm = 0; velocityZCm = 0;
      highVelocityX = 0; highVelocityY = 0; highVelocityZ = 0;
      Abs_velocityXCm = 0; Abs_velocityYCm = 0; Abs_velocityZCm = 0; 
      i = 0; count4++;
      /*
        Serial.print("Numerical accelerometer Readings in m/s^2 :  \t");
        Serial.print("AcX = "); Serial.print(AcX_Numerical);
        Serial.print(" | AcY = "); Serial.print(AcY_Numerical);
        Serial.print(" | AcZ = "); Serial.println(AcZ_Numerical);
      */
    }
     if((abs(mean_ax) > acel_deadzone) || (abs(mean_ay) > acel_deadzone) || (abs(16384 - mean_az) > acel_deadzone))
    {
      Serial.println("Offset Just get disturbed, So Setting Offset of MPU6050");
      SetOffsetOfMPU();
      OffsetDisturbed++;
    }
    delay(1000);
  }
}

  void EarthQuakeUsingAccel(double PGA)
  {
    double Magnitude = 0;
    if (PGA < 0.0017)
    {
      Magnitude = (PGA * 3.3) / .0016;
      Serial.print("This is an intensity 1 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("This Earthquake is not felt except by a few under especially favorable circumstances.");
    }
    if ((0.0017 <= PGA) && (PGA < 0.00785))
    {
      Magnitude = (PGA - 0.0017) * (3.9 - 3.4) / (0.00784 - 0.0017) + 3.4;
      Serial.print("This is an intensity 2 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("Felt only by a few persons at rest, especially on upper floors of buildings. Delicately suspended objects may swing");
    }
    if ((0.00785 <= PGA) && (PGA < 0.014))
    {
      Magnitude = (PGA - 0.00785) * (4.2 - 4.0) / (0.013 - 0.00785) + 4.0;
      Serial.print("This is an intensity 3 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("Felt quite noticeably indoors, especially on upper floors of buildings, but many people do not recognize it as an earthquake. Standing automobiles may rock slightly. Vibrations like passing of truck");
    }
    if ((0.014 <= PGA) && (PGA < 0.02))
    {
      Magnitude = (PGA - 0.014) * (4.8 - 4.3) / (0.01 - 0.014) + 4.3;
      Serial.print("This is an intensity 4 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println(" During the day felt indoors by many, outdoors by few. At night some awakened. Dishes, windows, doors disturbed,walls make creaking sound. Sensation like heavy truck striking building. Standing automobiles rocked noticeable.");
    }
    if ((0.02 <= PGA) && (PGA < 0.04))
    {
      Magnitude = (PGA - 0.02) * (5.4 - 4.9) / (0.03 - 0.02) + 4.9;
      Serial.print("This is an intensity 5 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("Felt by nearly everyone, many awakened. Some dishes, windows, and so on broken,cracked plaster in a few places,unstable objects overturned. Disturbances of trees, poles, and other tall objects sometimes noticed. Pendulum clocks may stop.");
    }
    if ((0.04 <= PGA) && (PGA < 0.07))
    {
      Magnitude = (PGA - 0.04) * (5.75 - 5.5) / (0.06 - 0.04) + 5.5;
      Serial.print("This is an intensity 6 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("Felt by all, many frightened and run outdoors. Some heavy furniture moved; a few instances of fallen plaster and damaged chimneys. Damage slight.");
    }
    if ((0.07 <= PGA) && (PGA < 0.15))
    {
      Magnitude = (PGA - 0.07) * (6.1 - 5.76) / (0.14 - 0.07) + 5.76;
      Serial.print("This is an intensity 7 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println(" Everybody runs outdoors. Damage negligible in buildings of good design and construction,slight to moderate in well-built ordinary structures, considerable in poorly built or badly designed structures, some chimneys broken. Noticed by persons driving cars");
    }
    if ((0.15 <= PGA) && (PGA < 0.3))
    {
      Magnitude = (PGA - 0.15) * (6.5 - 6.2) / (0.29 - 0.15) + 6.2;
      Serial.print("This is an intensity 8 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("Damage slight in specially designed structures, considerable in ordinary substantial buildings with partial collapse, great in poorly built structures. Panel walls thrown out of frame structures. Fall of chimneys, factory stacks, columns, monuments, walls. Heavy furniture overturned. Sand and mud ejected in small amounts. Changes in well water. Persons driving cars disturbed.");
    }
    if ((0.3 <= PGA) && (PGA < 0.61))
    {
      Magnitude = (PGA - 0.3) * (6.9 - 6.5) / (0.6 - 0.3) + 6.5;
      Serial.print("This is an intensity 9 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("Damage considerable in specially designed structures; well-designed frame structures thrown out of plumb; great in substantial buildings, with partial collapse. Buildings shifted of foundations. Ground cracked conspicuously. Underground pipes broken.");
    }
    if ((0.61 <= PGA) && (PGA < 0.7))
    {
      Magnitude = (PGA - 0.61) * (7.3 - 7.0) / (0.69 - 0.61) + 7.0;
      Serial.print("This is an intensity 10 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println(" Some well-built wooden structures destroyed, most masonry and frame structures destroyed with foundations, ground badly cracked. Rails bent. Landslides considerable from river banks and steep slopes. Shifted and mud. Water splashed, slopped over banks.");
    }
    if ((0.7 <= PGA) && (PGA < 0.91))
    {
      Magnitude = (PGA - 0.7) * (7.9 - 7.4) / (0.9 - 0.7) + 7.4;
      Serial.print("This is an intensity 11 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println("Few, if any,(masonry) structures remain standing. Bridges destroyed. Broad fissures in ground. Underground pipelines completely out of service. Earth slumps and land slips in soft ground. Rails bent greatly.");
    }
    if ((0.91 <= PGA) && (PGA < 1))
    {
      Magnitude = (PGA - 0.91) * (9.5 - 8.0) / (0.99 - 0.91) + 8.0;
      Serial.print("This is an intensity 12 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.println(" Damage total. Waves seen on ground surface. Lines of sight and level distorted. Objects thrown into the air.");
    }
    if (PGA >= 1)
    {
      Magnitude = 9.5;
      Serial.println(Magnitude);
      Serial.println("This earthquake is the biggest earthquake ever recorded");
    }
    Serial.println("Setting Offsets of MPU6050");
    SetOffsetOfMPU();
  }

  void EarthQuakeUsingVelocity (float PGV)
  {
    float Magnitude = 0;
    if ((20 <= PGV) && (PGV < 37.5))
    {
      Magnitude = (PGV - 20) * (6.5 - 6.2) / (37.4 - 20) + 6.2;
      Serial.print("This is an intensity 8 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.print("Damage slight in specially designed structures,considerable in ordinary substantial buildings with partial collapse, great in poorly built structures. Panel walls thrown out of frame structures. Fall of chimneys, factory stacks, columns, monuments, walls. Heavy furniture overturned. Sand and mud ejected in small amounts. Changes in well water. Persons driving cars disturbed.");
    }
    if ((37.5 <= PGV) && (PGV < 55))
    {
      Magnitude = (PGV - 37.5) * (6.9 - 6.6) / (54 - 37.5) + 6.6;
      Serial.print("This is an intensity 9 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.print("Damage considerable in specially designed structures, well-designed frame structures thrown out of plumb, great in substantial buildings, with partial collapse. Buildings shifted of foundations. Ground cracked conspicuously. Underground pipes broken");
    }
    if ((55 <= PGV) && (PGV < 81))
    {
      Magnitude = (PGV - 55) * (7.3 - 7.0) / (80 - 55) + 7.0;
      Serial.print("This is an intensity 10 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.print("Some well-built wooden structures destroyed, most masonry and frame structures destroyed with foundations, ground badly cracked. Rails bent. Landslides considerable from river banks and steep slopes. Shifted and mud. Water splashed, slopped over banks.");
    }
    if ((81 <= PGV) && (PGV < 95))
    {
      Magnitude = (PGV - 81) * (7.9 - 7.4) / (94 - 81) + 7.4;
      Serial.print("This is an intensity 11 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.print(" Few, if any(masonry) structures remain standing. Bridges destroyed. Broad fissures in ground. Underground pipelines completely out of service. Earth slumps and land slips in soft ground. Rails bent greatly.");
    }
    if ((95 <= PGV) && (PGV <= 105))
    {
      Magnitude = (PGV - 95) * (9.5 - 8.0) / (105 - 95) + 8.0;
      Serial.print("This is an intensity 12 Earthquake. Magnitude of the Earthquake is:");
      Serial.println(Magnitude);
      Serial.print("Damage total. Waves seen on ground surface. Lines of sight and level distorted. Objects thrown into the air.");
    }
    if (PGV > 105)
    {
      Magnitude = 9.5;
      Serial.print("This Earthquake has not happened in the history of mankind yet");
    }
    Serial.println("Setting Offsets of MPU6050");
    SetOffsetOfMPU();
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
       prev_ax_offset = ax_offset;
       prev_ay_offset = ay_offset;
       prev_az_offset = az_offset;
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
     /*Serial.println("ax\tay\taz");
     Serial.print(ax);
     Serial.print("\t");
     Serial.print(ay);
     Serial.print("\t");
     Serial.print(az);
     Serial.println("\t");
     */
       //delay(2);
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
