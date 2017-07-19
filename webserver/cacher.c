#include "cacher.h"
#include "convertweb.h"

void exit_on_error(int cond, char * msg){
	if(cond){
		fprintf(stderr, "Error in %s\n", msg);
		exit(EXIT_FAILURE);
	}
}

char * open_and_map_file(char * filename, int * size){

	int fd = open(filename, O_RDWR);
	exit_on_error(fd==-1, "open");
	
	int len = lseek(fd, 0, SEEK_END);
	exit_on_error(len==-1, "lseek");
	
	void * p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	exit_on_error(p==MAP_FAILED, "mmap");
	
	close(fd); //not critical
	
	*size = len;
	
	return (char *) p;
}

int openFittizia(){ /* serve solo per fare alcuni test velocemente */

	//n++;
	return 0;

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

	pthread_mutex_init(&(myCache->cmutex), NULL); //inizializzo il mutex

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
	lt->front = create_ramNode("theFront", NULL, 0);
	lt->rear = create_ramNode("theRear", NULL, 0);

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

	if (pthread_mutex_init(&(hn->hashLock), NULL) != 0){
		fprintf(stderr, "Error in pthread_mute_init\n");
		exit(EXIT_FAILURE);
	}

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

struct ramNode * create_ramNode(char *name, char * m, int size){ /* crea un nuovo ramNode */

	struct ramNode * rn;
	rn = alloc_ramNode();

	//rn->name = name;
	strcpy(rn->name, name);

	rn->next = NULL;
	rn->prev = NULL;
	rn->refhash = NULL;
	rn->m = m;
	rn->len = size;

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

	struct hashNode * hn = ramNodeToDel->refhash;
	
	int rc = pthread_mutex_lock(&(hn->hashLock));
	exit_on_error(rc !=0, "mutex_lock");
	
	if (hn->count > 0){
		int rc = pthread_mutex_unlock(&(hn->hashLock));
		exit_on_error(rc !=0, "mutex_unlock");
		return;
	}

	struct ramNode * ramNodeLeft = ramNodeToDel->prev;
	struct ramNode * ramNodeRight = ramNodeToDel->next; 

	ramNodeLeft->next = ramNodeRight;
	ramNodeRight->prev = ramNodeLeft;

	(lt->count)--;

	ramNodeToDel->refhash->refram = NULL; /* disassocia il ramNode con il suo hashNode*/

	printf("Eliminato dalla ram il file '%s' \n", ramNodeToDel->name);

	rc = munmap(ramNodeToDel->m, ramNodeToDel->len);
	exit_on_error(rc==-1, "munmap");

	free_ramNode(ramNodeToDel); /* libera la memoria del ramNode */
	
	rc = pthread_mutex_unlock(&(hn->hashLock));
	if (rc !=0){
		fprintf(stderr, "Error in mutex unlock\n");
		exit(EXIT_FAILURE);
	}
}

void visit_lruTable(struct lruTable * lt){	/* stampa il nome di tutti i ramNode presenti nella tabella LRU */

	struct ramNode * temp_ramNode = lt->front->next;
	struct ramNode * end_ramNode = lt->rear;

	while (temp_ramNode != end_ramNode){

		//printf("Il file '%s' è presente in ram, con fileDescriptor: %d\n", temp_ramNode->name, temp_ramNode->fd);
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
						printf("Il file '%s' è anche presente in ram con map: %p\n", temp_ramNode->name, 									temp_ramNode->m);
			temp_hashNode = temp_hashNode->next;
		}
	}
}

