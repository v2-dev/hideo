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
#include <sys/types.h>
#include <sys/syscall.h>

#define _GNU_SOURCE

#define SIZE_TO_FFLUSH 30
#define MSG_SIZE	4096

#define ERR	0x01
#define	WRN	0x02
#define NFO	0x03

/* logger senza una struttura dati troppo complicata */

struct logger {

	FILE *f;
	int loglvl;

};

struct logger * srvlog;
char MSG_LOG[MSG_SIZE];

pthread_mutex_t logger_mutex;
pthread_cond_t logger_cond;

pthread_mutex_t logger_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t logger_cond = PTHREAD_COND_INITIALIZER;

struct logger * alloc_logger(void);
struct logger * create_logger(char *, int);	/* PROTOTIPI DELLE FUNZIONI */

void toLog(int msgt, char * msg, struct logger * myLogger);
void *thread_logger_manager();
