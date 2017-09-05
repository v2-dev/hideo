#include "logger.h"


void abort_on_error(int cond, char * msg){
	if (cond){
		fprintf(stderr, "%s\n", msg);
		exit(EXIT_FAILURE);
	}
}


void * garbadge(void * p);
void lock_logger(struct logger * myLogger);
void unlock_logger(struct logger * myLogger);


struct logNode * alloc_logNode(void){	/* alloca la memoria per un logNode */

	struct logNode * ln;
	ln = malloc(sizeof(struct logNode));
	abort_on_error(ln==NULL, "Memory allocation error");
	return ln;
}


struct logNode * create_logNode(int type){	/* crea un logNode con messaggio uguale alla stringa text*/

	struct logNode * ln;

	ln = alloc_logNode();

	ln->text = calloc(200, sizeof(char));
	ln->timestamp = calloc(50, sizeof(char));
	abort_on_error(ln->text==NULL, "Memory allocation error");
	ln->next = NULL;
	ln->prev = NULL;
	ln->type = type;

	ln->tid = syscall(SYS_gettid);

	return ln;
}

struct logger * alloc_logger(void){	/* alloca la memoria per un logger */

	struct logger * myLogger;
	myLogger = malloc(sizeof(struct logger));
	abort_on_error(myLogger==NULL, "Memory allocation error");
	return myLogger;
}


void destroy_logNode(struct logNode * ln){	/* libera la memoria di un logNode */

	free(ln->text);
	free(ln);
}


struct logger * create_logger(char * pathLog, int log_lvl, int printOnScreen){	/* crea un logger che scrive nel file di log del percorso indicato da pathLog */

	struct logger * myLogger = alloc_logger();

	myLogger->tail = create_logNode(0);
	myLogger->head = create_logNode(0);

	myLogger->tail->prev = NULL;
	myLogger->tail->next = myLogger->head;

	myLogger->head->next = NULL;
	myLogger->head->prev = myLogger->tail;

	myLogger->log_lvl = log_lvl;

	myLogger->printOnScreen = printOnScreen;

	myLogger->log_fd = open(pathLog, O_RDWR | O_CREAT | O_APPEND, 0644);
	abort_on_error((myLogger->log_fd)==-1, "Error in open");

	int rc;

	rc = pthread_mutex_init(&(myLogger->log_mtx), NULL);
	abort_on_error(rc!=0, "Error in pthread_mutex_init logger");
	rc = pthread_cond_init(&(myLogger->log_cond), NULL);
	abort_on_error(rc!=0, "Error in pthread_cond_init logger");

	rc = pthread_create(&(myLogger->tid_garbadge), NULL, garbadge, myLogger);
	abort_on_error(rc!=0, "Error in pthread_create garbadge logger");

	return myLogger;
}

void insert_logNode(struct logger * myLogger, struct logNode * ln){	/* inserisce un nuovo logNode nel logger */

	ln->prev = myLogger->tail;
	ln->next = myLogger->tail->next;

	myLogger->tail->next->prev = ln;
	myLogger->tail->next = ln;
}

void delete_logNode(struct logNode * ln){	/* elimina un logNode dal logger */

	ln->prev->next = ln->next;
	ln->next->prev = ln->prev;

	//PER LIBERARE LA MEMORIA DEVO FARE LA DESTROY (LA FACCIO DA UN'ALTRA PARTE)
}

void summary_logger(struct logger * myLogger){	/*stampa il testo di tutti i logNode presenti nel logger */

	struct logNode * temp_logNode = myLogger->tail->next;

	while (temp_logNode != myLogger->head){

		//printf("%s\n", temp_logNode->text);
		temp_logNode = temp_logNode->next;
	}
}

void wait_for_message(struct logger * myLogger){

	int rc = pthread_cond_wait(&(myLogger->log_cond), &(myLogger->log_mtx));
	abort_on_error(rc !=0, "Error in pthread_cond_wait logger");
}

void lock_logger(struct logger * myLogger){

	int rc = pthread_mutex_lock(&(myLogger->log_mtx));
	abort_on_error(rc !=0, "Error in pthread_mutex_lock logger");
}


void unlock_logger(struct logger * myLogger){

	int rc = pthread_mutex_unlock(&(myLogger->log_mtx));
	abort_on_error(rc !=0, "Error in pthread_mutex_unlock logger");
}

int is_logger_empty(struct logger * myLogger){

	return (myLogger->tail->next == myLogger->head);
}

struct logNode * get_first_logNode(struct logger * myLogger){

	return myLogger->head->prev;
}

void write_on_file(struct logger * myLogger, struct logNode * ln){

	int fd = myLogger->log_fd;

	char messageType[5];
	if ((ln->type)==ERR) strcpy(messageType, "ERR");
	else if ((ln->type)==WRN) strcpy(messageType, "WRN");
	else strcpy(messageType, "NFO");

	char message[500];
	sprintf(message, "[%s] [%s] [%d] %s\n", messageType, ln->timestamp, ln->tid, ln->text);

	int to_write = strlen(message);

	int n;
	char * p = message;

	while(to_write){

		n = write(fd, p, to_write);
		abort_on_error(n==-1, "Error in write logger");

		p += n;
		to_write -= n;
	}
}

void compute_message(struct logger * myLogger, struct logNode * ln){

	myLogger=myLogger;

	if ((ln->type==ERR)&&((ERR) & (myLogger->log_lvl))){

		//if(myLogger->printOnScreen) fprintf(stderr, "[ERR] [%d] %s\n", ln->tid, ln->text);

		write_on_file(myLogger, ln);
	}

	else if ((ln->type==WRN)&&((WRN) & (myLogger->log_lvl))){

		//if(myLogger->printOnScreen) fprintf(stdout, "[WRN] [%d] %s\n", ln->tid, ln->text);

		write_on_file(myLogger, ln);
	}

	else if ((ln->type==NFO)&&((NFO) & ((myLogger->log_lvl)>>2))){

		//if(myLogger->printOnScreen) fprintf(stdout, "[NFO] [%d] %s\n", ln->tid, ln->text);

		write_on_file(myLogger, ln);
	}
}

void awake_garbadge(struct logger * myLogger){

	int rc = pthread_cond_signal(&(myLogger->log_cond));
	abort_on_error(rc !=0, "Error in pthread_cond_signal logger");
}


void * garbadge(void * p){

	struct logger * myLogger = (struct logger *) p;

	struct logNode * logNodeDel = NULL;

	while(1){

		lock_logger(myLogger);

		if (is_logger_empty(myLogger)) wait_for_message(myLogger);

		logNodeDel = get_first_logNode(myLogger);

		delete_logNode(logNodeDel);

		unlock_logger(myLogger);

		compute_message(myLogger, logNodeDel);

		destroy_logNode(logNodeDel);
	}

	pthread_exit(NULL); //non lo esegue mai
}

char* getDateString() {
    // Initialize and get current time
    time_t t = time( NULL );

    // Allocate space for date string
    char* date = (char*)malloc(50);

    // Format the time correctly %Y-%m-%d the time in 24-hour notation
		strftime (date, 100, "%F %T",localtime (&t));

    return date;
}

void toLog(int type, struct logger * myLogger, char * buf, ...){

	struct logNode * ln = create_logNode(type);

	ln->timestamp = getDateString();
	va_list arglist;
    	va_start(arglist, buf);

   	vsprintf(ln->text, buf, arglist);

   	va_end(arglist);

	lock_logger(myLogger);

	insert_logNode(myLogger, ln);
	awake_garbadge(myLogger);

	unlock_logger(myLogger);
}
