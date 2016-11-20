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


json_object * JsonFromFile();
void JsonToFile(json_object * jobj);
void setJsonValue(char * jKey, char *value);
char * getJsonValueFromFile(char * jKey);
char * getJsonValueFromObj(json_object * tempJson, char * jKey);

#endif /* SEVER_H_ */
