#include <PlayerServer.h>

int clntSock;

void *ThreadMain(void *threadArgs) {
	int clntSock; /* Socket descriptor for client connection */
	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */
	clntSock = ((struct ThreadArgs *) threadArgs)->clntSock; /* Extract socket file descriptor from argument */
	free(threadArgs); /* Deallocate memory for argument */
	HandleTCPClient(clntSock);
	return (NULL);
}

void startPlayerSrvr() {
	/* listen on sock_fd, new connection on clntSock */
	int sockfd, clntSock;
	/* my address information */
	struct sockaddr_in my_addr;
	/* connector.s address information */
	struct sockaddr_in their_addr;
	int sin_size;
	int yes = 1;

	unsigned short echoServPort; /* Server port */
	pthread_t threadID; /* Thread ID from pthread_create()*/
	struct ThreadArgs *threadArgs; /* Pointer to argument structure for thread */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Server-socket() error...!");
		exit(1);
	} else
		printf("Server-socket() sockfd is OK...\n");

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("Server-setsockopt() error...!");
		exit(1);
	} else
		printf("Server-setsockopt is OK...\n");

	/* host byte order */
	my_addr.sin_family = AF_INET;
	/* short, network byte order */
	my_addr.sin_port = htons(MYPORT);
	/* automatically fill with my IP */
	my_addr.sin_addr.s_addr = INADDR_ANY;

	printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr),
			MYPORT);

	/* zero the rest of the struct */
	memset(&(my_addr.sin_zero), '\0', 8);

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		perror("Server-bind() error...");
		exit(1);
	} else
		printf("Server-bind() is OK...\n");

	if (listen(sockfd, BACKLOG) == -1) {
		perror("Server-listen() error...");
		exit(1);
	}
	printf("Server-listen() is OK...Listening...\n");

	/* accept() loop */
	while (1) {
		sin_size = sizeof(struct sockaddr_in);
		if ((clntSock = accept(sockfd, (struct sockaddr *) &their_addr,
				&sin_size)) == -1) {
			perror("Server-accept() error");
			continue;
		} else
			printf("Server-accept() is OK...\n");
		printf("Server-new socket, clntSock is OK...\n");
		printf("Server: Got connection from %s\n",
				inet_ntoa(their_addr.sin_addr));

		if ((threadArgs = (struct ThreadArgs *) malloc(
				sizeof(struct ThreadArgs))) == NULL)
			exit(1);
		threadArgs->clntSock = clntSock;
		/* Create client thread */
		if (pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs)
				!= 0)
			exit(1);
	}
}

void sendData(char tempBuff[MAXDATASIZE]) {
	printf("Server-send(): %s\n", tempBuff);

	if (send(clntSock, tempBuff, MAXDATASIZE - 1, 0) == -1)
		perror("Server-send() error lol!");
	else
		printf("Server-send() is OK...\n");
}

char * receiveData() {
	int numbytes;
	memset(&tempBuff[0], 0, MAXDATASIZE);
	if ((numbytes = recv(clntSock, tempBuff, MAXDATASIZE - 1, 0)) == -1) {
		perror("recv()");
		exit(1);
	} else
		printf("Server-recv() is OK...\n");

	tempBuff[numbytes] = '\0';
	return tempBuff;
}

