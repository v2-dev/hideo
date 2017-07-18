#include "libhttp.h"
#include "mimetypes.h"
#include "utils.h"

void http_200(struct conndata *conn)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 200 OK\r\n");
	sprintf(buff, "%sServer: Hideo\r\n", buff);
	sprintf(buff, "%s<html><head><title>200 OK</title></head>"	,buff);
	sprintf(buff, "%s<body><h1>Everything is fine here!</h1></body></html>\r\n",	buff);
	send_msg(conn->socketint, buff);
}

void http_500(struct conndata *conn)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 500 INTERNAL SERVER ERROR\r\n");
	sprintf(buff, "%sServer: Hideo\r\n", buff);

	send_msg(conn->socketint, buff);
}

void http_501(struct conndata *conn)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 501 NOT IMPLEMENTED\r\n");
	sprintf(buff, "%sServer: Hideo\r\n", buff);
	sprintf(buff, "%s<html><head><title>501 NOT IMPLEMENTED</title></head>"
		,buff);
	sprintf(buff, "%s<body><h1>The method or request you made is not implemented</h1></body></html>\r\n",
		buff);

	send_msg(conn->socketint, buff);
}

void http_404(struct conndata *conn)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 404 NOT FOUND\r\n");
	sprintf(buff, "%sServer: Hideo\r\n\r\n", buff);
	sprintf(buff, "%s<html><head><title>404 Not Found</title></head>"
		,buff);
	sprintf(buff, "%s<body><h1>404 File Not found</h1></body></html>\r\n",
		buff);

	send_msg(conn->socketint, buff);
}


void http_400(struct conndata *conn)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 404 BAD REQUEST\r\n");
	sprintf(buff, "%sServer: Hideo\r\n\r\n", buff);
	sprintf(buff, "%s<html><head><title>400 bad request</title></head>"
		,buff);

	send_msg(conn->socketint, buff);
}


int find_quality(char * accept){

	int q;
	char * lastQ = strrchr(accept, 'q');

	if (lastQ == NULL) return 0;
	if (*(lastQ+1) != '=') return 0;
	if (*(lastQ+2) == '1') return 1;
	if (*(lastQ+2) != '0') return 0;
	if (*(lastQ+3) != '.') return 0;

	q = *(lastQ+4);
	if (!((q>'1')&&(q<'9'))) return 0;

	return ((q-'0')*10);
}


char *get_ext (char* mystr)
{
    char *retstr, *lastdot;
    char dot = '.';
    if (mystr == NULL) return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL) return NULL;
    lastdot = strrchr (retstr, dot);
	strcpy(retstr, "");
    sscanf(++lastdot, "%s", retstr);
    return retstr;
}


char *get_mimetype (char* pathstr)
{
	char extension[5] = "";

	char *ext;
	ext = strrchr(pathstr, '.');
	strcpy(extension, ext+1);

	if ((strcmp(extension, "html") == 0) || (strcmp(extension, "htm") == 0)) return "text/html";
	int i;
	for (i = 0; i< 70; i++)
	{
		if (strcmp(extension, mimetypes[i][1]) == 0) return mimetypes[i][0];
	}
	return "text/html";

}

struct conndata * create_conndata(void)
{
	struct conndata * lt;
	lt = malloc(sizeof(struct conndata));
	if (lt == NULL)
	{
		fprintf(stderr, "\nmemory allocation error");
		exit(EXIT_FAILURE);
	}
	return lt;
}

void print_message(struct conndata *p)
{
	printf("\n[%x]\t[%d]\t%s", p->process_id, p->socketint, p->messages);
	fflush(stdout);
	strcpy(p->messages, "");
}

/****************************
			User-Agent parsing
****************************/

int uacheck(char *optstring, struct conndata *p)
{
	/*
	 * parse dello user agent
	 *
	 * restituisce 1 se il char passato è uno user agent
	 * restituisce 0 se il char passato non è uno ua e non ci sono errori
	 * restituisce -1 in caso di errori
	 *
	 */
	char ua_head[] = "User-Agent:";
	size_t buf_idx = 0;
	while (buf_idx < 11)
	{
		if (buf_idx > 0 && ua_head[buf_idx] != optstring[buf_idx]) return 0;
		buf_idx++;
	}
	strcpy(p->useragent, optstring+12);
	/*
	strcpy(p->useragent, "");
	for (buf_idx = 11; buf_idx <= strlen(optstring); buf_idx++) strcat(p->useragent, optstring[buf_idx]);
	*/
	strcpy(p->messages, "User Agent = ");
	strcat(p->messages, p->useragent);
	print_message(p);
	return 1;
}


