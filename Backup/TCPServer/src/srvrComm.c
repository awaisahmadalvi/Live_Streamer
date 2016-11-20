#include <server.h>

char *strmID;

void HandleTCPClient(int clntSockAddr) {
	clntSock = clntSockAddr;
	time_t ticks;
	ticks = time(NULL);

	int i;
	for (i = 0; i < 1; i++) {

		receive();
		SIGLive();
		receive();

		receive();
		SIGLocal();
		receive();

		receive();
		SIGOff();
		receive();

	}

	printf("Server-clntSock is closing");
	close(clntSock);
}

void receive() {
	char *str = receiveData();
	printf("Server-Received: %s\n", str);
	msgParse(str);
}

void SIGOff() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"action\" : \"off\"}", strmID);
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void SIGLive() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"action\" : \"live\"}", strmID);
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void SIGLocal() {
	snprintf(tempBuff, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"action\" : \"local\"}", strmID);
	tempBuff[MAXDATASIZE] = '\0';
	sendData(tempBuff);
}

void msgParse(char tempStr[MAXDATASIZE]) {
	json_object * jobj = json_tokener_parse(tempStr);
	strmID = getJsonValueFromObj(jobj, "streamId");

	JsonToFile(jobj);
}
