#ifndef _MAIN_H_
#define _MAIN_H_

#define COLOR(x) (char *)(&(x)) // Приведение цвета из char[4]

extern const char DEFAULT_PARAM[128];
extern const char BG_COLOR[4];
extern const char ST_COLOR[4];
extern const char ST_FRAME_COLOR[4];
extern const char ST_TEXT_COLOR[4];
extern const char MST_COLOR[4];
extern const char MST_FRAME_COLOR[4];
extern const char MST_TEXT_COLOR[4];
extern const char M_TEXT[4];
extern const char M_CURSOR[4];
extern const char M_CURSOR_FRAME[4];
extern const char M_BG1[4];
extern const char M_BG2[4];
extern const char STOP_COLOR[4];
extern const char STOP_FRAME_COLOR[4];
extern const char STOP_TEXT_COLOR[4];
extern const char PR_COLOR[4];
extern const char ROLL_COLOR[4];
extern const char CUR_COLOR[4];
extern const char CUR_PIC_COLOR[4];

void FreeViewUrl(void);
char *globalstr(const char *s);
int ReadUrlFile(char *url_file);

typedef struct
{
  CSM_RAM csm;
  int view_id;
  int goto_url;
  int sel_bmk;
  int main_menu_id;
}MAIN_CSM;

enum I_INPUTMODE {MODE_FILE, MODE_URL, MODE_BOOKMARKS, MODE_NONE};

void FreeRawText(VIEWDATA *vd);

typedef struct
{
  GUI gui;
  VIEWDATA *vd;
  int cached;
  WSHDR *ws1;
  WSHDR *ws2;
}VIEW_GUI;

#endif /* _MAIN_H_ */
