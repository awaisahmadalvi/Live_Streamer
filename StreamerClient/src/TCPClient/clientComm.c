/*
 * clientComm.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <client.h>

void clientCommunication() {

	SIGReady();
	while (strcmp(getJsonValueFromFile("status"), "live") != 0)
		receive();
	while(TRUE);
}

char * getStreamId() {
	return getJsonValueFromFile("streamId");
}

void SIGReady() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"ready\"}", getStreamId());

	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "ready");
}

void SIGOff() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"off\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "off");
}

void SIGLive() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"live\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "live");
}

void SIGLocal() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"local\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "local");
}

void msgParse(char tempStr[MAXDATASIZE]) {
	json_object * jobj = json_tokener_parse(tempStr);
	if (strcmp(getJsonValueFromFile("streamId"),
			getJsonValueFromObj(jobj, "streamId")) == 0) {
		char *act = getJsonValueFromObj(jobj, "action");
		setJsonValue("action", act);
		if (strcmp(act, "live") == 0) {
			/* start live stream */
			if (strcmp(getJsonValueFromFile("status"), "local") == 0) {
				startLive();
				SIGLive();
			} else if (strcmp(getJsonValueFromFile("status"), "ready") == 0) {
				setAll();
				startStreaming();
				printf("Device is Live now.\n");
				SIGLive();
			}
		} else if (strcmp(act, "local") == 0) {
			if (strcmp(getJsonValueFromFile("status"), "live") == 0)
				stopLive();
			if ((strcmp(getJsonValueFromFile("status"), "off") == 0)
					|| (strcmp(getJsonValueFromFile("status"), "ready") == 0)) {
				setAll();
				stopLive();
				startStreaming();
			}
			printf("Device is Local now.\n");
			SIGLocal();
		} else if (strcmp(act, "off") == 0) {
			/* stop all stream */
			printf("Device is Off now.\n");
			stopStreaming();
			SIGOff();
		}
	}
}

void receive() {
	printf("Client-Receiving ******************** \n");

	char *str = receiveData();
	printf("Client-Received: %s\n", str);
	msgParse(str);
}

