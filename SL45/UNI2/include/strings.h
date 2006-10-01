#ifndef _STR_DEF
typedef char far*               STR;
typedef unsigned int far*       WSTR;
#define _STR_DEF
#endif

#ifndef _SIZE_T_DEF
typedef unsigned int    size_t;
#define _SIZE_T_DEF
#endif

typedef struct
{ 
 WSTR wstr;
 int maxlen;
 int len;
} WSHDR;


extern char *  strcpy   ( char *, const char * );
extern char *  strncpy  ( char *, const char *, size_t );
extern char *  strcat   ( char *, const char * );
extern char *  strncat  ( char *, const char *, size_t );
extern char *  strchr   ( const char *, int );
extern char *  strrchr  ( const char *, int );
//extern size_t  strspn   ( const char *, const char * );
//extern size_t  strcspn  ( const char *, const char * );
//extern char *  strpbrk  ( const char *, const char * );
//extern char *  strstr   ( const char *, const char * );
//extern char *  strtok   ( char *, const char * );
extern int     strcmp   ( const char *, const char * );
extern int     strncmp  ( const char *, const char *, size_t );
extern size_t  strlen   ( const char * );
extern void *  memcpy   ( void *, const void *, size_t );
extern void *  memmove  ( void *, const void *, size_t );
extern void *  memchr   ( const void *, int, size_t );
extern void *  memset   ( void *, int, size_t );
extern int     memcmp   ( const void *, const void *, size_t );
//extern char *  strerror ( int );
//extern int     strcoll  ( const char *, const char * );
//extern size_t  strxfrm  ( char *, const char *, size_t );


extern int     strcmp_nocase(const STR s1, const STR s2);
extern void *  memcpyW(void far*desr, const void far *s, unsigned long siz);


extern int     sprintf(STR, const STR, ... );
extern void    ReplaceCharInStr(STR s, char c1, char c2);
extern char    toupper(char);

extern void Word2DigitArray(int word, STR s);
extern void FormatNumber(int word, STR s, int noOfDigits);

extern void StoreTextAsStringID(int id, STR s, int UnkFlag);
extern void GetStringByID1(unsigned int id, STR dest);
extern void GetStringByID(unsigned int id, WSHDR far*, int zero);

extern void ClearWSTR(WSHDR far*, WSTR wstr, size_t size);
extern void AddCharToWSTR(WSHDR far*, size_t wchar);
extern void InsertCharToWSTR(WSHDR far*, size_t wchar, size_t pos); //first=1
extern void ConcatWSTR(WSHDR far* str1, WSHDR far* str2);
extern void ConvLangSTRToWSHDR(WSHDR far* dest, STR in, int flag1, int flag2); //flag1-#1;flag2-#0
extern WSHDR far* AllocWSTR(unsigned int sz, void huge*);
extern void CutWSTR(WSHDR far*,unsigned int newlen);
extern void AsciizToWSTR(WSHDR far*, const STR s);
extern int  wstrlen(WSHDR far*);
            
extern void STRtoWSTRP(WSTR*,STR);
extern void STRtoWSTR(WSTR,STR);

//unknown, use in input dialog
extern int sub_E70BAC(void far*);
extern void far* sub_E70774(void far*);
extern void sub_F07A66(void far*, int ,WSHDR far*);
