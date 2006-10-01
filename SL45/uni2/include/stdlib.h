#ifndef _SIZE_T_DEF
typedef unsigned int    size_t;
#define _SIZE_T_DEF
#endif


typedef struct  {
 int quot;
 int rem;
} div_t;

typedef struct  {
 long quot;
 long rem;
} ldiv_t;


extern int           atoi   ( const char * );
extern long          atol   ( const char * );
/*
extern double        atof   ( const char * );
extern long          strtol ( const char *, char * *, int );
extern unsigned long strtoul( const char *, char * *, int );
extern double        strtod ( const char *, char * * );
extern int           abs    ( int );
extern long          labs   ( long );
extern div_t         div    ( int, int );
extern ldiv_t        ldiv   ( long, long );
extern int           rand   ( void );
extern void          srand  ( unsigned int );
*/
