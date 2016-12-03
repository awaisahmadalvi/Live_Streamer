/*
 * clientComm.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <client.h>
#define ACK 1

/* "action":"live","status":"live" */
void clientComm() {

	SIGStatus();
	while (receive() != ACK) {
		SIGStatus();
	}
}

void SIGStatus() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"status\" : \"%s\"}",
			getJsonValueFromFile("streamId"), getJsonValueFromFile("status"));

	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
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
				initLive();
				startStreaming();
			}
			printf("Device is Live now.\n");
			setJsonValue("status", "live");
		} else if (strcmp(actRecv, "local") == 0) {
			if (strcmp(getJsonValueFromFile("status"), "live") == 0)
				stopLive();
			else if (strcmp(getJsonValueFromFile("status"), "ready") == 0) {
				setAll();
				startStreaming();
			}
			printf("Device is Local now.\n");
			setJsonValue("status", "local");
		} else if (strcmp(actRecv, "off") == 0) {
			/* stop all stream */
			printf("Device is Off/Ready now.\n");
			stopStreaming();
			setJsonValue("status", "gready");
		}
	}
}

int receive() {
	printf("Client in Receiving mode...\n");

	char *str = receiveData();
	printf("Client-Received: %s\n", str);
	if (strcmp(str, "ACK") == 0) {
		return ACK;
	}
	msgParse(str);
	return 0;
}
