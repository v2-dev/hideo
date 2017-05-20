#include <stdio.h>
#include <search.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h> 
#include <unistd.h>
#include <dirent.h>

#define TBL_SIZE	5000
#define PTH_SIZE	400

char *randstring(size_t length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!/";        
    char *randomString = NULL;

    if (length) {
        randomString = malloc(sizeof(char) * (length +1));

        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }

    return randomString;
}

struct ramnode
{
	char *rpath;
	int rvalue;
	struct ramnode * nextn;
	struct ramnode * prevn;
	FILE *rfile;
};

struct ramnode *alloc_node(void)
{
	struct ramnode *p;
	p = malloc(sizeof(struct ramnode));
	if (p == NULL)
	{
		fprintf(stderr, "malloc error\n");
		exit(EXIT_FAILURE);
	}
	return p;
}

void free_node(struct ramnode *p)
{
	free(p);
}

struct ramnode * create_node(char *n){ /* crea un nuovo nodo, primo parametro path dell'immagine */

	struct ramnode *t = alloc_node();
	t->rpath = n;
	t->rvalue = 1;
	t->nextn = NULL;
	t->prevn = NULL;
	t->rfile = fopen(n, "r");
	return t;
}

struct hddnode
{
	//struct ramnode * vpointer;
	char *hpath;
	struct ramnode * vpointer;
	int value;
	
};

struct hddnode *alloc_hnode(void)
{
	struct hddnode *p;
	p = malloc(sizeof(struct hddnode));
	if (p == NULL)
	{
		fprintf(stderr, "malloc error\n");
		exit(EXIT_FAILURE);
	}
	return p;
}

void free_hnode(struct hddnode *p)
{
	free(p);
}
struct hddnode * create_hnode(char *n){ /* crea un nuovo nodo, primo parametro path dell'immagine */

	struct hddnode *t = alloc_hnode();
	t->hpath = n;
	t->value = 1;
	t->vpointer = NULL;
	return t;
}

void nodcrt(char *s, char *dirpath, ENTRY item, char *str_ptr, int srands)
{
	srand(srands);
	int i = rand() % 99 + 1;
	srand(i);
	int j = rand() % 99 + 1;
	struct hddnode * node_ptr = create_hnode(s); 
	node_ptr->value = i;
	node_ptr->hpath = s;
			
	struct ramnode * a = create_node(s);
	node_ptr->vpointer = a;
			
	char arpath[PTH_SIZE];
	strcpy(arpath, dirpath);
	strcat(arpath, "/");
	strcat(arpath, s);
	strcat(arpath, " ");
	
	printf("%s", arpath);
	
	//char cpath[PTH_SIZE];
	//strcpy(cpath, arpath);
	//strcpy(a->rpath, arpath);
	//node_ptr->hpath = arpath;
	a->rpath = arpath; //arpath
	a->rvalue = j;
			
	item.key = s;
	item.data = node_ptr;
	str_ptr += strlen(str_ptr) + 1;
	node_ptr++;
		
	(void) hsearch(item, ENTER);
}
void dircrt(char *dirpath, char *str_ptr)
{
	ENTRY item;
	ENTRY *found_item;
	
   	//int i = 42;
   	DIR *d;
	struct dirent *dir;
	//d = opendir("./res/");
	d = opendir(dirpath);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{

		char *s = dir->d_name;
		if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
		item.key = s;
		if ((found_item = hsearch(item, FIND)) == NULL)
		{
			{
				srand(time(NULL));
				int srands= rand();
				printf("Added\t%s\n", s);
				nodcrt(s, dirpath, item, str_ptr, srands);
			/*
				//struct hddnode node[TBL_SIZE];
				//struct hddnode *node_ptr = node; 
				struct hddnode * node_ptr = create_hnode(s); 
				node_ptr->value = i;
				node_ptr->hpath = s;
				
				struct ramnode * a = create_node(s);
				node_ptr->vpointer = a;
			
				char arpath[PTH_SIZE];
				strcpy(arpath, dirpath);
				strcat(arpath, "/");
				strcat(arpath, s);
				//printf("%s", rpath);
				//strcpy(a->rpath, arpath);
				//node_ptr->hpath = arpath;
				a->rpath = arpath;
				a->rvalue = i*17%5;
			
				item.key = s;
				item.data = node_ptr;
				str_ptr += strlen(str_ptr) + 1;
				node_ptr++;
		
				(void) hsearch(item, ENTER);
				*/
			}
		}
	}		
}	
}
void dirchk(char *dirpath, char *str_ptr)
{
	ENTRY item;
	ENTRY *found_item;
   	DIR *d;
	struct dirent *dir;
	//d = opendir("./res/");
	d = opendir(dirpath);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
		char *s = dir->d_name;
		if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
		{
			item.key = s;
			if ((found_item = hsearch(item, FIND)) != NULL)
			{
				int valore = ((struct hddnode *)found_item->data)->value;
				int valorei = (((struct hddnode *)found_item->data)->vpointer)->rvalue;
				//char *rpath = ((struct hddnode *)found_item->data)->hpath;
				char rpath[400];
				//strcpy(rpath, ((struct hddnode *)found_item->data)->hpath);
				strcpy(rpath, (((struct hddnode *)found_item->data)->vpointer)->rpath);
				//char *rpath = (((struct hddnode *)found_item->data)->vpointer)->rpath;
				(void)printf("Found\t%s: %s, %d, %d\n", found_item->key, rpath, valore, valorei);
			}
        } //else (void)printf("not found %s\n", s_keyword);
		}
	}		
}	

