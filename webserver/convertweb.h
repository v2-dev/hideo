#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hconvert.h"

/****************************************************************************
	The program must be executed in the directory containing "res" dyrectory
*****************************************************************************/

/* separa il nome della cartella dal nome del file */
void file_extension(char * path, char * res, char * name){

	int len = strlen(path);
	int nbyte_directory = 0;
	char * last_slash;

	for(int i = 0; i<len; i++){
		if (path[i] == '/') last_slash = path+i;
	}

	strcpy(name, last_slash+1);
	int len_name = strlen(name);
	strncpy(res, path, len - len_name -1);
	res[len-len_name-1] = '\0';
}


/* toglie l'estensione dal nome del file */
void file_name(char *filename)
{
	int i;
	int len = strlen(filename);
	char *last_slash;

	for(i = 0; i<len; i++){
		if (filename[i] == '.'){
			filename[i] = '\0';
			break;
		}
	}
}

/* converte l'immagine */
void file_convert(char * path, char * ext, int width, int height, int q){

	char res[10];
	char name[50];
	file_extension(path, res,name);

	char name_alt[50]; /* nome del file senza estensione */
	strcpy(name_alt, name);
	file_name(name);

	char quality[5];
	char x[5];
	char y[5];
	char resolution[15];
	char *destination;
	int len;

	len = strlen(quality) + strlen(y) + strlen(x) + strlen(name) + strlen("cache") + 10;

	destination = malloc(len * sizeof(char));
	if(destination == NULL){
		perror("");
		exit(EXIT_FAILURE);
	}

	sprintf(y, "%d", height);
	sprintf(x, "%d", width);
	sprintf(quality, "%d", q);

	/* 1280x720 */
	strncpy(resolution, x, strlen(x)+1);
	strncat(resolution, "x", 1);
	strncat(resolution, y, strlen(y));

	strcpy(destination, "cache/");
	strcat(destination, name);
	strcat(destination, "/");
	strcat(destination, x);
	strcat(destination, "/");
	strcat(destination, y);
	strcat(destination, "/");
	strcat(destination, quality);

	/* fino a qua ci sta la costruzione di destination*/

	int len2 = strlen(destination) + strlen("mkdir -p ") + 5;
	char * create_folder_command = malloc(len2*sizeof(char));

	strncpy(create_folder_command, "mkdir -p ", strlen("mkdir -p ")+1);
	strncat(create_folder_command, destination, strlen(destination));
	system(create_folder_command);

	nConvert(res, destination, name_alt, ext, resolution, 0, q);

	free(destination);
	free(create_folder_command);
}
