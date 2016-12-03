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

#define KGRN "\033[0;32;32m"
#define KCYN "\033[0;36m"
#define KRED "\033[0;32;31m"
#define KYEL "\033[1;33m"
#define KMAG "\033[0;35m"
#define KBLU "\033[0;32;34m"
#define KCYN_L "\033[1;36m"
#define RESET "\033[0m"

/* the port streamer will be connecting to */
#define STRMPORT 4445
/* the port player will be connecting to */
#define PLYRPORT 4446

/* how many pending connections queue will hold */
#define BACKLOG 10
// max number of bytes we can get at once
#define MAXDATASIZE 255

static int destroy_flag = 0;

void clientComm();
void sendData(char tempBuff[MAXDATASIZE]);
void startStrmSrvr();

char * receiveData();
void * ThreadMain(void *threadArgs);
char * getJsonValueFromObj(char * jKey, json_object * tempJson);
char * getJsonValueFromFile(char * jKey, char *file);
void setJsonValue(char * jKey, char *value, char *file);
void JsonToFile(json_object * jobj, char *file);
json_object * JsonFromFile(char *file);

void receive();
void SIGAck();
void SIGAct();
void msgParse(char tempStr[MAXDATASIZE]);

char tempBufS[MAXDATASIZE];
char tempBufP[MAXDATASIZE];

struct ThreadArgs { /* Structure of arguments to pass to client thread */
	int clntSockfd; /* socket descriptor for client */
};

/*
 * Player Server
 */

int startPlyrSrvr();

#endif /* SEVER_H_ */
