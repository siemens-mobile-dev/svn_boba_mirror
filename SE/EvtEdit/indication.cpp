#include "mem.h"
#include "..\\include\Lib_Clara.h" 
#include "..\\include\Dir.h" 
#include "header\structs.h"
#include "header\pages.h"
#include "header\externals.h"
#include "header\lng.h"

#define MUSIC 1

extern const PAGE_DESC bk_indic;
extern const PAGE_DESC bk_Switch;
extern const PAGE_DESC bk_main;
extern const PAGE_DESC bk_indic;

void kill_switchlst(MyBOOK *mbk);
int isEvtEditBook(BOOK *);
int Switch_onLBMessage(GUI_MESSAGE * msg);
void Switch_OnBack(BOOK * bk, GUI* );

int indic_last=0;

int set_param(int par, int item)
{
  int x;
  int f=0xF;
  for (x=0;x<item;x++)
  {
    f=f*0x10;
  }
  return par^f;
};

int Indic_onLBMessage(GUI_MESSAGE * msg)
{
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    MyBOOK *mbk=(MyBOOK*)FindBook(isEvtEditBook);
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    wchar_t* str=(wchar_t *)ListElement_GetByIndex(mbk->switch_lst,item);
    SetMenuItemText0(msg,Str2ID(str,0,SID_ANY_LEN));
    if (item==0)
    {
      if (mbk->selev)
        SetMenuItemText1(msg,mbk->selev->rem==0x55555555 ? Str2ID(lng[ON],0,SID_ANY_LEN) : Str2ID(lng[OFF],0,SID_ANY_LEN));
      else
        SetMenuItemText1(msg,mbk->rem_types==0x55555555 ? Str2ID(lng[ON],0,SID_ANY_LEN) : Str2ID(lng[OFF],0,SID_ANY_LEN));
    }
    else
    {
      if (mbk->selev)
        SetMenuItemText1(msg,((mbk->selev->rem>>(4*(item-1)))&0xF) ? Str2ID(lng[ON],0,SID_ANY_LEN) : Str2ID(lng[OFF],0,SID_ANY_LEN));
      else
        SetMenuItemText1(msg,((mbk->rem_types>>(4*(item-1)))&0xF) ? Str2ID(lng[ON],0,SID_ANY_LEN) : Str2ID(lng[OFF],0,SID_ANY_LEN));
    }
    if (item==0 && !mbk->selev)
      GUIonMessage_SetItemDisabled (msg,1);
    if (item>0)
    {
      if (mbk->selev && mbk->selev->rem==0x55555555)
        GUIonMessage_SetItemDisabled (msg,1);
      else if (!mbk->selev && mbk->rem_types==0x55555555)
        GUIonMessage_SetItemDisabled (msg,1);
    }
    break;
  }
  return(1);
};
void Indic_OnSelect(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK*)bk;
  int item=ListMenu_GetSelectedItem(mbk->ind);
  if (mbk->selev)
  {
    if (item==0)
    {
      if (mbk->selev->rem!=0x55555555)
        mbk->selev->rem=0x55555555;
      else
        mbk->selev->rem=0;
    }
    else
    {
      int t=set_param(mbk->selev->rem, item-1);
      mbk->selev->rem=t;
    }
  }
  else
  {
    if (item==0)
    {
      if (mbk->rem_types!=0x55555555)
        mbk->rem_types=0x55555555;
      else
        mbk->rem_types=0;
    }
    else
    {
      int t=set_param(mbk->rem_types, item-1);
      mbk->rem_types=t;
    }
  }
  
  indic_last=item;
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
  BookObj_CallPage(bk,&bk_indic);
};

void Switch_SetIndfile(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->selectfiletype=2;
  mbk->fType=MUSIC;
  mbk->dir1=DIR_AUDIO+MEM_INTERNAL;
  mbk->dir2=DIR_AUDIO+MEM_EXTERNAL;
  BookObj_CallPage(bk,&SelectFile_page);
};

void Switch_curstat(BOOK *bk, GUI* )
{
  MyBOOK *mbk=(MyBOOK*)bk;
  if (mbk->indpath && mbk->indname)
  {
    wchar_t *str=new wchar_t[wstrlen(mbk->indpath)+1+wstrlen(mbk->indname)+1];
    wstrcpy(str,mbk->indpath);
    wstrcat(str,L"/");
    wstrcat(str,mbk->indname);
    MessageBox(0x6FFFFFFF,Str2ID(str,0,SID_ANY_LEN),0,1,0,0);
    delete(str);
  }
  else
    MessageBox(0x6FFFFFFF,Str2ID(lng[NOTSETTED],0,SID_ANY_LEN),0,1,0,0);
};

GUI_LIST * CreateIndicList(BOOK * book, int cnt)
{
  GUI_LIST * lo=0;
  if (lo=CreateListObject(book,0))
  {
    SetNumOfMenuItem(lo, cnt);
    SetCursorToItem(lo, indic_last);
    ListMenu_SetOnMessages(lo,Indic_onLBMessage);
    SetMenuItemStyle(lo,3);
    GUIObject_Softkey_SetAction(lo,ACTION_BACK, Switch_OnBack);
    GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,Indic_OnSelect);
    MyBOOK *mbk=(MyBOOK*)book;
    if (!mbk->selev)
    {
      GUIObject_Softkey_SetAction(lo,1,Switch_SetIndfile);
      GUIObject_Softkey_SetText(lo,1,Str2ID(lng[MUSICFILE],0,SID_ANY_LEN));
      GUIObject_Softkey_SetAction(lo,2,Switch_curstat);
      GUIObject_Softkey_SetText(lo,2,Str2ID(lng[CURSTATUS],0,SID_ANY_LEN));
    }
  }
  return(lo);
};

static int IndicPage_OnEnter(void *,BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  if (!mbk->switch_lst)
    mbk->switch_lst=List_New();
  wchar_t *n=new wchar_t[30];
  wstrcpy(n,lng[STANDART]);
  wchar_t *n1=new wchar_t[30];
  wstrcpy(n1, lng[COLORLEDS]);
  wchar_t *n2=new wchar_t[30];
  wstrcpy(n2, lng[ORANGELEDS]);
  wchar_t *n3=new wchar_t[30];
  wstrcpy(n3, lng[REDLEDS]);
  wchar_t *n4=new wchar_t[30];
  wstrcpy(n4, lng[VIBRA]);
  wchar_t *n5=new wchar_t[30];
  wstrcpy(n5, lng[SOUND]);
  ListElement_Add(mbk->switch_lst,n);
  ListElement_Add(mbk->switch_lst,n1);
  ListElement_Add(mbk->switch_lst,n2);
  ListElement_Add(mbk->switch_lst,n3);
  ListElement_Add(mbk->switch_lst,n4);
  ListElement_Add(mbk->switch_lst,n5);
  mbk->ind=CreateIndicList(bk, mbk->switch_lst->FirstFree);
  GuiObject_SetTitleText(mbk->ind, Str2ID(lng[INDICATION],0,SID_ANY_LEN));
  ShowWindow(mbk->ind);
  return 1;
};

static int IndicPage_OnExit(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  kill_switchlst(mbk);
  FREE_GUI(mbk->ind);
  return 1;  
};

const PAGE_MSG bk_msglst_indic[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    IndicPage_OnEnter,
  PAGE_EXIT_EVENT_TAG,     IndicPage_OnExit,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_indic = {"EvtEdit_IndicationMenu_Page", 0, bk_msglst_indic};
