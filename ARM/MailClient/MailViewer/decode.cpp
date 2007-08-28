#include "..\..\inc\swilib.h"
#include "base64.h"
#include "sf_mime.h"

extern int strncmp_nocase(const char *s1,const char *s2,unsigned int n);

enum {WIN_1251, KOI8_R, ISO_8859_5, UTF_8};



#pragma inline 
int is_space(int c)
{ 
  return (c==' '?NULL:1);
}

#pragma inline
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}


#define MIN(A,B) A<B?A:B

char * base64_decode(const char *str, size_t *size)
{
  char *output, *ou;
  unsigned int pr[6];
  int n;
  int doit;
  size_t _sf_b64_len;
  const char *s;
  int len;
  
  if(str == NULL)
    str = "";
  
  s=str;
  len=size?*size:strlen(str);
  ou = output = malloc(len+1);
  if(output == NULL)
    return NULL;
  
  if(size)
    *size = 0;
  
  
  doit = 1;
  
  do
  {
    n = 0;
    while(n < 4)
    {
      if ((str-s)>=len)
      {
        doit=0;
        break;
      }
      unsigned int ch;
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
  
  _sf_b64_len = (ou - output);
  if(size)
    *size = _sf_b64_len;
  
  *ou = '\0';	/* NUL-terminate */
  return (realloc(output,_sf_b64_len+1));
}


#pragma optimize=z 9
char * quoted_printable_decode(const char *str, size_t *size)
{
  char *buf;
  char *s, *d;
  register int n;
  register int len;
  
  if(!str) return 0;
  
  /*
  * Allocate sufficient space to hold decoded string.
  */
  d=(char *)str;
  len=size?*size:strlen(str);
  s=buf=malloc(len + 1);
  if(!buf)
    
    /* ENOMEM? */
    return 0;

  for(; *str && (str - d)<len; str++)
  {
    //if(*str == '_')
    //{
    //  *s++ = ' ';
    //  continue;
    //}
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
      
      
      switch(n)
      {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        *s = n - '0';
        break;
        
      case 'A': case 'B': case 'C':
      case 'D': case 'E': case 'F':
        *s=n - 'A' + 10;
        break;
        
      case 'a': case 'b': case 'c':
      case 'd': case 'e': case 'f':
        *s=n - 'a' + 10;
        break;
        
      case '\r':
      case '\n':
        while(str[1] == '\n' || str[1] == '\r') str++;
        continue;
        
      default:
        *s++ = '=';
        *s++ = n;
        continue;
      }
      
      *s <<= 4;                   
      n = *++str;
      
      switch(n)
      {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        *s |= n - '0';
        break;
        
      case 'A': case 'B': case 'C':
      case 'D': case 'E': case 'F':
        *s|=n - 'A' + 10;
        break;
       
      case 'a': case 'b': case 'c':
      case 'd': case 'e': case 'f':
        *s|=n - 'a' + 10;
        break;
        
      case '\r':
      case '\n':
        *s++ = n;
        continue;
        
      default:
        *s = ' ';
       	continue;
      }
      
      s++;     
      continue;
    }
    *s++ = *str;
  }
  
  *s = '\0';

//  if(_sf_qp_buf)
//    mfree(_sf_qp_buf);
  if(size) *size = (s - buf);
  return (realloc(buf,(s-buf)+1));
}


#define ENCODED_WORD_BEGIN	"=?"
#define ENCODED_WORD_END	"?="

const char kw[] = 
{
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  254,224,225,246,228,229,244,227,245,232,233,234,235,236,237,238,
  239,255,240,241,242,243,230,226,252,251,231,248,253,249,247,250,
  222,192,193,214,196,197,212,195,213,200,201,202,203,204,205,206,
  207,223,208,209,210,211,198,194,220,219,199,216,221,217,215,218
};

char iw[] = {
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191
};

void koi2win(char*d,char *s)
{
  int c;
  while((c=*s++))
  {
    *d++=c>=128?kw[c-128]:c;
  }
  *d=c;
}

void iso885952win(char*d,char *s)
{
  int c;
  while((c=*s))
  {
    if(c>=128)
      *d=iw[c-128];
    else *d=c;
    s++;
    d++;
  }
  *d=c;
}

void utf82win(char*d,const char *s)
{
  for (; *s; s+=2)
  {
    unsigned char ub = *s, lb = *(s+1);
    if (ub == 208)
      if (lb != 81)
        {*d = lb + 48; d++;}
      else
        {*d = 'Ё'; d++;}

    if (ub == 209)
      if (lb != 91)
        {*d = lb + 112; d++;}
      else
        {*d = 'ё'; d++;}

    if ((ub != 208) && (ub != 209) && (lb != 91) && (lb != 81))
    {
      *d = ub;
      d++;
      s--;
    }
  }
  *d = 0;
}

void strip_html(char *s)
{
  int b=-1, d_ptr = 0;

  //уберём переводы строк
  for(int i = 0, j = 0; s[i]; i++)
  {
    if(s[i] != '\n' && s[i] != '\r') s[j++] = s[i];
    if(!s[i+1]) s[j] = 0;
  }
  
  //посчитаем сколько места надо под буфер
  d_ptr = 0;
  char *next_br, *prev_br=s, *da = "\r\n";
  while(next_br = strstr(prev_br,"<br>"))
  {
    d_ptr += next_br-prev_br;
    d_ptr += 2;
    prev_br = next_br+strlen("<br>");
  }

  char *d = (char*) malloc(d_ptr+1);

  //добавим после <br> переводы строк
  d_ptr = 0;
  prev_br = s;
  while(next_br = strstr(prev_br,"<br>"))
  {
    memcpy(d+d_ptr, prev_br, next_br-prev_br);
    d_ptr += next_br-prev_br;
    memcpy(d+d_ptr, da, 2);
    d_ptr += 2;
    prev_br = next_br+strlen("<br>");
  }
  d[d_ptr] = 0;
  
  //вернём модифицированную строку в исходный буфер
  strcpy(s,d);

  //отфильтруем html - результат заведомо меньшей длины
  d_ptr = 0;
  for(int i = 0; i < strlen(s); i++)
  {
    if(s[i] == '>' && b == -1) b = i+1;
    if(s[i] == '<' && b != -1)
    if(i!=b)
    {
      memcpy(d+d_ptr, s+b, i-b);
      d_ptr += i-b;
      b = -1;
    }
    else
      b = -1;
  }
  d[d_ptr] = 0;

  //вернём модифицированную строку в исходный буфер
  strcpy(s,d);
  mfree(d);
}

int get_charset(char *charset)
{
  if (!strncmp_nocase(charset,"windows-1251",12))
    return WIN_1251;
  
  if (!strncmp_nocase(charset,"koi8-r",6))
    return KOI8_R; 
  
  if (!strncmp_nocase(charset,"ISO-8859-5",10))
    return ISO_8859_5;
  
  if (!strncmp_nocase(charset,"UTF-8",5))
    return UTF_8;

  return WIN_1251;
}

char *unmime_header(const char *encoded_str)
{
  const char *p = encoded_str;
  const char *eword_begin_p, *encoding_begin_p, *text_begin_p, *eword_end_p;

  char charset[32];
  int encoding;
  char *conv_str;
  char *outbuf;
  char *out_str;
  size_t out_len;
  size_t decoded_len;
  
  out_len=strlen(encoded_str) * 2;
  outbuf = (char*) malloc(out_len);
  zeromem(outbuf,out_len);
  
  while (*p != '\0')
  {
    char *decoded_text = NULL;
    int len;
    
    eword_begin_p = strstr(p, ENCODED_WORD_BEGIN);
    if (!eword_begin_p)
    {
      char * curbuf = (char*) malloc(strlen(p)+1);
      utf82win(curbuf, p);
      strcat(outbuf, curbuf);
      mfree(curbuf);
      //strcat(outbuf, p);
      break;
    }

    encoding_begin_p = strchr(eword_begin_p + 2, '?');
    if (!encoding_begin_p)
    {
      strcat(outbuf, p);
      break;
    }
    
    text_begin_p = strchr(encoding_begin_p + 1, '?');
    if (!text_begin_p) 
    {
      strcat(outbuf, p);
      break;
    }
    
    eword_end_p = strstr(text_begin_p + 1, ENCODED_WORD_END);
    if (!eword_end_p)
    {
      strcat(outbuf, p);
      break;
    }
    
    if (p == encoded_str)
    {
      strncat(outbuf, p, eword_begin_p - p);
      p = eword_begin_p;
    }
    else
    {
      /* ignore spaces between encoded words */
      const char *sp;
      for (sp = p; sp < eword_begin_p; sp++) 
      {
        if (!is_space(*sp)) 
        {
          strncat(outbuf, p, eword_begin_p - p);
          p = eword_begin_p;
          break;
        }
      }
    }
    len = MIN(sizeof(charset) - 1, encoding_begin_p - (eword_begin_p + 2));
    strncpy(charset, eword_begin_p + 2, len);
    charset[len] = '\0';
    encoding = toupper(*(encoding_begin_p + 1));
    if (encoding == 'B')
    {
      decoded_len=eword_end_p - (text_begin_p + 1);
      decoded_text = base64_decode(text_begin_p + 1,&decoded_len);
    }
    else if (encoding == 'Q') 
    {
      decoded_len=eword_end_p - (text_begin_p + 1);
      decoded_text = quoted_printable_decode(text_begin_p + 1,&decoded_len);
    }
    else
    {
      strncat(outbuf, p, eword_end_p + 2 - p);
      p = eword_end_p + 2;
      continue;
    }
    switch(get_charset(charset))
    {
    case WIN_1251:
      strcat(outbuf, decoded_text);
      p = eword_end_p + 2;
      continue;
    case KOI8_R:
      conv_str=(char*) malloc(strlen(decoded_text)+1);
      koi2win(conv_str,decoded_text);
      break;
    case ISO_8859_5:
      conv_str=(char*) malloc(strlen(decoded_text)+1);
      iso885952win(conv_str,decoded_text);
      break;
    case UTF_8:
      conv_str=(char*) malloc(strlen(decoded_text)+1);
      utf82win(conv_str,decoded_text);
      break;
         }

    strcat(outbuf, conv_str);
    mfree(conv_str);
    mfree(decoded_text);
    p = eword_end_p + 2;
  }
  out_str = outbuf;
  out_len = strlen(outbuf);
    
  return realloc(out_str, out_len + 1);
}
