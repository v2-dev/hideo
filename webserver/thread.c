#include <pthread.h>
#include "utils.h"
#include "thread.h"


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


void thread_make(int i)
{
	void	*thread_main(void *);
    int     n;

	fprintf(stdout, "begin segmentation fault\n");
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
        lock(&mlock);
		if ((connsd = accept(listensd, cliaddr, &clilen)) < 0 ) {
            fprintf(stderr, "accept error");
            exit(1);
		}
        unlock(&mlock);

		/* process request here */
		if (close(connsd) == -1) {
            fprintf(stderr, "close error");
            exit(1);
		}
	}
}