void releaseFile(struct cache * myCache, char * path, char * ext, int x, int y, int q){

	if (pthread_mutex_lock(&(myCache->cmutex)) != 0){ /* acquisisco mutex */
		fprintf(stderr, "Error in mutex lock\n");
		exit(EXIT_FAILURE);
	}

	char * full_name = malloc(250 * sizeof(char));

	if (full_name == NULL){
		fprintf(stderr, "Error in malloc()\n");
		exit(EXIT_FAILURE);
	}

	/* calcola il nome del file completo, cioe con tutte le directory */
	if (compute_full_name(full_name, path, ext, x, y, q) == -1){
		free(full_name);
		fprintf(stderr, "Error in compute_full_name\n");
		exit(EXIT_FAILURE);
	}

	struct hashNode * hn = get_hashNode(myCache->ht, full_name);

	if (hn==NULL){
			printf("%s\n", full_name);
			fflush(stdout);
			fprintf(stderr,"||Unexpected error hasnNode\n");
			exit(EXIT_FAILURE);
	}
	
	(hn->count)--;

	if (pthread_mutex_unlock(&(hn->hashLock)) != 0){
		fprintf(stderr, "Error in pthread_mutex_lock\n");
		exit(EXIT_FAILURE);
	}
	printf("File rilasciato\n");

	if (pthread_mutex_unlock(&(myCache->cmutex)) != 0){ /* acquisisco mutex */
		fprintf(stderr, "Error in mutex unlock\n");
		exit(EXIT_FAILURE);
	}

}

/* ritorna il file mappato, se non c'è ritorna NULL */
char * getAndLockFile(struct cache * myCache,char * stringFile, int * size){

	if (pthread_mutex_lock(&(myCache->cmutex)) != 0){ /* acquisisco mutex */
		fprintf(stderr, "Error in mutex lock\n");
		exit(EXIT_FAILURE);
	}

	printf("Hai richiesto il file '%s'. Processando...\n", stringFile);

	struct hashNode * hn = get_hashNode(myCache->ht, stringFile);

	if (hn != NULL){ /* se hn != NULL allora il file si trova come MINIMO nell'HDD */

		int rc;
		rc = pthread_mutex_lock(&(hn->hashLock));
		exit_on_error(rc != 0, "mutex_lock");
		
		if (hn->refram != NULL){ /* se la condizione è verificata allora il file è anche caricato in ram */

			printf("Il file '%s' si trova già in ram. Ti restituisco il suo map: %p\n", hn->name, hn->refram->m);
			moveOnTop_ramNode(myCache->lt, hn->refram);
				
			(hn->count)++;

			if (pthread_mutex_unlock(&(myCache->cmutex)) != 0){ /* rilascio mutex */
				fprintf(stderr, "Error in mutex unlock\n");
				exit(EXIT_FAILURE);
			}

			*size = hn->refram->len;
	
			printf("P00INTER: %p\n", hn->refram->m);

			return hn->refram->m;
		}

		else { /* il file non sta in ram, bisogna caricarlo mediante una open-mmap */

			printf("Il file '%s' è nell'HDD ma non è caricato in ram.\n", stringFile);
			printf("Caricamento del file '%s' in ram...\n", stringFile);
			
			int size;
			char * m = open_and_map_file(stringFile, &size);

			struct ramNode * rn = create_ramNode(stringFile, m, size); /* creazione e inserimento del ramNode associato a quel file */
			insert_ramNode(myCache->lt, rn, hn);

			controlSize_lruTable(myCache->lt);

			printf("Il file '%s' è stato caricato in ram. Ti restituisco il suo map: %p\n", rn->name, rn->m);

			if (pthread_mutex_unlock(&(myCache->cmutex)) != 0){ /* rilascio mutex */
				fprintf(stderr, "Error in mutex lock\n");
				exit(EXIT_FAILURE);
			}
			
			(hn->count)++;
			return m;
		}
	}

	printf("Non ho il file '%s' da te richiesto, mi dispiace! Ti restituisco un map di errore: NULL\n", stringFile);

	if (pthread_mutex_unlock(&(myCache->cmutex)) != 0){ /* rilascio mutex */
		fprintf(stderr, "Error in mutex unlock\n");
		exit(EXIT_FAILURE);
	}

	return NULL;
}

char * insertFile(struct cache * myCache, char * name, int * size){ /* inserisce un nuovo file in cache */

	pthread_mutex_lock(&(myCache->cmutex)); /* acquisisco mutex */

	struct hashNode * hn = create_hashNode(name);	/* creazione e inserimento nella tabella hash dell'hashNode relativo al file */
	insert_hashNode(myCache->ht, hn);
	
	char * m = open_and_map_file(name, size);

	struct ramNode * rn = create_ramNode(name, m, *size);	/* creazione e inserimento nella testa della tabella LRU del ramNode relativo al 													file */
	insert_ramNode(myCache->lt, rn, hn);

	//printf("Il file '%s' è stato inserito nell'HDD. E' stato anche inserito nella ram con fileDescriptor: %d \n", rn->name, rn->fd);

	controlSize_lruTable(myCache->lt);

	pthread_mutex_unlock(&(myCache->cmutex)); /* acquisisco mutex */
	
	(hn->count)++;
	
	printf("P00INTER||: %p\n", m);

	return m;
}

