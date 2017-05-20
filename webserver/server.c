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
#include <pthread.h>
#include <signal.h>

//#include "utils.h"
#include "utils_.h"
#include "readreq.h"
#include "mimetypes.h"
#include "conndf.h"

#define BACKLOG	10
#define MAXTHREADS 10

pthread_mutex_t mtx;

int listensd;
/*
void serve_request(int socket_int){

	struct httpread * httpr;

	httpr = read_request(socket_int);

	destroy_httpread(httpr);

}
*/

void lock(pthread_mutex_t * mtx)
{
	if (pthread_mutex_lock(mtx) != 0)
		err_exit("Error on pthread_mutex_lock", errno);
}

void unlock(pthread_mutex_t * mtx)
{
	if (pthread_mutex_unlock(mtx) != 0)
		err_exit("Error on pthread_mutex_unlock", errno);
}


int serve_request(struct conndata * cdata)
{
	struct httpread * httpr;
	httpr = read_request(cdata->socketint);
	/*
	 * if (httpr == NULL)
	{
		destroy_httpread(httpr);
		return 0;
	}
	* */
	if (httpr->dimArray == 0)
	{
		destroy_httpread(httpr);
		return 0;
	}
	if (httpr->dimArray == -1)
	{
		destroy_httpread(httpr);
		return -1;
	}

	if ( method_parse(*(httpr->array), cdata) > 0 )
	{
		if (path_parse(*(httpr->array), cdata) == 0)
		{
			//strcpy(cdata->messages, *(httpr->array));
			//print_message(cdata);
			int i;
			for(i = 1; i<httpr->dimArray; i++)
			{
				accheck(*(httpr->array+i), cdata);
				uacheck(*(httpr->array+i), cdata);
				//strcpy(cdata->messages, *(httpr->array+i));
				//print_message(cdata);
			}
			if (send_response(cdata) != 0) return -1;
		}
	}

	destroy_httpread(httpr);
	return 1;
}
//void * thread_main(void *p){
void * thread_main()
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
}

int main(int argc, char * argv[])
{

	signal(SIGPIPE, SIG_IGN);

	int SERV_PORT = 7800;	//porta di default se non inserita
	if (argc > 2)
	{
		printf("\nInserire soltanto la porta di esecuzione");
		exit(EXIT_FAILURE);
	}
	if (argc == 2 && argv[1][0] != '\0')
	{
		char *errptr;
		errno = 0;
		SERV_PORT = (int) strtol(argv[1], &errptr, 0);
		if (errno != 0 || *errptr != '\0')
		{
			fprintf(stderr, "\nNumero di porta non valido");
			return EXIT_FAILURE;
		}
	}

	int optval;
	socklen_t optlen = sizeof(optval);

	struct sockaddr_in servaddr;

	if ((listensd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{ 	/* crea il socket */
		perror("errore in socket");
		exit(1);
	}

	memset((void *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta connessioni su una qualunque delle sue intefacce di rete */
	servaddr.sin_port = htons(SERV_PORT); /* numero di porta del server */

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

	if (listen(listensd, BACKLOG) < 0 )
	{
		perror("errore in listen");
		exit(1);
	}

	//int set = 1;
	//setsockopt(listensd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));

	//creazione dei thread
	if(pthread_mutex_init(&mtx, NULL) != 0){
		perror("");
		exit(EXIT_FAILURE);
	}

	pthread_t replythread;
	int i;
	for(i=0;i < MAXTHREADS; i++){
		pthread_create(&replythread, NULL, thread_main, NULL);
	}

	thread_main();
	return EXIT_SUCCESS;
}
