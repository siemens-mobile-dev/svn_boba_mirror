#include "../inc/swilib.h"

#ifndef  NEWSGOLD
#define SL65
//#define CX70
#endif

extern void kill_data(void *p, void (*func_p)(void *));
extern void(*OldOnClose)(void *);
extern void(*OldOnCreate)(void *);
extern IMGHDR* create_imghdr(const char* fname);

extern int get_file_handler(void);
extern unsigned int* get_errno(void);
extern void set_file_handler(int f);
