#ifndef _MAIN_H_
  #define _MAIN_H_

typedef struct
{
  BOOK  book;
  GUI *si;  
}MyBOOK;

int MainPageOnCreate(void *, BOOK *bk);
int MainPageOnClose(void *, BOOK *bk);
int isScalkaBook(BOOK * struc);

#define FREE_GUI(a) if (a) a=GUI_Free(a)

#endif
