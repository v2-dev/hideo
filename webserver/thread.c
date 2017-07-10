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

	if ((n = pthread_create(&tptr[i].thread_tid, NULL, &thread_main, (void *) i)) != 0) {
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

	cliaddr = malloc(addrlen);

	printf("thread %d starting\n", (int) arg);
	for ( ; ; ) {
		clilen = addrlen;
        if ((n = pthread_mutex_lock(&mlock)) != 0) {
            fprintf(stderr, "pthread_mutex_lock error");
            exit(1);
        }
		if ((connsd = accept(listensd, cliaddr, &clilen)) < 0 ) {
            fprintf(stderr, "accept error");
            exit(1);
		}
        if ((n = pthread_mutex_unlock(&mlock)) != 0) {
            fprintf(stderr, "pthread_mutex_unlock error");
            exit(1);
        }
		tptr[(int) arg].thread_count++;

		/* process request here */
		if (close(connsd) == -1) {
            fprintf(stderr, "close error");
            exit(1);
		}
	}
}
