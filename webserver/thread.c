#include "utils.h"
#include "thread.h"
#include "libhttp.h"
#include "logger.h"
#include <time.h>


void millisleep(int milliseconds)
{
	usleep(milliseconds * 1000);
}

void thread_make(int i)
{
	void *thread_main(void *);
	pthread_attr_t attr;
	tptr[i].thread_count = i;

	if ((pthread_attr_init(&attr)) != 0) {
		toLog(ERR, "Error in pthread_attr_init()\n", srvlog);
		exit(EXIT_FAILURE);
	}

	if ((pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0) {
		toLog(ERR, "Error in pthread_attr_setdetachstate()\n", srvlog);
		exit(EXIT_FAILURE);
	}

	if ((pthread_create(&tptr[i].thread_tid, &attr, &thread_main, &tptr[i])) != 0) {
		toLog(ERR, "Error in pthread_create\n", srvlog);
		return;
	}

	if ((pthread_attr_destroy(&attr)) != 0) {
		toLog(ERR, "Error in pthread_attr_destroy()\n", srvlog);
		exit(EXIT_FAILURE);
	}

	return;			/* main thread returns */
}


void *thread_main(void *arg)
{
	int connsd, retval = 0;
	struct Thread *thread_data = (struct Thread *) arg;
	struct conndata *cdata;

	thread_data->thread_tid = pthread_self() / 256;

	char *buf = Malloc(256);
	sprintf(buf, "Thread created [%u]\n", (unsigned int) thread_data->thread_tid);
	toLog(NFO, buf, srvlog);
	free(buf);

	while (1) {

		if (pthread_mutex_lock(&mtx) < 0){
			toLog(ERR, "Error on pthread_mutex_lock()\n", srvlog);
			pthread_exit(NULL);
		}

		if ((connsd = accept(listensd, (struct sockaddr *) NULL, NULL)) < 0){
			toLog(ERR, "Error on accept()\n", srvlog);
			pthread_exit(NULL);
		}

		toLog(NFO, "...connection accepted!\n", srvlog);

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 80000;

		if (setsockopt(connsd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0){
			toLog(ERR, "setsockopt SO_RCVTIMEO failed\n", srvlog);
			pthread_exit(NULL);
		}

		if (setsockopt(connsd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout)) < 0){
			toLog(ERR, "setsockopt SO_SNDTIMEO failed\n", srvlog);
			pthread_exit(NULL);
		}

		if (pthread_mutex_unlock(&mtx) < 0){
			toLog(ERR, "error on pthread_mutex_unlock()\n", srvlog);
			pthread_exit(NULL);
		}


		cdata = create_conndata();
		cdata->socketint = connsd;
		cdata->process_id = thread_data->thread_tid;

		/******************************
			Serve request to the client
		*******************************/
		retval = 1;

		while (1) {
			/*To be implemented: thread sleep until next client request */
			retval = serve_request(cdata);
			if (retval == ERROR) {
				toLog(NFO, "closing connection...\n", srvlog);
				Free(cdata);
				close(cdata->socketint);
				toLog(NFO, "...connection closed.\n", srvlog);
				break;
			}

		}
	}

	return NULL;
}
