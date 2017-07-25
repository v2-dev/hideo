#include "libhttp.h"
#include "mimetypes.h"
#include "utils.h"
#include "cacher.h"
#include "resolutionDevice.h"
#include "logger.h"


void http_generic(struct conndata *conn, char *httpmsg)
{
	size_t len;
	char *buf;

	len = strlen(httpmsg);
	buf = Malloc(len);

	sprintf(buf, "%s", httpmsg);
	send_msg(conn->socketint, buf);
}


void http_200(struct conndata *conn)
{

	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 200 OK\r\n");
	sprintf(buff, "%sServer: Hideo\r\n", buff);
	sprintf(buff, "%s<html><head><title>200 OK</title></head>", buff);
	sprintf(buff, "%s<body><h1>Everything is fine here!</h1></body></html>\r\n", buff);
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
	sprintf(buff, "%s<html><head><title>501 NOT IMPLEMENTED</title></head>", buff);
	sprintf(buff, "%s<body><h1>The method or request you made is not implemented</h1></body></html>\r\n", buff);

	send_msg(conn->socketint, buff);
}

void http_404(struct conndata *conn)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 404 NOT FOUND\r\n");
	sprintf(buff, "%sServer: Hideo\r\n\r\n", buff);
	sprintf(buff, "%s<html><head><title>404 Not Found</title></head>", buff);
	sprintf(buff, "%s<body><h1>404 File Not found</h1></body></html>\r\n", buff);

	send_msg(conn->socketint, buff);
}


void http_400(struct conndata *conn)
{
	char buff[DATLEN];

	sprintf(buff, "HTTP/1.1 404 BAD REQUEST\r\n");
	sprintf(buff, "%sServer: Hideo\r\n\r\n", buff);
	sprintf(buff, "%s<html><head><title>400 bad request</title></head>", buff);

	send_msg(conn->socketint, buff);
}


int find_quality(char *token)
{
	char *begin;
	char *c;
	char num[10];
	char *last;
	int q;
	float allq;

	begin = strstr(token, "q=");
	if (begin == NULL)
		return 100;

	c = begin + 2;
	/*We assume the default value of q=1 */
	if (*c == '1' || *c != '0')
		return 100;

	last = strchr(begin, '.');
	if (last == NULL) {
		return 100;
	}

	c = last + 1;
	strcpy(num, "0.");

	int count = 0;

	while (isdigit(*c)) {
		c++;
		count += 1;
	}

	c = last + 1;
	int i = 0;

	char *p;
	p = num + 2;

	while (i < count) {
		*(p + i) = *(c + i);
		i++;
	}

	*(p + count) = '\0';

	allq = atof(num);
	q = (float) allq *100;

	return q;
}


char *get_ext(char *mystr)
{
	char *retstr, *lastdot;
	char dot = '.';
	if (mystr == NULL)
		return NULL;
	if ((retstr = malloc(strlen(mystr) + 1)) == NULL)
		return NULL;
	lastdot = strrchr(retstr, dot);
	strcpy(retstr, "");
	sscanf(++lastdot, "%s", retstr);
	return retstr;
}

void find_extension(char *token, char *extension)
{
	char *endptr;
	int i;

	for (i = 0; i < 70; i++) {
		endptr = strstr(token, mimetypes[i][0]);
		if (endptr != NULL)
			break;
	}

	if (endptr == NULL) {
		return;
	}

	int len = strlen(mimetypes[i][1]);
	strcpy(extension, mimetypes[i][1]);
	extension[len + 1] = '\0';
}


char *get_mimetype(char *pathstr)
{
	char extension[5] = "";

	char *ext;
	ext = strrchr(pathstr, '.');
	strcpy(extension, ext + 1);

	if ((strcmp(extension, "html") == 0) || (strcmp(extension, "htm") == 0))
		return "text/html";
	int i;
	for (i = 0; i < 70; i++) {
		if (strcmp(extension, mimetypes[i][1]) == 0)
			return mimetypes[i][0];
	}
	return "text/html";
}


struct conndata *create_conndata(void)
{
	struct conndata *lt;
	lt = Malloc(sizeof(struct conndata));
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
	while (buf_idx < 11) {
		if (buf_idx > 0 && ua_head[buf_idx] != optstring[buf_idx])
			return 0;
		buf_idx++;
	}
	strcpy(p->useragent, optstring + 12);
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
	while (buf_idx < 7) {
		if (buf_idx > 0 && ua_head[buf_idx] != optstring[buf_idx])
			return 0;
		buf_idx++;
	}

	strcpy(p->acceptfld, optstring + 8);

	fprintf(stdout, "ACCEPT FIELD HERE ---> %s\n", p->acceptfld);

	char *strg = p->acceptfld;

