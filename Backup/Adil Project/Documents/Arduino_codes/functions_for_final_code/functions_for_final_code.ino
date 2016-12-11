void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
if (((AcX == 0) && (AcY == 0) && (filter_valueZ == 0))  || ((AcX == -1) && (AcY == -1) && (filter_valueZ == -1)))
count2++;

if (count2==30)
{
 digitalWrite(3, HIGH);
 digitalWrite(2, LOW); 
  }
}

void EarthQuakeUsingAccel(float PGA)
{
  float Magnitude = 0;
  String one, two, three;
  if (PGA < 0.0017)
  {
    Magnitude = (PGA * 3.3) / .0016;
    one = "This is an intensity 1 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nThis Earthquake is not felt except by a few under especially favorable circumstances.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.0017 <= PGA) && (PGA < 0.00785))
  {
    Magnitude = (PGA - 0.0017) * (3.9 - 3.4) / (0.00784 - 0.0017) + 3.4;
    one = "This is an intensity 2 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nFelt only by a few persons at rest, especially on upper floors of buildings. Delicately suspended objects may swing";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.00785 <= PGA) && (PGA < 0.014))
  {
    Magnitude = (PGA - 0.00785) * (4.2 - 4.0) / (0.013 - 0.00785) + 4.0;
    one = "This is an intensity 3 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nFelt quite noticeably indoors, especially on upper floors of buildings, but many people do not recognize it as an earthquake.\nStanding automobiles may rock slightly. Vibrations like passing of truck";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.014 <= PGA) && (PGA < 0.02))
  {
    Magnitude = (PGA - 0.014) * (4.8 - 4.3) / (0.01 - 0.014) + 4.3;
    one = "This is an intensity 4 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nDuring the day felt indoors by many, outdoors by few. At night some awakened.\nDishes, windows, doors disturbed,walls make creaking sound.\nSensation like heavy truck striking building. Standing automobiles rocked noticeable.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.02 <= PGA) && (PGA < 0.04))
  {
    Magnitude = (PGA - 0.02) * (5.4 - 4.9) / (0.03 - 0.02) + 4.9;
    one = "This is an intensity 5 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nFelt by nearly everyone, many awakened. Some dishes, windows, and so on broken,cracked plaster in a few places,unstable objects overturned.\nDisturbances of trees, poles, and other tall objects sometimes noticed. Pendulum clocks may stop.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.04 <= PGA) && (PGA < 0.07))
  {
    Magnitude = (PGA - 0.04) * (5.75 - 5.5) / (0.06 - 0.04) + 5.5;
    one = "This is an intensity 6 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nFelt by all, many frightened and run outdoors.\nSome heavy furniture moved; a few instances of fallen plaster and damaged chimneys. Damage slight.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.07 <= PGA) && (PGA < 0.15))
  {
    Magnitude = (PGA - 0.07) * (6.1 - 5.76) / (0.14 - 0.07) + 5.76;
    one = "This is an intensity 7 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nEverybody runs outdoors. Damage negligible in buildings of good design and construction,\nslight to moderate in well-built ordinary structures, considerable in poorly built or badly designed structures,\nsome chimneys broken. Noticed by persons driving cars";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.15 <= PGA) && (PGA < 0.3))
  {
    Magnitude = (PGA - 0.15) * (6.5 - 6.2) / (0.29 - 0.15) + 6.2;
    one = "This is an intensity 8 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nDamage slight in specially designed structures, considerable in ordinary substantial buildings with partial collapse,\ngreat in poorly built structures. Panel walls thrown out of frame structures. Fall of chimneys,\nfactory stacks, columns, monuments, walls. Heavy furniture overturned. Sand and mud ejected in small amounts.\nChanges in well water. Persons driving cars disturbed.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.3 <= PGA) && (PGA < 0.61))
  {
    Magnitude = (PGA - 0.3) * (6.9 - 6.5) / (0.6 - 0.3) + 6.5;
    one = "This is an intensity 9 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nDamage considerable in specially designed structures, well-designed frame structures thrown out of plumb.\nGreat in substantial buildings, with partial collapse. Buildings shifted of foundations.\nGround cracked conspicuously. Underground pipes broken.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.61 <= PGA) && (PGA < 0.7))
  {
    Magnitude = (PGA - 0.61) * (7.3 - 7.0) / (0.69 - 0.61) + 7.0;
    one = "This is an intensity 10 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nSome well-built wooden structures destroyed, most masonry and frame structures destroyed with foundations, ground badly cracked.\nRails bent. Landslides considerable from river banks and steep slopes.Water splashed, slopped over banks.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.7 <= PGA) && (PGA < 0.91))
  {
    Magnitude = (PGA - 0.7) * (7.9 - 7.4) / (0.9 - 0.7) + 7.4;
    one = "This is an intensity 11 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nFew, if any,(masonry) structures remain standing. Bridges destroyed. Broad fissures in ground.\nUnderground pipelines completely out of service. Earth slumps and land slips in soft ground. Rails bent greatly.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((0.91 <= PGA) && (PGA < 1))
  {
    Magnitude = (PGA - 0.91) * (9.5 - 8.0) / (0.99 - 0.91) + 8.0;
    one = "This is an intensity 12 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nDamage total. Waves seen on ground surface. Lines of sight and level distorted. Objects thrown into the air.";
    data = one + two + three;
    Serial.println(data);
  }
  else if (PGA >= 1)
  {
    Magnitude = 9.5;
    one = "Magnitude of the EarthQuake is:";
    two =  Magnitude;
    three = "\nThis earthquake is the biggest earthquake ever recorded";
    data = one + two + three;
    Serial.println(data);
  }
  else
  {
  }

  Serial.println("Setting Offsets of MPU6050");
  SetOffsetOfMPU();
}

void EarthQuakeUsingVelocity (float PGV)
{
   float Magnitude = 0;
  String one, two, three;

  if ((20 <= PGV) && (PGV < 37.5))
  {
    Magnitude = (PGV - 20) * (6.5 - 6.2) / (37.4 - 20) + 6.2;
    one = "This is an intensity 8 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nDamage slight in specially designed structures, considerable in ordinary substantial buildings with partial collapse,\ngreat in poorly built structures. Panel walls thrown out of frame structures. Fall of chimneys,\nfactory stacks, columns, monuments, walls. Heavy furniture overturned. Sand and mud ejected in small amounts.\nChanges in well water. Persons driving cars disturbed.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((37.5 <= PGV) && (PGV < 55))
  {
    Magnitude = (PGV - 37.5) * (6.9 - 6.6) / (54 - 37.5) + 6.6;
    one = "This is an intensity 9 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nDamage considerable in specially designed structures, well-designed frame structures thrown out of plumb.\nGreat in substantial buildings, with partial collapse. Buildings shifted of foundations.\nGround cracked conspicuously. Underground pipes broken.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((55 <= PGV) && (PGV < 81))
  {
    Magnitude = (PGV - 55) * (7.3 - 7.0) / (80 - 55) + 7.0;
    one = "This is an intensity 10 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nSome well-built wooden structures destroyed, most masonry and frame structures destroyed with foundations, ground badly cracked.\nRails bent. Landslides considerable from river banks and steep slopes.Water splashed, slopped over banks.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((81 <= PGV) && (PGV < 95))
  {
    Magnitude = (PGV - 81) * (7.9 - 7.4) / (94 - 81) + 7.4;
    one = "This is an intensity 11 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nFew, if any,(masonry) structures remain standing. Bridges destroyed. Broad fissures in ground.\nUnderground pipelines completely out of service. Earth slumps and land slips in soft ground. Rails bent greatly.";
    data = one + two + three;
    Serial.println(data);
  }
  else if ((95 <= PGV) && (PGV <= 105))
  {
    Magnitude = (PGV - 95) * (9.5 - 8.0) / (105 - 95) + 8.0;
    one = "This is an intensity 12 Earthquake. Magnitude of the Earthquake is:";
    two = Magnitude;
    three = "\nDamage total. Waves seen on ground surface. Lines of sight and level distorted. Objects thrown into the air.";
    data = one + two + three;
    Serial.println(data);
  }
  else if (PGV > 105)
  {
    Magnitude = 9.5;
    one = "Magnitude of the EarthQuake is:";
    two =  Magnitude;
    three = "\nThis earthquake is the biggest earthquake ever recorded";
    data = one + two + three;
    Serial.println(data);
  }
  else
  {
  }

  delay(5);
  Serial.println("Setting Offsets of MPU6050");
  SetOffsetOfMPU();
}

void SetOffsetOfMPU ()
{
  accelgyro.setXAccelOffset(0);
  accelgyro.setYAccelOffset(0);
  accelgyro.setZAccelOffset(0);

  meansensors();                  // Reading Sensors for the first time
  calibration();                  // Calculating Offsets
  meansensors();

  accelgyro.setXAccelOffset(ax_offset);    
  accelgyro.setYAccelOffset(ay_offset);    
  accelgyro.setZAccelOffset(az_offset);
}

void EarthQuakeCalculate ()
{
  for (i = 0; i < 200; i++)
      {
        Wire.beginTransmission(MPU_addr);
        Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
        Wire.endTransmission(false);
        Wire.requestFrom(MPU_addr, 4, true); // request a total of 14 registers
        AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
        AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
        
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
        
        AcX_G = (fXg) / 16384; //Acceleration of X-axis in g, ranging from -2G to 2G
        AcY_G = (fYg) / 16384; //Acceleration of Y-axis in g, ranging from -2G to 2G
        AcZ_G = ((filter_valueZ) / 16384) - 1; //Acceleration of Z-axis in g, ranging from -2G to 2G

        Abs_AcX_G = abs(AcX_G);
        Abs_AcY_G = abs(AcY_G);
        Abs_AcZ_G = abs(AcZ_G);
        
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
      if (i == 200)
    {
      PGA = (highAccelX + highAccelY + highAccelZ) / 3;
      PGV = (highVelocityX + highVelocityY + highVelocityZ) / 3;
      Serial.println(PGA, 4);
      Serial.println(PGV);

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
      if ((PGA >= 0.15) && (20 <= PGV) && (PGV <= 80))
      {
        Serial.println("if 4");
        EarthQuakeUsingVelocity (PGV);
      }
      if ((PGA >= 0.15) && (PGV < 20))
      {
        Serial.println("if 5");
        EarthQuakeUsingAccel(PGA);
      }
      if ((PGA >= 0.15) && (PGV > 80))
      {
        Serial.println("if 6");
        EarthQuakeUsingAccel(PGA);
      }

      PGA = 0;
      PGV = 0;
      highAccelX = 0; highAccelY = 0; highAccelZ = 0;
      Abs_AcX_G = 0; Abs_AcY_G = 0; Abs_AcZ_G = 0;
      velocityXCm = 0; velocityYCm = 0; velocityZCm = 0;
      highVelocityX = 0; highVelocityY = 0; highVelocityZ = 0;
      Abs_velocityXCm = 0; Abs_velocityYCm = 0; Abs_velocityZCm = 0; 
      i = 0; count4++;
    }
  }
