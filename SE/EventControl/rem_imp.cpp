/*
*===========================================================================
*                     Модуль напоминальщика
*===========================================================================
*/
#include "..\deleaker\mem2.h"
#include "..\include\Lib_Clara.h" 
#include "..\include\Dir.h"
#include "header\structs.h"
#include "header\lng.h"

#define MBK(a) MyBOOK *mbk=(MyBOOK*)a

extern MyBOOK *ECBook;

int isEvtBook(BOOK *bk);

GUI_REMIND *GUI_REMIND_Create(BOOK *bk);
void GuiRemind_AddNote(GUI_REMIND *g, REMIND *rem);
void kill_rems(LIST *lst, MyBOOK *mbk, bool check);
//#define DELETE(x) if (x)delete x; x=0

/*
*===========================================================================
*                     Создаём напоминальщика...
*===========================================================================
*/
void CreateReminder(BOOK * bk, wchar_t *text, wchar_t *utext, wchar_t *time, bool vibra, bool replace)
{
  MBK(bk);
  if (vibra)
  {
    PAudioControl pAC = AudioControl_Init();
    if(!pAC)pAC = *GetAudioControlPtr();
    AudioControl_Vibrate(pAC, 500, 50, 3000);
  }
  bool created=false;
  if (mbk->remind)
  {
    if (replace)
    {
      kill_rems(mbk->remlst, mbk, 0);
      GUI_Free(mbk->remind);
      mbk->remind=GUI_REMIND_Create(bk);
      created=true;
    }
  }
  if (!mbk->remind)
  {
    mbk->remind=GUI_REMIND_Create(bk);
    created=true;
  }
  GUI *gRemind = mbk->remind;
  if (text && utext && time)
  {
    REMIND *rem=new REMIND;
    memset(rem,0,sizeof(REMIND));
    rem->text=new wchar_t[wstrlen(text)+1];
    wstrcpy(rem->text, text);
    rem->utext=new wchar_t[wstrlen(utext)+1];
    wstrcpy(rem->utext,utext);
    rem->time=new wchar_t[wstrlen(time)+1];
    wstrcpy(rem->time,time);
    GuiRemind_AddNote(mbk->remind, rem);
  }
  GUI_SetStyle(gRemind, 4);
  GuiObject_SetTitleType(gRemind, 1);
  GUIObject_HideSoftkeys(gRemind);
  if (created)
  {
    ShowWindow(gRemind);
    BookObj_Show(bk, 0);
    BookObj_SetFocus(bk, 0);
  }
};

