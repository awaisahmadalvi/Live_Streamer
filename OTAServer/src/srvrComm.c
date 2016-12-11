#include <server.h>

char *strmID;

void HandleTCPClient(int clntSockAddr) {
	clntSock = clntSockAddr;

	receive();

	printf("Server-clntSock is closing\n");
	close(clntSock);
}

void receive() {
	char *str = receiveData();
	printf("Server-Received: %s\n", str);
	msgParse(str);
}

void msgParse(char tempStr[MAXDATASIZE]) {
	char *act;
	json_object * jobj = json_tokener_parse(tempStr);

	act = getJsonValueFromObj(jobj, "action");
	if (strcmp(act, "update") == 0) {
		char *verTrgt, *verLatest;
		verTrgt = getJsonValueFromObj(jobj, "version");
		verLatest = getJsonValueFromFile("version");

		printf("Target: %s, Server: %s\n", verTrgt, verLatest);
		if (strcmp(verTrgt, verLatest) == 0) {
			SIGUpdated();
		} else {
			SIGAvailable();
		}
	}
}

void SIGUpdated() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE, "{ \"status\" : \"updated\" }\n");
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void SIGAvailable() {
	memset(&tempBuff[0], 0, MAXDATASIZE);
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"status\" : \"available\",\"addr\" : \"http://103.226.217.136/UpdatePackage.tar.xz\""
			",\"package\" : \"UpdatePackage.tar.xz\",\"version\" : %s }\n",getJsonValueFromFile("version"));
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

