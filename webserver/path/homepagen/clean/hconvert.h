#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "hpglobal.h"

/*
 * 	get** functions:
 * 
 * 	given full path of a picture (in filepath) returns:
 * 
 * 	getcresol	resolution *char (123x456 format)
 * 	getcheight	height *char
 * 	getcwidth	width *char
 * 	getiheight	height int
 * 	getiwidth	width int
 */
 
char *getcresol(char *filepath)
{
	char *valuec;
	if ((valuec = malloc (10)) == NULL) 
	{
		perror("malloc error on getcresol");
		return NULL;
	}
	char strCmd[400];
	strcpy(strCmd, "identify -format \"%wx%h\" ");
	strcat(strCmd, filepath);
	FILE *fpc = popen(strCmd, "r");
	ev = fscanf(fpc, "%s", valuec);
	if (errno != 0) perror("fscanf error on getcresol");
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
	if (errno != 0) perror("fscanf error on getiwidth");
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
	if (errno != 0) perror("fscanf error on getiheigth");
	pclose(fpc);
	return value;
}

char *getcwidth(char *filepath)
{
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
	if (errno != 0) perror("fscanf error on getcwidth");
	pclose(fpc);
	return valuec;
}

char *getcheight(char *filepath)
{
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
	if (errno != 0) perror("fscanf error on getcheight");
	pclose(fpc);
	return valuec;
}

/*
 * 	remove_ext: removes the "extension" from a file spec.
 * 		mystr	string to process.
 * 		dot		extension separator.
 * 		sep		path separator (0 means to ignore).
 * 
 * 	Returns an allocated string identical to the original but
 * 	with the extension removed. It must be freed when you're
 * 	finished with it.
 * 	If you pass in NULL or the new string can't be allocated,	
 * 	it returns NULL.
 */

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

void nConvert(char *old_path, char *new_path, char *namefile, char *format, char *resolution, int colorNumber, int quality, int verbose)
{
	/*
	 * 	main convert wrapper
	 * 
	 * 		old_path:		resource path
	 * 		new_path: 		destination path
	 * 		namefile:		resource file name (and root for destination file)
	 * 		format:			(if not NULL) destination file extension
	 * 		resolution: 	(if not NULL) destination resolution (using "123x456" format)
	 * 		colorNumber: 	(if not 0) number of colors in destination's palette 
	 * 		quality: 		(if not 0) an integer between 1-100 specifing quality of destination file
	 * 		verbose:		0 for silent exec, != 0 values print launched command, 
	 * 
	 * 	executes convert given those options
	 */

	char strCommand[400];
	char nnamefile[400];
	
	if (format != NULL)
	{
		strcpy(nnamefile, remove_ext(namefile, '.', '/'));
		strcat(nnamefile, ".");
		strcat(nnamefile, format);
	}
	else strcat(nnamefile, namefile);
	
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

	if (quality != 0)
	{
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
	
	if (verbose != 0) printf("%s", strCommand);
	ret = system(strCommand);
	if (WIFSIGNALED(ret) && 
		(WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)){
			perror("system error on ");
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
	if (WIFSIGNALED(ret) && (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
	{
		char stringerr[400];
		strcpy(stringerr, "system error executing ");
		strcat(stringerr, strCommand);
		perror(stringerr);
		exit(EXIT_FAILURE);
	}
}
