#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>


#ifndef PARSE_CONF_FILE_H
#define PARSE_CONF_FILE_H

/*
 * parse: parse simple name/value pairs. Skip comments and space
 **/

#define MAXLEN 80
#define CONFIG_FILE "server.cfg"


typedef struct {
	char port[MAXLEN];
	char threads[MAXLEN];
	char backlog[MAXLEN];
  char loglvl[MAXLEN];
} Config;
Config config_file;


/*configuration file functions*/
void init_parameters();
char *trim(char *);
int parse_config();


/* default parameters*/
void init_parameters ()
{
  strncpy (config_file.port, "5700", MAXLEN);
  strncpy (config_file.threads, "10", MAXLEN);
  strncpy (config_file.backlog, "64", MAXLEN);
  strncpy (config_file.loglvl, "7", MAXLEN);
}


char * trim (char * s)
{
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];

  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';

  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;

  /* Copy finished string */
  strcpy (s, s1);
  return s;
}


int parse_config()
{
  char *s, buff[256];
  FILE *fp = fopen (CONFIG_FILE, "r");
  if (fp == NULL)
  {
    return 0;
  }

  /* Read next line */
  while ((s = fgets (buff, sizeof buff, fp)) != NULL)
  {
    /* Skip blank lines and comments */
    if (buff[0] == '\n' || buff[0] == '#')
      continue;

    /* Parse name/value pair from line */
    char name[MAXLEN], value[MAXLEN];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else
      strncpy (name, s, MAXLEN);
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, MAXLEN);
    trim (value);

    /* Copy into correct entry in parameters struct */
    if (strcmp(name, "server")==0)
      strncpy (config_file.port, value, MAXLEN);
    else if (strcmp(name, "threads")==0)
      strncpy (config_file.threads, value, MAXLEN);
    else if (strcmp(name, "backlog")==0)
      strncpy (config_file.backlog, value, MAXLEN);
    else if (strcmp(name, "loglvl")==0)
      strncpy (config_file.loglvl, value, MAXLEN);
    else
      printf ("WARNING: %s/%s: Unknown name/value pair!\n",
        name, value);
  }

  /* Close file */
  fclose (fp);
  return 1;
}


#endif				/*PARSE_CONF_FILE_H */
