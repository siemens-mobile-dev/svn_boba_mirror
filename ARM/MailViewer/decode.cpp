#include "..\inc\swilib.h"
#include "base64.h"
#include "sf_mime.h"

char * base64_decode(const char *str, size_t *size)
{
  char *output, *ou;
  unsigned int pr[6];
  int n;
  int doit;
  
  if(str == NULL)
    str = "";
  ou = output = malloc((size?*size:strlen(str)) + 1);
  if(output == NULL)
    /* ENOMEM? */
    return NULL;
  
  if(size)
    *size = 0;
  
  doit = 1;
  
  do
  {
    n = 0;
    while(n < 4)
    {
      unsigned char ch;
      ch = _sf_uc_bi[*(unsigned char *)str];
      if(ch < B64_UNUSED)
      {
        pr[n] = ch;
        n++;
        str++;
      }
      else
        if(ch == B64_UNUSED)
        {
          str++;
	}
        else
        {
          doit = 0;
          break;
        }
    }
    
    if(doit == 0 && n < 4)
    {
      pr[n+2] = pr[n+1] = pr[n]='\0';
    }
    
    *(ou+0) = (pr[0] << 2) | (pr[1] >> 4);
    *(ou+1) = (pr[1] << 4) | (pr[2] >> 2);
    *(ou+2) = (pr[2] << 6) | (pr[3] >> 0);
    
    ou = ou+((n * 3) >> 2);
  }
  while(doit);
  
  *ou = '\0';	/* NUL-terminate */
  

  /*
   * Replace the old buffer.
  */
  //if(_sf_b64_buf)
    //mfree(_sf_b64_buf);
  return (output);
}



char * quoted_printable_decode(const char *str, size_t *size)
{
  char *buf;
  char *s;
  register int n;

  int first;
  int second;
  if(str == NULL) return NULL;
  
  /*
  * Allocate sufficient space to hold decoded string.
  */
  
  
  s=buf=malloc((size?*size:strlen(str)) + 1);
  if(buf == NULL)
    
    /* ENOMEM? */
    return NULL;

  for(; *str; str++)
  {
    if(*str == '_')
    {
      *s++ = ' ';
      continue;
    }
    if(*str == '=')
    {
      *s = '\0';
      n = *++str; 
      if(n == '\0')
      {
        str--;
        break;
      }
      if(str[1] == '\0')
        break;
      
      if (n>='0'&&n<='9')
        first = n - '0';
      
      if (n>='A'&&n<='F')
        first =n - 'A' + 10;
      
      if (n>='a'&&n<='f')
        first = n- 'a' + 10;
      
      if (n=='\r'||n=='\n')
      {
        *s++ = '=';
        *s++ = n;
        continue;
      }
      
      n = *++str;
      
      if (n>='0'&&n<='9')
        second = n - '0';
      
      
      if (n>='A'&&n<='F')
        second =n - 'A' + 10;
      
      if (n>='a'&&n<='f')
        second = n- 'a' + 10;
      
      if (n=='\r'||n=='\n')
      {
        *s++ = n;
        *s = ' ';
        continue;
      }
      *s=(first<<4)+second;
      s++;
      continue;
    }
    *s++ = *str;
  }
  
  *s = '\0';

//  if(_sf_qp_buf)
//    mfree(_sf_qp_buf);
  if(size) *size = (s - buf);
  return (buf);
}

