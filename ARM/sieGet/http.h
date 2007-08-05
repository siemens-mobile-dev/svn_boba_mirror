#ifndef _HTTP_H_
#define _HTTP_H_

typedef struct
{
  char *Cache-Control;
  char *Connection;
  char *Date;
  char *Pragma;
  char *Trailer;
  char *Transfer-Encoding;
  char *Upgrade;
  char *Via;
  char *Warning;
} HTTP_GENERAL_HEADERS;

typedef struct
{
  char *Accept;
  char *Accept-Charset;
  char *Accept-Encoding;
  char *Accept-Language;
  char *Authorization;
  char *Expect;
  char *From;
  char *Host;
  char *If-Match;
  char *If-Modified-Since;
  char *If-None-Match;
  char *If-Range;
  char *If-Unmodified-Since;
  char *Max-Forwards;
  char *Proxy-Authorization;
  char *Range;
  char *Referer;
  char *TE;
  char *User-Agent;
} HTTP_REQUEST_HEADERS;

typedef struct
{
  char *Accept-Ranges;
  char *Age;
  char *ETag;
  char *Location;
  char *Proxy-Authenticate;
  char *Retry-After;
  char *Server;
  char *Vary;
  char *WWW-Authenticate;
} HTTP_RESPONSE_HEADERS;

typedef struct
{
  int http_ver_major;
  int http_ver_minor;
  char[4] resp_code;
  char *resp_msg;
  HTTP_GENERAL_HEADERS gen_headers;
  HTTP_RESPONSE_HEADERS resp_headers;
} HTTP_RESPONSE;

#endif

