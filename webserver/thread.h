#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Server TCP pre-threaded
  accept() on worker thread with mutex
*/

struct Thread{
  int connsock;
  pthread_t		thread_tid;		/* thread ID */
  long			thread_count;	/* # connections handled */
};

struct Thread	*tptr;		/* array of Thread structures; calloc'ed */

int				listensd, nthreads;
socklen_t		addrlen;
pthread_mutex_t	mtx;

void lock(pthread_mutex_t *);
void unlock(pthread_mutex_t *);
void thread_make(int );
void millisleep(int milliseconds);

#endif
