#include "..\\include\Lib_Clara.h" 
#include "..\\include\Dir.h" 
#include "header\structs.h"
#include "header\extern.h"
#include "..\\include\var_arg.h"
#include "revision.h"

extern const PAGE_DESC bk_main;
extern const PAGE_DESC bk_keycode_input;
extern const PAGE_DESC bk_StringInput;
extern const PAGE_DESC bk_skin;

int GetStrID(wchar_t *txt);
int GetIconID(wchar_t *txt);
int onLBMessage(GUI_MESSAGE * msg);
int onLBMessage2(GUI_MESSAGE * msg);

int isSBook(BOOK *);
int isSoftEditBook(BOOK *bk);
void savecustomcfg(wchar_t *path, wchar_t *name);
void openeditor(BOOK *bk);

bool smthchanged=false;
void TryToUpdate()
{
  SKBOOK *sk=(SKBOOK*)FindBook(isSBook);
  if (sk)
  {
    if (sk->upd)
    {
      elfload((wchar_t const*)sk->upd(),0,0,0);
    }
  }
};

void RemoveItem(int index)
{
  if (!customsofts)return;
  if (index<customsofts->FirstFree && index>=0)
  {
    ITEM *it=(ITEM*)ListElement_Remove(customsofts,index);
    if (it)
    {
      DELETE(it->name);
      DELETE(it->lsi);
      DELETE(it->msi);
      DELETE(it->rsi);
      if (it->keys)
      {
        while (it->keys->FirstFree)
        {
          KEY *k=(KEY*)ListElement_Remove(it->keys,0);
          DELETE(k);
        }
        List_Free(it->keys);
        it->keys=0;
      }
      DELETE(it);
    }
  }
};

void OnDelGui(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->lst);
  if (mbk->curit==0)
  {
    mbk->main_lastindex=item;   
    if (customsofts)
    {
      ITEM *it=(ITEM*)ListElement_GetByIndex(customsofts,item);
      if (item>=0 && item<(customsofts->FirstFree))
      {
        if (wstrcmp(it->name,L"DEFAULT") && wstrcmp(it->name,L"StandbyBook"))
        {
          RemoveItem(item);
        }
      }
    }
    SetNumOfMenuItem(mbk->lst,customsofts->FirstFree+1);
    BookObj_Hide(bk,0);
    BookObj_Show(bk,0);
    smthchanged=true;
  }
  else
  {
    mbk->opt_lastindex=item;
    if (item==0)
    {
      if (wstrcmp(mbk->curit->name,L"DEFAULT") && wstrcmp(mbk->curit->name,L"StandbyBook"))
      {
        DELETE(mbk->curit->name);
        ListMenu_SetSecondLineText(mbk->lst,item,Str2ID(L"-empty-",0,SID_ANY_LEN));
      }
    }
    if (item==1)
    {
      DELETE(mbk->curit->lsi);
      ListMenu_SetSecondLineText(mbk->lst,item,Str2ID(L"-empty-",0,SID_ANY_LEN));
    }
    if (item==2)
    {
      DELETE(mbk->curit->msi);
      ListMenu_SetSecondLineText(mbk->lst,item,Str2ID(L"-empty-",0,SID_ANY_LEN));
    }
    if (item==3)
    {
      DELETE(mbk->curit->rsi);
      ListMenu_SetSecondLineText(mbk->lst,item,Str2ID(L"-empty-",0,SID_ANY_LEN));
    }
    else if (item==4)
    {
      int icon_id=0xFFFF;;
      int ids;
      mbk->curit->style=0;
      iconidname2id(IDN_CHECKBOX_UNCHECKED_ICON,-1,&icon_id);
      ids= 0x78000000 + icon_id;
      ListMenu_SetSecondLineText(mbk->lst,item,ids);
    }
    else if (item>4)
    {
      int key=-1;
      int index=item-5;
      if (mbk->curit->keys==0 && index==0)
      {
        key=-1;
      }
      else if (mbk->curit->keys)
      {
        if (mbk->curit->keys->FirstFree==index)
        {
          key=-1;
        }
        else
        {
          key=index;
        }
      }
      if (key!=-1)
      {
        KEY *k=(KEY*)ListElement_Remove(mbk->curit->keys, key);
        DELETE(k);
        BookObj_CallPage(bk,&bk_main);
      }
    }
    smthchanged=true;
  }
}
GUI *g_save=0;

void OnYesExitGui(BOOK * bk, void *)
{
  GUI_Free(g_save);
  savecustomcfg(GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG), L"AdvSoftkeys.cfg");
  TryToUpdate();
  FreeBook(bk);
};

