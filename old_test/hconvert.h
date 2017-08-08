#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define DATLEN 4096

int ret;
int ev;

// remove_ext: removes the "extension" from a file spec.
//   mystr is the string to process.
//   dot is the extension separator.
//   sep is the path separator (0 means to ignore).
// Returns an allocated string identical to the original but
//   with the extension removed. It must be freed when you're
//   finished with it.
// If you pass in NULL or the new string can't be allocated,
//   it returns NULL.

char *getcresol(char *filepath)
{
	//int value = 0;
	char *valuec;
	if ((valuec = malloc (10)) == NULL)
		{
			perror("malloc error on getcwidth");
			return NULL;
		}
	char strCmd[400];
	strcpy(strCmd, "identify -format \"%wx%h\" ");
	strcat(strCmd, filepath);
	FILE *fpc = popen(strCmd, "r");
	errno = 0;
	ev = fscanf(fpc, "%s", valuec);
		if (errno != 0) perror("fscanf");
	pclose(fpc);
	return valuec;
}

int getiwidth(char *filepath)
{
	int value = 0;
	char strCmd[400];
	strcpy(strCmd, "identify -format \"%w\" ");
	strcat(strCmd, filepath);
	FILE *fpc = popen(strCmd, "r");
	ev = fscanf(fpc, "%d", &value);
		if (errno != 0) perror("fscanf");
	pclose(fpc);
	return value;
}

int getiheight(char *filepath)
{
	int value = 0;
	char strCmd[400];
	strcpy(strCmd, "identify -format \"%h\" ");
	strcat(strCmd, filepath);
	FILE *fpc = popen(strCmd, "r");
	ev = fscanf(fpc, "%d", &value);
		if (errno != 0) perror("fscanf");
	pclose(fpc);
	return value;
}


char *getcwidth(char *filepath)
{
	//int value = 0;
	char *valuec;
	if ((valuec = malloc (10)) == NULL)
		{
			perror("malloc error on getcwidth");
			return NULL;
		}
	char strCmd[400];
	strcpy(strCmd, "identify -format \"%w\" ");
	strcat(strCmd, filepath);
	FILE *fpc = popen(strCmd, "r");
	ev = fscanf(fpc, "%s", valuec);	//&value
		if (errno != 0) perror("fscanf");
	//snprintf(valuec, 50, "%d", value);
	pclose(fpc);
	return valuec;
}

char *getcheight(char *filepath)
{
	//int value = 0;
	char *valuec;
	if ((valuec = malloc (10)) == NULL)
		{
			perror("malloc error on getcheight");
			return NULL;
		}
	char strCmd[400];
	strcpy(strCmd, "identify -format \"%h\" ");
	strcat(strCmd, filepath);
	FILE *fpc = popen(strCmd, "r");
	ev = fscanf(fpc, "%s", valuec); //&value
		if (errno != 0) perror("fscanf");
	//snprintf(valuec, 50, "%d", value);
	pclose(fpc);
	return valuec;
}

char *remove_ext (char* mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    // Error checks and allocate string.
    if (mystr == NULL) return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL) return NULL;

    // Make a copy and find the relevant characters.
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    // If it has an extension separator.
    if (lastdot != NULL) {

        // and it's before the extenstion separator.
        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                // then remove it.
                *lastdot = '\0';
            }
        } else {
            // Has extension separator with no path separator.
            *lastdot = '\0';
        }
    }
    // Return the modified string.
    return retstr;
}

void nConvert(char *old_path, char *new_path, char *namefile, char *format, char *resolution, int colorNumber, int quality)
{
	/*
	old_path:		percorso dell'immagine da modificare
	new_path: 	percorso in cui salvare l'immagine modificata (determina anche il formato!)
	resolution: 	risoluzione (se == NULL allora resolution non specificata)
	colorNumber: 	numero di colori (se == 0 allora colorNumber non specificato)
	quality: 	qualità dell'immagine, può essere un numero tra 1 e 100 (se == 0 allora quality non specificato)
	 */

	char strCommand[400];
	char nnamefile[400];
	if (format != NULL)
	{
		strcpy(nnamefile, remove_ext(namefile, '.', '/'));
		strcat(nnamefile, ".");
		strcat(nnamefile, format);
	}
	else
	{
		strcat(nnamefile, namefile);
	}

	strcpy(strCommand, "convert ");
	strcat(strCommand, old_path);
	strcat(strCommand, "/");
	strcat(strCommand, namefile);
	strcat(strCommand, " ");

	if (colorNumber != 0)
	{
		char strColorNumber[50];
		snprintf(strColorNumber, 50, "-colors %d ", colorNumber);
		strcat(strCommand, strColorNumber);
	}

	if (quality != 0) {

		char strQuality[50];
		snprintf(strQuality, 50, "-quality %d ", quality);
		strcat(strCommand, strQuality);
	}

	if (resolution != NULL)
	{
		strcat(strCommand, "-resize ");
		strcat(strCommand, resolution);
		strcat(strCommand, " ");
	}

	strcat(strCommand, new_path);
	strcat(strCommand, "/");
	strcat(strCommand, nnamefile);

	printf("%s", strCommand);
	ret = system(strCommand); /* esecuzione del programma convert */
	if (WIFSIGNALED(ret) &&
		(WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)){
			perror("system");
			exit(EXIT_FAILURE);
	}

}

void convertPic(char *old_path, char *new_path, char *resolution, int colorNumber, int quality){

	/*
	old_path:	percorso dell'immagine da modificare
	new_path: 	percorso in cui salvare l'immagine modificata (determina anche il formato!)
	resolution: 	risoluzione (se == NULL allora resolution non specificata)
	colorNumber: 	numero di colori (se == 0 allora colorNumber non specificato)
	quality: 	qualità dell'immagine, può essere un numero tra 1 e 100 (se == 0 allora quality non specificato)
	 */

	char strCommand[400];

	strcpy(strCommand, "convert ");
	strcat(strCommand, old_path);
	strcat(strCommand, " ");


	if (colorNumber != 0) {

		char strColorNumber[20];
		sprintf(strColorNumber, "%d", colorNumber);

		strcat(strCommand, "-colors ");
		strcat(strCommand, strColorNumber);
		strcat(strCommand, " ");
	}

	if (quality != 0) {

		char strQuality[5];
		sprintf(strQuality, "%d", quality);

		strcat(strCommand, "-quality ");
		strcat(strCommand, strQuality);
		strcat(strCommand, " ");
	}

	if (resolution != NULL) {

		strcat(strCommand, "-resize ");
		strcat(strCommand, resolution);
		strcat(strCommand, " ");

	}

	strcat(strCommand, new_path);
	printf("%s", strCommand);
	ret = system(strCommand); /* esecuzione del programma convert */
	if (WIFSIGNALED(ret) &&
		(WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)){
			perror("system");
			exit(EXIT_FAILURE);
	}
}
