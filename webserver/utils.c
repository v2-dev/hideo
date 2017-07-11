#include "utils.h"

void err_exit(const char *errmsg, int errnum)
{
	fprintf(stderr, "%s\n", errmsg);
	fprintf(stderr, "%s\n", strerror(errnum));
	exit(EXIT_FAILURE);
}

void unix_error(char *msg) /* unix-style error */
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

/*************************************
 WARNING message not necessary error
 *************************************/

void print_err_msg(const char *errmsg)
{
	fprintf(stderr, "%s\n", errmsg);
}

/* Fatal error unreletad to a system call.
	Print error message and return
*/
void printerr_and_exit(const char *errmsg)
{
	fprintf(stderr, "%s\n", errmsg);
	exit(EXIT_FAILURE);
}


/***************************************************
 * Wrappers for dynamic storage allocation functions
 ***************************************************/

void *Malloc(size_t size)
{
    void *p;

    if ((p  = malloc(size)) == NULL)
	unix_error("Malloc error");
    return p;
}

void *Realloc(void *ptr, size_t size)
{
    void *p;

    if ((p  = realloc(ptr, size)) == NULL)
	unix_error("Realloc error");
    return p;
}

void *Calloc(size_t nmemb, size_t size)
{
    void *p;

    if ((p = calloc(nmemb, size)) == NULL)
	unix_error("Calloc error");
    return p;
}

void Free(void *ptr)
{
    free(ptr);
}

/***************************************
 * Wrappers for memory mapping functions
 ***************************************/
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    void *ptr;

    if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
	unix_error("mmap error");
    return(ptr);
}

void Munmap(void *start, size_t length)
{
    if (munmap(start, length) < 0)
	unix_error("munmap error");
}

size_t recv_msg(int fd, char *usr_buff, size_t nbytes)
{
	size_t nleft = nbytes;
	ssize_t nrecv;
	char *recv_buff = usr_buff;

	while (nleft > 0) {
		if ((nrecv = recv(fd, recv_buff, nleft, 0)) < 0) {
			if (errno == EINTR)
				nrecv = 0;
			else
				err_exit("recv() error", errno);
		} else if ((nrecv == 0) || strstr(recv_buff, "\r\n"))	/*no things to read or carriage return */
			break;
		nleft -= nrecv;
		recv_buff += nrecv;
	}

	return nbytes - nleft;

}

int match(const char *s1, const char *s2)
{
	size_t n;
	int retval;

	n = strlen(s1);
	retval = strncmp(s1, s2, n);

	return retval;
}

int recv_line(int sockfd, char *buf, int size)	//on basic.c
{
	int i = 0;
	char c = '\0';
	int n;

	while ((i < size - 1) && (c != '\n')) {
		n = recv(sockfd, &c, 1, 0);
		if (n > 0) {
			if (c == '\r') {
				n = recv(sockfd, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n'))
					recv(sockfd, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		} else
			c = '\n';
	}
	buf[i] = '\0';
	return i;
}

/* send msg function */
size_t send_msg(int fd, char *buff)
{
	size_t nleft = strlen(buff);
	ssize_t nsend = 0;

	/* send content */
	printf("Bytes to send: %d\n", (int) nleft);
	while (nleft > 0) {
		if ((nsend = send(fd, buff, nleft, 0)) <= 0) {
			if (nsend < 0 && errno == EINTR)
				nsend = 0;
			else
				err_exit("error with send()", errno);
		}
		nleft -= nsend;
		buff += nsend;
	}
	if (nleft == 0)
		printf("Send success!!\n");
	return nleft;
}

ssize_t readn(int fd, void *buf, size_t n)
{
  size_t  nleft;
  ssize_t nread;
  char *ptr;

  ptr = buf;
  nleft = n;
  while (nleft > 0) {
    if ((nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR)
        nread = 0;
      else
        return(-1);
    }
    else if (nread == 0)
      break;	/* EOF */

    nleft -= nread;
    ptr += nread;
  }
  return(n-nleft);	/* restituisce >= 0 */
}

/******/
ssize_t writen(int fd, const void *buf, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = buf;
  nleft = n;
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
       if ((nwritten < 0) && (errno == EINTR)) nwritten = 0;
       else return(-1);	    /* errore */
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return(n-nleft);	/* restituisce >= 0 */
}
