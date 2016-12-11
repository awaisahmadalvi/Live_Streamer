#include <HashMap.h>
#include <Wire.h>

//define the max size of the hashtable
const byte HASH_SIZE = 8;
//storage
HashType<char*,char*> hashRawArray[HASH_SIZE];
//handles the storage [search,retrieve,insert]
HashMap<char*,char*> hashMap = HashMap<char*,char*>(hashRawArray , HASH_SIZE );

int AcX =18, AcY =200, filter_valueZ =1234, D = 123, E = 20;
String str;  char cstr[16];
String str1; char cstr1[16];
String str2; char cstr2[16];
String str3; char cstr3[1];
String str4; char cstr4[1];
String str5; char cstr5[1];
String str6; char cstr6[1];
String str7; char cstr7[500];
int System_Perfect, System_Mal, Temp_Alarm, Earthquake_Alarm;

void setup(){
  //setup hashmap
 
 
  Serial.begin(9600);
  //Serial.println( hashMap.getIndexOf("test"),DEC );
  //Serial.println( hashMap.getValueOf("test") );
 

}
 
void loop()
{
  str = String(AcX);
  str.toCharArray(cstr,16);
  str1 = String(AcY);
  str1.toCharArray(cstr1,16);
  str2 = String(filter_valueZ);
  str2.toCharArray(cstr2,16);
  str3 = String(System_Perfect);
  str3.toCharArray(cstr3,1);
  str4 = String(System_Mal);
  str4.toCharArray(cstr4,1);
  str5 = String(Temp_Alarm);
  str5.toCharArray(cstr5,1);
  str6 = String(Earthquake_Alarm);
  str6.toCharArray(cstr6,1);
  data.toCharArray(cstr7,500);
  
  hashMap[0]("AcX",cstr);
  hashMap[1]("AcY",cstr1);
  hashMap[2]("AcZ",cstr2);
  hashMap[3]("System Running Perfectly",cstr3);
  hashMap[4]("System Malfunctioning Alarm",cstr4);
  hashMap[5]("Temperature Alarm",cstr5);
  hashMap[6]("EarthQuake Alarm",cstr6);
  hashMap[7]("EarthQuake_Detail",cstr7);

  D++; E++;

    hashMap.debug();
    delay (500);
  }
