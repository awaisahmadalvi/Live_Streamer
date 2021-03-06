//#include <HashMap.h>
#include "I2Cdev.h"
#include "MPU6050.h"
//#include <SPI.h>
#include <stdio.h>
#include <stdint.h>
//#include <Ethernet.h>
//#include<Wire.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// MPU-6050 Short Example Sketch
// By Raspberry User Muhammad Adil
// June 13, 2016
// Private Domain

void SetOffsetOfMPU();
void EarthQuakeCalculate ();
void EarthQuakeUsingAccel(float);
void EarthQuakeUsingVelocity (float);
void meansensors();
void calibration();

using namespace std;
int buffersize = 1000;   //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone = 8;   //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int giro_deadzone = 1;   //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)


const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX = 0, AcY = 0, AcZ = 0, Tmp = 0;
double fXg = 0, fYg = 0, filter_valueZ = 0;
int32_t fZg = 0;
MPU6050 accelgyro;
const float alpha = 0.09, beta = 0.4;
int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, state = 0, count = 0, i = 0, count3 = 0;
int ax_offset, ay_offset, az_offset, OffsetDisturbed = 0;
double AcX_G = 0, AcY_G = 0, AcZ_G = 0, Abs_AcX_G = 0, Abs_AcY_G = 0 , Abs_AcZ_G = 0;
double velocityX = 0, velocityY = 0, velocityZ = 0, velocityXCm = 0 , velocityYCm = 0, velocityZCm = 0, Abs_velocityXCm = 0 , Abs_velocityYCm = 0 , Abs_velocityZCm = 0;
double  Accel_Round2X, Accel_Round2Y, Accel_Round2Z;
double highAccelX = 0, highAccelY = 0, highAccelZ = 0, highVelocityX = 0, highVelocityY = 0, highVelocityZ = 0;
double prevAccelX = 0, prevAccelY = 0, prevAccelZ = 0;
double VelocityX_LPF = 0, VelocityY_LPF = 0, VelocityZ_LPF = 0;
struct timeval currTimeX, prevTimeX, prevTimeY, prevTimeZ;
int count1 = 0, count2 = 0, count5 = 0, count6 = 0;
float  Tmp_C = 0, PGA = 0, PGV = 0;
int System_Perfect = 0, System_Mal = 0, Temp_Alarm = 0, Earthquake_Alarm = 0;
//byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
char c;
//boolean currentLineIsBlank;
std:: string data = "null";
//IPAddress ip(10 ,50 ,49 ,15);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
//EthernetServer server(80);

