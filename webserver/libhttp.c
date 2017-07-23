#include "libhttp.h"
#include "mimetypes.h"
#include "utils.h"
#include "cacher.h"
//#include "resolutionDevice.h" TOLTO PER GIULIA: DA RIMETTERE DOPO



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


int find_quality(char *token)
{
	char *begin;
	char *c;
	char *num;
	char *last;
	int q;
	float allq;

	begin = strstr(token, "q=");
	if(begin == NULL)
		return 1;
	fprintf(stdout, "%s\n", begin + 2);

	c = begin+2;
	/*We assume the default value of q=1*/
	if(*c == '1' || *c != '0')
		return 1;

	last = strchr(begin, '.');
	if(last == NULL)
		return 1;

	c = last + 1;

	num = malloc(10 * sizeof(char));
	strcpy(num, "0.");

	int count = 0;

	while(isdigit(*c)){
		fprintf(stdout, "digit %c", *c);
		c++;
		count += 1;
		fprintf(stdout, "number of digits %d\n", count);
	}
	c = last +1;
	int i = 0;

	char *p;
	p = num + 2;

	while(i < count){
		printf("My char %c\n", *c);
		*(p + i) = *(c + i);
		i++;
	}
	*(p + count) = '\0';

	allq = atof(num);

	q = (float) allq*100;
	fprintf(stdout, "integer %d\n", q);

	return q;
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

void find_extension(char *token)
{
	char *endptr;
	int i;

	for(i = 0; i < 70; i++){
		endptr = strstr(token, mimetypes[i][0]);
		if(endptr != NULL)
			break;
	}

	if(endptr == NULL){
		return;
	}

	fprintf(stdout, "Mimetypes %s and extension %s\n", mimetypes[i][0], mimetypes[i][1]);
	int len = strlen(mimetypes[i][1]);
	char * extn = malloc(len + 1);
	strcpy(extn, mimetypes[i][1]);
	extn[len + 1] = '\0';
	fprintf(stdout, "Extension %s\n", extn);
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
void quality_extension_on_accept(char *accept_string, struct conndata *p)
{

	accept_string = accept_string;	/* to avoid unused compiler warning */
	p = p;

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

	fprintf(stdout, "ACCEPT FIELD HERE ---> %s\n", p->acceptfld);

	char * strg = p->acceptfld;

	strg = strstr(p->acceptfld, "*/*"); //check if "*/*" subtoken does exist
	if(strg != NULL)
		p->quality_factor= find_quality(strg);  //find quality
		/*returns the pointer to image/*/
	strg = strstr(p->acceptfld, "image/");
	if(strg != NULL)
	{
		find_extension(strg);
		p->quality_factor = find_quality(strg);
	}

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
	int v = 0, w = 0;

	for(i = 1; i<httpr->dimArray; i++)
	{
		v += accheck(*(httpr->array+i), cdata);
		w += uacheck(*(httpr->array+i), cdata);
	}

	//accept field OR user agent missing --> BAD REQUEST
	if((v == 0) || (w == 0))
	{
		http_400(cdata);
		destroy_httpread(httpr);
		return ERROR;
	}

	if (send_response(cdata) != 0)
		return ERROR;

	return 1;

	return SUCCESS;
}


int send_response(struct conndata *p)
{

	char header200[150] = "HTTP/1.1 200 OK\r\nServer: ProgettoIIWIS\r\nConnection: keep-alive\r\nContent-type: ";
	//char *header400 = "HTTP/1.1 400 Bad Request\nServer: ProgettoIIW-IS\nContent-type: text/html\r\n";
	//char *header404 = "HTTP/1.1 404 Not Found\nServer: ProgettoIIW-IS\nContent-type: text/html\r\n";
	char *header400 = "HTTP/1.1 400 Bad Request\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";
	char *header404 = "HTTP/1.1 404 Not Found\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";
	//char *keepalive = "Connection: keep-alive\r\n";
	int req_fd = 0;
	p->return_code = 400;

	printf("\n\tPath =#%s#", p->path_r);
	if ( p->path_r[0] == '/' && (p->path_r[1] == '\0') ) strcpy(p->path_r, "/index.html");
	printf("\n\tPath =#%s#", p->path_r);
	int x, y;

	int cache_set = 0;
	int len;
	char mypath[300];
	char * m;
	if (strncmp("/res", p->path_r, 4)==0){
		cache_set = 1;
		printf("PRIMO FLUSSO\n");
		strcpy(mypath, "homepage");
		strcat(mypath, p->path_r);
		printf("RES: %s\n", mypath);

		//wurflrdt(hwurfl, p->useragent, &x, &y); TOLTO PER GIULIA: DA RIMETTERE DOPO E TOGLIERE LE 2 RIGHE QUI SOTTO
		x = 1024;
		y = 768;
		printf("x: %d, y: %d\n", x, y);
		m = obtain_file(web_cache, mypath, "jpg", x, y, 100, &len);
		if (m == MAP_FAILED){
			fprintf(stderr,"libhttpc error obtain file\n");
		}
		else {
			printf("pointer: %p\n", m);
			printf("SIZE: %d\n", len);
		}
		printf("UserAgent: %s\n",p->useragent);
	}

	else {
		printf("SECONDO FLUSSO\n");
		//TEST
		char testpath[300] = "homepage";
		strcat(testpath, p->path_r);
		strcpy(p->path_r, testpath);
		req_fd = open(p->path_r, O_RDONLY);
	}


	if (req_fd == -1 )
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
		printf("\nARRIVATO QUA\n");
		p->return_code = 200;
		unsigned int contlen;

		if (cache_set){
			contlen = (unsigned int) len;

		}

		else{
			struct stat st;
			stat(p->path_r, &st);
			contlen = st.st_size;
		}

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
		if (conndf_rv == -1) {
			if (cache_set){
				releaseFile(web_cache, mypath, "jpg", x, y, 100);
			}

			else close(req_fd);


			return 2;

		}

		size_t size = 1;
		char *temp;
		temp = Malloc(sizeof(char) * size);
		if (cache_set){
			char c;
			for (int i=0; i< len; i++){
				c = *(m+i);
				conndf_rv = writen(p->socketint, &c, 1);
				if (conndf_rv == -1) {
					printf("lol\n");
					return 2;
				}
			}

		}

		else{
		while (read(req_fd, temp, 1)>0)
		{
			conndf_rv = writen(p->socketint, temp, 1);
			if (conndf_rv == -1) return 2;
		}
		}

		if (cache_set){
			printf("\nInit release\n");
			fflush(stdout);
			releaseFile(web_cache, mypath, "jpg", x, y, 100);
		}
		else{
			close(req_fd);
		}
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
