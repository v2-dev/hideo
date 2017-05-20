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

#define TIME_SLEEP_GARBADGE 15

/* logger con struttura dati piu complicata */


struct logNode {	/* rappresenta un messaggio che deve essere scritto nel file di log*/
	
	char *text;
	struct logNode * next;
	struct logNode * prev;
	
};

struct logger {		/* è una struttura che contiene una coda di logNode */
	
	int count;
	struct logNode * tail;
	struct logNode * head;
	pthread_mutex_t lmutex;
	FILE *f;

};

struct logNode * alloc_logNode(void);
struct logNode * create_logNode(char *);
struct logger * alloc_logger(void);
void destroy_logNode(struct logNode *);
struct logger * create_logger(char *);
void insert_logNode(struct logger *, struct logNode *);
void delete_logNode(struct logger *, struct logNode *);
void summary_logger(struct logger *);
void empty_logger(struct logger *);
void * example(void *);
void * garbadge (void *);

struct logNode * alloc_logNode(void){	/* alloca la memoria per un logNode */
	
	struct logNode * ln; 
	ln = malloc(sizeof(struct logNode));
	if (ln == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}

	return ln;
}

struct logNode * create_logNode(char * text){	/* crea un logNode con messaggio uguale alla stringa text*/

	struct logNode * ln;
	
	ln = alloc_logNode();
	
	ln->text = calloc(100, sizeof(char)); 
	ln->next = NULL;
	ln->prev = NULL;
	
	strcpy(ln->text, text);

	return ln;
}

struct logger * alloc_logger(void){	/* alloca la memoria per un logger */
	
	struct logger * myLogger; 
	myLogger = malloc(sizeof(struct logger));
	if (myLogger == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}

	return myLogger;
}


void destroy_logNode(struct logNode * ln){	/* libera la memoria di un logNode */

	free(ln->text);
	free(ln);
}

struct logger * create_logger(char * pathLog){	/* crea un logger che scrive nel file di log del percorso indicato da pathLog */
	
	struct logger * myLogger = alloc_logger();
	
	myLogger->tail = create_logNode("theTail");
	myLogger->head = create_logNode("theHead");
	
	myLogger->tail->prev = NULL;
	myLogger->tail->next = myLogger->head;
	
	myLogger->head->next = NULL;
	myLogger->head->prev = myLogger->tail;

	myLogger->count = 0;
	
	myLogger->f = fopen(pathLog, "w");

	pthread_mutex_init(&(myLogger->lmutex), NULL); //inizializzo il mutex

	return myLogger;
	
}

void insert_logNode(struct logger * myLogger, struct logNode * ln){	/* inserisce un nuovo logNode nel logger */

	ln->prev = myLogger->tail;
	ln->next = myLogger->tail->next;
	
	myLogger->tail->next->prev = ln;
	myLogger->tail->next = ln;
	
	(myLogger->count)++;
}

void delete_logNode(struct logger * myLogger, struct logNode * ln){	/* elimina un logNode dal logger */

	ln->prev->next = ln->next;
	ln->next->prev = ln->prev;
	
	(myLogger->count)--;

	destroy_logNode(ln);

}

void summary_logger(struct logger * myLogger){	/*stampa il testo di tutti i logNode presenti nel logger */

	struct logNode * temp_logNode = myLogger->tail->next;
	
	while (temp_logNode != myLogger->head){
	
		printf("%s\n", temp_logNode->text);
		temp_logNode = temp_logNode->next;
	}
}


void empty_logger(struct logger * myLogger){	/* svuota il logger, scrivendo tutti i messaggi svuotati nel file di log dell'hard disk */
	
	while(myLogger->tail->next != myLogger->head){
		
		//printf("Eliminato: %s\n", myLogger->head->prev->text);
		fprintf(myLogger->f, "%s\n", myLogger->head->prev->text);	/* viene scritto quel messaggio nel file di log */
		delete_logNode(myLogger, myLogger->head->prev);
	}
	
	fflush(myLogger->f);	/* scrittura reale dei messaggi di log nel file di log dell'hard disk */
}

void * example(void *p){
	
	struct logger * myLogger = (struct logger *) p;
	
	while(1){
	
		int string_length = 20;
		char randomString[string_length];	/* GENERAZIONE CASUALE DI UNA STRINGA randomString di lunghezza 20 caratteri */
		srand(time(NULL));
		for( int i = 0; i < string_length; ++i){
    			randomString[i] = '0' + rand()%72; 
		}
		
		
		pthread_mutex_lock(&(myLogger->lmutex)); /* acquisizione mutex */
		
		struct logNode * ln = create_logNode(randomString);	/* crea il logNode e lo inserisce nel logger */
		insert_logNode(myLogger, ln);
	
		pthread_mutex_unlock(&(myLogger->lmutex)); /* rilascio mutex */
		
		sleep(1);
	}
}

void * garbadge (void *p){

	struct logger * myLogger = (struct logger *) p;

	while(1){
	
		sleep(TIME_SLEEP_GARBADGE);
		
		pthread_mutex_lock(&(myLogger->lmutex)); 		
		empty_logger(myLogger);				/* prendo mutex, svuoto il logger, rilascio mutex */
		pthread_mutex_unlock(&(myLogger->lmutex));

	}
}

int main(){

	struct logger * myLogger = create_logger("fileDiLog");

	/* i seguenti thread generano continuamente stringhe casuali da scrivere nel file di log */
	pthread_t t1;
	pthread_create(&t1,NULL, example,myLogger); 
	sleep(3); /* aspetto 3 secondi se no mi generano le stesse stringhe casuali */
	
	pthread_t t2;
	pthread_create(&t2,NULL, example,myLogger); 
	sleep(3); /* aspetto 3 secondi se no mi generano le stesse stringhe casuali */

	pthread_t t3;
	pthread_create(&t3,NULL, example,myLogger); 

	/* questo thread svuota il logger, va a dormire per un tempo prefissato, e risvuota il buffer nuovamente */
	pthread_t garb;
	pthread_create(&(garb),NULL, garbadge,myLogger); 
	
	sleep(500);
	return 0;
	
}
