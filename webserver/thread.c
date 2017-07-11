#include <pthread.h>
#include "utils.h"
#include "thread.h"
#include "libhttp.h"


void lock(pthread_mutex_t * mux)
{
	if (pthread_mutex_lock(mux) != 0)
		err_exit("Error on pthread_mutex_lock", errno);
}

void unlock(pthread_mutex_t * mux)
{
	if (pthread_mutex_unlock(mux) != 0)
		err_exit("Error on pthread_mutex_unlock", errno);
}


void thread_make(int i)
{
	void	*thread_main(void *);
    int     n;

	tptr[i].thread_count = i;

	if ((n = pthread_create(&tptr[i].thread_tid, NULL, &thread_main, &tptr[i])) != 0) {
        errno = n;
		fprintf(stderr, "pthread_create error");
		exit(1);
	}
	return;		/* main thread returns */
}


void *thread_main(void *arg)
{
	int				connsd, n;
	socklen_t		clilen;
	struct sockaddr	*cliaddr;

	struct Thread *thread_data = (struct Thread *)arg;

	cliaddr = malloc(addrlen);

	printf("thread %d starting\n", thread_data->thread_count);

	for ( ; ; ) {
		clilen = addrlen;
        lock(&mtx);
		if ((connsd = accept(listensd, cliaddr, &clilen)) < 0 ) {
            fprintf(stderr, "accept error");
            exit(1);
		}
        unlock(&mtx);

		//printf("server: got connection from %s port %d\n",inet_ntoa(cliaddr->sin_addr),	ntohs(cliaddr->sin_port));
		printf("Connection closed\n");

		if (close(connsd) == -1){
            fprintf(stderr, "close error");
            exit(1);
		}

	}
}
