int conndf_rv;

struct conndata
{
	int process_id;			//tid del thread
	int socketint;			//socket file descriptor
	char path_r[256];		//request path
	char method_r[5];		//request method
	char useragent[256];	//user agent
	char acceptfld[256];	//accept field
	char messages[3000];	//
	int msgtype;			//

	int get1head2;

	int keepalmaxn;
	char imgext[8];			//image extension
	float quality;			//image quality
	char options[3000];		//k
	int return_code;		//Return code
	char return_path[256];	//Return path
	int isImage;			//IS THIS REAL IMAGE? IS THIS JUST FANTASY?
};


static void http_200(int, struct st_trx *);
static void http_404(int, struct st_trx *);
static void http_500(int, struct st_trx *);
static void http_501(int, struct st_trx *);
//static int test_method(struct wb_req *);


static void http_200(int fd, struct conndata *conn)
{
	char buff[1024];

	sprintf(buff, "HTTP/1.0 200 OK\r\n");
	sprintf(buff, "%sServer: xWebserver\r\n", buff);
	sprintf(buff, "%sContent-length:%d\r\n", buff, conn->);
	sprintf(buff, "%sContent-type:%s\r\n\r\n", buff, wb_trx->file_type);

	send_msg(fd, buff);
}

static void http_500(int fd, struct st_trx *wb_trx)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.0 500 INTERNAL SERVER ERROR\r\n");
	sprintf(buff, "%sServer: xWebserver\r\n", buff);

	send_msg(fd, buff);
}

static void http_501(int fd, struct st_trx *wb_trx)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.0 501 NOT IMPLEMENTED\r\n");
	sprintf(buff, "%sServer: xWebserver\r\n", buff);

	send_msg(fd, buff);
}

static void http_404(int fd, struct st_trx *wb_trx)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.0 404 NOT FOUND\r\n");
	sprintf(buff, "%sServer: xWebserver\r\n\r\n", buff);
	sprintf(buff, "%s<html><head><title>404 Not Found</title></head>"
		,buff);
	sprintf(buff, "%s<body><h1>404 File Not found</h1></body></html>\r\n",
		buff);

	send_msg(fd, buff);
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
	/*
	strcpy(p->useragent, "");
	for (buf_idx = 11; buf_idx <= strlen(optstring); buf_idx++) strcat(p->useragent, optstring[buf_idx]);
	*/
	strcpy(p->messages, "Accept = ");
	strcat(p->messages, p->acceptfld);
	print_message(p);
	return 1;

}

