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

char *URL::Print()
{
  return NULL;
}

char host_term[] = ":/?#";
int  host_st[] = {ST_PORT, ST_PATH, ST_PARAM, ST_FRAGMENT};
char port_term[] = ":/?#";
int  port_st[] = {ST_ERR, ST_PATH, ST_PARAM, ST_FRAGMENT};
char path_term[] = "?#";
int  path_st[] = {ST_PARAM, ST_FRAGMENT};
char param_term[] = "#";
int  param_st[] = {ST_FRAGMENT};

int URL::Parse(char *s_url)
{
  char *tmp = s_url;
  char *scheme_delim = strstr(tmp, "://");
  if (scheme_delim)
  {
    int schlen = scheme_delim-tmp;
    this->scheme = new char[schlen+1];
    strncpy(this->scheme, tmp, schlen);
    this->scheme[schlen] = 0;
    tmp += schlen+3;
  }
  else
  {
    this->scheme = new char[5];
    strcpy(this->scheme, "http");
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
        this->host = new char[clen+1];
        strncpy(this->host, tmp, clen);
        this->host[clen] = 0;
        tmp += clen+1;
        len -= clen+1;
        clen = 0;
      }
      else
        clen++;
      break;
    case ST_PORT:
      this->port = strtoul(tmp, &tmp, 10);
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
        this->path = new char[clen+1];
        strncpy(this->path, tmp, clen);
        this->path[clen] = 0;
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
        this->param = new char[clen+1];
        strncpy(this->param, tmp, clen);
        this->param[clen] = 0;
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
        this->fragment = new char[clen+1];
        strncpy(this->fragment, tmp, clen);
        this->fragment[clen] = 0;
        tmp += clen+1;
        len -= clen+1;
        clen = 0;
      }
      else
        clen++;
      break;
    }
  }
  if (this->port==0)
    this->port = 80;
  return 1;
}

URL::~URL()
{
  if (this->fragment)
    delete this->fragment;
  if (this->host)
    delete this->host;
  if (this->param)
    delete this->param;
  if (this->path)
    delete this->path;
  if (this->scheme)
    delete this->scheme;
}

URL::URL()
{
  fragment = NULL;
  host = NULL;
  param = NULL;
  path = NULL;
  scheme = NULL;
}

