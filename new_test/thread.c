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


void thread_job(struct conndata *cdata)
{
	int retval = 1;

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


void *thread_main(void *arg)
{
	int connsd, err;
	struct timeval timeout;
	struct Thread *thread_data = (struct Thread *) arg;
	struct conndata *cdata;

	int *sock;

	connsd = -1;

	thread_data->thread_tid = pthread_self() / 256;

	toLog(NFO,srvlog, "Thread created [%u]", (unsigned int) thread_data->thread_tid);

	for (;;) {

		/*if (connsd != -1) {

			if (close(connsd) < 0) {
				fprintf(stderr, "Error in close: %d : %s\n", errno, strerror(errno));
				exit(EXIT_FAILURE);
			}

		}*/

		if ((err = pthread_mutex_lock(&pool_mutex)) != 0) {
			toLog(ERR,srvlog, "Error on pthread_mutex_lock()");
			pthread_exit(NULL);
		}

		while ((sock = remove_head(&list_sock)) == NULL) {
			if ((err = pthread_cond_wait(&pool_cond, &pool_mutex)) != 0) {
				toLog(ERR, srvlog, "Error in pthread_cond_wait: %d : %s\n", err, strerror(err));
				pthread_exit(NULL);
			}
		}

		toLog(NFO,srvlog, "...begin client service...");

		connsd = *sock;
		free(sock);

		if ((err = pthread_mutex_unlock(&pool_mutex)) != 0) {
			toLog(ERR, srvlog, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = 75000;

		if (setsockopt(connsd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0) {
			toLog(ERR, srvlog, "Error in setsockopt: %d : %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (setsockopt(connsd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout)) < 0) {
			toLog(ERR, srvlog, "Error in setsockopt: %d : %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}



		cdata = create_conndata();
		cdata->socketint = connsd;
		cdata->process_id = thread_data->thread_tid;

		/******************************
			Serve request to the client
		*******************************/
		thread_job(cdata);

	}

	return NULL;
}
