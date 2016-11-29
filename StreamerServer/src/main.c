#include <Server.h>

static int trig = 0;
static int flag = 1;
pthread_t threadStrm, threadPlyr; /* Thread ID from pthread_create()*/

void trigger(int sig) {
	if (sig == SIGALRM) {
		system("rm /root/Streamer/status/*");
		printf("All status json files deleted\n");
		trig = 0;
	}
}

static void INT_MAINHANDLER(int signo) {
	pthread_kill(threadStrm, SIGKILL);
	pthread_kill(threadPlyr, SIGKILL);
	destroy_flag = 1;
	flag = 0;
}

int main() {

	//* register the signal SIGINT handler */
	struct sigaction act;
	act.sa_handler = INT_MAINHANDLER;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, 0);

	pthread_create(&threadStrm, NULL, startStrmSrvr, NULL);
	pthread_create(&threadPlyr, NULL, startPlyrSrvr, NULL);
	//startPlayerServer();

	while (flag) {
		trig = 1;
		signal(SIGALRM, trigger);
		alarm(15);
		while (trig)
			;
	}
}
