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

#define RESET  "\x1B[0m"
#define GREEN   "\x1B[32m"

#define SIZE_HASH_TABLE 4
#define SIZE_RAM_CACHE 10

/*   
						CACHE
	
	           |-------------------------------------------------------------------------------|
	           |                                                                               |
	           |   |------------------------------------------------------------------------|  |
	|-----|	   |   v                                                                        |  |
	|  0  |-->|file1|-->|file4|                                                             |  |
	|-----|							                                |  |
	|  2  |                                                                                 |  |
	|-----|	    |-------------------------------------------------------|	   LRU TABLE	|  |				
	|  3  |	    |  |--------------------------------------------------| |                   |  |
	|-----|	    |  v					          | v                   |  v
	|  4  |-->|file7|						|-----|--->|-----|--->|-----|
	|-----|							   |--->|file7|	   |file9|    |file1|<----|
	|  5  |							   |	|-----|<---|-----|<---|-----|	  |
	|-----|							   |	   |	     ^	|	 |	  | 
	|  6  |-->|file3|-->|file9|-->|file15|			|-----|	   |	     |	|	 |      |-----|
	|-----|		      ^	 |				|front|<---|         |  |        |----->|rear |
	|  7  |		      |  |				|-----|	             |  |               |-----|
	|-----|               |  |                                                   |  |
	                      |  |---------------------------------------------------|  |
      TABELLA HASH            |                                                         |
                    	      |---------------------------------------------------------|	

*/

int n = 2; /* variabile per fare alcuni test */

struct hashNode { 	/* un hashNode rappresenta un file presente nell'hard disk */
	
	char name[300];
	struct hashNode * next;
	struct ramNode * refram;

};

struct ramNode{ 	/* un ramNode rappresenta un file caricato in ram */

	char name[300];
	struct ramNode * next;
	struct ramNode * prev;
	struct hashNode * refhash;
	int fd; /* descrittore del file */

};

struct lruTable{	/* la tabella LRU contiene tutti i ramNode, ovvero tutti i file più utilizzati */

	int count;
	struct ramNode * front;
	struct ramNode * rear;
	
};

struct cache { 		/* la cache è formata dalla tabella HASH e dalla tabella LRU */
	
	struct hashNode ** ht;
	struct lruTable * lt;

};

struct hashNode ** create_hashTable(void);
struct hashNode * alloc_hashNode(void);
struct hashNode * create_hashNode(char *);
int get_hashValue(char *);
void visit_hashLine(struct hashNode **, int);
struct hashNode * get_hashNode(struct hashNode **, char *);		/* PROTOTIPI DELLE FUNZIONI */
int insert_hashNode(struct hashNode **, struct hashNode *);							
struct ramNode * alloc_ramNode(void);
struct ramNode * create_ramNode(char *, int);
void free_ramNode(struct ramNode *);
void insert_ramNode(struct lruTable *, struct ramNode *, struct hashNode *);
void delete_ramNode(struct lruTable *, struct ramNode *);
void visit_lruTable(struct lruTable *);
struct lruTable * alloc_lruTable(void);
void free_lruTable(struct lruTable * lt);
struct cache * alloc_cache(void);
void free_cache(struct cache *);
struct lruTable * create_lruTable(void);
struct cache * create_cache();
void moveOnTop_ramNode(struct lruTable * , struct ramNode * );
int getFile(struct cache *,char *);
void controlSize_lruTable(struct lruTable *);
void associate(struct hashNode *, struct ramNode *);
int insertFile(struct cache *, char *, int);
void summary_cache(struct cache *);
void summary_cache2(struct cache * myCache);
void(clearScreen);

int openFittizia(){ /* serve solo per fare alcuni test velocemente */
	
	n++;
	return n;

}

struct cache * alloc_cache(void){ 	/* alloca la memoria per una cache */

	struct cache * ce;
	ce = malloc(sizeof(struct cache));
	if (ce == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}

	return ce;
}

void free_cache(struct cache * ce){	/* libera la memoria occupata dalla cache */

	free(ce);
}

