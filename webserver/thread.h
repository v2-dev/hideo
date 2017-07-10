#ifndef THREAD_H
#define THREAD_H

/*  Server TCP pre-threaded
  accept() in worker thread con mutex
*/

typedef struct {
  int connsock;
  pthread_t		thread_tid;		/* thread ID */
  long			thread_count;	/* # connections handled */
} Thread;

Thread	*tptr;		/* array of Thread structures; calloc'ed */

int				listensd, nthreads;
socklen_t		addrlen;
pthread_mutex_t	mlock;

void lock();
void unlock();
void thread_make(int );

#endif
