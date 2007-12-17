#ifndef _URL_H_
#define _URL_H_

// scheme://host:port/path?param#fragment
class URL
{
public:
  char *Print();
  int Parse(char *s_url);

  char *scheme;
  char *host;
  short port;
  char *path;
  char *param;
  char *fragment;

  URL();
  ~URL();
};

char *FormatUrl(URL *url);

int ParseUrl(char *s_url, URL *url);

void FreeUrlMem(URL *url);

#endif