int main()
{
	char table_size[PTH_SIZE*TBL_SIZE];
	char *str_ptr = table_size;
//	struct hddnode node_size[TBL_SIZE];
//	struct hddnode *node_ptr = node_size; 

	ENTRY item;
	ENTRY *found_item;
	char s_keyword[PTH_SIZE];
	int i = 0;
	
	(void) hcreate(TBL_SIZE);
	char *dirpath = "./res";

	while (2>0)
	{
		printf("\n*\n");
		dircrt("./res", str_ptr);
		dirchk("./res", str_ptr);
		
		
/*
   	int i = 42;
   	DIR *d;
	struct dirent *dir;
	//d = opendir("./res/");
	d = opendir(dirpath);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
		char *s = dir->d_name;
		if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
		{
			printf("%s\n", s);
			struct hddnode node[TBL_SIZE];
			struct hddnode *node_ptr = node; 
			node_ptr->value = i;
			struct ramnode * a = create_node(s);
			node_ptr->vpointer = a;
			
			char rpath[PTH_SIZE];
			strcpy(rpath, dirpath);
			strcat(rpath, "/");
			strcat(rpath, s);
			a->rpath = rpath;
			a->rvalue = 44;
			
			item.key = s;
			item.data = node_ptr;
			str_ptr += strlen(str_ptr) + 1;
			node_ptr++;
		
			(void) hsearch(item, ENTER);
		}
		}		
	}	


   	//DIR *d;
	//struct dirent *dir;
	//d = opendir("./res/");
	d = opendir(dirpath);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
		char *s = dir->d_name;
		if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
		{
			item.key = s;
			if ((found_item = hsearch(item, FIND)) != NULL)
			{
				int valore = ((struct hddnode *)found_item->data)->value;
				int valorei = (((struct hddnode *)found_item->data)->vpointer)->rvalue;
				char *rpath = (((struct hddnode *)found_item->data)->vpointer)->rpath;
				(void)printf("found %s: %s, %d, %d\n", found_item->key, rpath, valore, valorei);
			}
        } //else (void)printf("not found %s\n", s_keyword);
		}
	}		
*/
	fflush(stdout);
	sleep(2);
	}
	

		
/*
	while (i++ < TBL_SIZE)
	{
		char *s = randstring(i%50 + 1);
		printf("%s\n", s);
		
		node_ptr->value = i;
		item.key = s;
		item.data = node_ptr;
		str_ptr += strlen(str_ptr) + 1;
		node_ptr++;
		
		(void) hsearch(item, ENTER);
	}
	
	    item.key = s_keyword;
    while (1>0) {
		char *s = randstring(4);
		item.key = s;
        if ((found_item = hsearch(item, FIND)) != NULL) {


            int valore = ((struct hddnode *)found_item->data)->value;
            if (valore == 999)
            {
				int valorei = (((struct hddnode *)found_item->data)->vpointer)->rvalue;
				(void)printf("found %s, i = %d\ti = %d\n", found_item->key, valore, valorei);
			}
			else
			{
				(void)printf("found %s, i = %d\n", found_item->key, valore);
				((struct hddnode *)found_item->data)->value = 999;
				struct ramnode * a = create_node(s);
				((struct hddnode *)found_item->data)->vpointer = a;
				a->rvalue = 998;
			}
        } //else (void)printf("not found %s\n", s_keyword);
    }

*/
	return 0;
}	
