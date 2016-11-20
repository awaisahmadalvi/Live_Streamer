/*
 * main.h
 *
 *  Created on: Oct 14, 2016
 *      Author: root
 */

#ifndef SEVER_H_
#define SEVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> /* for POSIX threads */

#include <signal.h>
#include <time.h>

void *ThreadOTA(void *threadArgs);
void *ThreadStreamer(void *threadArgs);
void *ThreadPlayer(void *threadArgs);

#endif /* SEVER_H_ */
