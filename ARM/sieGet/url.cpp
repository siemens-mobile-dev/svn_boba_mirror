//#include "stdafx.h"
#include "../inc/swilib.h"
extern unsigned int strtoul(const char * str, char ** endptr, int base);

#include "url.h"

#define ST_ERR -1
#define ST_FINISH 0
#define ST_HOST 1
#define ST_PORT 2
#define ST_PATH 3
#define ST_PARAM 4
#define ST_FRAGMENT 5

char *FormatUrl(URL *url)
{
  return "";
}

char host_term[] = ":/?#";
int  host_st[] = {ST_PORT, ST_PATH, ST_PARAM, ST_FRAGMENT};
char port_term[] = ":/?#";
int  port_st[] = {ST_ERR, ST_PATH, ST_PARAM, ST_FRAGMENT};
char path_term[] = "?#";
int  path_st[] = {ST_PARAM, ST_FRAGMENT};
char param_term[] = "#";
int  param_st[] = {ST_FRAGMENT};

int ParseUrl(char *s_url, URL *url)
{
  char *tmp = s_url;
  char *scheme_delim = strstr(tmp, "://");
  if (scheme_delim)
  {
    int schlen = scheme_delim-tmp;
    url->scheme = (char *)malloc(schlen+1);
    strncpy(url->scheme, tmp, schlen);
    url->scheme[schlen] = 0;
    tmp += schlen+3;
  }
  else
  {
    url->scheme = (char *)malloc(5);
    strcpy(url->scheme, "http");
  }
  int st = ST_HOST;
  int len = strlen(tmp);
  int clen = 0;
  while (st>0)
  {
    switch(st)
    {
    case ST_HOST:
      if (strchr(host_term, tmp[clen]) || !tmp[clen])
      {
        if (tmp[clen])
          st = host_st[strchr(host_term, tmp[clen])-host_term];
        else
          st = ST_FINISH;
        url->host = (char *)malloc(clen+1);
        strncpy(url->host, tmp, clen);
        url->host[clen] = 0;
        tmp += clen+1;
        len -= clen+1;
        clen = 0;
      }
      else
        clen++;
      break;
    case ST_PORT:
      url->port = strtoul(tmp, &tmp, 10);
      if (tmp[clen])
        st = port_st[strchr(port_term, *tmp++)-port_term];
      else
        st = ST_FINISH;
      break;
    case ST_PATH:
      if (strchr(path_term, tmp[clen]) || !tmp[clen])
      {
        if (tmp[clen])
          st = path_st[strchr(path_term, tmp[clen])-path_term];
        else
          st = ST_FINISH;
        url->path = (char *)malloc(clen+1);
        strncpy(url->path, tmp, clen);
        url->path[clen] = 0;
        tmp += clen+1;
        len -= clen+1;
        clen = 0;
      }
      else
        clen++;
      break;
    case ST_PARAM:
      if (strchr(param_term, tmp[clen]) || !tmp[clen])
      {
        if (tmp[clen])
          st = param_st[strchr(param_term, tmp[clen])-param_term];
        else
          st = ST_FINISH;
        url->param = (char *)malloc(clen+1);
        strncpy(url->param, tmp, clen);
        url->param[clen] = 0;
        tmp += clen+1;
        len -= clen+1;
        clen = 0;
      }
      else
        clen++;
      break;
    case ST_FRAGMENT:
      if (!tmp[clen])
      {
        st = ST_FINISH;
        url->fragment = (char *)malloc(clen+1);
        strncpy(url->fragment, tmp, clen);
        url->fragment[clen] = 0;
        tmp += clen+1;
        len -= clen+1;
        clen = 0;
      }
      else
        clen++;
      break;
    }
  }
  if (url->port==0)
    url->port = 80;
  return 1;
}

void FreeUrlMem(URL *url)
{
  if (url->fragment)
    mfree(url->fragment);
  if (url->host)
    mfree(url->host);
  if (url->param)
    mfree(url->param);
  if (url->path)
    mfree(url->path);
  if (url->scheme)
    mfree(url->scheme);
}

