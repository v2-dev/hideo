#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

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
			//printf("\n#N = -1");
			fflush(stdout);
			httpr->dimArray = -1;
			break;
		}
		if (n==0) 
		{
			//printf("\n#N = -0");
			fflush(stdout);
			break;
		}
		//printf("%c", *(tmpString+index));
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
