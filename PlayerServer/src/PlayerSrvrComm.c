#include <PlayerServer.h>

char *strmID;

int clntSock;

void HandleTCPClient(int clntSockAddr) {
	clntSock = clntSockAddr;
	time_t ticks;
	ticks = time(NULL);

	int i;

	receive();
	SIGLive();
	receive();

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
