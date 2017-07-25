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


void toLog(int msgt, char * msg, struct logger * myLogger){

	pid_t tid;
	tid = syscall(SYS_gettid);

	if ((msgt==ERR)&&((ERR) & (myLogger->loglvl))){

		//fprintf(stderr, "[ERR] [%d] %s\n", tid, msg); //da togliere se non voglio stampare il msg sullo schermo
		fprintf(myLogger->f, "[ERR] [%d] %s\n", tid, msg);
	}


	else if ((msgt==WRN)&&((WRN) & (myLogger->loglvl))){

		//fprintf(stdout, "[WRN] [%d] %s\n", tid, msg);  //da togliere se non voglio stampare il msg sullo schermo
		fprintf(myLogger->f, "[WRN] [%d] %s\n", tid, msg);

	}

	else if ((msgt==NFO)&&((NFO) & ((myLogger->loglvl)>>2))){

		//fprintf(stdout, "[NFO] [%d] %s\n", tid, msg);  //da togliere se non voglio stampare il msg sullo schermo
		fprintf(myLogger->f, "[NFO] [%d] %s\n", tid, msg);
	}

}
