/*
	Progetto - Homepage Generator
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

void convertPic(char *old_path, char *new_path, char *resolution, int colorNumber, int quality);

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
			
			if (height > width)
				{
					kfact = (float)maxsize/height; 
				}
			else 
				{
					kfact = (float)maxsize/width;
				}
			
			/*	
			char oldPath[300];
			char newPath[300];

			
			strcpy(oldPath, respath);
			strcat(oldPath, "/");
			strcat(oldPath, s);
			
			strcpy(newPath, thbpath);
			strcat(newPath, "/");
			strcat(newPath, s);
			*/
			
			if (kfact >= 1)
				{
					printf(ANSI_COLOR_GREEN "\nNot scaling "ANSI_COLOR_CYAN"%s"ANSI_COLOR_GREEN": new resolution would upscale it!\n"ANSI_COLOR_RESET, s);
					//printf("(still converting to GIF:\t[");
					printf("\t\t\t\t[");
					
					
					nConvert(respath, thbpath, s, "gif", NULL, 0, 0);
					//nConvert(respath, thbpath, s, "jpg", NULL, 0, 0);
					//convertPic(oldPath, newPath, NULL, 0, 0);
					//printf("])\n");
					printf("]\n");


				}
			else
				{
					height = height * kfact;
					width = width * kfact;
			
					
					//itoa(width, strwr, 10);
					//itoa(height, strhr, 10);

					/*
					snprintf(strwr, 20, "%d", width);
					snprintf(strhr, 20, "%d", height);
					strcpy(strRes, strwr);
					strcat(strRes, "x");
					strcat(strRes, strhr);
					*/
					
					char strRes[22];
					snprintf(strRes, 22, "%dx%d", width, height);

					printf(ANSI_COLOR_GREEN "\nProcessing "ANSI_COLOR_CYAN"%s\n"ANSI_COLOR_GREEN"New resolution: "ANSI_COLOR_MAGENTA"%s\t"ANSI_COLOR_RESET"[", s, strRes);
					
					//convertPic(oldPath, newPath, strRes, 0, 0);
					//nConvert(
					//nConvert(respath, thbpath, s, "jpg", strRes, 0, 0);
					nConvert(respath, thbpath, s, "gif", strRes, 0, 0);
					//convertPic(oldPath, newPath, strRes, 0, 0);
			
					printf("]\n");
				}
		}
	}

    closedir(d);
	}

	printf(ANSI_COLOR_GREEN"\nThumbnails path: "ANSI_COLOR_CYAN"%s\n" ANSI_COLOR_RESET, thbpath);
}

