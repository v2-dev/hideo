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
#define	MAXLINE			4096
#define BUFSIZE			4096
#define ERROR				-1
#define SUCCESS      0


struct list {
	int size;
	struct node_t *list_head;
	struct node_t *list_tail;
};

struct node_t {
	void *value;
	struct node_t *next;
};

struct list list_sock;

struct node_t *alloc_node();
void insert_tail(void *v, struct list *l);
void *remove_head(struct list *l);

/*Error handlers Wrappers*/
void unix_error(char *msg);
void err_exit(const char *errmsg, int errnum);
void print_err_msg(const char *errmsg);
void printerr_and_exit(const char *errmsg);

/*IO functions Wrappers*/
size_t recv_msg(int fd, char *usr_buff, size_t nbytes);
int recv_line(int sockfd, char *buf, int size);
size_t send_msg(int fd, char *buff);
ssize_t readn(int fd, void *buf, size_t n);
ssize_t writen(int fd, const void *buf, size_t n);
ssize_t Readline(int sockd, void *vptr, ssize_t maxlen);
ssize_t Writeline(int sockd, const void *vptr, ssize_t n);

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


#endif				/*UTILS_H */
