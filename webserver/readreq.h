#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>

#define BUFSIZE 4096

struct httpread
{
	int dimArray;
	char **array;
};

void * create_httpread()
{
	struct httpread * httpr;
	httpr = malloc(sizeof(struct httpread));

	if (httpr == NULL)
	{
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	httpr->dimArray = 0;
	httpr->array = malloc(sizeof(char *));
	return httpr;
}

void destroy_httpread(struct httpread * httpr)
{
	int i;
	for (i = 0; i < httpr->dimArray; i++)
	{
			free(*(httpr->array+i));
	}
	free(httpr->array);
	free(httpr);
}


char * read_string(int fd)
{
	int n = 0;
	int size = 0;

	char *tmpString = malloc(sizeof(char) * BUFSIZE);
	if(tmpString == NULL){
		fprintf(stderr, "error on malloc\n");
		return NULL;
	}

	n = readn(fd, tmpString, BUFSIZE);
	if (n==-1)
		return NULL;

	size += n;
	tmpString[size -1] = '\0';

	return tmpString;

}


struct httpread * read_request(int fd)
{
	struct httpread * httpr = create_httpread();
	char **tmpArray = httpr->array;
	int n = 0;
	int index = 0;
	char *tmpString = malloc(sizeof(char)*300);


	while(1)
	{
		//------------
		n = readn(fd, tmpString+index, 1);
		if (n==-1)
		{
			httpr->dimArray = -1;
			break;
		}
		if (n==0)
			break;

		if (index==1)
		{
			if ((*(tmpString+index) == '\n')&&(*(tmpString+index-1) == '\r')) break;
		}

		if (*(tmpString+index) == '\n') {

			if (*(tmpString+index-1) != '\r')
			{
				//free(tmpString);
				//destroy_httpread(httpr);
				break;
				//return NULL;
			}

			*(tmpString+index) = '\0';
			*(tmpString+index-1) = '\0';
			*(tmpArray) = tmpString; //copio
			(httpr->dimArray)++;
			httpr->array = realloc(httpr->array, sizeof(char *)*(httpr->dimArray+1));
			tmpArray = httpr->array + httpr->dimArray;
			tmpString = malloc(sizeof(char)*300);
			index = 0;
		}

		else index++;



	}

	free(tmpString);
	return httpr;

}