void OnBackExitGui(BOOK * bk, void *)
{
  GUI_Free(g_save);
  FreeBook(bk);
};
    
void Escape(MyBOOK *mbk, bool save)
{
  if (mbk->curit==0)
  {
    if (smthchanged)
    {
      g_save=CreateYesNoQuestionVA(0,
                                   VAR_BOOK(&mbk->book),
                                   VAR_YESNO_PRE_QUESTION(Str2ID(L"Options were changed.",0,SID_ANY_LEN)),
                                   VAR_YESNO_QUESTION(Str2ID(L"Do you want to save?",0,SID_ANY_LEN)),
                                   0);
      GUIObject_Softkey_SetAction(g_save,ACTION_YES,OnYesExitGui);
      GUIObject_Softkey_SetAction(g_save,ACTION_NO,OnBackExitGui);
      GUIObject_Softkey_SetAction(g_save,ACTION_BACK,OnBackExitGui);
    }
    else
    {
      GUI_Free(g_save);
      FreeBook(&mbk->book);
    }
  }
  else
  {
    FREE_GUI(mbk->lst);
    mbk->curit=0;
    openeditor(&mbk->book);
  }
};

void OnBackGui(BOOK * bk, void *)
{
  Escape((MyBOOK*)bk, false);
};

void onSkin(BOOK *bk, void *)
{
  BookObj_CallPage(bk,&bk_skin);
};

void OnAuthor(BOOK *bk, void *)
{
  wchar_t text[100];
  snwprintf(text,99,L"SoftEdit\n© UltraShot\n\nrevision %d", __SVN_REVISION__ );
  MessageBox(SID_NULL,Str2ID(text,0,SID_ANY_LEN),0, 1 ,5000, bk);
};

void onSave(BOOK *bk, void *)
{
  savecustomcfg(GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG), L"AdvSoftkeys.cfg");
};

void OnSelectGui(BOOK *bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->lst);
  if (mbk->curit==0)
  {
    if (customsofts)
    {
      if (item>=0 && item<(customsofts->FirstFree+1))
      {
        mbk->main_lastindex=item;
        if (item==customsofts->FirstFree)
        {
          smthchanged=true;
          ITEM *it = new ITEM;
          memset(it,0,sizeof(ITEM));
          ListElement_Add(customsofts, it);
        }
        mbk->curit=(ITEM*)ListElement_GetByIndex(customsofts,item);
        FREE_GUI(mbk->lst);
        openeditor(bk);
      }
    }
    else
    {
      ITEM *it = new ITEM;
      memset(it,0,sizeof(ITEM));
      if (!customsofts)customsofts=List_New();
      ListElement_Add(customsofts, it);
      mbk->curit=(ITEM*)ListElement_GetByIndex(customsofts,item);
      FREE_GUI(mbk->lst);
      openeditor(bk);
      smthchanged=true;
    }
  }
  else
  {
    smthchanged=true;
    mbk->opt_lastindex=item;
    if (item==0)
    {
      mbk->inputType=IT_STRING;
      mbk->maxint=100;
      mbk->StringInputType=0;
      BookObj_CallPage(&mbk->book,&bk_StringInput);
    }
    if (item==1)
    {
      mbk->inputType=IT_STRING;
      mbk->maxint=100;
      mbk->StringInputType=1;
      BookObj_CallPage(&mbk->book,&bk_StringInput);
    }
    if (item==2)
    {
      mbk->inputType=IT_STRING;
      mbk->maxint=100;
      mbk->StringInputType=2;
      BookObj_CallPage(&mbk->book,&bk_StringInput);
    }
    if (item==3)
    {
      mbk->inputType=IT_STRING;
      mbk->maxint=100;
      mbk->StringInputType=3;
      BookObj_CallPage(&mbk->book,&bk_StringInput);
    }
    else if (item==4)
    {
      int icon_id=0xFFFF;;
      int ids;
      if (mbk->curit->style==0)
      {
        mbk->curit->style=1;
        iconidname2id(IDN_CHECKBOX_CHECKED_ICON,-1,&icon_id);
        ids= 0x78000000 + icon_id;
      }
      else if (mbk->curit->style==2)
      {
        mbk->curit->style=0;
        iconidname2id(IDN_CHECKBOX_UNCHECKED_ICON,-1,&icon_id);
        ids= 0x78000000 + icon_id;
      }
      else
      {
        mbk->curit->style=2;
        ids=Str2ID(L"standart",0,SID_ANY_LEN);
      }
      ListMenu_SetSecondLineText(mbk->lst,item,ids);
      mbk->opt_lastindex=item;
      FREE_GUI(mbk->lst);
      openeditor(bk);
    }
    else if (item>4)
    {
      KEY *key=0;
      int index=item-5;
      if (mbk->curit->keys==0 && index==0)
      {
        key = new KEY;
        memset(key,0,sizeof(KEY));
        if (!mbk->curit->keys)mbk->curit->keys=List_New();
        ListElement_Add(mbk->curit->keys, key);
      }
      else if (mbk->curit->keys)
      {
        if (mbk->curit->keys->FirstFree==index)
        {
          key = new KEY;
          memset(key,0,sizeof(KEY));
          if (!mbk->curit->keys)mbk->curit->keys=List_New();
          ListElement_Add(mbk->curit->keys, key);
        }
        else
        {
          key=(KEY*)ListElement_GetByIndex(mbk->curit->keys, index);
        }
      }
      if (key)
      {
        mbk->key_stage=0;
        mbk->curkey=key;
        BookObj_CallPage(&mbk->book,&bk_keycode_input);
      }
    }
  }
};



