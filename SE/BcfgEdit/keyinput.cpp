#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

#include "..\\include\cfg_items.h"
#include "main.h"
#include "keyinput.h"




void KeyCode_KeyHook(BOOK *bk, int key, int unk, int unk2)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  wchar_t ustr[64];
  STRID sid[3];
  
  snwprintf(ustr,MAXELEMS(ustr)-1,L"\n\nHEX: 0x%02X\nDEC: %d", key, key);
  *((int *)((char *)mbk->cur_hp+sizeof(CFG_HDR)))=key;
  
  sid[0]=Str2ID(L"Current key:\n\n",0,SID_ANY_LEN);
  sid[1]=KeyCode2Name(key);
  sid[2]=Str2ID(ustr,0,SID_ANY_LEN);
  Feedback_SetTextExtended(mbk->key_input,Str2ID(sid,5,3),0);
  Feedback_SetTimeout(mbk->key_input,3000);
}

void KeyCode_OnClose(BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->key_input);
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
}

int KeyCode_OnEnter(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  wchar_t ustr[64];
  int key;
  STRID sid[3];
  mbk->key_input=TextFeedbackWindow(&mbk->book,0);
  key=*((int *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  snwprintf(ustr,MAXELEMS(ustr)-1,L"\n\nHEX: 0x%02X\nDEC: %d", key, key);

  sid[0]=Str2ID(L"Current key:\n\n",0,SID_ANY_LEN);
  sid[1]=KeyCode2Name(key);
  sid[2]=Str2ID(ustr,0,SID_ANY_LEN);
  
  Feedback_SetTextExtended(mbk->key_input,Str2ID(sid,5,3),0);
  GUI_SetStyle(mbk->key_input,1);
  Feedback_SetKeyHook(mbk->key_input,KeyCode_KeyHook);
  Feedback_SetOnClose(mbk->key_input,KeyCode_OnClose);
  Feedback_SetTimeout(mbk->key_input,3000);
  ShowWindow(mbk->key_input);
  return (1);
}

int KeyCode_OnExit(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int key;
  key=*((int *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  ListMenu_SetSecondLineText((GUI_LIST*)mbk->key_sel_list,0,KeyCode2Name(key));
  return (1);
}

const PAGE_MSG bk_msglst_keycodeinput[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,            KeyCode_OnEnter,
  PAGE_EXIT_EVENT_TAG,             KeyCode_OnExit,
  FLIP_OPEN_EVENT_TAG,             NULL,
  PHF_PRESSED_EVENT_TAG,           NULL,
  CAMERA_LENSCOVER_OPEN_EVENT_TAG, NULL,
  CAMERA_LENSCOVER_CLOSE_EVENT_TAG,NULL,
  MEDIAPLAYERKEY_PRESSED_EVENT_TAG,NULL,
  GOMUSICKEY_PRESSED_EVENT_TAG,    NULL,
  MULTIKEY_PRESSED_EVENT_TAG,      NULL,
  LAMPKEY_PRESSED_EVENT_TAG,       NULL,
  ONOFFKEY_SHORT_PRESS_EVENT_TAG,  NULL,
  ONOFFKEY_LONG_PRESS_EVENT_TAG,   NULL,
  LOCKKEY_EVENT_TAG,               NULL,
  OPERATORKEY_PRESSED_EVENT_TAG,   NULL,
  NIL_EVENT_TAG,                   NULL
};

const PAGE_DESC bk_keycode_input = {"BcfgEdit_KeyCodeInput_Page",0,bk_msglst_keycodeinput};

//----------------------

const wchar_t * modes[] =
{
  L"Short Press",
  L"Long Press",
  L"Repeat",
  L"Short Release",
  L"Long Release"
};

STRID GetKeyModeName(int mode)
{
  return (mode<MAXELEMS(modes)?Str2ID(modes[mode],0,SID_ANY_LEN):LGP_NULL);
}

void KeyModeSelect_OnCloseCBoxGui(BOOK * bk, GUI *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->keymode_sel_list);
}

void KeyModeSelect_OnSelectCBoxGui(BOOK * bk, GUI *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  int item=OneOfMany_GetSelected(myBook->keymode_sel_list);
  *((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)+sizeof(int)))=item;
  ListMenu_SetSecondLineText((GUI_LIST*)myBook->key_sel_list,1,Str2ID(modes[item],0,SID_ANY_LEN));
  FREE_GUI(myBook->keymode_sel_list); 
}

void KeyModeSelect_CreateCBoxGui(MyBOOK *myBook)
{
  STRID strid[MAXELEMS(modes)];
  GUI_ONEOFMANY *om=CreateOneOfMany(&myBook->book);
  myBook->keymode_sel_list=om;
  
  CFG_HDR *hp=myBook->cur_hp;
  wchar_t ustr[64];
  win12512unicode(ustr,hp->name,MAXELEMS(ustr)-1);
  GuiObject_SetTitleText(om,Str2ID(ustr,0,SID_ANY_LEN));
  for (int i=0; i<MAXELEMS(modes); i++) strid[i]=GetKeyModeName(i);
  OneOfMany_SetTexts(om,strid,MAXELEMS(modes));
  OneOfMany_SetChecked(om,*((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)+sizeof(int))));
  GUIObject_Softkey_SetAction(om,ACTION_BACK,KeyModeSelect_OnCloseCBoxGui);
  GUIObject_Softkey_SetAction(om,ACTION_SELECT1,KeyModeSelect_OnSelectCBoxGui);
  ShowWindow(om);
}


void KeyCodeSelect_onEnterPressed(BOOK * bk, GUI *)
{
  MyBOOK * mbk=(MyBOOK *)bk;

  int item=ListMenu_GetSelectedItem(mbk->key_sel_list);
  switch (item)
  {
  case 0:
    BookObj_CallPage(&mbk->book,&bk_keycode_input);
    break;
  case 1:
    
    KeyModeSelect_CreateCBoxGui(mbk);
    break;
  }
}

void KeyCodeSelect_OnBack(BOOK * bk, GUI *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->key_sel_list);
}

