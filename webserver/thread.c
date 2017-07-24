#include "utils.h"
#include "thread.h"
#include "libhttp.h"
#include <time.h>


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


void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
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
    		timeout.tv_sec = 0;
    		timeout.tv_usec = 80000;

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
		int ii = 0;
		while(1){
				printf("thread.c cicle: %d\n", ii++);
			/*To be implemented: thread sleep until next client request*/
				retval = serve_request(cdata);
				printf("retval: %d\n", retval);
				if (retval == ERROR){
					Free(cdata);
					close(cdata->socketint);
					break;
				}
				
			}
	}

	return NULL;
}
