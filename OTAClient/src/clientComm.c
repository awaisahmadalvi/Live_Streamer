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
	if (strcmp(getJsonValueFromObj(jobj, "status"), "available") == 0) {

		char *package = getJsonValueFromObj(jobj, "package");
		printf("%s\n",package);
		sprintf(tempBuff, "#!/bin/bash\n"
				"mkdir /root/update\n"
				"cd /root/update\n"
				"wget %s\n"
				"tar -xJf %s\n"
				"rm -rf %s\n"
				"sh update.sh\n"
				"exit\n", getJsonValueFromObj(jobj, "addr"),package,package);
		printf("CMD: %s\n",tempBuff);
		system(tempBuff);
		setJsonValue("version",getJsonValueFromObj(jobj, "version"));
		printf("System is Updated successfully to \"%s\"...\n",getJsonValueFromObj(jobj, "version"));
	}
}

void receive() {

	char *str = receiveData();
	printf("Client-Received: %s\n", str);
	msgParse(str);
}