	p->extension = malloc(10 * sizeof(char));
	int len = strlen("png");
	strcpy(p->extension, "png");
	p->extension[len + 1] = '\0';

	strg = strstr(p->acceptfld, "*/*");	//check if "*/*" subtoken does exist
	if (strg != NULL)
		p->quality_factor = find_quality(strg);	//find quality
	/*returns the pointer to image/ */
	strg = strstr(p->acceptfld, "image/");
	if (strg != NULL) {
		find_extension(strg, p->extension);
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
	 *              0       not HEAD, nor GET or error
	 *              1       GET
	 *              2       HEAD
	 */
	if (!strncmp(optstring, "GET ", 4)) {
		strcpy(p->method, "GET");
		p->get1head2 = 1;
		return 1;
	}

	else if (!strncmp(optstring, "HEAD ", 5)) {
		strcpy(p->method, "HEAD");
		p->get1head2 = 2;
		return 2;
	}
	p->get1head2 = 0;
	strcpy(p->method, "");

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
	if (p->get1head2 == 2)
		choffset++;
	for (i = 0; choffset < strlen(optstring); i++) {
		//printf("\n%d %s", i, optstring+choffset + i);
		if (optstring[choffset + i] == ' ') {
			p->path[i] = '\0';
			choffset = choffset + i;
			break;
		}
		p->path[i] = optstring[choffset + i];
	}

	char header[] = "HTTP/1.1";
	if (!strncmp(optstring + choffset, header, 8)) {
		strcpy(p->messages, "Header HTTP mancante od incompleto!");
		print_message(p);
		return ERROR;
	}

	strcpy(p->messages, "Header HTTP OK");
	http_200(p);
	print_message(p);

	strcpy(p->messages, "Path richiesto: ");
	strcat(p->messages, p->path);
	print_message(p);

	//strcpy(p->path_r, path_r);
	return 0;
}


void *create_httpread()
{
	struct httpread *httpr;
	httpr = malloc(sizeof(struct httpread));

	if (httpr == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		exit(EXIT_FAILURE);
	}

	httpr->dimArray = 0;
	httpr->array = malloc(sizeof(char *));

	return httpr;
}

void destroy_httpread(struct httpread *httpr)
{
	int i;
	for (i = 0; i < httpr->dimArray; i++) {
		free(*(httpr->array + i));
	}
	free(httpr->array);
	free(httpr);
}


int serve_request(struct conndata *cdata)
{
	struct httpread *httpr;
	httpr = read_request(cdata->socketint);

	/* Nothing read */
	if (httpr->dimArray == 0) {
		destroy_httpread(httpr);
		printf("Niente da leggere\n");
		return ERROR;
	}

	if (httpr->dimArray == -1) {
		destroy_httpread(httpr);
		return ERROR;
	}

	if (method_parse(*(httpr->array), cdata) == ERROR) {
		http_400(cdata);
		destroy_httpread(httpr);
		return ERROR;
	}

	if (path_parse(*(httpr->array), cdata) == ERROR) {
		http_400(cdata);
		destroy_httpread(httpr);
		return ERROR;
	}
	//strcpy(cdata->messages, *(httpr->array));
	//print_message(cdata);
	int i;
	int v = 0, w = 0;

	for (i = 1; i < httpr->dimArray; i++) {
		printf("accheck iniziata\n");
		v += accheck(*(httpr->array + i), cdata);
		printf("accheck completata\n");
		w += uacheck(*(httpr->array + i), cdata);
	}

	//accept field OR user agent missing --> BAD REQUEST
	if ((v == 0) || (w == 0)) {
		http_400(cdata);
		destroy_httpread(httpr);
		return ERROR;
	}

	if (send_response(cdata) != 0)
		return ERROR;

	return 1;

}


int send_response(struct conndata *p)
{
	char header200[200] = "HTTP/1.1 200 OK\r\nServer: ProgettoIIWIS\r\nConnection: keep-alive\r\nContent-type: ";
	char *header400 = "HTTP/1.1 400 Bad Request\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";
	char *header404 = "HTTP/1.1 404 Not Found\r\nServer: ProgettoIIWIS\r\nConnection: close\r\n\r\n";

	//se ho selezionato la root
	if (p->path[0] == '/' && (p->path[1] == '\0'))
		strcpy(p->path, "/index.html");

	int req_fd = 0;
	p->return_code = 400;
	int cache_set = 0;
	int x, y, len;
	char mypath[300];
	char *m;
	int fileNotFound = 0;

	if (strncmp("/res", p->path, 4) == 0) {

		cache_set = 1;
		strcpy(mypath, "homepage");
		strcat(mypath, p->path);
		wurflrdt(hwurfl, p->useragent, &x, &y);

		m = obtain_file(web_cache, mypath, p->extension, x, y, p->quality_factor, &len);
		if (m == NULL) {
			fileNotFound = 1;
		}
	}

	else {
		char testpath[300] = "homepage";
		strcat(testpath, p->path);
		strcpy(p->path, testpath);
		req_fd = open(p->path, O_RDONLY);
		if (req_fd == -1)
			fileNotFound = 1;
	}


	if (fileNotFound) {
		strcpy(p->messages, "File ");
		strcat(p->messages, p->path);
		strcat(p->messages, " non trovato, invio header 404\n");
		print_message(p);
		http_404(p);
		fprintf(stderr, "\nNon presente il file: %s", p->path);
	}

	else {

		unsigned int contlen;

		if (cache_set) {
			contlen = (unsigned int) len;

		}

		else {
			struct stat st;
			stat(p->path, &st);
			contlen = st.st_size;
		}

		//CONTROLLO DELL'ESTENSIONE
		char mimetype_t[30] = "";
		strcpy(mimetype_t, get_mimetype(p->path));
		strcat(header200, mimetype_t);
		strcat(header200, "\r\nContent-Length: ");

		char scontlen[15];

		sprintf(scontlen, "%d\r\n\r\n", contlen);
		strcat(header200, scontlen);

		conndf_rv = writen(p->socketint, header200, strlen(header200));
		if (conndf_rv == -1) {
			if (cache_set) {
				releaseFile(web_cache, mypath, p->extension, x, y, p->quality_factor);
			} else
				close(req_fd);
			return 2;
		}

		if (p->get1head2 == 2) {
			if (cache_set) {
				releaseFile(web_cache, mypath, p->extension, x, y, p->quality_factor);
			} else
				close(req_fd);
			strcpy(p->messages, "Inviato solo l'Header, metodo HEAD richiesto");
			print_message(p);
		}


		char *bytesToSend = Malloc(sizeof(char) * contlen);
		if (cache_set) {
			conndf_rv = writen(p->socketint, m, contlen);
			Free(bytesToSend);
			releaseFile(web_cache, mypath, p->extension, x, y, p->quality_factor);
			if (conndf_rv == -1) {
				return 2;
			}
		}

		else {
			readn(req_fd, bytesToSend, contlen);
			conndf_rv = writen(p->socketint, bytesToSend, contlen);
			Free(bytesToSend);
			close(req_fd);
			if (conndf_rv == -1)
				return 2;
		}

		strcpy(p->messages, "File servito");
		print_message(p);
		return 0;
	}

	if (p->return_code == 404) {
		conndf_rv = writen(p->socketint, header404, strlen(header404));
		if (conndf_rv == -1)
			return 2;
	}

	else {
		conndf_rv = writen(p->socketint, header400, strlen(header400));
		if (conndf_rv == -1)
			return 2;
	}

	return 1;
}


char *read_string(int fd)
{
	int n = 0;
	int size = 0;

	char *tmpString = Malloc(sizeof(char) * BUFSIZE);

	n = readn(fd, tmpString, BUFSIZE);
	if (n == -1)
		return NULL;

	size += n;
	tmpString[size - 1] = '\0';

	return tmpString;

}


struct httpread *read_request(int fd)
{
	struct httpread *httpr = create_httpread();
	char **tmpArray = httpr->array;
	int n = 0;
	int index = 0;
	char *tmpString = malloc(sizeof(char) * 300);


