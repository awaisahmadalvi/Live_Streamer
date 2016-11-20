#include <pthread.h> /* for POSIX threads */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
/* the port users will be connecting to */
#define MYPORT 3490
/* how many pending connections queue will hold */
#define BACKLOG 10
// max number of bytes we can get at once
#define MAXDATASIZE 300

void HandleTCPClient(int clntSock);

struct ThreadArgs { /* Structure of arguments to pass to client thread */
	int clntSock; /* socket descriptor for client */
};

void *ThreadMain(void *threadArgs) {
	int clntSock; /* Socket descriptor for client connection */
	pthread_detach (pthread_self()); /* Guarantees that thread resources are deallocated upon return */
clntSock	= ((struct ThreadArgs *) threadArgs)->clntSock; /* Extract socket file descriptor from argument */
	free(threadArgs); /* Deallocate memory for argument */
	HandleTCPClient(clntSock);
	return (NULL);
}

int startSrvr() {
	/* listen on sock_fd, new connection on clntSock */
	int sockfd, clntSock;
	char buf[300];
	/* my address information */
	struct sockaddr_in my_addr;
	/* connector.s address information */
	struct sockaddr_in their_addr;
	int sin_size;
	struct sigaction sa;
	int yes = 1;

	unsigned short echoServPort; /* Server port */
	pthread_t threadID; /* Thread ID from pthread_create()*/
	struct ThreadArgs *threadArgs; /* Pointer to argument structure for thread */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Server-socket() error...\n!");
		exit(1);
	} else
		printf("Server-socket() sockfd is OK...\n");

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("Server-setsockopt() error...\n!");
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
		perror("Server-bind() error...\n");
		exit(1);
	} else
		printf("Server-bind() is OK...\n");

	if (listen(sockfd, BACKLOG) == -1) {
		perror("Server-listen() error...\n");
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
	return 0;
}


int main(){
	startSrvr();
}

