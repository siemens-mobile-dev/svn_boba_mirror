#ifndef _MAIN_H_
  #define _MAIN_H_





typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

void UpdateCSMname(void);

void maincsm_oncreate(CSM_RAM *data);
void maincsm_onclose(CSM_RAM *csm);
int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg);

int CalcPic(char* picture);
int strcmp_nocase(const char *s1,const char *s2);
void ElfKiller(void);

void Draw(int i);
void GUI_Draw(int i, int pos);
void GUI_DrawSelect(int pos);
void DoLabel(int pos);


#endif
