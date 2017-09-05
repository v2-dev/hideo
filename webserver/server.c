#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "thread.h"
#include "cacher.h"
#include <signal.h>
#include "utils.h"
#include "parse_conf_file.h"
#include "resolutionDevice.h"
#include "libhttp.h"
#include <sys/resource.h>
#include <sys/time.h>
#include "logger.h"

int listensd;
char *request;

pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pool_cond = PTHREAD_COND_INITIALIZER;
struct list list_sock = { 0, NULL, NULL };

void pr_cpu_time(void)
{
	double user, sys;
	struct rusage myusage, childusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0) {
    		toLog(ERR, srvlog, "error in getrusage\n");
		exit(EXIT_FAILURE);
	}

	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0) {
		toLog(ERR, srvlog, "error in getrusage\n");
		exit(EXIT_FAILURE);
	}

	user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec / 1000000.0;
	user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec / 1000000.0;
	sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec / 1000000.0;
	sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec / 1000000.0;

  char buf[256];
	sprintf(buf, "\nuser time = %g, sys time = %g\n", user, sys);
  toLog(NFO,srvlog ,buf);
}

void sig_int()
{
	void pr_cpu_time(void);

	pr_cpu_time();

	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	static int nthreads;
	static short servport;
	static int backlog;
	int loglvl, i;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	int *sock;
	int optval;
	socklen_t optlen = sizeof(optval);
	int connsd;
	int err = 0;

	if (argc > 1)
		printf("\n %s : No arguments required. Use only server.cfg\n ", argv[0]);

	signal(SIGPIPE, SIG_IGN);

	if (signal(SIGINT, sig_int) == SIG_ERR) {
		fprintf(stderr, "signal error");
		exit(1);
	}

	fprintf(stdout, "Initializing parameters to default values...\n");
	init_parameters();

	fprintf(stdout, "Reading config file " CONFIG_FILE "\n");
	if (!parse_config()) {
		fprintf(stderr, "Failed to read config file: %s\n", strerror(errno));
    fprintf(stderr, "[WRN] %s will be executed with default values\n", argv[0]);
	}

	//fprintf(stdout, "Final values:\n");
	fprintf(stdout, "Server port: %s, Number of threads: %s, backlog: %s, LOG_LEVEL: %s\n", config_file.port, config_file.threads, config_file.backlog, config_file.loglvl);

	nthreads = atoi(config_file.threads);	/*number of thread in prethreading */
	servport = atoi(config_file.port);	/*convert in short integer */
	backlog = atoi(config_file.backlog);	/*backlog size */
	loglvl = atoi(config_file.loglvl);

	srvlog = create_logger("server.log", loglvl, 1);

	web_cache = create_cache();

     toLog(NFO,srvlog, "Server port: %s, Number of threads: %s, backlog: %s, LOG_LEVEL: %s\n", config_file.port, config_file.threads, config_file.backlog, config_file.loglvl);

	hwurfl = get_wurfldb("wurfl-eval.xml");
	if (hwurfl == NULL) {
    toLog(ERR,srvlog, "Error in wurlfd load database\n");
		exit(EXIT_FAILURE);
	}

	signal(SIGPIPE, SIG_IGN);

	if (signal(SIGINT, sig_int) == SIG_ERR) {
		toLog(ERR,srvlog, "Error in signal\n");
		exit(EXIT_FAILURE);
	}

	if ((listensd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {	/* crea il socket */
		perror("error in socket()\n");
    toLog(ERR,srvlog, "Error in socket()\n");
		exit(EXIT_FAILURE);
	}

	memset((void *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* il server accetta connessioni su una qualunque delle sue intefacce di rete */
	servaddr.sin_port = htons(servport);	/* numero di porta del server */

	optval = 1;
	optlen = sizeof(optval);

	if (setsockopt(listensd, SOL_SOCKET, SO_REUSEPORT, &optval, optlen) < 0)
    toLog(WRN,srvlog, "Unable to set SO_REUSEPORT on listening socket\n");

	if ((err = setsockopt(listensd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen)) < 0) {
		toLog(ERR, srvlog, "cannot set SO_KEEPALIVE option on socket. Abort.: %d : %s\n", err, strerror(err));
		close(listensd);
		exit(EXIT_FAILURE);
	}

  toLog(NFO,srvlog ,"SO_KEEPALIVE set\n");

	/* assegna l'indirizzo al socket */
	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
      toLog(ERR, srvlog, "error on bind()");
      exit(EXIT_FAILURE);
  }

	if (listen(listensd, backlog) < 0) {
    toLog(ERR,srvlog,"error on listen socket()");
    exit(EXIT_FAILURE);
  }

	tptr = (struct Thread *) Calloc(nthreads + 1, sizeof(struct Thread));

	for (i = 0; i < nthreads; i++)
		thread_make(i);	/* only main thread returns */


	for (;;) {

		len = sizeof(cliaddr);
		if ((connsd = accept(listensd, (struct sockaddr *) &cliaddr, &len)) < 0) {
			toLog(ERR,srvlog, "Error in accept %d : %s\n", connsd, strerror(connsd));
			exit(EXIT_FAILURE);
		}

		sock = malloc(sizeof(int));

		*sock = connsd;

		if ((err = pthread_mutex_lock(&pool_mutex)) != 0) {
			toLog(ERR, srvlog, "Error on pthread_mutex_lock");
			exit(EXIT_FAILURE);
		}

		insert_tail(sock, &list_sock);

		if ((err = pthread_cond_signal(&pool_cond)) != 0) {
			toLog(ERR, srvlog, "Error in pthread_cond_signal: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		if ((err = pthread_mutex_unlock(&pool_mutex)) != 0) {
			toLog(ERR, srvlog, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

	}

}
