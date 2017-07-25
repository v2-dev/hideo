#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/syscall.h> 

#define _GNU_SOURCE

#define ERR	0x01
#define	WRN	0x02
#define NFO	0x03

struct logNode {	/* rappresenta un messaggio che deve essere scritto nel file di log*/
	
	char *text;
	struct logNode * next;
	struct logNode * prev;
	int type;	//ERR, WRN, NFO
	int tid;
	
};

struct logger{ 		/* è una struttura che contiene una coda di logNode */

	int log_lvl;
	int log_fd;

	struct logNode * tail;
	struct logNode * head;

	pthread_mutex_t log_mtx;
	pthread_cond_t log_cond;
		
	pthread_t tid_garbadge;

	int printOnScreen;
};

struct logger * srvlog;

void toLog(int type, struct logger * myLogger, char * buf, ...);
struct logger * create_logger(char * pathLog, int log_lvl, int printOnScreen);