void clearScreen()
{
  const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
  int n = write(STDOUT_FILENO,CLEAR_SCREE_ANSI,12);
	if (n==-1){
		fprintf(stderr, "Error in write");
		exit(EXIT_FAILURE);
	}
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
						//printf("\n\tRam: %d", temp_ramNode->fd);
			temp_hashNode = temp_hashNode->next;
			printf("\n");
		}
	}

	/* visit_lruTable(myCache->lt); ulteriore test per vedere se i puntatori sono correttamente agganciati */
}


/* calcola nome completo del file, comprese tutte le directory, ritorna -1 se errore */
int compute_full_name(char * full_name, char * path, char * ext, int width, int height, int q){

	char name[50];
	strcpy(name, path);
	char * tempStr = strrchr(name, '/');

	if (tempStr == NULL){
		fprintf(stderr, "Formato nome file errato(1)\n");
		return -1;
	}

	tempStr++;

	char * tempStrTwo =  strrchr(name, '.');
	if (tempStrTwo == NULL){
		fprintf(stderr, "Formato nome file errato(2)\n");
		return -1;
	}

	tempStrTwo[0] = '\0';

	char quality[5];
	char x[5];
	char y[5];
	sprintf(y, "%d", height);
	sprintf(x, "%d", width);
	sprintf(quality, "%d", q);

	strcpy(full_name, "cache/");
	strcat(full_name, tempStr);
	strcat(full_name, "/");
	strcat(full_name, x);
	strcat(full_name, "/");
	strcat(full_name, y);
	strcat(full_name, "/");
	strcat(full_name, quality);
	strcat(full_name, "/");
	strcat(full_name, tempStr);
	strcat(full_name, ".");
	strcat(full_name, ext);

	return 0;
}




/* ritorna il file descriptor del file (-1 se errore) */
char * obtain_file(struct cache * web_cache, char * path, char * ext, int x, int y, int q, int * size){

	char * full_name = malloc(220 * sizeof(char));
	char * m;

	if (full_name == NULL){
		fprintf(stderr, "Error in malloc()\n");
		exit(EXIT_FAILURE);
	}

	/* calcola il nome del file completo, cioe con tutte le directory */
	if (compute_full_name(full_name, path, ext, x, y, q) == -1){
		free(full_name);
		return NULL;
	}

	/* se il file è nella cache, ritorna il suo fileDescriptor */
	m = getAndLockFile(web_cache, full_name, size);
	if (m != NULL) return m;

	/* il file non c'è, bisogna convertirlo dall'originale */
	file_convert(path, ext, x, y, q);
	printf("fullname: %s\n", full_name);

	m = insertFile(web_cache, full_name, size);
	if (m == NULL){
		fprintf(stderr, "Error with mmap\n");
		exit(EXIT_FAILURE);
	}
	 

	free(full_name);

	return m;

}

/*
int main(){

	char * path[] = { "/home/giorgio/Scrivania/img/DonatoFidato.jpg", "/home/giorgio/Scrivania/img/DonatoFidato.jpg",
	 "/home/giorgio/Scrivania/img/DonatoFidato.jpg", "/home/giorgio/Scrivania/img/DoppiaD.jpg"};

	int x[] = {100,100,40,70};
	int y[] = {200,500,24,100};
	char *ext[] = {"png","jpg","jpg","jpg"};
	int q[] = {20,10,50,60};

	web_cache = create_cache();

	char * m;
	int size;
	int i = 0;
	while(1){

		for(int i = 0;i< 4;i++)  {

			m = obtain_file(web_cache, path[i], ext[i], x[i], y[i], q[i], &size);
			releaseFile(web_cache, path[i], ext[i], x[i], y[i], q[i]);
			m = m;
			printf("SIZE: %d\n", size);

		}
		
		printf("\n\n PLUF PLUF %d \n\n", i);
		i++;
		sleep(2);
	
	}
}*/

