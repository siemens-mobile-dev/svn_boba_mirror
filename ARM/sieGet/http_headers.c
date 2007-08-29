#include "..\inc\swilib.h"
#include "http_headers.h"

typedef struct
{
  int header_id;
  char *header_name;
} HTTP_HEADER_DESC;

HTTP_HEADER_DESC resp_headers[] =
{
  {H_Accept_Ranges     , "Accept-Ranges"},
  {H_Age               , "Age"},
  {H_Cache_Control     , "Cache-Control"},
  {H_Connection        , "Connection"},
  {H_Content_Encoding  , "Content-Encoding"},
  {H_Content_Language  , "Content-Language"},
  {H_Content_Length    , "Content-Length"},
  {H_Content_Location  , "Content-Location"},
  {H_Content_MD5       , "Content-MD5"},
  {H_Content_Range     , "Content-Range"},
  {H_Content_Type      , "Content-Type"},
  {H_Date              , "Date"},
  {H_ETag              , "ETag"},
  {H_Expires           , "Expires"},
  {H_Last_Modified     , "Last-Modified"},
  {H_Location          , "Location"},
  {H_Pragma            , "Pragma"},
  {H_Proxy_Authenticate, "Proxy-Authenticate"},
  {H_Retry_After       , "Retry-After"},
  {H_Server            , "Server"},
  {H_Trailer           , "Trailer"},
  {H_Transfer_Encoding , "Transfer-Encoding"},
  {H_Upgrade           , "Upgrade"},
  {H_Vary              , "Vary"},
  {H_Via               , "Via"},
  {H_WWW_Authenticate  , "WWW-Authenticate"},
  {H_Warning           , "Warning"}
};

int GetHeaderId(char *header_name)
{
  int l = 0;
  int r = N_RESP_HEADERS-1;
  while (l<=r)
  {
    int m = (l+r)>>1;
    int cmp_res = strcmp(resp_headers[m].header_name, header_name);
    if (cmp_res==0) return resp_headers[m].header_id;
    if (cmp_res>0)
      r = m-1;
    else
      l = m+1;
  }
  return -1;
}

int SplitHeader(char *header, char *name, char *value)
{
  int len = strlen(header);
  int delim_pos = strchr(header, ':')-header;
  if (delim_pos!=0 && delim_pos<len-1)
  {
    strncpy(name, header, delim_pos);
    name[delim_pos] = 0;
    int vstart_pos = delim_pos+1;
    while (vstart_pos<len && header[vstart_pos]==' ')
      vstart_pos++;
    strncpy(value, header+vstart_pos, len-vstart_pos+1);
    return 1;
  }
  else
    return 0;
}

