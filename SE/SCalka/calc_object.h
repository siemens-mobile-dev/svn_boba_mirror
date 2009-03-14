#ifndef _CALC_OBJECT_H_
  #define _CALC_OBJECT_H_

typedef struct {
  char dummy[0x2c]; 
} GUI_CALC;

typedef struct {
  DISP_OBJ dsp_obj;
  int current_tab;
  int cursor_pos;
  int curx_pos;
  STRID answer_sid;
  int x1, x2, y1, y2;
  int cur_base;
  STRID yx[4][12];
  int names_len[4][12];
} DISP_OBJ_CALC;

GUI_CALC *CreateCalkGUI(BOOK *bk);
#define LGP_NULL 0x6FFFFFFF

#endif
