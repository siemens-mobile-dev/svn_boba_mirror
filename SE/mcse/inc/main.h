#ifndef _MAIN_H
#define _MAIN_H

typedef struct
{
  BOOK  book;
  GUI *main_gui;
  GUI *stop_progr;
  GUI *yes_no;
  void (*YesNoFunc)(int); //  1 - yes, 0 - no
}MyBOOK;

typedef struct {
  DISP_OBJ dsp_obj;
} DISP_OBJ_MAIN;

typedef struct {
  char gui[0x2c];
} MAIN_GUI;


#define FREE_GUI(a) if (a) a=GUI_Free(a)
#endif