//define the max size of the hashtable
//const byte HASH_SIZE = 8;
//storage
//HashType<char*, char*> hashRawArray[HASH_SIZE];
//handles the storage [search,retrieve,insert]
/*HashMap<char*, char*> hashMap = HashMap<char*, char*>(hashRawArray , HASH_SIZE );
String str;  char cstr[16];
String str1; char cstr1[16];
String str2; char cstr2[16];
String str3; char cstr3[2];
String str4; char cstr4[2];
String str5; char cstr5[2];
String str6; char cstr6[2];
char cstr7[500];
*/
void setup() {

 //   Wire.begin();
 //   Ethernet.begin(mac, ip);
   // server.begin();
   // printf("server is at ");
  //printf(Ethernet.localIP());

  //TWBR = 12; // 400kHz I2C clock, Where TWBR Stands for Two Wire Bit Rate Register on page 33 of datasheet.
  //Serial.begin(9600);
  accelgyro.initialize();   //initialize device

  //while (Serial.available() && Serial.read()); // empty buffer
  printf(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  printf("\nSetting Offset of MPU6050\n");
  SetOffsetOfMPU();

  //Wire.beginTransmission(MPU_addr);
  //Wire.write(0x6B);  // PWR_MGMT_1 register
  //Wire.write(0);     // set to zero (wakes up the MPU-6050)
  //Wire.endTransmission(true);

  //accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);   // Setting Full Scale Range from -2G to +2G
}

void loop() {

    if (count1 == 0)
    {
      count1 = -5;
      printf("Monitoring EarthQuake\n");
    }
    count1++;
    //accelgyro.beginTransmission(MPU_addr);
    //accelgyro.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    //Wire.endTransmission(false);
    //Wire.requestFrom(MPU_addr, 4, true); // request a total of 4 registers
    AcX = accelgyro.getAccelerationX(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = accelgyro.getAccelerationY(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)

    for (int k = 0; k < 15; k++) // Applying moving average filter
    {
      //Wire.beginTransmission(MPU_addr);
      //Wire.write(0x3F);
      //Wire.endTransmission(false);
      //Wire.requestFrom(MPU_addr, 2, true);
      AcZ = accelgyro.getAccelerationZ(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      fZg = fZg + AcZ;
      count++;
      AcZ = 0;
    }
    filter_valueZ = fZg / count;
    count = 0;
    fZg = 0;

    if (((AcX == 0) && (AcY == 0) && (filter_valueZ == 0))  || ((AcX == -1) && (AcY == -1) && (filter_valueZ == -1)))
      count2++;

    if (count2 >= 30)                    // System Malfunctioning Alarm
    {
      System_Perfect = 0;
      System_Mal = 1;
    }

    //Wire.beginTransmission(MPU_addr);
    //Wire.write(0x41);
    //Wire.endTransmission(false);
    //Wire.requestFrom(MPU_addr, 2, true);
    Tmp = accelgyro.getTemperature(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    Tmp_C = Tmp / 340.00 + 36.53;
    printf("Temperature is:" );
    printf("%.2f\n", Tmp_C);

    fXg = AcX * alpha + (fXg * (1.0 - alpha));
    fYg = AcY * alpha + (fYg * (1.0 - alpha));

    AcX_G = (fXg) / 16384; //Acceleration of X-axis in g, ranging from -2G to 2G
    AcY_G = (fYg) / 16384; //Acceleration of Y-axis in g, ranging from -2G to 2G
    AcZ_G = ((filter_valueZ) / 16384) - 1; //Acceleration of Z-axis in g, ranging from -2G to 2G

    Abs_AcX_G = fabs(AcX_G);
    Abs_AcY_G = fabs(AcY_G);
    Abs_AcZ_G = fabs(AcZ_G);

    if ((((Abs_AcX_G > 0.003) && (Abs_AcY_G > 0.003)) || ((Abs_AcY_G > 0.003) && (Abs_AcZ_G > 0.003)) || ((Abs_AcZ_G > 0.003) && (Abs_AcX_G > 0.003))) && (count5 == 10))
    { 
      printf("Calculating EarthQuake");
      count5 = 0;
      Earthquake_Alarm = 1;
      EarthQuakeCalculate();
    }
    printf("%s\n", data.c_str());
    //std::cout<<""<< data;
    if (count6 == 1)
    {
      count5 = 10;
      count6 = 0;
    }
    
    if (count5 < 10)                // EarthQuake printing
    {
      count5++;
      printf("Count5 is: %d\n", count5);
      if (count5 == 9)
      {
        count6++;
        data = "null";
      }
    }
    
    Earthquake_Alarm = 0;

    if ((abs(mean_ax) > acel_deadzone) || (abs(mean_ay) > acel_deadzone) || (abs(16384 - mean_az) > acel_deadzone))
    {
      printf("Offset Just get disturbed, So Setting Offset of MPU6050\n");
      SetOffsetOfMPU();
      OffsetDisturbed++;
    }

    if (Tmp_C > 35)          //Temperature Alarm
    {
      //  client.println("<br />Temperature has exceeded its specified limit");
      Temp_Alarm = 1;
    }
    else
      Temp_Alarm = 0;

    if (count2 < 30) {                  // System Running Perfectly
      //client.println("<br />System is Running Perfectly");
      System_Perfect = 1;
      System_Mal = 0;
    }

  /*  str = String(AcX);
    str.toCharArray(cstr, 16);
    str1 = String(AcY);
    str1.toCharArray(cstr1, 16);
    str2 = String(filter_valueZ);
    str2.toCharArray(cstr2, 16);
    str3 = String(System_Perfect);
    str3.toCharArray(cstr3, 2);
    str4 = String(System_Mal);
    str4.toCharArray(cstr4, 2);
    str5 = String(Temp_Alarm);
    str5.toCharArray(cstr5, 2);
    str6 = String(Earthquake_Alarm);
    str6.toCharArray(cstr6, 2);
    data.toCharArray(cstr7, 500);

    hashMap[0]("AcX", cstr);
    hashMap[1]("AcY", cstr1);
    hashMap[2]("AcZ", cstr2);
    hashMap[3]("System Running Perfectly", cstr3);
    hashMap[4]("System Malfunctioning Alarm", cstr4);
    hashMap[5]("Temperature Alarm", cstr5);
    hashMap[6]("EarthQuake Alarm", cstr6);
    hashMap[7]("EarthQuake Detail", cstr7);
    // listen for incoming clients
    EthernetClient client = server.available();
    Serial.println(client);
    if (client) {
      Serial.println("new client");
      // an http request ends with a blank line
      currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 1");  // refresh the page automatically every 1 sec
            client.println();

            for (byte i = 0; i < HASH_SIZE; i++) {
              client.print(hashMap[i].getHash());
              client.print("=");
              client.print(hashMap[i].getValue());
              client.print("=");
              client.print(";");
            }



            client.println("</html>");
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(2);
      //      close the connection:
      client.stop();
    } */
    //sleep(1);
}

void EarthQuakeCalculate ()
{
  for (i = 0; i < 240; i++)
  {
    //Wire.beginTransmission(MPU_addr);
    //Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    //Wire.endTransmission(false);
    //Wire.requestFrom(MPU_addr, 4, true); // request a total of 14 registers
    AcX = accelgyro.getAccelerationX(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = accelgyro.getAccelerationY(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)

    for (int k = 0; k < 15; k++) // Applying moving average filter
    {
      //Wire.beginTransmission(MPU_addr);
      //Wire.write(0x3F);
      //Wire.endTransmission(false);
      //Wire.requestFrom(MPU_addr, 2, true);
      AcZ = accelgyro.getAccelerationZ(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
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

    Abs_AcX_G = fabs(AcX_G);
    Abs_AcY_G = fabs(AcY_G);
    Abs_AcZ_G = fabs(AcZ_G);

    Accel_Round2X = ((int)(AcX_G * 100 + .5) / 100.0); //Round off the acceleration to 2 decimal places
    Accel_Round2Y = ((int)(AcY_G * 100 + .5) / 100.0);
    Accel_Round2Z = ((int)(AcZ_G * 100 + .5) / 100.0);

    gettimeofday(&currTimeX, NULL);
    velocityX  = (Accel_Round2X + prevAccelX) / 2 * ((double)(currTimeX.tv_usec - prevTimeX.tv_usec) / 1000000 + (double)(currTimeX.tv_sec - prevTimeX.tv_sec)); //1000000 added to get the same units
    gettimeofday(&prevTimeX, NULL);
    VelocityX_LPF = velocityX * beta + (VelocityX_LPF * (1.0 - beta));
    prevAccelX = Accel_Round2X;
   
    
   // gettimeofday(&currTimeY, NULL); 
    velocityY  = (Accel_Round2Y + prevAccelY) / 2 * ((double)(currTimeX.tv_usec - prevTimeY.tv_usec) / 1000000 + (double)(currTimeX.tv_sec - prevTimeY.tv_sec)); //1000000 added to get the same units
    gettimeofday(&prevTimeY, NULL);
    VelocityY_LPF = velocityY * beta + (VelocityY_LPF * (1.0 - beta));
    prevAccelY = Accel_Round2Y;
    
    
   // gettimeofday(&currTimeZ, NULL);
    velocityZ  = (Accel_Round2Z + prevAccelZ) / 2 * ((double)(currTimeX.tv_usec - prevTimeZ.tv_usec) / 1000000 + (double)(currTimeX.tv_sec - prevTimeZ.tv_sec)); //1000000 added to get the same units
    gettimeofday(&prevTimeZ, NULL);
    VelocityZ_LPF = velocityZ * beta + (VelocityZ_LPF * (1.0 - beta));
    prevAccelZ = Accel_Round2Z;

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
      printf("Calculating Peak Ground Accelerations and Peak Ground Velocities in X, Y and Z direction\n");
      count3 = -40;
    }
    count3++;
    usleep(250000);

 /* str = String(AcX);
    str.toCharArray(cstr, 16);
    str1 = String(AcY);
    str1.toCharArray(cstr1, 16);
    str2 = String(filter_valueZ);
    str2.toCharArray(cstr2, 16);
    str3 = String(System_Perfect);
    str3.toCharArray(cstr3, 2);
    str4 = String(System_Mal);
    str4.toCharArray(cstr4, 2);
    str5 = String(Temp_Alarm);
    str5.toCharArray(cstr5, 2);
    str6 = String(Earthquake_Alarm);
    str6.toCharArray(cstr6, 2);
    data.toCharArray(cstr7, 500);

    hashMap[0]("AcX", cstr);
    hashMap[1]("AcY", cstr1);
    hashMap[2]("AcZ", cstr2);
    hashMap[3]("System Running Perfectly", cstr3);
    hashMap[4]("System Malfunctioning Alarm", cstr4);
    hashMap[5]("Temperature Alarm", cstr5);
    hashMap[6]("EarthQuake Alarm", cstr6);
    hashMap[7]("EarthQuake_Detail", cstr7);

    EthernetClient client = server.available();
    Serial.println(client);
    if (client) {
      Serial.println("new client");
      // an http request ends with a blank line
      currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          c = client.read();
          Serial.write(c);
          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 1");  // refresh the page automatically every 1 sec
            client.println();

            if (count2 < 30) {                  // System Running Perfectly
              System_Perfect = 1;
              System_Mal = 0;
            }

            for (byte i = 0; i < HASH_SIZE; i++) {
              client.print(hashMap[i].getHash());
              client.print("=");
              client.print(hashMap[i].getValue());
              client.print("=");
              client.print(";");
            }

            client.println("</html>");
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(2);
      //      close the connection:
      client.stop();
    } */
  }

  if (i == 240)
  {
    PGA = (highAccelX + highAccelY + highAccelZ) / 3;
    PGV = (highVelocityX + highVelocityY + highVelocityZ) / 3;
    printf("%.4f\n" ,PGA);
    printf("%.2f\n", PGV);

    if ((PGA < 0.15) && (20 <= PGV) && (PGV <= 105))
    {
      printf ("if 1\n");
      EarthQuakeUsingAccel(PGA);
    }
    if ((PGA < 0.15) && (PGV < 20))
    {
      printf("if 2\n");
      EarthQuakeUsingAccel(PGA);
    }
    if ((PGA < 0.15) && (PGV > 105))
    {
      printf("if 3\n");
      EarthQuakeUsingAccel(PGA);
    }
    if ((PGA >= 0.15) && (20 <= PGV) && (PGV <= 80))
    {
      printf("if 4\n");
      EarthQuakeUsingVelocity (PGV);
    }
    if ((PGA >= 0.15) && (PGV < 20))
    {
      printf("if 5\n");
      EarthQuakeUsingAccel(PGA);
    }
    if ((PGA >= 0.15) && (PGV > 80))
    {
      printf("if 6\n");
      EarthQuakeUsingAccel(PGA);
    }

    PGA = 0;
    PGV = 0;
    highAccelX = 0; highAccelY = 0; highAccelZ = 0;
    Abs_AcX_G = 0; Abs_AcY_G = 0; Abs_AcZ_G = 0;
    velocityXCm = 0; velocityYCm = 0; velocityZCm = 0;
    highVelocityX = 0; highVelocityY = 0; highVelocityZ = 0;
    Abs_velocityXCm = 0; Abs_velocityYCm = 0; Abs_velocityZCm = 0;
    i = 0;
  }

}

inline std::string stringify(float x)
{
	std::ostringstream o;
	o<<x;
	return o.str(); 
	}

void EarthQuakeUsingAccel(float PGA)
{
  float Magnitude = 0;
  std::string one, two, three, four;
  if (PGA < 0.0017)
  {
    Magnitude = (PGA * 3.3) / .0016;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 1 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nThis Earthquake is not felt except by a few under especially favorable circumstances.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.0017 <= PGA) && (PGA < 0.00785))
  {
    Magnitude = (PGA - 0.0017) * (3.9 - 3.4) / (0.00784 - 0.0017) + 3.4;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 2 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nFelt only by a few persons at rest, especially on upper floors of buildings. Delicately suspended objects may swing.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.00785 <= PGA) && (PGA < 0.014))
  {
    Magnitude = (PGA - 0.00785) * (4.2 - 4.0) / (0.013 - 0.00785) + 4.0;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 3 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nFelt quite noticeably indoors, especially on upper floors of buildings, but many people do not recognize it as an earthquake.\nStanding automobiles may rock slightly. Vibrations like passing of truck.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.014 <= PGA) && (PGA < 0.02))
  {
    Magnitude = (PGA - 0.014) * (4.8 - 4.3) / (0.01 - 0.014) + 4.3;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 4 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nDuring the day felt indoors by many, outdoors by few. At night some awakened.\nDishes, windows, doors disturbed,walls make creaking sound.\nSensation like heavy truck striking building. Standing automobiles rocked noticeable.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.02 <= PGA) && (PGA < 0.04))
  {
    Magnitude = (PGA - 0.02) * (5.4 - 4.9) / (0.03 - 0.02) + 4.9;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 5 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nFelt by nearly everyone, many awakened. Some dishes, windows, and so on broken,cracked plaster in a few places,unstable objects overturned.\nDisturbances of trees, poles, and other tall objects sometimes noticed. Pendulum clocks may stop.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.04 <= PGA) && (PGA < 0.07))
  {
    Magnitude = (PGA - 0.04) * (5.75 - 5.5) / (0.06 - 0.04) + 5.5;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 6 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nFelt by all, many frightened and run outdoors.\nSome heavy furniture moved a few instances of fallen plaster and damaged chimneys. Damage slight.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.07 <= PGA) && (PGA < 0.15))
  {
    Magnitude = (PGA - 0.07) * (6.1 - 5.76) / (0.14 - 0.07) + 5.76;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 7 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nEverybody runs outdoors. Damage negligible in buildings of good design and construction,\nslight to moderate in well-built ordinary structures, considerable in poorly built or badly designed structures,\nsome chimneys broken. Noticed by persons driving cars.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.15 <= PGA) && (PGA < 0.3))
  {
    Magnitude = (PGA - 0.15) * (6.5 - 6.2) / (0.29 - 0.15) + 6.2;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 8 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nDamage slight in specially designed structures, considerable in ordinary substantial buildings with partial collapse,\ngreat in poorly built structures. Panel walls thrown out of frame structures. Fall of chimneys,\nfactory stacks, columns, monuments, walls. Heavy furniture overturned. Sand and mud ejected in small amounts.\nChanges in well water. Persons driving cars disturbed.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.3 <= PGA) && (PGA < 0.61))
  {
    Magnitude = (PGA - 0.3) * (6.9 - 6.5) / (0.6 - 0.3) + 6.5;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 9 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nDamage considerable in specially designed structures, well-designed frame structures thrown out of plumb.\nGreat in substantial buildings, with partial collapse. Buildings shifted of foundations.\nGround cracked conspicuously. Underground pipes broken.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.61 <= PGA) && (PGA < 0.7))
  {
    Magnitude = (PGA - 0.61) * (7.3 - 7.0) / (0.69 - 0.61) + 7.0;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 10 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nSome well-built wooden structures destroyed, most masonry and frame structures destroyed with foundations, ground badly cracked.\nRails bent. Landslides considerable from river banks and steep slopes.Water splashed, slopped over banks.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.7 <= PGA) && (PGA < 0.91))
  {
    Magnitude = (PGA - 0.7) * (7.9 - 7.4) / (0.9 - 0.7) + 7.4;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 11 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nFew, if any,(masonry) structures remain standing. Bridges destroyed. Broad fissures in ground.\nUnderground pipelines completely out of service. Earth slumps and land slips in soft ground. Rails bent greatly.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if ((0.91 <= PGA) && (PGA < 1))
  {
    Magnitude = (PGA - 0.91) * (9.5 - 8.0) / (0.99 - 0.91) + 8.0;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 12 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nDamage total. Waves seen on ground surface. Lines of sight and level distorted. Objects thrown into the air.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else if (PGA >= 1)
  {
    Magnitude = 9.5;
    one = "Magnitude of the EarthQuake is:";
    two = stringify(Magnitude);
    three = "\nThis earthquake is the biggest earthquake ever recorded.";
    four = "\nThe decision is based on PGA.";
    data = one + two + three + four;
  }
  else
  {
  }

  printf("\nSetting Offsets of MPU6050");
  SetOffsetOfMPU();
}

void EarthQuakeUsingVelocity (float PGV)
{
   float Magnitude = 0;
   std::string one, two, three, four;

  if ((20 <= PGV) && (PGV < 37.5))
  {
    Magnitude = (PGV - 20) * (6.5 - 6.2) / (37.4 - 20) + 6.2;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 8 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nDamage slight in specially designed structures, considerable in ordinary substantial buildings with partial collapse,\ngreat in poorly built structures. Panel walls thrown out of frame structures. Fall of chimneys,\nfactory stacks, columns, monuments, walls. Heavy furniture overturned. Sand and mud ejected in small amounts.\nChanges in well water. Persons driving cars disturbed.";
    four = "\nThe decision is based on PGV.";
    data = one + two + three + four;
  }
  else if ((37.5 <= PGV) && (PGV < 55))
  {
    Magnitude = (PGV - 37.5) * (6.9 - 6.6) / (54 - 37.5) + 6.6;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 9 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nDamage considerable in specially designed structures, well-designed frame structures thrown out of plumb.\nGreat in substantial buildings, with partial collapse. Buildings shifted of foundations.\nGround cracked conspicuously. Underground pipes broken.";
    four = "\nThe decision is based on PGV.";
    data = one + two + three + four;
  }
  else if ((55 <= PGV) && (PGV < 81))
  {
    Magnitude = (PGV - 55) * (7.3 - 7.0) / (80 - 55) + 7.0;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 10 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nSome well-built wooden structures destroyed, most masonry and frame structures destroyed with foundations, ground badly cracked.\nRails bent. Landslides considerable from river banks and steep slopes.Water splashed, slopped over banks.";
    four = "\nThe decision is based on PGV.";
    data = one + two + three + four;
  }
  else if ((81 <= PGV) && (PGV < 95))
  {
    Magnitude = (PGV - 81) * (7.9 - 7.4) / (94 - 81) + 7.4;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 11 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nFew, if any,(masonry) structures remain standing. Bridges destroyed. Broad fissures in ground.\nUnderground pipelines completely out of service. Earth slumps and land slips in soft ground. Rails bent greatly.";
    four = "\nThe decision is based on PGV.";
    data = one + two + three + four;
  }
  else if ((95 <= PGV) && (PGV <= 105))
  {
    Magnitude = (PGV - 95) * (9.5 - 8.0) / (105 - 95) + 8.0;
    Magnitude = ceilf(Magnitude * 10) /10;   //round off to one decimal place
    one = "This is an intensity 12 Earthquake. Magnitude of the Earthquake is:";
    two = stringify(Magnitude);
    three = "\nDamage total. Waves seen on ground surface. Lines of sight and level distorted. Objects thrown into the air.";
    four = "\nThe decision is based on PGV.";
    data = one + two + three + four;
  }
  else if (PGV > 105)
  {
    Magnitude = 9.5;
    one = "Magnitude of the EarthQuake is:";
    two = stringify(Magnitude);
    three = "\nThis earthquake is the biggest earthquake ever recorded.";
    four = "\nThe decision is based on PGV.";
    data = one + two + three + four; 
  }
  else
  {
  }
  
  printf("\nSetting Offsets of MPU6050");
  SetOffsetOfMPU();
}

void SetOffsetOfMPU()
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
    usleep(200); //Needed so we don't get repeated measures //sleep for 2000 micro-seconds or 2 milli seconds
  }
}

