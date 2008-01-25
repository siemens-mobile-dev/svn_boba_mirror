#ifndef _VARS_H_
  #define _VARS_H_

#ifdef DBG
#define _printf(...) {debug_printf("\nOS: ElfPack -> ");debug_printf(__VA_ARGS__);}
#else
#define _printf(...)
#endif

#ifdef DB2020
#define PAGE_ENTER_EVENT 7
#define PAGE_EXIT_EVENT 8
#define ACCEPT_EVENT 2
#define PREVIOUS_EVENT 3
#define CANCEL_EVENT 4
#else
#define PAGE_ENTER_EVENT 5
#define PAGE_EXIT_EVENT 6
#define ACCEPT_EVENT 0x0F
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11
#endif

typedef struct
{
  void * unk;
  void * unk1;
  BOOK * book;
}DATA;

typedef struct
{
  u16* fname;
  u16* fpath;
}FILETYPE;

void ELFExtrRegister(EP_DATA * epd);


#endif
