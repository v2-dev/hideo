#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Server TCP pre-threaded with dynamic pool size
  accept() on main, worker thread with condition and mutex
*/

#define PERC_MIN_FREE		0.20
#define PERC_MAX_FREE		0.50
#define START_POOL_SIZE		2

pthread_mutex_t pool_size_mutex;
pthread_mutex_t pool_mutex;
pthread_cond_t pool_cond;
int pool_size;
int pool_free;

int				listensd, nthreads;
socklen_t		addrlen;
pthread_mutex_t	mtx;

struct list {
	int size;
	struct node_t *list_head;
	struct node_t *list_tail;
};

struct node_t {
	void *value;
	struct node_t *next;
};

struct Thread{
  int connsock;
  pthread_t		thread_tid;		/* thread ID */
  long			thread_count;	/* # connections handled */
};

struct Thread	*tptr;		/* array of Thread structures; calloc'ed */
struct list list_sock;


struct node_t *alloc_node();
void insert_tail(void *v, struct list *l);
void *remove_head(struct list *l);

void check_pool_size();

void lock(pthread_mutex_t *);
void unlock(pthread_mutex_t *);
void thread_make(int );
void millisleep(int milliseconds);

#endif
