#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

#define ERR 0x01		
#define	WRN	0x02
#define NFO	0x03

/*	 /-----------------------------\ 
 * 	|			Log Level			|
 * 	|								|
 * 	| Info	| Warn	| Error	| Level	|
 * 	|-------|-------|-------|-------|
 * 	|		|		|		|	0	|
 * 	|-------|-------|-------|-------|
 * 	|		|		|   *	|	1	|
 * 	|-------|-------|-------|-------|
 * 	|		|   *	|		|	2	|
 * 	|-------|-------|-------|-------|
 * 	|		|   *	|	*	|	3	|
 * 	|-------|-------|-------|-------|
 * 	|   *	|		|		|	4	|
 * 	|-------|-------|-------|-------|
 * 	|   *	|		|   *	|	5	|
 * 	|-------|-------|-------|-------|
 * 	|   *	|   *	|		|	6	|
 * 	|-------|-------|-------|-------|
 * 	|   *	|   *	|	*	|	7	|
 * 	 \-----------------------------/
 * 
 */
 
struct tlogger
{
	FILE *flog;
	pthread_mutex_t mtx;
	int level;
};


void ts_write(struct tlogger *, char *msg, int msgt);
void ts_read(struct tlogger *, char *msg, int msgt);

char* get_timestamp()
{
  time_t now = time (NULL);

  struct tm *date;
	date = localtime(&now);
    // Format the time correctly
	return asctime(date);
}

void ts_write(struct tlogger *sf, char *msg, int msgt)
{
	/*
	 * msgt		1	ERR
	 * 			2	WRN
	 * 			4	NFO
	 */
	
	errno = 0;
	if(pthread_mutex_lock(&sf->mtx) != 0){
		perror("pthread_mutex_lock");
		exit(EXIT_FAILURE);
	}
	
	if ((msgt) & (sf->level)) 			fprintf(sf->flog, "[ERR] [%s] %s", get_timestamp(), msg);
	if ((msgt >> 1) & (sf->level >> 1)) fprintf(sf->flog, "[WRN] [%s] %s", get_timestamp(), msg);
	if ((msgt >> 2) & (sf->level >> 2))	fprintf(sf->flog, "[NFO] [%s] %s", get_timestamp(), msg);
	
	if(pthread_mutex_unlock(&sf->mtx) != 0){
		perror("pthread_mutex_unlock");
		exit(EXIT_FAILURE);
	}

		
		fprintf(sf->flog, "[%s] %s", get_timestamp(), msg);
	
}

void ts_flush(struct tlogger *sf)
{
	fflush(sf->flog);
}
