#include <client.h>

int connect2Client(char *srvrIp) {
	struct hostent *he;
	// connector.s address information
	struct sockaddr_in their_addr;
	// get the host info
	if ((he = gethostbyname(srvrIp)) == NULL) {
		perror("gethostbyname()");
		return -1;
	} else
		printf("Client-The remote host is: %s\n", srvrIp);

	if ((srvrSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket()");
		return -1;
	} else
		printf("Client-The socket() srvrSock is OK...\n");

	// host byte order
	their_addr.sin_family = AF_INET;
	// short, network byte order
	printf("Server-Using %s and port %d...\n", srvrIp, PORT);
	their_addr.sin_port = htons(PORT);
	their_addr.sin_addr = *((struct in_addr *) he->h_addr);
	// zero the rest of the struct
	memset(&(their_addr.sin_zero), '\0', 8);

	if (connect(srvrSock, (struct sockaddr *) &their_addr,
			sizeof(struct sockaddr)) == -1) {
		perror("connect()");
		return -1;
	} else
		printf("Client-The connect() is OK...\n");
	return 0;
}

void sendData(char tempBuff[MAXDATASIZE]) {

	printf("Client-send(): %s\n", tempBuff);
	if (send(srvrSock, tempBuff, MAXDATASIZE - 1, 0) == -1) {
		perror("send()");
		exit(1);
	} else
		printf("Client-send() is OK...\n");
	memset(&tempBuff[0], 0, MAXDATASIZE);
}

char * receiveData() {

	memset(&tempBuff[0], 0, MAXDATASIZE);
	if ((numbytes = recv(srvrSock, tempBuff, MAXDATASIZE - 1, 0)) == -1) {
		perror("recv()");
		exit(1);
	} else
		printf("Client-recv() is OK...\n");
	tempBuff[numbytes] = '\0';
	return tempBuff;
}


int main() {
	char *val;
	val = getJsonValueFromFile("serverIp");
	if (connect2Client(val) != 0)
		exit(1);

	clientCommunication();

	printf("Client-Closing srvrSock\n");
	close(srvrSock);
}
