#include <wurfl/wurfl.h>
#include <stdio.h>

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

wurfl_handle hwurfl;

int get_height(wurfl_device_handle hdevice);
int get_width(wurfl_device_handle hdevice);
wurfl_device_handle get_device(wurfl_handle hwurfl, char *userAgent);
wurfl_handle get_wurfldb(char *root);
void wurflpdt(wurfl_handle wurfldb, char *uastring);
void wurflrdt(wurfl_handle wurfldb, char *uastring, int *heightv, int *widthv);

/*
int main(){	TEST
int hv = 0;
int wv = 0;
	printf("k");

	wurfl_handle hwurfl;
	hwurfl = get_wurfldb("wurfl-eval.xml");	 //percorso del file .xml

	if (hwurfl != NULL){

	for (;;)
{
	FILE* file = fopen("prova3.txt", "r");
	char line[4000];

    	while (fgets(line, sizeof(line), file))
		{
	     	//	printf("\n%s", line);
			wurflrdt(hwurfl, line, &hv, &wv);
//			printf("%d\t%d\n", hv, wv);
	        }
    	fclose(file);
}
wurfldt(hwurfl, "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:49.0) Gecko/20100101 Firefox/49.0");
wurfldt(hwurfl, "Mozilla/5.0 (Linux; Android 6.0.1; SAMSUNG SM-G900F Build/MMB29M) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/4.0 Chrome/44.0.2403.133 Mobile Safari/537.36");
	}

wurfl_destroy(hwurfl);
return 0;

}*/