/****************************
			Accept field parsing
****************************/
int accheck(char *optstring, struct conndata *p)
{
	/*
	 * parse del campo accept
	 *
	 * restituisce 1 se il char passato è un campo accept
	 * restituisce 0 se il char passato non è uno ua e non ci sono errori
	 * restituisce -1 in caso di errori
	 *
	 */
	char ua_head[] = "Accept:";
	size_t buf_idx = 0;
	while (buf_idx < 7)
	{
		if (buf_idx > 0 && ua_head[buf_idx] != optstring[buf_idx]) return 0;
		buf_idx++;
	}
	strcpy(p->acceptfld, optstring+8);

	p->quality_factor = find_quality(p->acceptfld);
	fprintf(stdout, "quality factor %d\n", p->quality_factor);
	/*
	strcpy(p->useragent, "");
	for (buf_idx = 11; buf_idx <= strlen(optstring); buf_idx++) strcat(p->useragent, optstring[buf_idx]);
	*/
	strcpy(p->messages, "Accept = ");
	strcat(p->messages, p->acceptfld);
	print_message(p);
	return 1;

}

/****************************
			Method parsing
****************************/

int method_parse(char *optstring, struct conndata *p)
{
	/*
	 * Parsing of method
	 * This function returns:
	 * 		0	not HEAD, nor GET or error
	 * 		1	GET
	 * 		2	HEAD
	 */
	if ( !strncmp(optstring, "GET ", 4) )
	{
		strcpy(p->method_r, "GET");
		p->get1head2 = 1;
		return 1;
	}

	else if ( !strncmp(optstring, "HEAD ", 5) )
	{
		strcpy(p->method_r, "HEAD");
		p->get1head2 = 2;
		return 2;
	}
	p->get1head2 = 0;
	strcpy(p->method_r, "");
	/*return BAD_REQUEST*/
	return ERROR;
}

int path_parse(char *optstring, struct conndata *p)
{
	/*
	 * Parse del path
	 * restituisce 0 se non ci sono errori
	 * restituisce ERROR se il path non è corretto
	 * []GET/HEAD[ ]pathpippo[ ]HTTP/1.1[/0]
	 */
	unsigned int choffset = 4;
	int i;
	if (p->get1head2 == 2) choffset++;
	for(i = 0; choffset < strlen(optstring); i++)
	{
		//printf("\n%d %s", i, optstring+choffset + i);
		if (optstring[choffset + i] == ' ')
		{
			p->path_r[i] = '\0';
			choffset = choffset + i;
			break;
		}
		p->path_r[i] = optstring[choffset + i];
	}

	char header[] = "HTTP/1.1";
	if ( !strncmp(optstring+choffset, header, 8) )
		{
			strcpy(p->messages, "Header HTTP mancante od incompleto!");
			print_message(p);
			return ERROR;
		}

	strcpy(p->messages, "Header HTTP OK");
	print_message(p);

	strcpy(p->messages, "Path richiesto: ");
	strcat(p->messages, p->path_r);
	print_message(p);

	//strcpy(p->path_r, path_r);
	return 0;
}


