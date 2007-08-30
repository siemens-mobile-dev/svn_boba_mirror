#include "..\inc\swilib.h"
#include "http.h"

int ParseHeaders(char *buf, int len, HTTP_RESPONSE *result)
{
  int l_len = 2; // Длина текущей строки (учитывая CRLF)
  char l_buf[512]; // Буфер текущей строки
  char h_name[128]; // Имя текущего заголовка
  int header_id; // ID соответствующий имени хедера
  char *h_value; // Значение текущего заголовка

  zeromem(result, sizeof(HTTP_RESPONSE));

  while (!(buf[l_len-1]=='\n'&&buf[l_len-2]=='\r') && (l_len<len))
    l_len++;
  if (!(buf[l_len-1]=='\n'&&buf[l_len-2]=='\r'))
    return 0;
  strncpy(l_buf, buf, l_len-2);
  l_buf[l_len-2] = 0;
  if (sscanf(l_buf, "HTTP/%d.%d %d", &result->http_ver_major, &result->http_ver_minor, &result->resp_code)<3)
    return 0;
  if (strlen(l_buf)>13)
  {
    result->resp_msg = (char *)malloc(strlen(l_buf)-13+1);
    strcpy(result->resp_msg, l_buf+13);
  }
  buf += l_len;
  len -= l_len;

  do
  {
    l_len = 2;
    while (!(buf[l_len-1]=='\n'&&buf[l_len-2]=='\r') && (l_len<len))
      l_len++;
    if (!(buf[l_len-1]=='\n'&&buf[l_len-2]=='\r'))
      return 0;
    if (l_len==2)
      return 1;
    strncpy(l_buf, buf, l_len-2);
    l_buf[l_len-2] = 0;
    h_value = (char *)malloc(l_len);
    if (!SplitHeader(l_buf, h_name, h_value))
    {
      mfree(h_value);
      return 0;
    }

    header_id = GetHeaderId(h_name);
    if (header_id!=-1)
    {
      switch(header_id)
      {
      case H_Accept_Ranges:
        result->resp_headers.Accept_Ranges = h_value;
        break;

      case H_Age:
        result->resp_headers.Age = h_value;
        break;

      case H_Cache_Control:
        result->gen_headers.Cache_Control = h_value;
        break;

      case H_Connection:
        result->gen_headers.Connection = h_value;
        break;

      case H_Content_Encoding:
        result->ent_headers.Content_Encoding = h_value;
        break;

      case H_Content_Language:
        result->ent_headers.Content_Language = h_value;
        break;

      case H_Content_Length:
        result->ent_headers.Content_Length = h_value;
        break;

      case H_Content_Location:
        result->ent_headers.Content_Location = h_value;
        break;

      case H_Content_MD5:
        result->ent_headers.Content_MD5 = h_value;
        break;

      case H_Content_Range:
        result->ent_headers.Content_Range = h_value;
        break;

      case H_Content_Type:
        result->ent_headers.Content_Type = h_value;
        break;

      case H_Date:
        result->gen_headers.Date = h_value;
        break;

      case H_ETag:
        result->resp_headers.ETag = h_value;
        break;

      case H_Expires:
        result->ent_headers.Expires = h_value;
        break;

      case H_Last_Modified:
        result->ent_headers.Last_Modified = h_value;
        break;

      case H_Location:
        result->resp_headers.Location = h_value;
        break;

      case H_Pragma:
        result->gen_headers.Pragma = h_value;
        break;

      case H_Proxy_Authenticate:
        result->resp_headers.Proxy_Authenticate = h_value;
        break;

      case H_Retry_After:
        result->resp_headers.Retry_After = h_value;
        break;

      case H_Server:
        result->resp_headers.Server = h_value;
        break;

      case H_Trailer:
        result->gen_headers.Trailer = h_value;
        break;

      case H_Transfer_Encoding:
        result->gen_headers.Transfer_Encoding = h_value;
        break;

      case H_Upgrade:
        result->gen_headers.Upgrade = h_value;
        break;

      case H_Vary:
        result->resp_headers.Vary = h_value;
        break;

      case H_Via:
        result->gen_headers.Via = h_value;
        break;

      case H_WWW_Authenticate:
        result->resp_headers.WWW_Authenticate = h_value;
        break;

      case H_Warning:
        result->gen_headers.Warning = h_value;
        break;
      }
    }
    buf += l_len;
    len -= l_len;
  } while (l_len>2);
  return 1;
}

void FreeHTTPMem(HTTP_RESPONSE *resp)
{
  if (resp->resp_msg)
    mfree(resp->resp_msg);

  if (resp->resp_headers.Accept_Ranges)
    mfree(resp->resp_headers.Accept_Ranges);

  if (resp->resp_headers.Age)
    mfree(resp->resp_headers.Age);

  if (resp->gen_headers.Cache_Control)
    mfree(resp->gen_headers.Cache_Control);

  if (resp->gen_headers.Connection)
    mfree(resp->gen_headers.Connection);

  if (resp->ent_headers.Content_Encoding)
    mfree(resp->ent_headers.Content_Encoding);

  if (resp->ent_headers.Content_Language)
    mfree(resp->ent_headers.Content_Language);

  if (resp->ent_headers.Content_Length)
    mfree(resp->ent_headers.Content_Length);

  if (resp->ent_headers.Content_Location)
    mfree(resp->ent_headers.Content_Location);

  if (resp->ent_headers.Content_MD5)
    mfree(resp->ent_headers.Content_MD5);

  if (resp->ent_headers.Content_Range)
    mfree(resp->ent_headers.Content_Range);

  if (resp->ent_headers.Content_Type)
    mfree(resp->ent_headers.Content_Type);

  if (resp->gen_headers.Date)
    mfree(resp->gen_headers.Date);

  if (resp->resp_headers.ETag)
    mfree(resp->resp_headers.ETag);

  if (resp->ent_headers.Expires)
    mfree(resp->ent_headers.Expires);

  if (resp->ent_headers.Last_Modified)
    mfree(resp->ent_headers.Last_Modified);

  if (resp->resp_headers.Location)
    mfree(resp->resp_headers.Location);

  if (resp->gen_headers.Pragma)
    mfree(resp->gen_headers.Pragma);

  if (resp->resp_headers.Proxy_Authenticate)
    mfree(resp->resp_headers.Proxy_Authenticate);

  if (resp->resp_headers.Retry_After)
    mfree(resp->resp_headers.Retry_After);

  if (resp->resp_headers.Server)
    mfree(resp->resp_headers.Server);

  if (resp->gen_headers.Trailer)
    mfree(resp->gen_headers.Trailer);

  if (resp->gen_headers.Transfer_Encoding)
    mfree(resp->gen_headers.Transfer_Encoding);

  if (resp->gen_headers.Upgrade)
    mfree(resp->gen_headers.Upgrade);

  if (resp->resp_headers.Vary)
    mfree(resp->resp_headers.Vary);

  if (resp->gen_headers.Via)
    mfree(resp->gen_headers.Via);

  if (resp->resp_headers.WWW_Authenticate)
    mfree(resp->resp_headers.WWW_Authenticate);

  if (resp->gen_headers.Warning)
    mfree(resp->gen_headers.Warning);
}

