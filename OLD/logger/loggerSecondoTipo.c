#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>

#define SIZE_TO_FFLUSH 30

/* logger senza una struttura dati troppo complicata */

struct logger {

	FILE *f;
	pthread_mutex_t lmutex;
	
};


struct logger * alloc_logger(void);
struct logger * create_logger(char *);	/* PROTOTIPI DELLE FUNZIONI */
void * example(void *);

struct logger * alloc_logger(void){	/* alloca la memoria per un logger */
	
	struct logger * myLogger; 
	myLogger = malloc(sizeof(struct logger));
	if (myLogger == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}

	return myLogger;
}

struct logger * create_logger(char * logPath){	/* crea un nuovo logger, che scriverà nel file di log indicato da logPath */
		
	struct logger * myLogger = alloc_logger();
	
	myLogger->f = fopen(logPath, "w+");
	
	pthread_mutex_init(&(myLogger->lmutex), NULL); //inizializzo il mutex
	
	return myLogger;
}


void * example(void *p){
	
	struct logger * myLogger = (struct logger *) p;
	
	while(1){
	
		int string_length = 20;
		char randomString[string_length];		/* GENERAZIONE CASUALE DI UNA STRINGA randomString di lunghezza 20 caratteri */
		srand(time(NULL));
		for( int i = 0; i < string_length; ++i){
    			randomString[i] = '0' + rand()%72; 
		}
		
		
		pthread_mutex_lock(&(myLogger->lmutex)); /* acquisisco mutex */
		
		fprintf(myLogger->f, "%s\n", randomString); /* scrivo il messaggio randomString nel file di log
								(il sistema operativo lo scriverà solo in ram per ora, in una coda da lui 									gestita) */
	
		pthread_mutex_unlock(&(myLogger->lmutex)); /* rilascio mutex */
		
		sleep(1);
	
	}
}
	
	
int main(){

	struct logger * myLogger = create_logger("fileDiLog");
	
	/* i seguenti thread generano continuamente stringhe casuali da scrivere nel file di log */
	pthread_t t1;
	pthread_create(&t1,NULL, example,myLogger);
	sleep(5);

	pthread_t t2;
	pthread_create(&t2,NULL, example,myLogger);
	sleep(7);
	
	pthread_t t3;
	pthread_create(&t3,NULL, example,myLogger);

	pthread_exit(0); /* il main dorme per 500 secondi, perche se muore lui si chiudono tutti i thread */
	return 0;
}
