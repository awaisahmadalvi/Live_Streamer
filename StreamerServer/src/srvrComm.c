#include <Server.h>

char *strmID;

void HandleTCPClient(int clntSockAddr) {
	clntSock = clntSockAddr;

	receive();
	receive();

	printf("Server-clntSock is closing");
	close(clntSock);
}

void SIGAct() {
	memset(&tempBufS[0], 0, MAXDATASIZE);
	snprintf(tempBufS, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"action\" : \"%s\"}", strmID,
			getJsonValueFromFile("action", strmID));
	tempBufS[MAXDATASIZE] = '\0';
	sendData(tempBufS);
}

void SIGAck() {
	memset(&tempBufS[0], 0, MAXDATASIZE);
	snprintf(tempBufS, MAXDATASIZE, "ACK");
	tempBufS[MAXDATASIZE] = '\0';
	sendData(tempBufS);
}

void msgParse(char tempStr[MAXDATASIZE]) {
	json_object * jobj = json_tokener_parse(tempStr);
	strmID = getJsonValueFromObj("streamId", jobj);
	setJsonValue("streamId", strmID, strmID);
	setJsonValue("status", getJsonValueFromObj("status", jobj), strmID);
	if (strcmp(getJsonValueFromObj("status", jobj),
			getJsonValueFromFile("action", strmID)) != 0)
		SIGAct();
	/*if (strcmp(getJsonValueFromFile("action", strmID)) != 0)
	 SIGAct();
	 */
	else
		SIGAck();
}

void receive() {
	char *str = receiveData();
	printf("Server-Received: %s\n", str);
	msgParse(str);
}