int KeyCodeSelect_OnEnter(void *, BOOK * bk)
{
  MyBOOK * mbk = (MyBOOK*)bk;
  CFG_HDR *hp=mbk->cur_hp;
  wchar_t ustr[64];
  GUI_LIST * lo;
  STRID strid[2];
  int * p;
  
  strid[0]=Str2ID(L"Select key",0,SID_ANY_LEN);
  strid[1]=Str2ID(L"Key mode",0,SID_ANY_LEN);
  lo=CreateListObject(&mbk->book,0);
  mbk->key_sel_list=lo;


  win12512unicode(ustr,hp->name,MAXELEMS(ustr)-1);
  GuiObject_SetTitleText(lo,Str2ID(ustr,0,SID_ANY_LEN));
  SetNumOfMenuItem(lo,2);
  OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,strid,2);
  p=((int *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  ListMenu_SetSecondLineText(lo,0,KeyCode2Name(*p));
  p++;
  ListMenu_SetSecondLineText(lo,1,Str2ID(modes[*p],0,SID_ANY_LEN));
  SetCursorToItem(lo,0);
  SetMenuItemStyle(lo,1);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,KeyCodeSelect_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,KeyCodeSelect_OnBack);
  ShowWindow(lo);
  return (1);
}


int KeyCodeSelect_OnExit(void *, BOOK * bk)
{
  return (1);
}

const PAGE_MSG bk_msglst_keycodeselect[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,            KeyCodeSelect_OnEnter,
  PAGE_EXIT_EVENT_TAG,             KeyCodeSelect_OnExit,
  NIL_EVENT_TAG,                   NULL
};

const PAGE_DESC bk_keycode_select = {"BcfgEdit_KeyCodeSelect_Page",0,bk_msglst_keycodeselect};
