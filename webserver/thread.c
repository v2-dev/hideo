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
	int				connsd, n, j = 0, retval = 0;
	socklen_t		clilen;
	struct sockaddr_in	*cliaddr;

	struct Thread *thread_data = (struct Thread *)arg;
	struct conndata * cdata;

	cliaddr = Malloc(addrlen);

	printf("thread %ld starting\n", thread_data->thread_count);

	for ( ; ; ) {
		clilen = addrlen;
    lock(&mtx);
		if ((connsd = accept(listensd, (struct sockaddr *) cliaddr, &clilen)) < 0 )
            unix_error("error on accept()\n");
    unlock(&mtx);

		struct timeval tv;
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		if (setsockopt(listensd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,sizeof(struct timeval)) < 0){
				//warning message here
				perror("error on setsockopt\n");
				close(listensd);
				break;
			}
		if (setsockopt(listensd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv,sizeof(struct timeval)) < 0){
				perror("error on setsockopt ");
				close(listensd);
				break; //da migliorare la robustezza
			}

		cdata = create_conndata();
		cdata->socketint = connsd;
		strcpy(cdata->messages, "Thread connesso\n");

		printf("server: got connection from %s ; port %d\n",inet_ntoa(cliaddr->sin_addr),	ntohs(cliaddr->sin_port));
		/******************************
			Read request here
		*******************************/
		for(;;){
			retval = serve_request(cdata);
			if ( retval == -1 ) break;
			if ( retval == 0) j++;
			if ( j > 2 ) break;
			cdata->keepalmaxn--;
			if (cdata->keepalmaxn == 0) break;
		}

		Free(cdata);
		if (close(connsd) == -1){
            unix_error("error on close()\n");
		}

	}
}
