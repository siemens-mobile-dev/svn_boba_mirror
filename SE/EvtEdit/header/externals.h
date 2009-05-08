#ifndef EXTERNALS_H
  #define EXTERNALS_H
  #define FREE_GUI(a) if (a) a=GUI_Free(a)
#define DELETE(a) if (a) { delete(a);a=0; }

int GetStrID(wchar_t *txt);
#endif
