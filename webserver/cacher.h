#ifndef CACHER_H
#define CACHER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/mman.h>


#define RESET  "\x1B[0m"
#define GREEN   "\x1B[32m"

#define SIZE_HASH_TABLE 4
#define SIZE_RAM_CACHE 4

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


//int n = 2; /* variabile per fare alcuni test */

struct hashNode { 	/* un hashNode rappresenta un file presente nell'hard disk */

	char name[300];
	struct hashNode * next;
	struct ramNode * refram;
	pthread_mutex_t hashLock;
	int count; //how many threads are using it?
};

struct ramNode{ 	/* un ramNode rappresenta un file caricato in ram */

	char name[300];
	struct ramNode * next;
	struct ramNode * prev;
	struct hashNode * refhash;
	int len;
	char * m;
};

struct lruTable{	/* la tabella LRU contiene tutti i ramNode, ovvero tutti i file più utilizzati */

	int count;
	struct ramNode * front;
	struct ramNode * rear;

};

struct cache { 		/* la cache è formata dalla tabella HASH e dalla tabella LRU */

	struct hashNode ** ht;
	struct lruTable * lt;
	pthread_mutex_t cmutex;

};

struct cache * web_cache;

char * obtain_file(struct cache * web_cache, char * path, char * ext, int x, int y, int q, int * size);
void releaseFile(struct cache * myCache, char * path, char * ext, int x, int y, int q);
int compute_full_name(char * full_name, char * path, char * ext, int width, int height, int q);
struct hashNode ** create_hashTable(void);
struct hashNode * alloc_hashNode(void);
struct hashNode * create_hashNode(char *);
int get_hashValue(char *);
void visit_hashLine(struct hashNode **, int);
struct hashNode * get_hashNode(struct hashNode **, char *);		/* PROTOTIPI DELLE FUNZIONI */
int insert_hashNode(struct hashNode **, struct hashNode *);
struct ramNode * alloc_ramNode(void);
struct ramNode * create_ramNode(char *name, char * m, int size);
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
char * insertFile(struct cache * myCache, char * name, int * size);
void summary_cache(struct cache *);
void summary_cache2(struct cache * myCache);
void clearScreen();


#endif /* _CACHER_H_ */
