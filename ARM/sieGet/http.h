#ifndef _HTTP_H_
#define _HTTP_H_

#include "http_headers.h"

typedef struct
{
  int http_ver_major;
  int http_ver_minor;
  int resp_code;
  char *resp_msg;
  HTTP_GENERAL_HEADERS gen_headers;
  HTTP_RESPONSE_HEADERS resp_headers;
  HTTP_ENTITY_HEADERS ent_headers;
} HTTP_RESPONSE;

int ParseHeaders(char *buf, int len, HTTP_RESPONSE *result);
void FreeHTTPMem(HTTP_RESPONSE *resp);

#endif