struct cache * create_cache(){		/* crea una nuova cache */

	struct cache * myCache = alloc_cache();
	
	myCache->ht = create_hashTable();
	myCache->lt = create_lruTable(); 
	
	return myCache;
}


struct lruTable * alloc_lruTable(void){ 	/* alloca la memoria per una tabella LRU */

	struct lruTable * lt;
	lt = malloc(sizeof(struct lruTable));
	if (lt == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}

	return lt;
}

void free_lruTable(struct lruTable * lt){	/* libera la memoria occupata da una tabella LRU */

	free(lt);
}


struct lruTable * create_lruTable(void){	/* crea una tabella LRU */

	struct lruTable * lt;	
	lt = alloc_lruTable();
	
	lt->count = 0;
	lt->front = create_ramNode("theFront", -1);
	lt->rear = create_ramNode("theRear", -1);
	
	lt->front->next = lt->rear;
	lt->rear->prev = lt->front;
	
	return lt;
}


struct hashNode ** create_hashTable(void){	/* crea una tabella hash */

	struct hashNode ** ht;
	
	size_t size = sizeof(struct hashNode *) * (SIZE_HASH_TABLE);
	ht = malloc(size);
	
	if (ht == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}
	
	for(int i = 0; i < SIZE_HASH_TABLE; i++) ht[i] = NULL;
		
	return ht;
}

struct hashNode * alloc_hashNode(void){		/* alloca la memoria per un hashNode */

	struct hashNode * hn;
	hn = malloc(sizeof(struct hashNode));
	if (hn == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}
	
	return hn;
}

struct hashNode * create_hashNode(char * name){		/* crea un nuovo hashNode */

	struct hashNode * hn = alloc_hashNode();
	
	//hn->name = name;
	strcpy(hn->name, name);
	hn->next = NULL;
	hn->refram = NULL;
	
	return hn;
}

int get_hashValue(char *s){ 	/* ottiene la chiave per la stringa s per la tabella hash */

	long value = 0;
	
	for (int i =0;*(s+i) != '\0'; i++)
		value = value + *(s+i);
	
	int key = (int) value % SIZE_HASH_TABLE;
	return key;
}

void visit_hashLine(struct hashNode ** ht, int key){ /* stampa i nomi degli hashNode di una determinata riga della tabella hash */
	
	struct hashNode *temp = ht[key];
	
	while (temp != NULL){
		
		printf("%s\n", temp->name);
		temp = temp->next;
	}
}

struct hashNode * get_hashNode(struct hashNode ** ht, char * name){	/* ritorna l'hashNode con il nome scelto */
	
	int key = get_hashValue(name);
	
	struct hashNode * temp = ht[key];
	
	while (temp != NULL){
		
		if (strcmp(temp->name, name)==0) break;
		else temp = temp->next;
	}
	
	return temp;

}

int insert_hashNode(struct hashNode ** ht, struct hashNode * hn){ 	/* inserisce un hashNode nella tabella hash */

	struct hashNode * hashNodeToSearch = get_hashNode(ht, hn->name);
	
	if (hashNodeToSearch != NULL) return 0; /* questo hashNode è già presente */
	
	int key = get_hashValue(hn->name);

	if (ht[key] == NULL) ht[key] = hn;
	else {
		struct hashNode * temp = ht[key];
		ht[key] = hn;
		hn->next = temp;
	}
	
	return 1;
}

struct ramNode * alloc_ramNode(void){	 /* alloca la memoria per un nuovo ramNode */

	struct ramNode * rn;
	rn = malloc(sizeof(struct ramNode));
	if (rn == NULL) { 
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);        
	}

	return rn;
}

void free_ramNode(struct ramNode * rn){	  /* libera la memoria del ramNode */

	free(rn);
}

struct ramNode * create_ramNode(char *name, int fd){ /* crea un nuovo ramNode */
	
	struct ramNode * rn;
	rn = alloc_ramNode();

	//rn->name = name;
	strcpy(rn->name, name);

	rn->next = NULL;
	rn->prev = NULL;
	rn->refhash = NULL;
	rn->fd = fd;

