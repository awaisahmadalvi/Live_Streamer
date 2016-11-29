#include <Server.h>

char *strmID;
int clntSock, numbytes; /* Socket descriptor for client connection */
int ACK = FALSE;

void *ThreadMain(void *threadArgs) {
	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */
	clntSock = ((struct ThreadArgs *) threadArgs)->clntSockfd; /* Extract socket file descriptor from argument */
	free(threadArgs); /* Deallocate memory for argument */
	clientComm();
	return (NULL);
}

void SIGLive() {
	memset(&tempBufS[0], 0, MAXDATASIZE);
	snprintf(tempBufS, MAXDATASIZE,
			"{ \"streamId\" : \"%s\",\"action\" : \"live\"}", strmID);
	tempBufS[MAXDATASIZE] = '\0';
	sendData(tempBufS);
}

void clientComm() {
	ACK = FALSE;

	receive();
	SIGLive();
	receive();
	//SIGAck();

	/*while (!ACK) {
	 receive();
	 }*/
	printf("Server-clntSock is closing\n");
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
	ACK = TRUE;
	//setJsonValue("action", "done", strmID);
}

void receive() {
	char *str = receiveData();
	printf("Server-Received: %s\n", str);
	msgParse(str);
}

void msgParse(char tempStr[MAXDATASIZE]) {
	/*json_object * jobj = json_tokener_parse(tempStr);
	strmID = getJsonValueFromObj("streamId", jobj);
	setJsonValue("streamId", strmID, strmID);
	setJsonValue("status", getJsonValueFromObj("status", jobj), strmID);
	getJsonValueFromFile("status", strmID);
	if (strcmp(getJsonValueFromFile("action", strmID), "done") == 0) {
		SIGAck();
	} else if ((strcmp(getJsonValueFromObj("status", jobj), "ready") == 0)
			&& (strcmp(getJsonValueFromFile("action", strmID), "off") == 0)) {
		SIGAck();
	} else if (strcmp(getJsonValueFromObj("status", jobj),
			getJsonValueFromFile("action", strmID)) != 0)
		SIGAct();
	else
		SIGAck();
		*/
}
void sendData(char tempBufS[MAXDATASIZE]) {
	printf("Server-send(): %s\n", tempBufS);

	if (send(clntSock, tempBufS, MAXDATASIZE - 1, 0) == -1)
		perror("Server-send() error lol!");
	else
		printf("Server-send() is OK...\n");
}

char * receiveData() {
	memset(&tempBufS[0], 0, MAXDATASIZE);
	if ((numbytes = recv(clntSock, tempBufS, MAXDATASIZE - 1, 0)) == -1) {
		perror("recv()");
		exit(1);
	} else
		printf("Server-recv() is OK...\n");

	tempBufS[numbytes] = '\0';
	return tempBufS;
}