	while (1) {
		//------------
		n = readn(fd, tmpString + index, 1);
		if (n == -1) {
			printf("SOCKET 1\n");
			httpr->dimArray = -1;
			break;
		}
		if (n == 0) {
			printf("SOCKET 2\n");
			httpr->dimArray = -1;
			break;
		}

		if (index == 1) {
			printf("SOCKET 3\n");
			if ((*(tmpString + index) == '\n') && (*(tmpString + index - 1) == '\r'))
				break;
		}

		if (*(tmpString + index) == '\n') {
			printf("SOCKET 4\n");
			if (*(tmpString + index - 1) != '\r') {
				printf("SOCKET 5\n");
				//free(tmpString);
				//destroy_httpread(httpr);
				break;
				//return NULL;
			}

			*(tmpString + index) = '\0';
			*(tmpString + index - 1) = '\0';
			*(tmpArray) = tmpString;	//copio
			(httpr->dimArray)++;
			httpr->array = realloc(httpr->array, sizeof(char *) * (httpr->dimArray + 1));
			tmpArray = httpr->array + httpr->dimArray;
			tmpString = malloc(sizeof(char) * 300);
			index = 0;
		}

		else {
			index++;
			printf("SOCKET 6\n");
		}
	}


	free(tmpString);
	return httpr;

}
