


char *string_lowercase(char *string)
/*
	char *string_lowercase(char *string)

	replaces all uppercase chars in
	a string with lowercase chars
*/
{
  char *p = (char *)string;
  while (*p) { *p = tolower(*p); p++; }
  return string;
}

int file_ext(const char * filename, const char * ext)
/*
	int file_ext(const char * filename, const char * ext)

	if filename has at least len(ext) chars and
	ends with ext, returns true
*/
{

	int fnamelen = strlen(filename);
	int extlen = strlen(ext);
	char *fstr = (char*)filename;
	while (*fstr) {*fstr = tolower(*fstr); ++fstr;}
//	char *estr = (char*)ext;
//	while (*estr) {*estr = tolower(*estr); ++estr;}
	//char *filenamek = string_lowercase(filename);
	//char *extk = string_lowercase(ext);
	return (fnamelen >= extlen) && (0 == strcmp(fstr + (fnamelen-extlen), ext));
}

