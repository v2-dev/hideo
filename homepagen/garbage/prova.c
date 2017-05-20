#include<stdio.h>
#include<string.h>
#include<ctype.h>

char *strlwr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = tolower(*p);
      p++;
  }

  return str;
}

int main()
{
    char str[ ] = "MODIFY This String To LOwer";
    printf("%s\n",strlwr(str));
    return  0;
}
