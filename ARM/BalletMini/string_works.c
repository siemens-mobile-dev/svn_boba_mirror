#include "../inc/swilib.h"
#include "string_works.h"

int strcmp_nocase(const char *s, const char *d)
{
  int cs;
  int ds;
  do
  {
    cs=*s++;
    if (cs&0x40) cs&=0xDF;
    ds=*d++;
    if (ds&0x40) ds&=0xDF;
    cs-=ds;
    if (cs) break;
  }
  while(ds);
  return(cs);
}

char *globalstr(const char *s)
{
  int l=strlen(s)+1;
  char *d=malloc(l);
  memcpy(d,s,l);
  return d;
}

void freegstr(char **ps)
{
  mfree(*ps);
  *ps=NULL;
}

char *replacegstr(char **ps, const char *s, int len)
{
  freegstr(ps);
  char *d=malloc(len+1);
  memcpy(d,s,len);
  d[len]=0;
  return *ps=d;
}



