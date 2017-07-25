#include "logger.h"

struct logger * alloc_logger(void){	/* alloca la memoria per un logger */

	struct logger * myLogger;
	myLogger = malloc(sizeof(struct logger));
	if (myLogger == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	return myLogger;
}

struct logger * create_logger(char * logPath, int loglvl){	/* crea un nuovo logger, che scriverà nel file di log indicato da logPath */

	struct logger * myLogger = alloc_logger();

	myLogger->f = fopen(logPath, "w+");
	myLogger->loglvl = loglvl;

	return myLogger;
}

void *thread_logger_manager()
{
	int err = 0;

	for(;;){

		if ((err = pthread_mutex_lock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_lock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		while ((strlen(MSG_LOG) < 1) {
			if ((err = pthread_cond_wait(&logger_cond, &pool_mutex)) != 0) {
				fprintf(stderr, "Error in pthread_cond_wait: %d : %s\n", err, strerror(err));
				exit(EXIT_FAILURE);
			}
		}

		fprintf(srvlog->f, MSG_LOG);

		if ((err = pthread_mutex_unlock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}


	}
}


void toLog(int msgt, char * msg, struct logger * myLogger){

	pid_t tid;
	int err;
	tid = syscall(SYS_gettid);

	if ((err = pthread_mutex_lock(&logger_mutex)) != 0) {
		fprintf(stderr, "Error in pthread_mutex_lock: %d : %s\n", err, strerror(err));
		exit(EXIT_FAILURE);
	}

	memset(MSG_LOG, 0, MSG_SIZE);

	if ((err = pthread_mutex_unlock(&logger_mutex)) != 0) {
		fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((msgt==ERR)&&((ERR) & (myLogger->loglvl))){

		//fprintf(stderr, "[ERR] [%d] %s\n", tid, msg); //da togliere se non voglio stampare il msg sullo schermo

		//WAKE UP! signal for thread_logger_manager
		if ((err = pthread_mutex_lock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_lock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		sprintf(MSG_LOG, "[ERR] [%d] %s\n", tid, msg);

		if ((err = pthread_cond_signal(&logger_cond, &logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_cond_signal: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		if ((err = pthread_mutex_unlock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

	}


	else if ((msgt==WRN)&&((WRN) & (myLogger->loglvl))){

		//fprintf(stdout, "[WRN] [%d] %s\n", tid, msg);  //da togliere se non voglio stampare il msg sullo schermo
		if ((err = pthread_mutex_lock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_lock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		sprintf(MSG_LOG, "[WRN] [%d] %s\n", tid, msg);

		//WAKE UP! signal for thread_logger_manager
		if ((err = pthread_cond_signal(&logger_cond, &logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_cond_signal: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		if ((err = pthread_mutex_unlock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		//fprintf(myLogger->f, "[WRN] [%d] %s\n", tid, msg);

	}

	else if ((msgt==NFO)&&((NFO) & ((myLogger->loglvl)>>2))){

		//fprintf(stdout, "[NFO] [%d] %s\n", tid, msg);  //da togliere se non voglio stampare il msg sullo schermo
		if ((err = pthread_mutex_lock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_lock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		sprintf(MSG_LOG, "[INFO] [%d] %s\n", tid, msg);

		//WAKE UP! signal for thread_logger_manager
		if ((err = pthread_cond_signal(&logger_cond, &logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_cond_signal: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		if ((err = pthread_mutex_unlock(&logger_mutex)) != 0) {
			fprintf(stderr, "Error in pthread_mutex_unlock: %d : %s\n", err, strerror(err));
			exit(EXIT_FAILURE);
		}

		//fprintf(myLogger->f, "[NFO] [%d] %s\n", tid, msg);
	}


}
