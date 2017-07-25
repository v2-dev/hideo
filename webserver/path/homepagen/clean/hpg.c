/*
	Hideo - Homepage Generator
	GPLv3
*/

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>

/*
 * #include "hpglobal.h" if not included elsewhere.
 * hconvert.h already includes it.
 */

//#include "hpglobal.h"
#include "hconvert.h"
#include "ansicolor.h"

int isInt(char number[])
{
	/*
	 * 		Returns 1 if char number[] contains only digits
	 */
    for (int i = 0; number[i] != 0; i++) if (!isdigit(number[i])) return 0;
    return 1;
}

void mkthumbs(char *respath, char *thbpath, int maxsize)
{
	/*
	 *		Given 	*respath 	resource path ("./path")
	 * 				*thbpath	destination path ("./path")
	 * 				maxsize		maximum pic size (int)
	 *
	 * 		converts all pics from resource path in .GIF files in destination path, setting maximum dimension
	 * 		(maximum between width and height) as maxsize value.
	 */

	DIR *d;
	struct dirent *dir;
	d = opendir(respath);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			char *s = dir->d_name;
			if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
			{

				float kfact;
				char cfilepath[400];

				strcpy(cfilepath, respath);
				strcat(cfilepath, "/");
				strcat(cfilepath, s);

				int height = getiheight(cfilepath);
				int width = getiwidth(cfilepath);

				kfact = (float)maxsize/width;
				if (height > width) kfact = (float)maxsize/height;

				if (kfact >= 1)
				{
					printf(ANSI_COLOR_GREEN "\nNot scaling "ANSI_COLOR_CYAN"%s"ANSI_COLOR_GREEN": new resolution would upscale it!\nConverting to GIF:\t"ANSI_COLOR_RESET"[", s);
					nConvert(respath, thbpath, s, "gif", NULL, 0, 0, 1);
					printf("]\n");
				}

				else
				{
					height = height * kfact;
					width = width * kfact;

					char strRes[22];
					snprintf(strRes, 22, "%dx%d", width, height);

					printf(ANSI_COLOR_GREEN "\nScaling "ANSI_COLOR_CYAN"%s"ANSI_COLOR_GREEN", new resolution: "ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_GREEN"\nConverting to GIF:\t"ANSI_COLOR_RESET"[", s, strRes);
					nConvert(respath, thbpath, s, "gif", strRes, 0, 0, 1);
					printf("]\n");
				}
			}
		}

    closedir(d);
	}

	printf(ANSI_COLOR_GREEN"\nThumbnails path: "ANSI_COLOR_CYAN"%s\n" ANSI_COLOR_RESET, thbpath);
}