	return rn;
}

void insert_ramNode(struct lruTable * lt, struct ramNode * new_ramNode, struct hashNode * hn){  /* inserisce un ramNode nella tabella LRU */
	
	new_ramNode->next = lt->front->next;
	new_ramNode->prev = lt->front;
		
	lt->front->next->prev = new_ramNode;
	lt->front->next = new_ramNode;
	
	(lt->count)++;

	associate(hn, new_ramNode);  /* collega il ramNode con il rispettivo hashNode e viceversa */
	
}

void controlSize_lruTable(struct lruTable * lt){ /* se ci sono più di SIZE_RAM_CACHE istanze di ramNode, 
							elimina il ramNode utilizzato meno recentemente */
	if (lt->count > SIZE_RAM_CACHE)	{
		
		printf("Raggiunto il massimo numero di file in ram!\n");
		delete_ramNode(lt, lt->rear->prev);
	}
}

void moveOnTop_ramNode(struct lruTable * lt, struct ramNode * ramNodeToTop){ /* sposta il ramNode scelto nella testa della tabella LRU */
	
	struct ramNode * ramNodeLeft = ramNodeToTop->prev;
	struct ramNode * ramNodeRight = ramNodeToTop->next;

	ramNodeLeft->next = ramNodeRight;
	ramNodeRight->prev = ramNodeLeft;
	
	ramNodeToTop->prev = lt->front;
	ramNodeToTop->next = lt->front->next;
	
	lt->front->next->prev = ramNodeToTop;
	lt->front->next = ramNodeToTop;
}

void delete_ramNode(struct lruTable * lt, struct ramNode * ramNodeToDel){ /* elimina il ramNode scelto dalla tabella LRU */
	
	struct ramNode * ramNodeLeft = ramNodeToDel->prev;
	struct ramNode * ramNodeRight = ramNodeToDel->next;

	ramNodeLeft->next = ramNodeRight;
	ramNodeRight->prev = ramNodeLeft;
	
	(lt->count)--;
	
	ramNodeToDel->refhash->refram = NULL; /* disassocia il ramNode con il suo hashNode*/
	
	printf("Eliminato dalla ram il file '%s' e perso il suo fileDescriptor: %d \n", ramNodeToDel->name,ramNodeToDel->fd);
		
	/*   ----------->QUI BISOGNA AGGIUNGERE LA CLOSE DEL FILE, MOLTO IMPORTANTE!!!!!<-----------   */
	
	free_ramNode(ramNodeToDel); /* libera la memoria del ramNode */	
}

void visit_lruTable(struct lruTable * lt){	/* stampa il nome di tutti i ramNode presenti nella tabella LRU */

	struct ramNode * temp_ramNode = lt->front->next;
	struct ramNode * end_ramNode = lt->rear;

	while (temp_ramNode != end_ramNode){
		
		printf("Il file '%s' è presente in ram, con fileDescriptor: %d\n", temp_ramNode->name, temp_ramNode->fd);
		temp_ramNode = temp_ramNode->next;
	}
}

void associate(struct hashNode * hn, struct ramNode * rn){	/* associa il ramNode con il relativo hashNode */
	
	hn->refram = rn;
	rn->refhash = hn;
}

void summary_cache(struct cache * myCache){	/* stampa varie informazioni sullo stato della cache */
	
	printf("\n\n............RIEPILOGO STATO DELLA CACHE............\n\n");
	
	printf("In questo momento ci sono %d file caricati in ram.\n\n", myCache->lt->count);
	
	struct hashNode * temp_hashNode = NULL;
	struct ramNode * temp_ramNode = NULL;
	struct hashNode ** ht = myCache->ht;
	
	for(int i = 0; i < SIZE_HASH_TABLE; i++){
	
		temp_hashNode = ht[i];
	
		while (temp_hashNode != NULL){
			
			printf("Il file '%s' è presente nell'HDD, con key: %d\n", temp_hashNode->name, i);
			if ((temp_ramNode = temp_hashNode->refram) != NULL)
						printf("Il file '%s' è anche presente in ram, con fileDescriptor: %d\n", temp_ramNode->name, temp_ramNode->fd);
			temp_hashNode = temp_hashNode->next;
		}
	}
	
	/* visit_lruTable(myCache->lt); ulteriore test per vedere se i puntatori sono correttamente agganciati */
}

