#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hconvert.h"

/*

	 NB: Il programma deve essere eseguito nella directory contenente la cartella res
		(res è la cartella contente le immagini originali)


*/

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


void file_convert(char * path, char * ext, int width, int height, int q){


	char res[10];
	char name[50];
	file_extension(path, res,name);

	char name_alt[50];
	strcpy(name_alt, name);
	file_name(name);
	printf("%s\n", res);
	printf("new file  is %s\n", name);

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

	strncpy(resolution, x, strlen(x)+1);
	strncat(resolution, "x", 1);
	strncat(resolution, y, strlen(y));

	printf("la %s %s la\n", x, y);
	printf("%s\n", resolution);

	strcpy(destination, "cache/");
	strcat(destination, name);
	strcat(destination, "/");
	strcat(destination, x);
	strcat(destination, "/");
	strcat(destination, y);
	strcat(destination, "/");
	strcat(destination, quality);
	fprintf(stdout, "%s\n", destination);

	int len2 = strlen(destination) + strlen("mkdir -p ") + 5;
	char * create_folder_command = malloc(len2*sizeof(char));

	strncpy(create_folder_command, "mkdir -p ", strlen("mkdir -p ")+1);
	strncat(create_folder_command, destination, strlen(destination));
	printf("%s\n", create_folder_command);
	system(create_folder_command);


 	nConvert(res, destination, name_alt, ext, resolution, 0, q);


}


/*int main(){


	char path[] = "res/a.jpg";
	int x = 800;
	int y = 600;
	char * ext = "PNG";
	int q = 50;

	file_convert(path, ext, x, y, q);

	return EXIT_SUCCESS;

}*/
