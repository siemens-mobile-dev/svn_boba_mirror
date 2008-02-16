#include "view.h"

void debugf(char *file,int line);
#define DEBUGF(void) debugf(__FILE__,__LINE__)
void debugv(char *file,int line,void *p, int sz);
#define DEBUGV(a,b) debugv(__FILE__,__LINE__,a,b)
unsigned int _rshort2(char *p);
int strcmp_nocase(const char *s, const char *d);
int omstrcmp(VIEWDATA *vd, unsigned int ps, unsigned int pd);
char *globalstr(const char *s);
void freegstr(char **ps);
char *replacegstr(char **ps, const char *s, int len);
int char16to8(int c);
int char8to16(int c);
void ascii2ws(WSHDR *ws, const char *s);
char *extract_omstr(VIEWDATA *vd, unsigned int pos);
int char_win2utf8(char*d,const char *s);
char * ToWeb(char *src,int special);
char* Correct_UTF8_String(char* utf8_str);
void oms2ws(WSHDR *ws, const char *text, int len);
