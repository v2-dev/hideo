//#include "libcacher.h"
#include "CACHER.h"
	char filename[300];

void printmt (char **tbl, int sizetbl)
{
	for (int i=0; i<sizetbl; i++)
	{
		printf("\n%s", tbl[i]);
	}
}

void isfile (char *filename, struct cache *myCache)
{
	int fd = getFile(myCache, filename); /* vedo se il file che cerco è presente in cache, la funzione ritorna il fileDescriptor */
	printf("\n%s: %d", filename, fd);
	if (fd == -1)
	{ 
		fd = open(filename, O_RDONLY, 0); /* apro il file appena convertito */
		printf("\nINSERISCO");
		insertFile(myCache, filename, fd); /* inserisco il file in cache */
		printf("\nnew %s: %d", filename, fd);
		if (fd == -1) exit(EXIT_FAILURE);
	}
}
int main(){

	
	struct cache * myCache = create_cache(); /* creazione cache */

	int fd;
	int x = 1;
	int y;
	
while(1)
{
	//int val=0;
	DIR *d;
	struct dirent *dir;
	d = opendir("./res/");
	//d = opendir(respath);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
    {
	char *s = dir->d_name;
	if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
	{
		char filepath[300];
		strcpy(filepath, "./res/");
		strcat(filepath, s);
		printf("\n%s", filepath);
		isfile(filepath, myCache);

		}
	}

	}
	closedir(d);
	printf("\n");
	//sleep(1);
	clearScreen();
	summary_cache2(myCache);
	x = 0;
}

/*	
	fd = open("./res/1.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/1.jpg", fd);
	fd = open("./res/2.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/2.jpg", fd);
	fd = open("./res/3.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/3.jpg", fd);
	fd = open("./res/4.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/4.jpg", fd);
	fd = open("./res/5.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/5.jpg", fd);
	fd = open("./res/6.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/6.jpg", fd);
	fd = open("./res/7.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/7.jpg", fd);
	fd = open("./res/8.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/8.jpg", fd);
	fd = open("./res/9.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/9.jpg", fd);
	fd = open("./res/10.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/10.jpg", fd);
*/
char * vector[] = {"./res/1.jpg", "./res/2.jpg", "./res/3.jpg", "./res/4.jpg", "./res/5.jpg", "./res/6.jpg"};
while (0)
{
	srand(clock() * time(NULL));
	int r = rand() % 6;	
	char filename[300];
	
	summary_cache2(myCache);
	printmt(vector, 6);
	sleep(1);
	
	strcpy(filename, vector[r]);
	isfile(filename, myCache);
	
	summary_cache2(myCache);
	printmt(vector, 6);
	sleep(2);
}

for (int ii = 0; ii<6; ii++) {
	printf("\n-1-%s\t%s", filename, vector[ii]);
	strncpy(filename, vector[ii], 300);
	printf("\n-2-%s\t%s", filename, vector[ii]);
	isfile(filename, myCache);
	printf("\n-3-%s\t%s\n", filename, vector[ii]);
	
	};


printf("\nPUPPA");
printmt(vector, 6);
printf("\nPUPPA");
summary_cache2(myCache);
printf("\nPUPPA");
printmt(vector, 6);
printf("\nPUPPA");
/*
isfile(vector[0], myCache);
isfile(vector[1], myCache);
isfile(vector[2], myCache);
isfile(vector[3], myCache);
isfile(vector[4], myCache);
isfile(vector[5], myCache);
*/

/*	fd = open("./res/7.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/7.jpg", fd);
	fd = open("./res/8.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/8.jpg", fd);
	fd = open("./res/9.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/9.jpg", fd);
	fd = open("./res/10.jpg", O_RDONLY, 0); 
	insertFile(myCache, "./res/10.jpg", fd);
*/	
while (0)
{
	srand(clock() * time(NULL));
	int r = rand() % 20;
	char rc[200] = "";
	snprintf(rc, 10, "%d.jpg", r);
	char filename[300] = "";
	strcpy(filename, "./res/");
	strcat(filename, rc);
	printf("\n%s", filename);

/*
	DIR *d;
	struct dirent *dir;
	d = opendir("./res/");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
		char *s = dir->d_name;
		if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
		{
*/
	fd = open(filename, O_RDONLY, 0);
	if ( fd != -1 )
	{
	fd = getFile(myCache, filename); /* vedo se il file che cerco è presente in cache, la funzione ritorna il fileDescriptor */
		if (fd == -1)
		{ 
			fd = open(filename, O_RDONLY, 0); /* apro il file appena convertito */
			insertFile(myCache, filename, fd); /* inserisco il file in cache */
		}
	}
	//summary_cache2(myCache);
	sleep(1);
}
//summary_cache2(myCache);
	return 0;

}
