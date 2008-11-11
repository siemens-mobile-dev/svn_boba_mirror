#ifndef _MAIN_H_
#define _MAIN_H_


int isBookManager(BOOK * struc);
char * unicode2win1251(char *s, wchar_t *ws, int len);
int GetActiveTab(void);

extern GUI_LIST * java_list_menu;
extern LIST * java_list;

extern GUI_LIST * mode_list;
extern GUI_LIST * but_list;
//extern GUI * str_inp;
extern char * buffer;
extern int buf_size;
extern int MainMenuID;
extern int ActiveTab;


#endif