GUI_LIST * CreateGuiList(BOOK * book)
{
  GUI_LIST * lo=0;
  if (lo=CreateListObject(book,0))
  {
    MyBOOK *bk=(MyBOOK*)book;
    if (!bk->curit)
    {
      GUIObject_Softkey_SetAction(lo,1, onSave);
      GUIObject_Softkey_SetText(lo,1,Str2ID(L"Save",0,SID_ANY_LEN));
      GUIObject_Softkey_SetAction(lo,2, onSkin);
      GUIObject_Softkey_SetText(lo,2,Str2ID(L"Skin",0,SID_ANY_LEN));
      GUIObject_Softkey_SetAction(lo,ACTION_HELP, OnAuthor);
      GUIObject_Softkey_SetText(lo,ACTION_HELP, GetStrID(L"JAVA_APP_NR_ABOUT"));
      GuiObject_SetTitleText(lo,Str2ID(L"SoftEdit",0,SID_ANY_LEN));
      if (customsofts)
      {
        if (customsofts->FirstFree)
        {
          SetNumOfMenuItem(lo,customsofts->FirstFree+1);
          if (bk->main_lastindex>=0 && bk->main_lastindex<customsofts->FirstFree+1)
          {
            SetCursorToItem(lo,bk->main_lastindex);
          }
          else
          {
            SetCursorToItem(lo,0);
          }
          ListMenu_SetOnMessages(lo,onLBMessage);
        }
        else
        {
          ListMenu_SetNoItemText(lo,GetStrID(L"MP_AUDIO_NO_ITEMS_TXT"));
          SetNumOfMenuItem(lo,1);
          SetCursorToItem(lo,0);
          ListMenu_SetOnMessages(lo,onLBMessage);
        }
      }
      else
      {
        SetNumOfMenuItem(lo,1);
        SetCursorToItem(lo,0);
        ListMenu_SetOnMessages(lo,onLBMessage);
      }
      SetMenuItemStyle(lo,0);
    }
    else
    {
      wchar_t text[100];
      if (bk->curit->name)
      {
        wstrcpy(text,bk->curit->name);
      }
      else
      {
        wstrcpy(text,L"-empty-");
      }
      GuiObject_SetTitleText(lo,Str2ID(text,0,SID_ANY_LEN));
      int howmany=0;
      if (bk->curit->keys)
      {
        howmany=howmany+bk->curit->keys->FirstFree;
      }
      howmany=howmany+6;
      SetNumOfMenuItem(lo,howmany);
      if (bk->opt_lastindex>0 && bk->opt_lastindex<howmany)
      {
        SetCursorToItem(lo,bk->opt_lastindex);
      }
      else if (bk->opt_lastindex>howmany)
      {
        SetCursorToItem(lo,howmany);
      }
      else
      {
        SetCursorToItem(lo,0);
      }
      SetMenuItemStyle(lo,3);
      ListMenu_SetOnMessages(lo,onLBMessage2);
    }
    GUIObject_Softkey_SetAction(lo,ACTION_BACK, OnBackGui);
    GUIObject_Softkey_SetAction(lo,ACTION_SELECT1, OnSelectGui);
    GUIObject_Softkey_SetAction(lo,ACTION_DELETE, OnDelGui);
  }
  return(lo);
};

void openeditor(BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->lst=CreateGuiList(bk);
  ShowWindow(mbk->lst);
};

static int main_OnEnter(void *, BOOK *bk)
{
  openeditor(bk);
  return 1;
};

const PAGE_MSG bk_msglst_main[] @ "DYN_PAGE"  =
{
  PAGE_ENTER_EVENT_TAG,    main_OnEnter,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_main = {"Softkeys_Main",0,bk_msglst_main};
