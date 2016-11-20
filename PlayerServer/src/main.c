/*
 * main.c
 *
 *  Created on: Nov 2, 2016
 *      Author: root
 */

#include <main.h>
pthread_t threadPlayerID, threadStreamerID, threadOTAID; /* Thread ID from pthread_create()*/

int main() {
	/* Create OTA client thread */
	pthread_create(&threadPlayerID, NULL, ThreadOTA, NULL);
	/* Create Streamer client thread */
	pthread_create(&threadStreamerID, NULL, ThreadStreamer, NULL);
	/* Create Player client thread */
	pthread_create(&threadOTAID, NULL, ThreadPlayer, NULL);
}

void *ThreadOTA(void *threadArgs) {
	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */
	startOTASrvr();
	return (NULL);
}

void *ThreadStreamer(void *threadArgs) {
	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */
	startPlayerSrvr();
	return (NULL);
}

void *ThreadPlayer(void *threadArgs) {
	pthread_detach(pthread_self()); /* Guarantees that thread resources are deallocated upon return */
	startStreamerSrvr();
	return (NULL);
}
