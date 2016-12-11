#include <Server.h>
#include <dirent.h>

static int trig = 0;
static int flag = 1;
pthread_t threadStrm, threadPlyr; /* Thread ID from pthread_create()*/

void trigger(int sig) {
	if (sig == SIGALRM) {
		//system("rm status/*");
		struct dirent *de; // Pointer for directory entry

		// opendir() returns a pointer of DIR type.
		DIR *dr = opendir("status/");

		if (dr == NULL) // opendir returns NULL if couldn't open directory
		{
			printf("Could not open current directory\n");
			return;
		}

		while ((de = readdir(dr)) != NULL) {
			if (de->d_type==DT_DIR) {
				printf("ignored %s\n", de->d_name);
				continue;
			}
			printf("%s\n", de->d_name);
			char *fn = (char *) malloc(sizeof(char) * 64);
			snprintf(fn, 64, "status/%s", de->d_name);
			json_object *tempObj = json_object_from_file(fn);
			json_object *newEntry = json_object_new_string("off");
			json_object_object_add(tempObj, "status", newEntry);
			json_object_to_file(fn, tempObj);
		}

		closedir(dr);

		printf("All status JSON files deleted\n");
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
	trigger(SIGALRM);
	pthread_create(&threadStrm, NULL, startStrmSrvr, NULL);
	pthread_create(&threadPlyr, NULL, startPlyrSrvr, NULL);
	//startPlayerServer();

	while (flag) {
		trig = 1;
		signal(SIGALRM, trigger);
		alarm(30);
		while (trig)
			sleep(10);
	}
}
