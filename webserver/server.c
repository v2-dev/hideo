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


int listensd;
char *request;


void pr_cpu_time(void)
{
  double	user, sys;
  struct rusage	myusage, childusage;

  if (getrusage(RUSAGE_SELF, &myusage) < 0)
  {
    fprintf(stderr, "errore in getrusage");
    exit(1);
  }

  if (getrusage(RUSAGE_CHILDREN, &childusage) < 0)
  {
    fprintf(stderr, "errore in getrusage");
    exit(1);
  }

  user = (double) myusage.ru_utime.tv_sec +
		myusage.ru_utime.tv_usec/1000000.0;
  user += (double) childusage.ru_utime.tv_sec +
		 childusage.ru_utime.tv_usec/1000000.0;
  sys = (double) myusage.ru_stime.tv_sec +
		 myusage.ru_stime.tv_usec/1000000.0;
  sys += (double) childusage.ru_stime.tv_sec +
		 childusage.ru_stime.tv_usec/1000000.0;

  printf("\nuser time = %g, sys time = %g\n", user, sys);
}

void sig_int()
{
	int		i;
	void	pr_cpu_time(void);

	pr_cpu_time();

	for (i = 0; i < nthreads; i++)
		printf("thread %d, %ld connections\n", i, tptr[i].thread_count);

	exit(0);
}


int main(int argc, char **argv)
{
	static int nthreads;
	static short servport;
	static int backlog;
	int i = 0;

	if (argc > 1)
	{
		printf("\n %s : No arguments required. Use only server.cfg\n ", argv[0]);
	}

	web_cache = create_cache();

	/*
	hwurfl = get_wurfldb("wurfl-eval.xml");
	if (hwurfl == NULL){
		fprintf(stderr, "Error in wurlfd load database\n");	TOLTO PER GIULIA: DA RIMETTERE
		exit(EXIT_FAILURE);
	}*/

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
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Final values:\n");
	fprintf(stdout, "Server port: %s, Number of threads: %s, backlog: %s\n", config_file.port, config_file.threads, config_file.backlog);

	nthreads = atoi(config_file.threads);	/*number of thread in prethreading */
	servport = atoi(config_file.port);	/*convert in short integer */
	backlog = atoi(config_file.backlog); /*backlog size */

	int optval;
	socklen_t optlen = sizeof(optval);

	struct sockaddr_in servaddr;

	if ((listensd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{ 	/* crea il socket */
		perror("error in socket()\n");
		exit(1);
	}

	memset((void *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta connessioni su una qualunque delle sue intefacce di rete */
	servaddr.sin_port = htons(servport); /* numero di porta del server */

	optval = 1;
	optlen = sizeof(optval);

  if(setsockopt(listensd, SOL_SOCKET, SO_REUSEPORT, &optval, optlen) < 0)
		print_err_msg("Unable to set SO_REUSEPORT on listening socket\n");

	if(setsockopt(listensd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
	{
		perror("errore in setsockopt");
		close(listensd);
		exit(EXIT_FAILURE);
	}
	printf("SO_KEEPALIVE impostata\n");

	if(getsockopt(listensd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0)
	{
		perror("errore in getsockopt");
		close(listensd);
		exit(EXIT_FAILURE);
	}
	printf("SO_KEEPALIVE è %s\n", (optval ? "ON" : "OFF"));

	/* assegna l'indirizzo al socket */
	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)
	{
		unix_error("error on bind()\n");
	}

	if (listen(listensd, backlog) < 0 )
	{
		unix_error("listen\n");
	}

	tptr = (struct Thread *)Calloc(nthreads, sizeof(struct Thread));

  /*mutex initializer*/
  if(pthread_mutex_init(&mtx, NULL) != 0)
    err_exit("Error on pthread_mutex_init()\n", errno);

	for (i = 0; i < nthreads; i++)
		thread_make(i);			/* only main thread returns */


	for ( ; ; )
		pause();	/* everything done by threads */
}
