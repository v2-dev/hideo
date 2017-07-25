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

#define ERR	0x01
#define	WRN	0x02
#define NFO	0x03

/* logger senza una struttura dati troppo complicata */

struct logger {

	FILE *f;
	int loglvl;

};

pthread_mutex_t log_mutex;
pthread_cond_t log_cond;

struct logger * srvlog;


struct logger * alloc_logger(void);
struct logger * create_logger(char *, int);	/* PROTOTIPI DELLE FUNZIONI */
void toLog(int msgt, char * msg, struct logger * myLogger);
