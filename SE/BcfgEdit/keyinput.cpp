#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

#include "..\\include\cfg_items.h"
#include "main.h"
#include "keyinput.h"




void KeyCode_KeyHook(BOOK *bk, int key, int unk, int unk2)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  wchar_t ustr[64];
  snwprintf(ustr,MAXELEMS(ustr)-1,L"Current key:\nHEX: 0x%02X\nDEC: %d", key, key);
  *((int *)((char *)mbk->cur_hp+sizeof(CFG_HDR)))=key;
  Feedback_SetTextExtended(mbk->key_input,Str2ID(ustr,0,SID_ANY_LEN),0);
  Feedback_SetTimeout(mbk->key_input,3000);
}

void KeyCode_OnClose(BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  if (mbk->key_input)
  {
    GUI_Free((GUI *)mbk->key_input);
    mbk->key_input=NULL;
  }
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
}

int KeyCode_OnEnter(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  wchar_t ustr[64];
  int key;
  mbk->key_input=TextFeedbackWindow(&mbk->book,0);
  key=*((int *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  snwprintf(ustr,MAXELEMS(ustr)-1,L"Current key:\nHEX: 0x%02X\nDEC: %d", key, key);
  Feedback_SetTextExtended(mbk->key_input,Str2ID(ustr,0,SID_ANY_LEN),0);
  GUI_SetStyle(mbk->key_input,1);
  Feedback_SetKeyHook(mbk->key_input,KeyCode_KeyHook);
  Feedback_SetOnClose(mbk->key_input,KeyCode_OnClose);
  Feedback_SetTimeout(mbk->key_input,3000);
  ShowWindow(mbk->key_input);
  return (1);
}

int KeyCode_OnExit(void *, BOOK * bk)
{
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
  NIL_EVENT_TAG,                   NULL
};

const PAGE_DESC bk_keycode_input = {"BcfgEdit_KeyCodeInput_Page",0,bk_msglst_keycodeinput};
