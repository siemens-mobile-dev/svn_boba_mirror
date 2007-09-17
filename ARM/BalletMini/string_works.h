int strcmp_nocase(const char *s, const char *d);
int strcmp_safe(const char *s, const char *d);
char *globalstr(const char *s);
void freegstr(char **ps);
char *replacegstr(char **ps, const char *s, int len);
int char16to8(int c);
int char8to16(int c);
void ascii2ws(WSHDR *ws, const char *s);
