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
		toLog(ERR,srvlog, "Error in pthread_attr_init()");
		exit(EXIT_FAILURE);
	}

	if ((pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0) {
		toLog(ERR, srvlog, "Error in pthread_attr_setdetachstate()");
		exit(EXIT_FAILURE);
	}

	if ((pthread_create(&tptr[i].thread_tid, &attr, &thread_main, &tptr[i])) != 0) {
		toLog(ERR,srvlog, "Error in pthread_create");
		return;
	}

	if ((pthread_attr_destroy(&attr)) != 0) {
		toLog(ERR,srvlog, "Error in pthread_attr_destroy()");
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
	sprintf(buf, "Thread created [%u]", (unsigned int) thread_data->thread_tid);
	toLog(NFO,srvlog, buf);
	free(buf);

	while (1) {

		if (pthread_mutex_lock(&mtx) < 0){
			toLog(ERR,srvlog, "Error on pthread_mutex_lock()");
			pthread_exit(NULL);
		}

		if ((connsd = accept(listensd, (struct sockaddr *) NULL, NULL)) < 0){
			toLog(ERR,srvlog, "Error on accept()");
			pthread_exit(NULL);
		}

		toLog(NFO,srvlog, "...connection accepted!");

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 75000;

		if (setsockopt(connsd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0){
			toLog(ERR, srvlog, "setsockopt SO_RCVTIMEO failed");
			pthread_exit(NULL);
		}

		if (setsockopt(connsd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout)) < 0){
			toLog(ERR, srvlog, "setsockopt SO_SNDTIMEO failed");
			pthread_exit(NULL);
		}

		if (pthread_mutex_unlock(&mtx) < 0){
			toLog(ERR, srvlog, "error on pthread_mutex_unlock()");
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
				toLog(NFO,srvlog, "closing connection...");
				Free(cdata);
				close(cdata->socketint);
				toLog(NFO,srvlog, "...connection closed.");
				break;
			}

		}
	}

	return NULL;
}