void homepagen(char *respt, char *thbpt, char *wrfpt, char *indpt, char *indxnam, int imgSize)
{
	/*
	 *		Given	respt	resource path ("./path")
	 * 				thbpt	thumbnail path (as well)
	 * 				wrfpt	WURFL metapath (^)
	 * 				indpt	index*.html path (still "./path")
	 * 				indxnam	basename for index.html (will be index*.html)
	 * 				imgSize	maximum pic size (int)
	 *
	 * 		generates HTML Index 
	 */
	char mthbpt[300];
	char mkThPth[300];
	char mkThPthi[300];
	char mkInPth[300];
	char strSize[20];
	char indexfp[300];

	snprintf(strSize, 20, "%d", imgSize);
	strcat(mthbpt, thbpt);
	strcat(mthbpt, "/");
	strcat(mthbpt, strSize);

	strcpy(mkThPth, "mkdir ");
	strcat(mkThPth, thbpt);

	strcpy(mkThPthi, "mkdir ");
	strcat(mkThPthi, mthbpt);

	strcpy(mkInPth, "mkdir ");
	strcat(mkInPth, indpt);

	printf(ANSI_COLOR_RED "Executing [%s]\n"ANSI_COLOR_RESET, mkThPth);

	ret = system(mkThPth);
	if (WIFSIGNALED(ret) &&
                   (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
                       exit(EXIT_FAILURE);


	printf(ANSI_COLOR_RED "Executing [%s]\n"ANSI_COLOR_RESET, mkThPthi);

	ret = system(mkThPthi);
	if (WIFSIGNALED(ret) &&
                   (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
                       exit(EXIT_FAILURE);

    printf(ANSI_COLOR_RED "Executing [%s]\n"ANSI_COLOR_RESET, mkThPthi);

	ret = system(mkInPth);
	if (WIFSIGNALED(ret) &&
                   (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
                       exit(EXIT_FAILURE);

	mkthumbs(respt, mthbpt, imgSize);

	char indxnum[20];
	snprintf(indxnum, 20, "%d", imgSize);
	strcat(indxnam, indxnum);
	strcat(indxnam, ".html");

	strcpy(indexfp, indpt);
	strcat(indexfp, "/");
	strcat(indexfp, indxnam);

	printf(ANSI_COLOR_GREEN"\nGenerating "ANSI_COLOR_CYAN"%s\n"ANSI_COLOR_RESET, indexfp);

	FILE *fp;

	fp = fopen(indexfp, "w+");
	fprintf(fp,"<HTML>\n<body background=\"bgr.gif\">\n<HEADER>\n<TITLE>Hideo Webserver - Homepage</TITLE>\n</HEADER>");
   	fprintf(fp,"\n\n<BODY>\n<table align=\"center\">\n<tr><th>Files disponibili per il download</th><th>Link Originale (No WURFL)</th></tr>");

   	DIR *d;
	struct dirent *dir;
	d = opendir(mthbpt);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			char *s = dir->d_name;
			if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
			{
				char strPath[300];
				char thbPath[300];
				char wrfPath[300];

				strcpy(thbPath, mthbpt);
				strcat(thbPath, "/");
				strcat(thbPath, s);

				strcpy(strPath, respt);
				strcat(strPath, "/");
				strcat(strPath, s);

				strcpy(wrfPath, wrfpt);
				strcat(wrfPath, "/");
				strcat(wrfPath, s);

				printf(ANSI_COLOR_GREEN"\nFile "ANSI_COLOR_CYAN"%s"ANSI_COLOR_GREEN"\n\tResource link\t"ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_GREEN"\n\tThumb link\t"ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_GREEN"\n\tWURFL link\t"ANSI_COLOR_MAGENTA"%s" ANSI_COLOR_RESET"\n...done!\n",s, strPath, thbPath, wrfPath);
				fprintf(fp,"\n<tr><td><a href=\"%s\"><img src=\"%s\" alt=\"%s\" style=\"width:100%%;height:100%%;border:0\"></a></td><td><a href=\"%s\">Link</a></td></tr>", wrfPath, thbPath, s, strPath);
			}
		}
	}

	fprintf(fp,"\n</table>\n</BODY>\n\n</HTML>");
	fclose(fp);
}

int main(int argc, char** argv)

/*
 *		index.html generator, main
 * 		Reads from stdinput an integer value -if given, and config file	*TODO*
 */

{
	char thbpt[] = "./thumbs";
	char respt[] = "../res";
	char wrfpt[] = "./wurfl";
	char indpt[] = ".";
	char indxnam[] = "index";
	int imgSize = 400;

	if (argc > 1)
	{
		if (isInt(argv[1])==0 )
		{
			printf("\n"ANSI_COLOR_RED"[ERROR] Invalid thumbnail size value detected!\n"ANSI_COLOR_RESET"Hideo homepage generator needs a positive integer value.\nIf not provided, default value is 400 pixels.\n\n\tExample:\n\t\t./hpg 200\twill set thumbnail maximum size to 200x200\n\t\t./hpg\t\t will use default value of 400x400\n\n");
			exit(EXIT_FAILURE);
		}
		imgSize = atoi(argv[1]);
	}

	printf(ANSI_COLOR_MAGENTA"    __    _     __         \n   / /_  (_)___/ /__  ____\n  / __ \\/ / __  / _ \\/ __ \\\n / / / / / /_/ /  __/ /_/ /\n/_/ /_/_/\\__,_/\\___/\\____/\n"ANSI_COLOR_CYAN"- index.html generator -\n\n"ANSI_COLOR_RESET); 
	printf(ANSI_COLOR_GREEN"\nUsing "ANSI_COLOR_MAGENTA"%dx%d "ANSI_COLOR_GREEN"as thumbnail resolution limit\n\n"ANSI_COLOR_RESET,imgSize, imgSize);
	
	homepagen(respt, thbpt, wrfpt, indpt, indxnam, imgSize);
	
	return(EXIT_SUCCESS);
}
