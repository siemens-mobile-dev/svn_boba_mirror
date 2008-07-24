#ifndef _VISUAL_H_
  #define _VISUAL_H_

#include "main.h"


#define COLOR_GET_A(x) ((unsigned int)x>>24)
#define COLOR_GET_R(x) (((unsigned int)x>>16)&0xFF)
#define COLOR_GET_G(x) (((unsigned int)x>>8)&0xFF)
#define COLOR_GET_B(x) ((unsigned int)x&0xFF)

#define COLOR_RGBA(r,g,b,a) (((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b)|((unsigned int)a<<24))

typedef struct {
  char gui[0x2c];
} GUI_COLOR;

typedef struct {
  DISP_OBJ dsp_obj;
  int r, g, b, a;
  int type;
  int current_column;
  STRID str_id;
  int need_str;
  int cstep;
} DISP_OBJ_COLOR;

typedef union {
  unsigned int int_color;
  char *char_color;
} COLOR_TYPE;

typedef struct {
  char gui[0x2c];
} GUI_COORDINATES;

typedef struct {
  DISP_OBJ dsp_obj;
  int x, y;
  STRID str_id;
  int need_str;
  int cstep;
} DISP_OBJ_COORD;


GUI_COLOR *CreateEditColorGUI(MyBOOK * myBook, COLOR_TYPE, int type);
GUI_COORDINATES *CreateEditCoordinatesGUI(MyBOOK * myBook);
#endif
