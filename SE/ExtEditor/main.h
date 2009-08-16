#ifndef _MAIN_H
#define _MAIN_H

typedef struct
{
  BOOK  book;
  GUI_LIST *main_gui;
  char *ebuf;
  int ebuf_len;
  LIST *astr;
  
  GUI_LIST *edit_list;
  int edit_id; 
  char el_ext[36];
  char el_elf[128];
  char el_smicon[64];
  char el_bicon[64];
  
  int DB_State; // 0 -select elf, 1 - small, 2 - big
  
  GUI *si_editext;
  
  GUI *yes_no;
  void (*YesNoFunc)(BOOK * mbk, int); //  1 - yes, 0 - no
}MyBOOK;

typedef struct
{
  BOOK * book;
}MSG;


GUI_LIST * CreateGuiList(MyBOOK * bk);
#define LGP_NULL 0x6FFFFFFF
#define FREE_GUI(a) if (a) a=GUI_Free(a)
#endif
