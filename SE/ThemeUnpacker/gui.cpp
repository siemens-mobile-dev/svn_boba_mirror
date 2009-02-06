#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "structs.h"
int tar_uncompress(wchar_t *path, wchar_t *name);
extern const PAGE_DESC bk_gui;
extern const PAGE_DESC bk_main;

void OnBackGui(BOOK * bk, void *)
{
  FreeBook(bk);
};
int isA2Book(BOOK *bk)
{
  if (!strcmp(bk->xbook->name,"Softkeys")) { return 1; }
  else { return 0; }
};

void OnEnterGui(BOOK * bk, void *)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  int x=ListMenu_GetSelectedItem(mbk->lst);
  if (x==0)
  {
    BOOK *bk=SESSION_GetTopBook(root_list_get_session(root_list_get_session_count() - 1));
    SetTheme(mbk->path, mbk->name, BOOK_GetBookID(bk), 1);
    UI_Event(THEME_CHANGED_EVENT);
    FreeBook(bk);
  }
  if (x==1)
  {
    int res=tar_uncompress(mbk->path,mbk->name);
    wchar_t ustr[64];
    if (res>0)
    {
      snwprintf(ustr,MAXELEMS(ustr)-1,L"Unpacked\nTotal files: %d", res);
    }
    else
    {
      snwprintf(ustr,MAXELEMS(ustr)-1,L"Unpacking failed\nTotal files: %d", res);
    }
    MessageBox(0x6FFFFFFF,Str2ID(ustr,0,MAXELEMS(ustr)-1),0, 1 ,5000,0);
    FreeBook(bk);
  }
};
int onLBMessage(GUI_MESSAGE * msg)
{
  switch(msg->msg)
  {
  case 1:
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    if (item==0)
    {
      SetMenuItemText0(msg,Str2ID(L"Activate",0,SID_ANY_LEN));
    }
    else
    {
      SetMenuItemText0(msg,Str2ID(L"Unpack",0,SID_ANY_LEN));
    }
  }
  return(1);
};

GUI_LIST * CreateGuiList(BOOK * book)
{
  GUI_LIST * lo=0;
  if (lo=CreateListObject(book,0))
  {
    GuiObject_SetTitleText(lo,Str2ID(L"Install theme",0,SID_ANY_LEN));
    SetNumOfMenuItem(lo,2);
    SetCursorToItem(lo,0);
    ListMenu_SetOnMessages(lo,onLBMessage);
    SetMenuItemStyle(lo,0);
    GUI_SetStyle(lo,9);
    GUIObject_Softkey_SetAction(lo,ACTION_BACK, OnBackGui);
    GUIObject_Softkey_SetAction(lo,ACTION_SELECT1, OnEnterGui);
  }
  return(lo);
};

GUI_LIST *create_ed(BOOK *book)
{
  MyBOOK *mbk=(MyBOOK *)book;
  mbk->lst=CreateGuiList(book);
  ShowWindow(mbk->lst);
  return(mbk->lst);
};

int CreateGui(void *data, BOOK * book)
{
  MyBOOK *mbk=(MyBOOK*)book;
  mbk->lst=create_ed(book);
  return 1;
};

int RemoveGUI(void *data, BOOK * book)
{
  MyBOOK *mbk=(MyBOOK*)book;
  GUI_Free(mbk->lst);
  return 0;
};

const PAGE_MSG bk_msglst_gui[] @ "DYN_PAGE"  =
{
  PAGE_ENTER_EVENT_TAG,CreateGui,
  PAGE_EXIT_EVENT_TAG,RemoveGUI,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_gui = {"ThmUnpacker_GUI",0,bk_msglst_gui};
