#include "view.h"

int strcmp_nocase(const char *s, const char *d);
int omstrcmp(VIEWDATA *vd, unsigned int ps, unsigned int pd);
char *globalstr(const char *s);
void freegstr(char **ps);
char *replacegstr(char **ps, const char *s, int len);
int char16to8(int c);
int char8to16(int c);
void ascii2ws(WSHDR *ws, const char *s);
char *extract_omstr(VIEWDATA *vd, unsigned int pos);

