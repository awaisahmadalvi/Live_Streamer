/*
 * clientComm.c
 *
 *  Created on: Oct 17, 2016
 *      Author: root
 */
#include <client.h>
#include <stdlib.h>

void clientCommunication() {

	SIGCheckUpdate();
	receive();
}

char * getStreamId() {
	return getJsonValueFromFile("streamId");
}
char * getVersion() {
	return getJsonValueFromFile("version");
}

void SIGCheckUpdate() {

	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"version\" : \"%s\",\"action\" : \"update\"}",
			getStreamId(), getVersion());

	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void msgParse(char tempStr[MAXDATASIZE]) {

	json_object * jobj = json_tokener_parse(tempStr);
	if (strcmp(getJsonValueFromObj(jobj,"status"), "available") == 0) {

		char *addr ;
		sprintf(tempBuff,"/root/OTA/./updateOTA.sh %s",getJsonValueFromObj(jobj,"addr"));
		system(tempBuff);
	}

}

void receive() {

	char *str = receiveData();
	printf("Client-Received: %s\n", str);
	msgParse(str);
}

