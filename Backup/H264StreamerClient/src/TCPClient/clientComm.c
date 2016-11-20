/*
 * clientComm.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <client.h>

void clientCommunication() {

	SIGReady();
	receive();
}

char * getStreamId() {
	return getJsonValueFromFile("streamId");
}

void SIGReady() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"ready\"}", getStreamId());

	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void SIGOff() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"off\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void SIGLive() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"live\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void SIGLocal() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"local\"}", getStreamId());
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void msgParse(char tempStr[MAXDATASIZE]) {

	json_object * jobj = json_tokener_parse(tempStr);
	if (strcmp(getJsonValueFromFile("streamId"),
			getJsonValueFromObj(jobj, "streamId")) == 0) {
		char *act = getJsonValueFromObj(jobj, "action");
		setJsonValue("action", act);
		if (strcmp(act, "live") == 0) {
			/* start live stream */
			gstreamer_main();
			printf("Device is Live now.\n");
			SIGLive();
		}
		if (strcmp(act, "local") == 0) {
			/* start local stream */
			printf("Device is Local now.\n");
			SIGLocal();
		}
		if (strcmp(act, "off") == 0) {
			/* stop all stream */
			printf("Device is Off now.\n");
			SIGOff();
		}
	}
}

void receive() {

	char *str = receiveData();
	printf("Client-Received: %s\n", str);
	msgParse(str);
}

