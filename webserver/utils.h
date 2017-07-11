#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

//#define BACKLOG 256
#define CONFIG_FILE "server.cfg"
#define	MAXLINE			4096

typedef struct {
	char port[MAXLINE];
	char threads[MAXLINE];
	char backlog[MAXLINE];
} Config;
Config config_file;

/*Error handlers Wrappers*/
void unix_error(char *msg);
void err_exit(const char *errmsg, int errnum);
void print_err_msg(const char *errmsg);
void printerr_and_exit(const char *errmsg);

/*IO functions Wrappers*/
size_t recv_msg(int fd, char *usr_buff, size_t nbytes);
size_t send_msg(int fd, char *buff);
ssize_t readn(int fd, void *buf, size_t n);
ssize_t	writen(int fd, const void *buf, size_t n);

/* Memory mapping wrappers */
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length);

/* Dynamic storage allocation wrappers */
void *Malloc(size_t size);
void *Realloc(void *ptr, size_t size);
void *Calloc(size_t nmemb, size_t size);
void Free(void *ptr);

int match(const char *, const char *);
void usage(char *);

/*configuration file functions*/
void init_parameters();
char *trim(char *);
void parse_config();

#endif				/*UTILS_H */
