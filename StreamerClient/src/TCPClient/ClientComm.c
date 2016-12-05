/*
 * clientComm.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <Client.h>
#define ACK 1

/* "status":"live" */
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
			getJsonValueFromFile("streamId"), status);

	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void msgParse(char tempStr[MAXDATASIZE]) {
	json_object * jobj = json_tokener_parse(tempStr);
	if (strcmp(getJsonValueFromFile("streamId"),
			getJsonValueFromObj("streamId", jobj)) == 0) {
		char *actRecv = getJsonValueFromObj("action", jobj);
		if (strcmp(actRecv, "live") == 0) {
			/* start live stream */
			if (strcmp(status, "local") == 0) {
				startLive();
			} else if (strcmp(status, "ready") == 0) {
				setAll();
				initLive();
				startStreaming();
			}
			printf("Device is Live now.\n");
			snprintf(status, MAXDATASIZE/4,"live");
		} else if (strcmp(actRecv, "local") == 0) {
			if (strcmp(status, "live") == 0)
				stopLive();
			else if (strcmp(status, "ready") == 0) {
				setAll();
				startStreaming();
			}
			printf("Device is Local now.\n");
			snprintf(status, MAXDATASIZE/4,"local");
		} else if (strcmp(actRecv, "off") == 0) {
			/* stop all stream */
			printf("Device is Off/Ready now.\n");
			stopStreaming();
			snprintf(status, MAXDATASIZE/4,"ready");
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