void calibration() {
  int64_t count5 = 0;
  ax_offset = -mean_ax / 8;
  ay_offset = -mean_ay / 8;
  az_offset = (16384 - mean_az) / 8;

  while (1) {
    int data_ready = 0;
    accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);

    meansensors();
    printf("...\n");
    count5++;
   /* if (count5 > 30)                 // System Malfunctioning Alarm
    {
      System_Mal = 1;
      System_Perfect = 0;

      str = String(AcX);
      str.toCharArray(cstr, 16);
      str1 = String(AcY);
      str1.toCharArray(cstr1, 16);
      str2 = String(filter_valueZ);
      str2.toCharArray(cstr2, 16);
      str3 = String(System_Perfect);
      str3.toCharArray(cstr3, 2);
      str4 = String(System_Mal);
      str4.toCharArray(cstr4, 2);
      str5 = String(Temp_Alarm);
      str5.toCharArray(cstr5, 2);
      str6 = String(Earthquake_Alarm);
      str6.toCharArray(cstr6, 2);
      data.toCharArray(cstr7, 500);

      hashMap[0]("AcX", cstr);
      hashMap[1]("AcY", cstr1);
      hashMap[2]("AcZ", cstr2);
      hashMap[3]("System Running Perfectly", cstr3);
      hashMap[4]("System Malfunctioning Alarm", cstr4);
      hashMap[5]("Temperature Alarm", cstr5);
      hashMap[6]("EarthQuake Alarm", cstr6);
      hashMap[7]("EarthQuake_Detail", cstr7);


      EthernetClient client = server.available();
      Serial.println(client);
      if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        currentLineIsBlank = true;
        while (client.connected()) {
          if (client.available()) {
            c = client.read();
            Serial.write(c);
            // if you've gotten to the end of the line (received a newline
            // character) and the line is blank, the http request has ended,
            // so you can send a reply
            if (c == '\n' && currentLineIsBlank) {
              // send a standard http response header
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");  // the connection will be closed after completion of the response
              client.println("Refresh: 1");  // refresh the page automatically every 1 sec
              client.println();

              for (byte i = 0; i < HASH_SIZE; i++) {
                client.print(hashMap[i].getHash());
                client.print("=");
                client.print(hashMap[i].getValue());
                client.print("=");
                client.print(";");
              }
              client.println("</html>");
              break;
            }
            if (c == '\n') {
              // you're starting a new line
              currentLineIsBlank = true;
            } else if (c != '\r') {
              // you've gotten a character on the current line
              currentLineIsBlank = false;
            }
          }
        }
        // give the web browser time to receive the data
        delay(2);
        //      close the connection:
        client.stop();
      } */

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

    if (abs(16384-mean_az) <= acel_deadzone)
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

int main()
{
    setup();
    for (;;)
        loop();
}
