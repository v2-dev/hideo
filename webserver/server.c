#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "thread.h"
#include <signal.h>
#include "utils.h"
#include "readreq.h"
#include "parse_conf_file.h"
#include "libhttp.h"
#include <sys/resource.h>
#include <sys/time.h>

int listensd;
char *request;


void pr_cpu_time(void)
{
  double	user, sys;
  struct rusage	myusage, childusage;

  if (getrusage(RUSAGE_SELF, &myusage) < 0) {
    fprintf(stderr, "errore in getrusage");
    exit(1);
  }

  if (getrusage(RUSAGE_CHILDREN, &childusage) < 0) {
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

void sig_int(int signo)
{
	int		i;
	void	pr_cpu_time(void);

	pr_cpu_time();

	for (i = 0; i < nthreads; i++)
		printf("thread %d, %ld connections\n", i, tptr[i].thread_count);

	exit(0);
}

int serve_request(struct conndata * cdata)
{
	struct httpread * httpr;

	request = malloc(sizeof(char) * BUFSIZE);
	if(request == NULL)
		return 0;

	request = read_string(cdata->socketint);
	fprintf(stdout, "<------- http request -------> \n%s\n", request);

	return -1; //TO CHANGE!
}
//void * thread_main(void *p){
/*void * thread_main()
{
	int pthread_sid;
	pthread_sid = pthread_self() / 256;
	printf("\nThread created [%x]", pthread_sid);
	fflush(stdout);

	int socket_int;
	struct conndata * cdata;
	int j;
	int retval;

	while(1)
	{
		int c = 0;
		if ((socket_int = accept(listensd, (struct sockaddr *)NULL, NULL)) < 0)
			perror("error in accept\n");

		struct timeval tv;
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		if (setsockopt(socket_int, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,sizeof(struct timeval)) < 0)
			{
				perror("errore in setsockopt");
				close(socket_int);
				break;
			}
		if (setsockopt(socket_int, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv,sizeof(struct timeval)) < 0)
			{
				perror("errore in setsockopt");
				close(socket_int);
				break;
			}

		j = 0;
		cdata = create_conndata();
		cdata->process_id = pthread_sid;
		cdata->socketint = socket_int;
		cdata->keepalmaxn = 5;
		strcpy(cdata->messages, "Thread connesso");
		print_message(cdata);
		while(1)
		{
				strcpy(cdata->messages, "Servendo il client");
				print_message(cdata);
				retval = serve_request(cdata);
				if ( retval == -1 ) break;
				if ( retval == 0) j++;
				if ( j > 2 ) break;
				cdata->keepalmaxn--;
				if (cdata->keepalmaxn == 0) break;
		}
		strcpy(cdata->messages, "Chiusura connessione");
		print_message(cdata);
		close(socket_int);
		free(cdata);
	}

	return EXIT_SUCCESS;
}*/

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

	signal(SIGPIPE, SIG_IGN);

	if (signal(SIGINT, sig_int) == SIG_ERR) {
		fprintf(stderr, "signal error");
		exit(1);
	}

	fprintf(stdout, "Initializing parameters to default values...\n");
	init_parameters();

	fprintf(stdout, "Reading config file...\n");
	parse_config();

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
		perror("errore in bind");
		exit(1);
	}

	if (listen(listensd, backlog) < 0 )
	{
		perror("errore in listen");
		exit(1);
	}

	tptr = (struct Thread *)calloc(nthreads, sizeof(struct Thread));
    if (tptr == NULL) {
        fprintf(stderr, "calloc error\n");
        exit(1);
    }

	for (i = 0; i < nthreads; i++)
		thread_make(i);			/* only main thread returns */


	for ( ; ; )
		pause();	/* everything done by threads */
}
