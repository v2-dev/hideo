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
#include "convertweb.h"
#include "CACHER.h"
	
struct cache * mycache;

/* calcola nome completo del file, comprese tutte le directory, ritorna -1 se errore */
int compute_full_name(char * full_name, char * path, char * ext, int width, int height, int q){ 

	char name[50];
	strcpy(name, path);
	char * tempStr = strrchr(name, '/');
	
	if (tempStr == NULL){
		fprintf(stderr, "Formato nome file errato(1)\n");
		return -1;
	}

	tempStr++;
	
	char * tempStrTwo =  strrchr(name, '.');
	if (tempStrTwo == NULL){
		fprintf(stderr, "Formato nome file errato(2)\n");
		return -1;
	}
	
	tempStrTwo[0] = '\0';

	char quality[5];
	char x[5];
	char y[5];
	sprintf(y, "%d", height);
	sprintf(x, "%d", width);
	sprintf(quality, "%d", q);

	strcpy(full_name, "cache/");
	strcat(full_name, tempStr);
	strcat(full_name, "/");
	strcat(full_name, x);
	strcat(full_name, "/");
	strcat(full_name, y);
	strcat(full_name, "/");
	strcat(full_name, quality);
	strcat(full_name, "/");
	strcat(full_name, tempStr);
	strcat(full_name, ".");
	strcat(full_name, ext);

	return 0;
}




/* ritorna il file descriptor del file (-1 se errore) */
int obtain_file(char * path, char * ext, int x, int y, int q){

	char * full_name = malloc(220 * sizeof(char));
	int fd;
	
	if (full_name == NULL){
		fprintf(stderr, "Error in malloc()\n");
		exit(EXIT_FAILURE);
	}

	/* calcola il nome del file completo, cioe con tutte le directory */
	if (compute_full_name(full_name, path, ext, x, y, q) == -1){
		free(full_name);
		return -1;
	}
	
	/* se il file è nella cache, ritorna il suo fileDescriptor */
	fd = getFile(mycache, full_name);
	if (fd != -1) return fd;
	
	/* il file non c'è, bisogna convertirlo dall'originale */
	file_convert(path, ext, x, y, q);
	printf("fullname: %s\n", full_name);
	fd = open(full_name, O_RDWR);
	if (fd ==-1){
		fprintf(stderr, "Error with open()\n");
		exit(EXIT_FAILURE);
	}
	
	/* inserisci il file convertito nella cache */
	insertFile(mycache, full_name, fd);
	
	free(full_name);
	
	return fd;
	
}

int main(){
	
	
	char * path[] = { "res/DonatoFidato.jpg", "res/DonatoFidato.jpg", "res/DonatoFidato.jpg", "res/DoppiaD.jpg"};
	int x[] = {50,100,150,200};
	int y[] = {200,500,24,100};
	char *ext[] = {"png","jpg","jpg","jpg"};
	int q[] = {20,10,50,60}; 
	
	mycache = create_cache();
	int fd;
	for(int i = 0;i< 4;i++)  fd = obtain_file(path[i], ext[i], x[i], y[i], q[i]);
	
	


}
