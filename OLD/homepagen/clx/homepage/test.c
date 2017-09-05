/*
#include "hconvert.h"
#include <time.h>
#include <dirent.h>
#include <unistd.h>
int main()
{

	int x;
	char ss[300];
	for (;;)
	{
		
		DIR *d;
		struct dirent *dir;
		d = opendir("./res/");
		if (d) while ((dir = readdir(d)) != NULL)
		{
			char *s = dir->d_name;
			if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
				{
					//printf("\n%s",s);
					strcpy(ss, "./res/");
					strcat(ss, s);
					srand(time(NULL) * clock());
					x = rand() % 50 + 5001;
					//printf("\n%s",ss);
					//printf("\n%d %d", getiwidth(ss), getiheight(ss));

					printf("\n%s %d x %s %d %s %s", getcwidth(ss), getiwidth(ss), getcheight(ss), getiheight(ss), ss, remove_ext(ss, '.', '/'));

					//printf("\n%s %d x %s %d %s %s", "", getiwidth(ss), "", getiheight(ss), ss, remove_ext(ss, '.', '/'));
					int i = rand() % 256;
					//nConvert("./res", "./res", s, "xyz", "999x888", i, 0);
					char resl[20];
					snprintf(resl, 20, "%dx%d", rand()%500+300, rand()%500+300);
					nConvert("./res", "./res", s, "xyz", resl, i, 0);
					//free(**ss);


					//printf("\n%s\t%s", ss, getcresol(ss));
				}
			
		}
	//sleep(1);
		closedir(d);
	}

	return 0;
}	
	*/			
		
		
