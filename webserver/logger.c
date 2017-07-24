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
	
	if ((msgt==ERR)&&((ERR) & (myLogger->loglvl))){
		
		fprintf(myLogger->f, "[ERR] %s\n", msg);
	}
	
	
	else if ((msgt==WRN)&&((WRN) & (myLogger->loglvl))){
		
		fprintf(myLogger->f, "[WRN] %s\n", msg);
		
	}
	
	else if ((msgt==NFO)&&((NFO) & ((myLogger->loglvl)>>2))){
		
		fprintf(myLogger->f, "[NFO] %s\n", msg);
	}
	
}
