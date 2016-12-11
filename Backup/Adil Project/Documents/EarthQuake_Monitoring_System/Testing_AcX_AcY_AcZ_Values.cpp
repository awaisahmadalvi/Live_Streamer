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
#include <inttypes.h>

// MPU-6050 Short Example Sketch
// By Arduino User Muhammad Adil
// June 13, 2016
// Private Domain

void SetOffsetOfMPU();
void meansensors();
void calibration();

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
struct timeval currTimeX, currTimeY, currTimeZ, prevTimeX, prevTimeY, prevTimeZ;
int count1 = 0, count2 = 0, count5 = 0, count6 = 0;
float  Tmp_C = 0, PGA = 0, PGV = 0;
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
int System_Perfect = 0, System_Mal = 0, Temp_Alarm = 0, Earthquake_Alarm = 0;
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

  printf("Setting Offset of MPU6050");
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
      printf("Monitoring EarthQuake");
    }
    count1++;
    //accelgyro.beginTransmission(MPU_addr);
    //accelgyro.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    //Wire.endTransmission(false);
    //Wire.requestFrom(MPU_addr, 4, true); // request a total of 4 registers
    AcX = accelgyro.getAccelerationX(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY = accelgyro.getAccelerationY(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    Tmp = accelgyro.getTemperature();
    Tmp_C = Tmp / 340.00 + 36.53;
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
    printf("Value of AcX: % "  PRId16 "\n", AcX);
    printf("\nTemperature of the room is:   %f ", Tmp_C);
    sleep(1);
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
    usleep(2000); //Needed so we don't get repeated measures //sleep for 2000 micro-seconds or 2 milli seconds
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
