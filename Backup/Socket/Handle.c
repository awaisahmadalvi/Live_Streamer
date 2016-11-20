#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

// max number of bytes we can get at once
#define MAXDATASIZE 300

int clntSock;

void HandleTCPClient(int clntSockAddr) {
	clntSock = clntSockAddr;
	time_t ticks;
	char buf[MAXDATASIZE];
	ticks = time(NULL);

	snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
	sendData(buf);

	recvData(buf);
	printf("Server-Received: %s", buf);
	close(clntSock);
}

int sendData(char buf[MAXDATASIZE]) {
	if (send(clntSock, buf, MAXDATASIZE - 1, 0) == -1)
		perror("Server-send() error lol!");
	else
		printf("Server-send() is OK...\n");
}

int recvData(char buf[MAXDATASIZE]) {
	int numbytes;
	if ((numbytes = recv(clntSock, buf, MAXDATASIZE - 1, 0)) == -1) {
		perror("recv()");
		exit(1);
	} else
		printf("Server-recv() is OK...\n");

	buf[numbytes] = '\0';

}