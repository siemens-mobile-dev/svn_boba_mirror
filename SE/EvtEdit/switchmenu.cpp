#include "mem.h"
#include "..\\include\Lib_Clara.h" 
#include "..\\include\Dir.h" 
#include "header\structs.h"
#include "header\pages.h"
#include "header\externals.h"
#include "header\lng.h"

extern const PAGE_DESC bk_Switch;
extern int indic_last;

int isEvtEditBook(BOOK * struc);

void kill_switchlst(MyBOOK *mbk)
{
  if (mbk->switch_lst)
  {
    while (mbk->switch_lst->FirstFree)
    {
      wchar_t* str=(wchar_t*)ListElement_Remove(mbk->switch_lst,0);
      DELETE(str);
    }
    List_Free(mbk->switch_lst);
    mbk->switch_lst=0;
  }
};

int Switch_onLBMessage(GUI_MESSAGE * msg)
{
  switch(msg->msg)
  {
  case 1:
    MyBOOK *mbk=(MyBOOK*)FindBook(isEvtEditBook);
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    wchar_t* str=(wchar_t *)ListElement_GetByIndex(mbk->switch_lst,item);
    SetMenuItemText0(msg,Str2ID(str,0,SID_ANY_LEN));
    break;
  }
  return(1);
};

void Switch_OnSelect(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  int item=ListMenu_GetSelectedItem(mbk->switch_gui);
  DELETE(mbk->selev->param);
  mbk->selev->param=new wchar_t[2];
  snwprintf(mbk->selev->param,1,L"%d", item);
  BookObj_CallPage(bk,&bk_main);
};
void Switch_OnBack(BOOK * bk, void *)
{
  indic_last=0;
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
};

GUI_LIST * CreateSwitchList(BOOK * book, int cnt)
{
  GUI_LIST * lo=0;
  if (lo=CreateListObject(book,0))
  {
    SetNumOfMenuItem(lo, cnt);
    SetCursorToItem(lo,0);
    ListMenu_SetOnMessages(lo,Switch_onLBMessage);
    SetMenuItemStyle(lo,3);
    GUIObject_Softkey_SetAction(lo,ACTION_BACK, Switch_OnBack);
    GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,Switch_OnSelect);
  }
  return(lo);
};

static int SwitchPage_OnEnter(void *,BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->switch_gui=CreateSwitchList(bk, mbk->switch_lst->FirstFree);
  GuiObject_SetTitleText(mbk->switch_gui, Str2ID(lng[CHOOSE],0,SID_ANY_LEN));
  ShowWindow(mbk->switch_gui);
  return 1;
};

static int SwitchPage_OnExit(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  kill_switchlst(mbk);
  FREE_GUI(mbk->switch_gui);
  return 1;  
};

const PAGE_MSG bk_msglst_Switch[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    SwitchPage_OnEnter,
  PAGE_EXIT_EVENT_TAG,     SwitchPage_OnExit,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_Switch = {"EvtEdit_SwitchMenu_Page",0,bk_msglst_Switch};
