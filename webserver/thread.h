#ifndef THREAD_H
#define THREAD_H

/* Server TCP pre-threaded
  accept() in worker thread con mutex
*/

struct Thread{
  int connsock;
  pthread_t		thread_tid;		/* thread ID */
  long			thread_count;	/* # connections handled */
};

struct Thread	*tptr;		/* array of Thread structures; calloc'ed */

int				listensd, nthreads;
socklen_t		addrlen;
pthread_mutex_t	mlock;

void lock(pthread_mutex_t *);
void unlock(pthread_mutex_t *);
void thread_make(int );

#endif
