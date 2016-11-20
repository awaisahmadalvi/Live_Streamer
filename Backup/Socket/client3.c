#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// the port client will be connecting to
#define PORT 3490
// max number of bytes we can get at once
#define MAXDATASIZE 300
int srvrSock, numbytes;

int connect(char *srvrIp) {
	char buf[MAXDATASIZE];
	struct hostent *he;
	// connector.s address information
	struct sockaddr_in their_addr;

	// get the host info
	if ((he = gethostbyname(srvrIp)) == NULL) {
		perror("gethostbyname()");
		exit(1);
	} else
		printf("Client-The remote host is: %s\n", srvrIp);

	if ((srvrSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket()");
		exit(1);
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
		exit(1);
	} else
		printf("Client-The connect() is OK...\n");

	receiveData(buf);
	printf("Client-Received: %s", buf);

	snprintf(buf, MAXDATASIZE, "Hello How r u.\r\n");
	buf[MAXDATASIZE] = '\0';
	sendData(buf);

	printf("Client-Closing srvrSock\n");
	close(srvrSock);
	return 0;
}

void receiveData(char buf[MAXDATASIZE]) {

	if ((numbytes = recv(srvrSock, buf, MAXDATASIZE - 1, 0)) == -1) {
		perror("recv()");
		exit(1);
	} else
		printf("Client-recv() is OK...\n");

	buf[numbytes] = '\0';
}

void sendData(char buf[MAXDATASIZE]) {

	if (send(srvrSock, buf, MAXDATASIZE - 1, 0) == -1) {
		perror("send()");
		exit(1);
	} else
		printf("Client-send() is OK...\n");
}

int main(){
	connect('0.0.0.0');
}

