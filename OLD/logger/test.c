#include "nlogger.h"


int isInt(char number[])
{
	/*
	 * 		Returns 1 if char number[] contains only digits
	 */
	if (number == NULL) return 0;
    for (int i = 0; number[i] != 0; i++) if (!isdigit(number[i])) return 0;
    return 1;
}

FILE *f;

int main(int argc, char **argv)
{
	struct tlogger ts;
	
	//1.passare come argomento il livello di log
	//2. aprire il file di log
	//3. inizializzare il mutex
	//4. creare n thread, riempire i campi della struttura thread_safe
	//5. eseguire safe_write
	
	if ((isInt(argv[1])==0 ) || (argc != 2))
	{
		printf("Usage: %s (int) <LOG LEVEL>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	f = fopen("log.log", "a");
	if(f == NULL){
		perror("k");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_mutex_init(&ts.mtx, NULL) != 0){
		perror("error on pthread_mutex_init");
		exit(EXIT_FAILURE);
	}
	
	ts.level = atoi(argv[1]);
	
	
	printf("\n%d\n", atoi(argv[1]));
	return 0;
}
