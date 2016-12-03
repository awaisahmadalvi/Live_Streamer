/*
 * client.h
 *
 *  Created on: Oct 14, 2016
 *      Author: root
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <json/json.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// the port client will be connecting to
#define PORT 4445
// max number of bytes we can get at once
#define MAXDATASIZE 255
int srvrSock, numbytes;

char tempBuff[MAXDATASIZE];

int setAll();
int stopStreaming();
void startStreaming();
void stopLive();
void startLive();
int initLive();

void clientComm();

int connect2Client(char *srvrIp);
char * receiveData();
void sendData(char buf[MAXDATASIZE]);

char * getJsonValueFromObj(char * jKey, json_object * tempJson);
char * getJsonValueFromFile(char * jKey);
void setJsonValue(char * jKey, char *value);
void JsonToFile(json_object * jobj);
json_object * JsonFromFile();

int receive();
void SIGStatus();
void msgParse(char tempStr[MAXDATASIZE]);

void intHandler(int dummy);

#endif /* CLIENT_H_ */
