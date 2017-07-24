#include "utils.h"
#include "thread.h"
#include "libhttp.h"
#include <time.h>


void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
}

void check_pool_size()
{
	int i, delta_pool_size, err;
	pthread_t pthread;

	if ((err = pthread_mutex_lock(&pool_size_mutex)) != 0) {
		fprintf(stderr, "Error in pthread_mutex_lock: %d : %s\n", err, strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((pool_free / (double) pool_size) < PERC_MIN_FREE) {

		delta_pool_size = pool_size;
		pool_free += pool_size;
		pool_size += pool_size;

		if ((err = pthread_mutex_unlock(&pool_size_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < delta_pool_size; i++)
			create_pthread(&pthread, thread_main, NULL, PTHREAD_CREATE_DETACHED);

		return;

	} else if ((pool_free / (double) pool_size) > PERC_MAX_FREE && pool_size > 2) {

		pool_size--;

		if ((err = pthread_mutex_unlock(&pool_size_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		pthread_exit(NULL);

	}

	if ((err = pthread_mutex_unlock(&pool_size_mutex)) != 0) {
		fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
		exit(EXIT_FAILURE);
	}
}


void *thread_socket_manager()
{
	int listensd, connsd, err, optval;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	int *sock;

	if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Error in socket: %d : %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	optval = 1;
	if (setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
		fprintf(stderr, "Error in setsockopt: %d : %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset((void *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
		fprintf(stderr, "Error in bind: %d : %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (listen(listensd, BACKLOG) < 0) {
		fprintf(stderr, "Error in listen: %d : %s\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (;;) {

		len = sizeof(cliaddr);
		if ((connsd = accept(listensd, (struct sockaddr *) &cliaddr, &len)) < 0) {
			fprintf(stderr, "Error in accept: %d : %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if ((sock = malloc(sizeof(int))) == NULL) {
			fprintf(stderr, "Error in malloc\n");
			exit(EXIT_FAILURE);
		}

		*sock = connsd;

		if ((err = pthread_mutex_lock(&pool_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_lock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		insert_tail(sock, &list_sock);

		if ((err = pthread_cond_signal(&pool_cond)) != 0) {
			fprintf(stderr, "Error in pthread_cond_signal: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		if ((err = pthread_mutex_unlock(&pool_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

	}
}


void thread_make(int i)
{
	void	*thread_main(void *);

	tptr[i].thread_count = i;

	if ((pthread_create(&tptr[i].thread_tid, NULL, &thread_main, &tptr[i])) != 0) {
		err_exit("Error on pthread_create", errno);
	}
	return;		/* main thread returns */
}


void *thread_main(void *arg)
{
	int	connsd, j = 0, retval = 0;
	struct Thread *thread_data = (struct Thread *)arg;
	struct conndata * cdata;

	j=j; /* compiler warning */
	thread_data->thread_tid = pthread_self() / 256;
	printf("Thread created [%u]\n", (unsigned int) thread_data->thread_tid);
	fflush(stdout);

	while(1) {

		if(pthread_mutex_lock(&mtx) < 0)
			pthread_exit(NULL);

		if ((connsd = accept(listensd, (struct sockaddr *)NULL, NULL)) < 0)
			perror("error in accept\n");

		fprintf(stdout, "...connection accepted!\n");

		struct timeval timeout;
    		timeout.tv_sec = 1;
    		timeout.tv_usec = 0;

   	if (setsockopt(connsd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    	unix_error("setsockopt failed\n");

    if (setsockopt(connsd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    	unix_error("setsockopt failed\n");

		if(pthread_mutex_unlock(&mtx) < 0)
			pthread_exit(NULL);

		cdata = create_conndata();
		cdata->socketint = connsd;
		cdata->process_id = thread_data->thread_tid;

		/******************************
			Serve request to the client
		*******************************/
		retval = 1;
		while(1){
			/*To be implemented: thread sleep until next client request*/
				retval = serve_request(cdata);
				if (retval == ERROR){
					Free(cdata);
					close(cdata->socketint);
					break;
				}

			}
	}

	return NULL;
}