void homepagen(char *respt, char *thbpt, char *wrfpt, char *indxnam, int imgSize)
{
	int i = 0;
	char mthbpt[300];
	char mkThPth[300];
	char mkThPthi[300];
	char strSize[20];
	
	snprintf(strSize, 20, "%d", imgSize);
	strcpy(mthbpt, thbpt);
	strcat(mthbpt, "/");
	strcat(mthbpt, strSize);
	
	strcpy(mkThPth, "mkdir -p ");
	strcat(mkThPth, thbpt);
	/*
	strcpy(mkThPthi, "mkdir ");
	strcat(mkThPthi, mthbpt);
	*/

	strcpy(mkThPthi, mkThPth);
	strcat(mkThPthi, "/");
	strcat(mkThPthi, strSize);

/*
	printf("\trespt:\t%s\n", respt);
	printf("\tthbpt:\t%s\n", thbpt);
	printf("\tstrSize:\t%s\n", strSize);

	printf("\timgSize:\t%d\n", imgSize);
	printf("\tmkThPth:\t%s\n", mkThPth);
	printf("\tmkThPthi:\t%s\n", mkThPthi);
*/
	
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
                       
		ret = system("mkdir -p ./homepage/res");
	if (WIFSIGNALED(ret) &&
                   (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
                       exit(EXIT_FAILURE);

	
	mkthumbs(respt, mthbpt, imgSize);
	strcpy(mthbpt, "./thumbs/");
	snprintf(strSize, 20, "%d", imgSize);
	strcat(mthbpt, strSize);
/*
	char indxnum[20];
	snprintf(indxnum, 20, "%d", imgSize);
	strcat(indxnam, indxnum);
*/
	strcat(indxnam, ".html");
	
	printf(ANSI_COLOR_GREEN"\nGenerating "ANSI_COLOR_CYAN"%s\n"ANSI_COLOR_RESET, indxnam);

	FILE *fp;
	
	fp = fopen(indxnam, "w+");
//	fprintf(fp,"<HTML>\n<body background=\"bgr.gif\">\n<HEADER>\n<TITLE>Progetto Webserver - Homepage</TITLE>\n</HEADER>");
	//fprintf(fp,"<BODY><table><tr><th>Anteprima File</th><th>Link</th><th>Opzioni</th></tr>");
 //  	fprintf(fp,"\n\n<BODY>\n<table align=\"center\">\n<tr><th>Files disponibili per il download</th><th>Link Originale (No WURFL)</th></tr>");
	fprintf(fp,"<HTML>\n<body background=\"bgr.gif\">\n<HEADER>\n<TITLE>Hideo Webserver - Homepage</TITLE>\n</HEADER>");
   	fprintf(fp,"\n\n<BODY>\n<div style=\"text-align:center\"><p style=\"font-size:30px\">Files disponibili per il download</p></div>\n<table align=\"center\">\n<tr></tr>");
   	DIR *d;
	struct dirent *dir;
	//d = opendir(mthbpt);
	d = opendir(respt);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
    {
	char *s = dir->d_name;
	if ((strcmp(s, "..") != 0) && (strcmp(s, ".")!=0)) 
		{
			strcpy(mkThPth, "cp ");
			strcat(mkThPth, s);
			strcat(mkThPth, " homepage/res");
			ret = system(mkThPth);
			if (WIFSIGNALED(ret) &&
                   (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
                       exit(EXIT_FAILURE);
			i++;
			
			char strPath[300];
			char thbPath[300];
			char wrfPath[300];
			char filename[300];
			strcpy(filename, remove_ext(s, '.', '/'));
			
			
			//strcpy(thbPath, mthbpt);
			strcpy(thbPath, mthbpt);
			strcat(thbPath, "/");
			//strcpy(thbPath, strSize);
			//strcat(thbPath, "/");
			strcat(thbPath, filename);
			strcat(thbPath, ".gif");
			
			strcpy(strPath, respt);
			strcat(strPath, "/");
			strcat(strPath, s);
			
			strcpy(wrfPath, wrfpt);
			strcat(wrfPath, "/");
			strcat(wrfPath, s);
			
			//printf(ANSI_COLOR_GREEN"\nFile "ANSI_COLOR_CYAN"%s"ANSI_COLOR_GREEN"\n\tResource link\t"ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_GREEN"\n\tThumb link\t"ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_GREEN"\n\tWURFL link\t"ANSI_COLOR_MAGENTA"%s" ANSI_COLOR_RESET"\n...done!\n",s, strPath, thbPath, wrfPath);
			printf(ANSI_COLOR_GREEN"\nFile "ANSI_COLOR_CYAN"%s"ANSI_COLOR_GREEN"\n\tResource link\t"ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_GREEN"\n\tThumb link\t"ANSI_COLOR_MAGENTA"%s"ANSI_COLOR_GREEN"\n...done!\n",s, strPath, thbPath);

			/*
			 * <a href=\"%s\"><img src=\"%s\" alt=\"%s\" style=\"width:%dpx;height:%dpx;border:0\"></a>
			 */
			 
			//fprintf(fp,"<tr><td><a href=\"%s\"><img src=\"%s\" alt=\"%s\" align=\"middle\" style=\"width:100%%;height:100%%;border:0\"></a></td><td>Link</td><td>Options</td></tr>", strPath, strPath, s);
			if(i % 2) fprintf(fp, "\n</tr>\n<tr>");
			fprintf(fp,"\n<td><a href=\"%s\"><img src=\"%s\" alt=\"%s\" style=\"width:100%%;height:100%%;border:0\"></a></td>", strPath, thbPath, s);
		
//			fprintf(fp,"\n<tr><td><a href=\"%s\"><img src=\"%s\" alt=\"%s\" style=\"width:100%%;height:100%%;border:0\"></a></td><td><a href=\"%s\">Link</a></td></tr>", wrfPath, thbPath, s, strPath);
		}
	}
}
	fprintf(fp,"\n</tr>\n</table>\n</BODY>\n\n</HTML>");

	//fprintf(fp,"\n</table>\n</BODY>\n\n</HTML>");
	fclose(fp);
	ret = system("cp bgr.gif homepage/");
	if (WIFSIGNALED(ret) &&
                   (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
                       exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
/*
 *	index.html generator, main func 
 */ 

{
	//Con WURFL si può ottenere un diverso valore 
	int imgSize = 300;
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
	//printf(ANSI_COLOR_MAGENTA"\n- index.html generator -\n\n"ANSI_COLOR_RESET); 
	printf(ANSI_COLOR_GREEN"\nUsing "ANSI_COLOR_MAGENTA"%dx%d "ANSI_COLOR_GREEN"as thumbnail resolution limit\n\n"ANSI_COLOR_RESET,imgSize, imgSize);
	char thbpt[] = "./homepage/thumbs";
	char respt[] = "./res";
	char wrfpt[] = "./homepage/wurfl";
	char indxnam[] = "./homepage/index";

	homepagen(respt, thbpt, wrfpt, indxnam, imgSize);


  return(EXIT_SUCCESS);
}