void * create_httpread()
{
	struct httpread * httpr;
	httpr = malloc(sizeof(struct httpread));

	if (httpr == NULL)
	{
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	httpr->dimArray = 0;
	httpr->array = malloc(sizeof(char *));
	return httpr;
}

void destroy_httpread(struct httpread * httpr)
{
	int i;
	for (i = 0; i < httpr->dimArray; i++)
	{
			free(*(httpr->array+i));
	}
	free(httpr->array);
	free(httpr);
}


int serve_request(struct conndata * cdata)
{
	struct httpread * httpr;
	httpr = read_request(cdata->socketint);
	/*
	 * if (httpr == NULL)
	{
		destroy_httpread(httpr);
		return 0;
	}
	* */
	if (httpr->dimArray == 0)
	{
		destroy_httpread(httpr);
		return 0;
	}
	if (httpr->dimArray == -1)
	{
		destroy_httpread(httpr);
		return ERROR;
	}

	if ( method_parse(*(httpr->array), cdata) == ERROR)
	{
			http_400(cdata);
			destroy_httpread(httpr);
			return ERROR;
	}

	if (path_parse(*(httpr->array), cdata) == ERROR)
	{
			http_400(cdata);
			destroy_httpread(httpr);
			return ERROR;
	}
			//strcpy(cdata->messages, *(httpr->array));
			//print_message(cdata);
	int i;
	for(i = 1; i<httpr->dimArray; i++)
	{
		accheck(*(httpr->array+i), cdata);
		// check for return value
		uacheck(*(httpr->array+i), cdata);
		// check for return value
	}
	if (send_response(cdata) != 0)
		return ERROR;

}


int send_response(struct conndata *p)
{

	char header200[150] = "HTTP/1.1 200 OK\r\nServer: ProgettoIIWIS\r\nConnection: keep-alive\r\nContent-type: ";
	//char *header400 = "HTTP/1.1 400 Bad Request\nServer: ProgettoIIW-IS\nContent-type: text/html\r\n";
	//char *header404 = "HTTP/1.1 404 Not Found\nServer: ProgettoIIW-IS\nContent-type: text/html\r\n";
	char *header400 = "HTTP/1.1 400 Bad Request\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";
	char *header404 = "HTTP/1.1 404 Not Found\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";
	//char *keepalive = "Connection: keep-alive\r\n";
	int req_fd;
	p->return_code = 400;

	printf("\n\tPath =#%s#", p->path_r);
	if ( p->path_r[0] == '/' && (p->path_r[1] == '\0') ) strcpy(p->path_r, "/index.html");
	printf("\n\tPath =#%s#", p->path_r);

	//TEST
	char testpath[300] = "path/homepagen";
	strcat(testpath, p->path_r);
	strcpy(p->path_r, testpath);

	if ((req_fd = open(p->path_r, O_RDONLY)) < 0)
	{
		p->return_code = 404;
		strcpy(p->messages, "File ");
		strcat(p->messages, p->path_r);
		strcat(p->messages, " non trovato, invio header 404");
		print_message(p);
		fprintf(stderr, "\nErrore nella open di %s", p->path_r);
	}
	else
	{
		p->return_code = 200;

		struct stat st;
		stat(p->path_r, &st);
		unsigned int contlen = st.st_size;

		//CONTROLLO DELL'ESTENSIONE
		char mimetype_t[30] = "";
		strcpy(mimetype_t, get_mimetype(p->path_r));
		strcpy(p->messages, "Mimetype: ");
		strcat(p->messages, mimetype_t);
		print_message(p);
		strcat(header200, mimetype_t);
		strcat(header200, "\r\nContent-Length: ");
		char scontlen[15];
		sprintf(scontlen, "%d\r\n\r\n", contlen);
		strcat(header200, scontlen);

		conndf_rv = writen(p->socketint, header200, strlen(header200));
		if (conndf_rv == -1) return 2;

		size_t size = 1;
		char *temp;
		temp = Malloc(sizeof(char) * size);
		while (read(req_fd, temp, 1)>0)
		{
			conndf_rv = writen(p->socketint, temp, 1);
			if (conndf_rv == -1) return 2;
		}
		close(req_fd);
		free(temp);
		//writen(p->socketint, "\r\n", 2);
		strcpy(p->messages, "File servito");
		print_message(p);
		return 0;
	}

	if (p->return_code == 404)
	{
		conndf_rv = writen(p->socketint, header404, strlen(header404));
		if (conndf_rv == -1) return 2;
	}

	else
	{
		conndf_rv = writen(p->socketint, header400, strlen(header400));
		if (conndf_rv == -1) return 2;
	}

	return 1;
}


char * read_string(int fd)
{
	int n = 0;
	int size = 0;

	char *tmpString = Malloc(sizeof(char) * BUFSIZE);

	n = readn(fd, tmpString, BUFSIZE);
	if (n==-1)
		return NULL;

	size += n;
	tmpString[size -1] = '\0';

	return tmpString;

}


struct httpread * read_request(int fd)
{
	struct httpread * httpr = create_httpread();
	char **tmpArray = httpr->array;
	int n = 0;
	int index = 0;
	char *tmpString = malloc(sizeof(char)*300);


	while(1)
	{
		//------------
		n = readn(fd, tmpString+index, 1);
		if (n==-1)
		{
			httpr->dimArray = -1;
			break;
		}
		if (n==0)
			break;

		if (index==1)
		{
			if ((*(tmpString+index) == '\n')&&(*(tmpString+index-1) == '\r')) break;
		}

		if (*(tmpString+index) == '\n') {

			if (*(tmpString+index-1) != '\r')
			{
				//free(tmpString);
				//destroy_httpread(httpr);
				break;
				//return NULL;
			}

			*(tmpString+index) = '\0';
			*(tmpString+index-1) = '\0';
			*(tmpArray) = tmpString; //copio
			(httpr->dimArray)++;
			httpr->array = realloc(httpr->array, sizeof(char *)*(httpr->dimArray+1));
			tmpArray = httpr->array + httpr->dimArray;
			tmpString = malloc(sizeof(char)*300);
			index = 0;
		}

		else index++;



	}

	free(tmpString);
	return httpr;

}
