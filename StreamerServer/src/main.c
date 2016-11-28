#include <Server.h>

int main() {
	pthread_t threadStrm, threadPlyr; /* Thread ID from pthread_create()*/

	pthread_create(&threadStrm, NULL, startStrmSrvr, NULL);

	//pthread_create(&threadPlyr, NULL, startPlayerServer, NULL);
	startPlayerServer();
}
