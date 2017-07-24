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

struct node_t *alloc_node()
{
	struct node_t *p;

	p = Malloc(sizeof(struct node_t));
	return p;
}

void insert_tail(void *v, struct list *l)
{
	struct node_t *new_tail;
	struct node_t *old_tail;

	new_tail = alloc_node();
	new_tail->value = v;
	old_tail = l->list_tail;

	new_tail->next = NULL;
	l->list_tail = new_tail;

	if (l->size != 0)
		old_tail->next = new_tail;
	else
		l->list_head = new_tail;

	l->size += 1;

	return;
}

void *remove_head(struct list *l)
{
	if (l->size > 0) {
		struct node_t *old_head;
		void *value;

		old_head = l->list_head;
		value = old_head->value;
		if (l->size != 1)
			l->list_head = old_head->next;
		else {
			l->list_head = NULL;
			l->list_tail = NULL;
		}

		l->size -= 1;
		free(old_head);

		return value;
	} else
		return NULL;
}



void sig_int()
{
	int		i;

	for (i = 0; i < nthreads; i++)
		printf("thread %d, %ld connections\n", i, tptr[i].thread_count);

	exit(0);
}


int main(int argc, char **argv)
{
	static int nthreads;
	static short servport;
	static int backlog;
  int loglvl;
  int *sock;

	int i = 0;

	if (argc > 1)
		printf("\n %s : No arguments required. Use only server.cfg\n ", argv[0]);

	web_cache = create_cache();

	hwurfl = get_wurfldb("wurfl-eval.xml");
	if (hwurfl == NULL){
		fprintf(stderr, "Error in wurlfd load database\n");
		exit(EXIT_FAILURE);
	}

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
	fprintf(stdout, "Server port: %s, Number of threads: %s, backlog: %s, LOG_LEVEL: %s\n", config_file.port, config_file.threads, config_file.backlog, config_file.loglvl);

	nthreads = atoi(config_file.threads);	/*number of thread in prethreading */
	servport = atoi(config_file.port);	/*convert in short integer */
	backlog = atoi(config_file.backlog); /*backlog size */
  loglvl = atoi(config_file.loglvl);

	srvlog = create_logger("server.log", loglvl);

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
		unix_error("error on bind()\n");

	if (listen(listensd, backlog) < 0 )
		unix_error("listen\n");

	tptr = (struct Thread *)Calloc(nthreads, sizeof(struct Thread));

  /*mutex initializer*/
  if(pthread_mutex_init(&mtx, NULL) != 0)
    err_exit("Error on pthread_mutex_init()\n", errno);

	for (i = 0; i < nthreads; i++)
		thread_make(i);			/* only main thread returns */


	for ( ; ; )
		pause();	/* everything done by threads */
}
