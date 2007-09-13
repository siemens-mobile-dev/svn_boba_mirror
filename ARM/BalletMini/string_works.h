int strcmp_nocase(const char *s, const char *d);
int strcmp_safe(const char *s, const char *d);
char *globalstr(const char *s);
void freegstr(char **ps);
char *replacegstr(char **ps, const char *s, int len);