int method_parse(char *optstring, struct conndata *p)
{
	/*
	 * Parse del metodo:
	 *
	 * Restituisce
	 * 	-1	errori
	 * 	0	non HEAD, non GET
	 * 	1	HEAD
	 * 	2	GET
	 */

	if (strlen(optstring) < 12) return -1;

	if ( !strncmp(optstring, "GET ", 4) )
	{
		strcpy(p->method_r, "GET");
		strcpy(p->messages, "Metodo GET");
		print_message(p);
		p->get1head2 = 1;
		return 1;
	}

	else if ( !strncmp(optstring, "HEAD ", 5) )
	{
		strcpy(p->method_r, "HEAD");
		strcpy(p->messages, "Metodo HEAD");
		print_message(p);
		p->get1head2 = 2;
		return 2;
	}
	p->get1head2 = 0;
	strcpy(p->method_r, "");
	strcpy(p->messages, "Metodo diverso da GET o HEAD");
	/*return BAD_REQUEST*/
	return 0;
}
int path_parse(char *optstring, struct conndata *p)
{
	/*
	 * Parse del path
	 *
	 * restituisce 0 se non ci sono errori
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
			return 1;
		}

	strcpy(p->messages, "Header HTTP OK");
	print_message(p);

	strcpy(p->messages, "Path richiesto: ");
	strcat(p->messages, p->path_r);
	print_message(p);

	//strcpy(p->path_r, path_r);
	return 0;
}
/*
int options_parse(struct conndata *p)
{
	//
	 * Parse del campo options
	 * Ritorna 0 in assenza di errori.
	//

	//strcpy(p->options, "");

	size_t buf_idx = 0;
	char buf[400] = { 0 };
	char endl[] = "\r\n";
	unsigned int i = 0;
	//char options[3000];
	strcpy(p->acceptfld, "");
	strcpy(p->useragent, "");
	for (int exitv = 0; exitv < 150; exitv++)
	//
	// Parse di 150 linee di opzioni (Apache 2.3 per sicurezza
	// pone questo valore a circa 100 richieste)
	//
	{
		buf_idx = 0;
		read(p->socketint, &buf[0], 2);
		buf_idx = 2;
		if ((buf[0] == endl[0]) && (buf[1] == endl[1])) break;


		for (;;)
		{
			read(p->socketint, &buf[buf_idx], 1);
			if (buf[buf_idx - 1] == endl[0] && buf[buf_idx] == endl[1])
			{
				buf[buf_idx - 1] = '\0';
				buf[buf_idx] = '\0';
				buf_idx--;
				if ( accheck(buf) == 1)
				{
					int sbuf_idx = 7;
					if (buf[7] == ' ') sbuf_idx++;
					for (i = 0;i<buf_idx-sbuf_idx;i++) buf[i] = buf[sbuf_idx+i];
					buf[buf_idx-sbuf_idx] = '\0';
					strcpy(p->acceptfld, buf);
				}
				if ( uacheck(buf) == 1)
				{
					int sbuf_idx = 11;
					if (buf[11] == ' ') sbuf_idx++;
					for (i = 0;i<buf_idx-sbuf_idx;i++) buf[i] = buf[sbuf_idx+i];
					buf[buf_idx-sbuf_idx] = '\0';
					strcpy(p->useragent, buf);
				}

				break;
			}
			buf_idx++;
		}
	}
	//printf("\nEnd of HTTP GET/HEAD! All options are:");
	//printf("%s", p->options);
	strcpy(p->messages, "User Agent: ");
	strcat(p->messages, p->useragent);
	print_message(p);
	strcpy(p->messages, "Campo Accept: ");
	strcat(p->messages, p->acceptfld);
	print_message(p);
	return 0;

}
*/
int send_response(struct conndata *p)
{
	//char *header200html = "HTTP/1.1 200 OK\nServer: ProgettoIIW-IS\nContent-type: text/html\n";

	//char *header200gif = "HTTP/1.1 200 OK\nServer: ProgettoIIW-IS\nContent-type: image/gif\n";
	//char *header200html = "HTTP/1.1 200 OK\nServer: ProgettoIIW-IS\nContent-type: text/html\n\r\n";
	//char *header200gif = "HTTP/1.1 200 OK\nServer: ProgettoIIW-IS\nContent-type: image/gif\n\r\n";

	char header200[150] = "HTTP/1.1 200 OK\r\nServer: ProgettoIIWIS\r\nConnection: keep-alive\r\nContent-type: ";
	//char *header400 = "HTTP/1.1 400 Bad Request\nServer: ProgettoIIW-IS\nContent-type: text/html\r\n";
	//char *header404 = "HTTP/1.1 404 Not Found\nServer: ProgettoIIW-IS\nContent-type: text/html\r\n";
	char *header400 = "HTTP/1.1 400 Bad Request\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";
	char *header404 = "HTTP/1.1 404 Not Found\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";
	//char *keepalive = "Connection: keep-alive\r\n";
	int req_fd;
	p->return_code = 400;

	//printf("\n\tPath =#%s#", p->path_r);
	if ( p->path_r[0] == '/' && (p->path_r[1] == '\0') ) strcpy(p->path_r, "/index.html");
	//printf("\n\tPath =#%s#", p->path_r);

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
		temp = malloc(sizeof(char) * size);
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