int getFile(struct cache * myCache,char * stringFile){	/* ritorna il descrittore del file con il nome scelto, se NON c'è ritorna -1 */
	
	printf("Hai richiesto il file '%s'. Processando...\n", stringFile);

	struct hashNode * hn = get_hashNode(myCache->ht, stringFile);

	if (hn != NULL){ /* se hn != NULL allora il file si trova come MINIMO nell'HDD */
		
		if (hn->refram != NULL){ /* se la condizione è verificata allora il file è anche caricato in ram */
			
			printf("Il file '%s' si trova già in ram. Ti restituisco il suo fileDescriptor: %d\n", hn->name, hn->refram->fd);
			moveOnTop_ramNode(myCache->lt, hn->refram);
			return hn->refram->fd;
		}
	
		else { /* il file non sta in ram, bisogna caricarlo mediante una open */
		
			printf("Il file '%s' è nell'HDD ma non è caricato in ram.\n", stringFile);
			printf("Caricamento del file '%s' in ram...\n", stringFile);
			
			int fd = open(stringFile, O_RDONLY, 0);
			
			struct ramNode * rn = create_ramNode(stringFile, fd); /* creazione e inserimento del ramNode associato a quel file */
			insert_ramNode(myCache->lt, rn, hn);
			
			controlSize_lruTable(myCache->lt);
		
			printf("Il file '%s' è stato caricato in ram. Ti restituisco il suo fileDescriptor: %d\n", rn->name, rn->fd);
			
			return fd;
		}
	}
	
	else {
		printf("Non ho il file '%s' da te richiesto, mi dispiace! Ti restituisco un fileDescriptor di errore: -1\n", stringFile);
		return -1;
	}
}

int insertFile(struct cache * myCache, char * name, int fd){ /* inserisce un nuovo file in cache */
	
	struct hashNode * hn = create_hashNode(name);	/* creazione e inserimento nella tabella hash dell'hashNode relativo al file */
	insert_hashNode(myCache->ht, hn);
	
	struct ramNode * rn = create_ramNode(name, fd);	/* creazione e inserimento nella testa della tabella LRU del ramNode relativo al file */	
	insert_ramNode(myCache->lt, rn, hn);
	
	printf("Il file '%s' è stato inserito nell'HDD. E' stato anche inserito nella ram con fileDescriptor: %d \n", rn->name, rn->fd);
	
	controlSize_lruTable(myCache->lt);
	
	return fd;
}

void clearScreen()
{
  const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO,CLEAR_SCREE_ANSI,12);
}


void summary_cache2(struct cache * myCache){	/* stampa varie informazioni sullo stato della cache */
	
	clearScreen();
	
	printf("\n\t[RIEPILOGO STATO DELLA CACHE]\n");	
	printf("\nIn questo momento ci sono %d file caricati in ram.", myCache->lt->count);
	
	struct hashNode * temp_hashNode = NULL;
	struct ramNode * temp_ramNode = NULL;
	struct hashNode ** ht = myCache->ht;
	
	for(int i = 0; i < SIZE_HASH_TABLE; i++){
	
		printf("\n\nKey %d", i);
		temp_hashNode = ht[i];
	
		while (temp_hashNode != NULL){
			//print_hashNode(temp_hashNode);
			printf("\n\tHash: %s", temp_hashNode->name);
			if ((temp_ramNode = temp_hashNode->refram) != NULL)
						//print_ramNode(temp_hashNode->refram);
						printf("\n\tRam: %d", temp_ramNode->fd);
			temp_hashNode = temp_hashNode->next;
			printf("\n");
		}
	}
	
	/* visit_lruTable(myCache->lt); ulteriore test per vedere se i puntatori sono correttamente agganciati */
}

