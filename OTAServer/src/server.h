/*
 * sever.h
 *
 *  Created on: Oct 14, 2016
 *      Author: root
 */

#ifndef SEVER_H_
#define SEVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> /* for POSIX threads */
#include <unistd.h>
#include <errno.h>
#include <json/json.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
/* the port users will be connecting to */
#define MYPORT 4444
/* how many pending connections queue will hold */
#define BACKLOG 10
// max number of bytes we can get at once
#define MAXDATASIZE 255

void HandleTCPClient(int clntSock);
void HandleTCPClient(int clntSockAddr);
void sendData(char tempBuff[MAXDATASIZE]);
void startSrvr();

char * receiveData();
void * ThreadMain(void *threadArgs);
char * getJsonValueFromObj(json_object * tempJson, char * jKey);
char * getJsonValueFromFile(char * jKey);

void setJsonValue(char * jKey, char *value);
void JsonToFile(json_object * jobj);
json_object * JsonFromFile();


void receive();
void msgParse(char tempStr[MAXDATASIZE]);

int clntSock, numbytes;
char tempBuff[MAXDATASIZE];

struct ThreadArgs { /* Structure of arguments to pass to client thread */
	int clntSock; /* socket descriptor for client */
};

#endif /* SEVER_H_ */
