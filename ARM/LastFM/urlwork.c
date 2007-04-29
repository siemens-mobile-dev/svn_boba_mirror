#include "..\inc\swilib.h"
#include "urlwork.h"

void urlescape(char *out, const char *source)
{
  const char * h = "0123456789abcdef";
  int c;
  int i=0;
  while((c =*source++)!=0)
  {
    if ('a' <= c && c <= 'z'
	|| 'A' <= c && c <= 'Z'
	  || '0' <= c && c <= '9'
	    || c == '-' || c == '_' || c == '.')
      out[i++]= c;
    else 
      if( c == ' ' )
	out[i++]= '+';
      else {
	out[i++]= '%';
	out[i++]= h[c >> 4];
	out[i++]= h[c & 0x0f];
      }
  }
  out[i]=0;
}

void urlcat(char *out, const char *source)
{
  urlescape(out+strlen(out),source);
}

