/*
 * clientComm.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <client.h>
#define ACK 1

void clientComm() {

	SIGStatus();
	while (receive() != ACK) {
	}
}

char * getStreamId() {
	return getJsonValueFromFile("streamId");
}

void SIGStatus() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"%s\"}", getStreamId(),
			getJsonValueFromFile("status"));

	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void SIGReady() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"ready\"}", getStreamId());

	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "ready");
}

void SIGOff() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"off\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "off");
}

void SIGLive() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"live\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "live");
}

void SIGLocal() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"local\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
	setJsonValue("status", "local");
}

void msgParse(char tempStr[MAXDATASIZE]) {
	json_object * jobj = json_tokener_parse(tempStr);
	if (strcmp(getJsonValueFromFile("streamId"),
			getJsonValueFromObj("streamId", jobj)) == 0) {
		char *actRecv = getJsonValueFromObj("action", jobj);
		setJsonValue("action", actRecv);
		if (strcmp(actRecv, "live") == 0) {
			/* start live stream */
			if (strcmp(getJsonValueFromFile("status"), "local") == 0) {
				startLive();
			} else if (strcmp(getJsonValueFromFile("status"), "ready") == 0) {
				setAll();
				startStreaming();
			}
			printf("Device is Live now.\n");
			SIGLive();
		} else if (strcmp(actRecv, "local") == 0) {
			if (strcmp(getJsonValueFromFile("status"), "live") == 0)
				stopLive();
			else if ((strcmp(getJsonValueFromFile("status"), "off") == 0)
					|| (strcmp(getJsonValueFromFile("status"), "ready") == 0)) {
				setAll();
				stopLive();
				startStreaming();
			}
			printf("Device is Local now.\n");
			SIGLocal();
		} else if (strcmp(actRecv, "off") == 0) {
			/* stop all stream */
			printf("Device is Off now.\n");
			stopStreaming();
			SIGOff();
		}
	}
}

int receive() {
	printf("Client-Receiving ******************** \n");

	char *str = receiveData();
	printf("Client-Received: %s\n", str);
	if (strcmp(str, "ACK") == 0)
		return ACK;
	msgParse(str);
	return 0;
}
