#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\cfg_items.h"
#include "conf_loader.h"
#include "config_data.h"
#include "bookman_daemon.h"
#include "..\\include\var_arg.h"

#define COPYRIGHT_STRING STR("\nBookManager v2.8\nbuild 060808\nCopyright (c) 2007-2008\nHussein\n\nRespect\nIronMaster,KreN\n\n")
#define MESSAGE(__STR__) MessageBox(0x6fffffff,__STR__,0, 1 ,11000,(BOOK*)BookManager_Book);
#define BOOKLIST 0
#define ELFLIST 1
#define ICONS_COUNT 4

#define IDN_BOOKS_DEACT_ICON _T("RN_TAB_SESSION_MANAGER_DESELECTED_DISABLED_ICN")
#define IDN_BOOKS_ACT_ICON _T("RN_TAB_SESSION_MANAGER_DESELECTED_ANI_ICN")
#define IDN_ELFS_DEACT_ICON _T("RN_TAB_BOOKMARKS_DESELECTED_DISABLED_ICN")
#define IDN_ELFS_ACT_ICON _T("RN_TAB_BOOKMARKS_DESELECTED_ANI_ICN")

wchar_t *id_names[4]=
{
  IDN_BOOKS_DEACT_ICON,
  IDN_BOOKS_ACT_ICON,
  IDN_ELFS_DEACT_ICON,
  IDN_ELFS_ACT_ICON
};

const char * img_names[4]=
{
  tab1_inact,
  tab1_act,
  tab2_inact,
  tab2_act
};

int onUserInactivity(void * r0, BOOK * bk);
int onRootListChanged(void * r0, BOOK * bk);
void onMyBookClose(BOOK *);
void CloseMyBook(BOOK * Book, void *);
int CreateMenu(void * r0, BOOK * bk);
void TerminateManager(BOOK * Book, void *);
int NewKey(int key, int r1 , int mode);
void SessoinListsFree(BOOK * book);
int get_file(wchar_t * fname,char ** buf_set);
int CreateButtonList(void *data, BOOK * book);

typedef struct
{
  int BookID;
}DESTROYBOOK_DATA;

typedef struct
{
  u16 ImageID;
}IMG;

GUI_LIST * blist;
GUI_LIST * elist;
GUI_LIST * mode_list;
GUI_LIST * but_list;
GUI * str_inp;
char * buffer=0;
int buf_size=0;
int MainMenuID=-1;
int FocusFlag=1;
int blistpos=0;
int elistpos=0;
int blistcnt=0;
int elistcnt=0;
int ActiveTab=0;
IMG dyn_image[ICONS_COUNT];


typedef struct
{
  BOOK  book;
  char * filebuf;
  void * oldOnKey;
  void * oldOnKey1;
  LIST * session_list;
  LIST * elfs_list;
}MyBOOK;

typedef struct
{
  BOOK * book;
  UI_APP_SESSION * session;
  int book_name;
  
}BOOK_ITEM;


typedef struct
{
  BOOK * book;
  int book_name;
  int session_name;
  int isGuiBook;
}BOOK_LIST_ITEM;


typedef struct
{
  UI_APP_SESSION * session;
  LIST * book_list;
  int session_name;
  int book_in_session;
  int isGuiBook;
}SESSION_ITEM;


MyBOOK * BookManager_Book=0;

int isBookManager(BOOK * struc);

wchar_t * get_path()
{
  wchar_t * path = new wchar_t[wstrlen(GetDir(DIR_INI))+8+1];
  wstrcpy(path,GetDir(DIR_INI));
  wstrcat(path,L"/bookman");
  return(path);
}


void Shortcut_Append(wchar_t * name_buf,char * mask_buf,wchar_t * path)
{
  int f;
  if ((f=_fopen(path,L"shortcuts.ini",0x108,0x180,0))>=0)
  {
    char * temp_buf=new char[strlen(mask_buf)+wstrlen(name_buf)+4];
    strcpy(temp_buf,mask_buf);
    strcat(temp_buf," ");
    wstr2strn(temp_buf+strlen(temp_buf),name_buf,wstrlen(name_buf));
    strcat(temp_buf,"\r\n");
    fwrite(f,temp_buf,strlen(temp_buf));
    fclose(f);
    delete(temp_buf);
  }
  else
  {
    MessageBox(0x6fFFFFFF,STR("Can't open shortcuts.ini"),0, 1 ,5000,0);
  }
}
            

void ReWriteShortcut(wchar_t * name_buf,char * mask_buf,wchar_t * path)
{
  int f;
  char * pos;
  char * param;
  if (buffer)
  {
    if (param=manifest_GetParam(buffer,mask_buf,0))
    {
      if (pos=strstr(buffer,mask_buf))
      {
        if ((f=_fopen(path,L"shortcuts.ini",0x204,0x180,0))>=0)
        {
          char str_buf[100];          
          int len=wstrlen(name_buf);
          int len_minus=strlen(param);
          wstr2strn(str_buf,name_buf,len);
          char * new_buf=new char[buf_size+len-len_minus];
          memcpy(new_buf,buffer,pos-buffer+9+ActiveTab);
          memcpy(new_buf+(pos-buffer+9+ActiveTab),str_buf,len);
          memcpy(new_buf+(pos-buffer+9+ActiveTab+len),pos+9+ActiveTab+len_minus,(buf_size-(pos-buffer+9+ActiveTab))-len_minus);
          fwrite(f,new_buf,buf_size+len-len_minus);
          delete(new_buf);
          fclose(f);
        }
        else
        {
          MessageBox(0x6fFFFFFF,STR("Can't open shortcuts.ini"),0, 1 ,5000,0);
        }
      }
      else
      {
        Shortcut_Append(name_buf,mask_buf,path);
      }
      mfree(param);
    }
    else
    {
      if (pos=strstr(buffer,mask_buf))
      {
        if ((f=_fopen(path,L"shortcuts.ini",0x204,0x180,0))>=0)
        {
          char str_buf[100];
          int len=wstrlen(name_buf);
          wstr2strn(str_buf,name_buf,len);
          char * new_buf=new char[buf_size+len];
          memcpy(new_buf,buffer,pos-buffer+9+ActiveTab);
          memcpy(new_buf+(pos-buffer+9+ActiveTab),str_buf,len);
          memcpy(new_buf+(pos-buffer+9+ActiveTab+len),pos+9+ActiveTab,buf_size-(pos-buffer+9+ActiveTab));
          fwrite(f,new_buf,buf_size+len);
          delete(new_buf);
          fclose(f);
        }
        else
        {
          MessageBox(0x6fFFFFFF,STR("Can't open shortcuts.ini"),0, 1 ,5000,0);
        }
      }
      else
      {
        Shortcut_Append(name_buf,mask_buf,path);
      }
    }
  }
  else
  {
    Shortcut_Append(name_buf,mask_buf,path);
  }
}

void WriteShortcut(wchar_t * name_buf)
{
  wchar_t * path = get_path();
  char mask_buf[10];
  int dig_num=GetFocusetListObjectItem(but_list);
  if (!GetFocusetListObjectItem(mode_list))
  {
    if (!ActiveTab) sprintf(mask_buf,"[S_KEY%d]",dig_num);
    else sprintf(mask_buf,"[ES_KEY%d]",dig_num);
    ReWriteShortcut(name_buf,mask_buf,path);
  }
  else
  {
    if (!ActiveTab) sprintf(mask_buf,"[L_KEY%d]",dig_num);
    else sprintf(mask_buf,"[EL_KEY%d]",dig_num);
    ReWriteShortcut(name_buf,mask_buf,path);
  }
  delete(path);
}

void onShortcutSet(BOOK * MainMenu,void*)
{
  wchar_t*name_buf=MenuBook_Desktop_GetSelectedItemID(MainMenu);
  if (name_buf)
  {
    WriteShortcut(name_buf);
    mfree(name_buf);
  }
  FreeBook(MainMenu);
  BookObj_ReturnPage((BOOK*)BookManager_Book,ACCEPT_EVENT);
}


int onPrevious_MainMenu_DB(void *data, BOOK * book)
{
  BookObj_ReturnPage(book,NIL_EVENT);
  return(1);
}


int onCancel_MainMenu_DB(void *data, BOOK * book)
{
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
  return(1);
}


int CreateMainMenu(void *data, BOOK * book)
{
  BOOK * MainMenu;
  if (MainMenu=MenuBook_Desktop(1,BOOK_GetSessionID(book)))
  {
    MainMenuID=BOOK_GetSessionID(MainMenu);
    BookObj_Softkey_SetAction(MainMenu,0x0,onShortcutSet);
    int str_id;
    textidname2id(L"SHC_SET_SHORTCUT_SK",SID_ANY_LEN,&str_id);
    BookObj_Softkey_SetText(MainMenu,0x0,str_id);
    return(1);
  }
  BookObj_ReturnPage(book,ACCEPT_EVENT);
  return(0);
}

int DB_Filter(const wchar_t * ext_table,const wchar_t * path ,const wchar_t * name)
{
  FSTAT fstat;
  if (DataBrowser_isFileInListExt(ext_table,path,name)) return(1);
  isFileExist(path,name,&fstat);
  if ((fstat.unk1&0x10000)) return(1);
  return(0);
}


int onAccept_DB(void * data, BOOK * book)
{
  wchar_t * path_ptr=wstrwstr(((FILEITEM*)data)->path,L"ZBin/");
  wchar_t * elf_buf=new wchar_t[wstrlen(path_ptr+4)+wstrlen(((FILEITEM*)data)->fname)+1];
  elf_buf[0]=0;
  if (path_ptr) wstrcpy(elf_buf,path_ptr+4);
  wstrcat(elf_buf,L"/");
  wstrcat(elf_buf,((FILEITEM*)data)->fname);
  WriteShortcut(elf_buf);
  delete(elf_buf);
  BookObj_ReturnPage(book,ACCEPT_EVENT);
  return(0);
}

int CreateDB(void *data, BOOK * book)
{
  void * DB_Desc=DataBrowserDesc_Create();
  const wchar_t * folder_list[3];
  folder_list[0]=GetDir(DIR_ELFS | MEM_INTERNAL);
  folder_list[1]=GetDir(DIR_ELFS | MEM_EXTERNAL);
  folder_list[2]=0;
  DataBrowserDesc_SetHeaderText(DB_Desc,STR("ZBin"));
  DataBrowserDesc_SetBookID(DB_Desc,BOOK_GetSessionID(book));
  DataBrowserDesc_SetFolders(DB_Desc,folder_list);
  DataBrowserDesc_SetFileExtList(DB_Desc,L"*.elf");
  DataBrowserDesc_SetItemFilter(DB_Desc,DB_Filter);
  DataBrowserDesc_SetFoldersNumber(DB_Desc,2);
  DataBrowserDesc_SetSelectAction(DB_Desc,1);
  DataBrowser_Create(DB_Desc);
  DataBrowserDesc_Destroy(DB_Desc);
  return(0);
}

void onPrevious_SI(BOOK * book)
{
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}

void onCancel_SI(BOOK * book)
{
  BookObj_ReturnPage(book,ACCEPT_EVENT);
  BookObj_ReturnPage(book,ACCEPT_EVENT);
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}

int onExit_SI(void *data, BOOK * book)
{
  if (str_inp)
  {
    GUI_Free(str_inp);
    str_inp=0;
  }
  return(0);
}

void onAccept_SI(BOOK * book,wchar_t * string,int len)
{
  WriteShortcut(string);
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}


int CreateSI(void *data, BOOK * book)
{
  int str_id;
  int editable_strID=0x6FFFFFFF;
  if (buffer)
  {
    char mask_buf[10];
    int dig_num=GetFocusetListObjectItem(but_list);
    if (!GetFocusetListObjectItem(mode_list)) sprintf(mask_buf,"[S_KEY%d]",dig_num);
    else sprintf(mask_buf,"[L_KEY%d]",dig_num);
    char * param;
    if (param=manifest_GetParam(buffer,mask_buf,0))
    {
      editable_strID=Str2ID(param,6,SID_ANY_LEN);
      mfree(param);
    }
  }
  textidname2id(L"SHC_SET_SHORTCUT_SK",SID_ANY_LEN,&str_id);
  if (str_inp) GUI_Free(str_inp);
  str_inp=CreateStringInput(0,
                                                         VAR_PREV_ACTION_PROC(onPrevious_SI),
                                                         VAR_LONG_BACK_PROC(onCancel_SI),
                                                         VAR_BOOK(book),
                                                         VAR_OK_PROC(onAccept_SI),
                                                         VAR_STRINP_FIXED_TEXT(str_id),
                                                         VAR_STRINP_TEXT(editable_strID),
                                                         VAR_STRINP_MIN_LEN(0),
                                                         VAR_STRINP_MAX_LEN(200),
                                                         0);
  return(0);
}


const PAGE_MSG SelectShortcut_PageEvents[]@ "DYN_PAGE" ={
  PAGE_ENTER_EVENT_TAG,CreateMainMenu,
  PREVIOUS_EVENT_TAG,onPrevious_MainMenu_DB,
  CANCEL_EVENT_TAG,onCancel_MainMenu_DB,
  NIL_EVENT_TAG,0
};

PAGE_DESC SelectShortcut_page ={"BookManager_ChangeShortcuts_SelectShortcut_Page",0,SelectShortcut_PageEvents};


const PAGE_MSG SelectElf_PageEvents[]@ "DYN_PAGE" ={
  PAGE_ENTER_EVENT_TAG,CreateDB,
  ACCEPT_EVENT_TAG,onAccept_DB,
  PREVIOUS_EVENT_TAG,onPrevious_MainMenu_DB,
  CANCEL_EVENT_TAG,onCancel_MainMenu_DB,
  NIL_EVENT_TAG,0
};

PAGE_DESC SelectElf_page ={"BookManager_ChangeShortcuts_SelectElf_Page",0,SelectElf_PageEvents};


const PAGE_MSG EditShortcut_PageEvents[]@ "DYN_PAGE" ={
  PAGE_ENTER_EVENT_TAG,CreateSI,
  PAGE_EXIT_EVENT_TAG,onExit_SI,
  NIL_EVENT_TAG,0
};

PAGE_DESC ChangeShortcuts_Edit_page ={"BookManager_ChangeShortcuts_Edit_Page",0,EditShortcut_PageEvents};



void DestroyButList(BOOK * book, void *)
{
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}



int but_list_callback(GUI_MESSAGE * msg)
{
  int str_id;
  char * param=0;
  switch(msg->msg)
  {
  case 1:
    int item_num=GUIonMessage_GetCreatedItemIndex(msg);
    char item_buf[100];
    char mask_buf[10];
    if (buffer)
    {
      if (!GetFocusetListObjectItem(mode_list))
      {
        if (!ActiveTab) sprintf(mask_buf,"[S_KEY%d]",item_num);
        else sprintf(mask_buf,"[ES_KEY%d]",item_num);
        if (param=manifest_GetParam(buffer,mask_buf,0))
        {
          if (strlen(param))
          {
            sprintf(item_buf,"%d: %s",item_num,param);
            SetMenuItemText0(msg,Str2ID(item_buf,6,0xFFFF));
            return(0);
          }
        }
      }
      else
      {
        if (!ActiveTab) sprintf(mask_buf,"[L_KEY%d]",item_num);
        else sprintf(mask_buf,"[EL_KEY%d]",item_num);
        if (param=manifest_GetParam(buffer,mask_buf,0))
        {
          if (strlen(param))
          {
            sprintf(item_buf,"%d: %s",item_num,param);
            SetMenuItemText0(msg,Str2ID(item_buf,6,0xFFFF));
            return(0);
          }
        }
      }
    }
    textidname2id(L"SHC_NONE_NAME_TXT",SID_ANY_LEN,&str_id);
    int strID_array[4];
    strID_array[0]=int2strID(item_num);
    strID_array[1]=0x7800003A;
    strID_array[2]=0x78000020;
    strID_array[3]=str_id;
    SetMenuItemText0(msg,Str2ID(strID_array,5,4));
  }
  if (param) mfree(param);
  return(1);
}

void But_EditShortcut(BOOK * book, void *)
{
  BookObj_CallPage(book,&ChangeShortcuts_Edit_page);
}

void But_SetMM(BOOK * book, void *)
{
  wchar_t name_buf[20];
  wstrcpy(name_buf,L"MainMenu");
  WriteShortcut(name_buf);
  CreateButtonList(0,book);
}

void But_onEnter(BOOK * book, void *)
{
  if (!ActiveTab) BookObj_CallPage(book,&SelectShortcut_page);
  else BookObj_CallPage(book,&SelectElf_page);
}


int ExitButtonList(void *data, BOOK * book)
{
  if (but_list)
  {
    GUI_Free((GUI*)but_list);
    but_list=0;
  }
  return(0);
}


void CancelButtonList(BOOK * book, void *)
{
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
}


void DeleteShortcut(char * mask_buf,int f)
{
  char * pos;
  char * param=0;
  if (buffer)
  {
    if (param=manifest_GetParam(buffer,mask_buf,0))
    {
      int len_minus=strlen(param);
      if (len_minus)
      {
        pos=strstr(buffer,mask_buf);
        char * new_buf=new char[buf_size-len_minus];
        memcpy(new_buf,buffer,pos-buffer+9+ActiveTab);
        memcpy(new_buf+(pos-buffer+9+ActiveTab),pos+9+ActiveTab+len_minus,buf_size-(pos-buffer+9+ActiveTab)-len_minus);
        fwrite(f,new_buf,buf_size-len_minus);
        delete(new_buf);
      }
    }
    if (param) mfree(param);
  }
}

void But_onDelete(BOOK * book, void *)
{
   int f;
   wchar_t * path = get_path();
   if ((f=_fopen(path,L"shortcuts.ini",0x204,0x180,0))>=0)
   {
     char mask_buf[10];
     int dig_num=GetFocusetListObjectItem(but_list);
     if (!GetFocusetListObjectItem(mode_list))
     {
       if (!ActiveTab) sprintf(mask_buf,"[S_KEY%d]",dig_num);
       else sprintf(mask_buf,"[ES_KEY%d]",dig_num);
       DeleteShortcut(mask_buf,f);
     }
     else
     {
       if (!ActiveTab) sprintf(mask_buf,"[L_KEY%d]",dig_num);
       else sprintf(mask_buf,"[EL_KEY%d]",dig_num);
       DeleteShortcut(mask_buf,f);
     }
     fclose(f);
     CreateButtonList(0,book);
   }
   else
   {
     MessageBox(0x6fFFFFFF,STR("Can't open shortcuts.ini"),0, 1 ,5000,0);
   }
   delete(path);
}




int CreateButtonList(void *data, BOOK * book)
{
  int str_id;
  char * sp;
  buf_size=get_file(L"shortcuts.ini",&sp);
  if (buffer) delete(buffer);
  buffer=sp;
  int but_pos=0;
  if (but_list)
  {
    but_pos=GetFocusetListObjectItem(but_list);
    GUI_Free((GUI*)but_list);
  }
  but_list=CreateListObject(book,0);
  GuiObject_SetTitleText(but_list,STR("Buttons"));
  SetNumOfMenuItem(but_list,10);
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)but_list,but_list_callback);
  SetCursorToItem(but_list,but_pos);
  AddMSGHook(but_list,ACTION_BACK,DestroyButList);
  AddMSGHook(but_list,ACTION_LONG_BACK,CancelButtonList);
  AddMSGHook(but_list,ACTION_SELECT1,But_onEnter);
  AddMSGHook(but_list,ACTION_DELETE,But_onDelete);
  SoftKey_SetVisible(but_list,ACTION_DELETE,0);
  if (!ActiveTab)
  {
    AddMSGHook(but_list,0,But_SetMM);
    textidname2id(L"SHC_SET_MAINMENU_TXT",SID_ANY_LEN,&str_id);
    AddCommand(but_list,0,str_id);
    AddMSGHook(but_list,1,But_EditShortcut);
    textidname2id(L"CALE_EDIT_EVENT_TXT",SID_ANY_LEN,&str_id);
    AddCommand(but_list,1,str_id);
  }
    
  ShowWindow(but_list);
  return(0);
}


const PAGE_MSG ChangeShortcuts_Buttons_PageEvents[]@ "DYN_PAGE" ={
  PAGE_ENTER_EVENT_TAG,CreateButtonList,
  ACCEPT_EVENT_TAG,CreateButtonList,
  PAGE_EXIT_EVENT_TAG,ExitButtonList,
  NIL_EVENT_TAG,0
};

PAGE_DESC ChangeShortcuts_Buttons_page ={"BookManager_ChangeShortcuts_Buttons_Page",0,ChangeShortcuts_Buttons_PageEvents};


void DestroyModeList(BOOK * book, void *)
{
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}


void onEnter_ModeList(BOOK * book, void *)
{
  BookObj_CallPage(book,&ChangeShortcuts_Buttons_page);
}


int list_callback(GUI_MESSAGE * msg)
{
  switch(msg->msg)
  {
  case 1:
    if (GUIonMessage_GetCreatedItemIndex(msg))
    {
      SetMenuItemText0(msg,STR("Long"));
    }
    else
    {
      SetMenuItemText0(msg,STR("Short"));
    }
  }
  return(1);
}


int CreateModeList(void *data, BOOK * book)
{
  int mode_list_pos=0;
  if (mode_list)
  {
    mode_list_pos=GetFocusetListObjectItem(mode_list);
    GUI_Free((GUI*)mode_list);
  }
  mode_list=CreateListObject(book,0);
  GuiObject_SetTitleText(mode_list,STR("Press Mode"));
  SetNumOfMenuItem(mode_list,2);
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)mode_list,list_callback);
  SetCursorToItem(mode_list,mode_list_pos);
  AddMSGHook(mode_list,ACTION_BACK,DestroyModeList);
  AddMSGHook(mode_list,ACTION_LONG_BACK,DestroyModeList);
  AddMSGHook(mode_list,ACTION_SELECT1,onEnter_ModeList);
  ShowWindow(mode_list);
  return(0);
}


int ExitShortcutsSet(void *data, BOOK *)
{
  if (mode_list)
  {
    GUI_Free((GUI*)mode_list);
    mode_list=0;
  }
  if (but_list)
  {
    GUI_Free((GUI*)but_list);
    but_list=0;
  }
  if (buffer)
  {
    delete(buffer);
    buffer=0;
  }
  if (MainMenuID!=-1)
  {
    BOOK * MainMenu=Root_GetBookByID(MainMenuID);
    if (MainMenu) FreeBook(MainMenu);
    MainMenuID=-1;
  }
  return(0);
}


const PAGE_MSG ChangeShortcuts_PageEvents[]@ "DYN_PAGE" ={
  PAGE_ENTER_EVENT_TAG,CreateModeList,
  ACCEPT_EVENT_TAG,CreateModeList,
  PAGE_EXIT_EVENT_TAG,ExitShortcutsSet,
  NIL_EVENT_TAG,0
};

PAGE_DESC ChangeShortcuts_page ={"BookManager_ChangeShortcuts_Main_Page",0,ChangeShortcuts_PageEvents};


void Shortcuts(BOOK * book,void * lt)
{
  BookObj_CallPage(book,&ChangeShortcuts_page);
}

int onMWLostFocus(void *data, BOOK *)
{
  FocusFlag=0;
  return(0);
}

int onMWGotFocus(void *data, BOOK *)
{
  FocusFlag=1;
  return(1);
}


const PAGE_MSG bk_msglst[] @ "DYN_PAGE"  =
{
  BOOK_DESTROYED_EVENT_TAG,onRootListChanged,
  USER_INACTIVITY_EVENT_TAG,onUserInactivity,
  MAIN_WINDOW_GOT_FOCUS_EVENT_TAG,onMWGotFocus,
  MAIN_WINDOW_LOST_FOCUS_EVENT_TAG,onMWLostFocus,
  NIL_EVENT_TAG,0
};

const PAGE_DESC BookManager_Base_Page = {"BookManager",0,bk_msglst};



void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

//=====================================================================================
/*
int RegisterImage(IMG * i , wchar_t * path, wchar_t * fname)
{
  int r=0;
  char error_code;
  i->ImageID=0xFFFF;
  i->ImageHandle=0xFFFF;
  // если файл существует
  if (!isFileExist(path,fname,0))
  {
    // получаем новый IMAGEHANDLE
    if (!REQUEST_IMAGEHANDLER_INTERNAL_GETHANDLE(SYNC,&i->ImageHandle,&error_code))
    {
      // получаем динамический ID картинки
      if (!REQUEST_IMAGEHANDLER_INTERNAL_REGISTER(SYNC,i->ImageHandle,path,fname,0,&i->ImageID,&error_code))
      {
        if (error_code)
        {
          i->ImageHandle=0xFFFF;
        }
        else r=1;
      }
    }
  }
  return (r);
};
*/

int isBookManager(BOOK * struc)
{
  if (struc->onClose==(void*)onMyBookClose)
  {
    return(1);
  }
  else
  {
    return(0);
  }
}

void win12512unicode(wchar_t *ws, char *s, int len)
{
  int c;
  while((c=*s++)&&((len--)>0))
  {
    if (c==0xA8) c=0x401;
    if (c==0xAA) c=0x404;
    if (c==0xAF) c=0x407;
    if (c==0xB8) c=0x451;
    if (c==0xBA) c=0x454;
    if (c==0xBF) c=0x457;
    if ((c>=0xC0)&&(c<0x100)) c+=0x350;
    *ws++=c;
  }
  *ws=0;
}

char *unicode2win1251(char *s, wchar_t *ws, int len)
{
  char *d=s;
  int c;
  while((c=*ws++)&&((len--)>0))
  {
    if (c==0x401) c=0xA8;
    if (c==0x404) c=0xAA;
    if (c==0x407) c=0xAF;
    if (c==0x451) c=0xB8;
    if (c==0x454) c=0xBA;
    if (c==0x457) c=0xBF;
    if ((c>=0x410)&&(c<0x450)) c-=0x350;
    *s++=c;
  }
  *s=0;
  return(d);
}

int w1251toUNICODE (u16 * wstr)
{
  u16 * ws=wstr;
  u16 c;
  while(*ws)
  {
    c=*ws;
    if (c==0xA8) c=0x401;
    if (c==0xAA) c=0x404;
    if (c==0xAF) c=0x407;
    if (c==0xB8) c=0x451;
    if (c==0xBA) c=0x454;
    if (c==0xBF) c=0x457;
    if ((c>=0xC0)&&(c<0x100)) c+=0x350;
    *ws++=c;
  }
  return(Str2ID(wstr,0,SID_ANY_LEN));
};

// проверка, обрабатывает ли БЕЙЗ_ПЕЙДЖ книги событие 
int CheckEv(BOOK * bk, int ev)
{
  PAGE_MSG * pm;
  if (bk->base_page)
  {
    pm=(PAGE_MSG *)bk->base_page->msg_lst;
    if (pm)
    {
      while ((pm->msg) && (pm->proc))
      {
        if (pm->msg==ev)
        {
          return(1);
        }
        pm++;
      }
    }
  }
  return(0);
}

// взять значение из ини-файла
int GetParam(char * name)
{
  wchar_t ws[50];
  int sID=0;
  MyBOOK * myBook = (MyBOOK *) FindBook(isBookManager);
  
  if (myBook->filebuf)
  {
    char * param;
    if (param=manifest_GetParam(myBook->filebuf,name,0))
    {
      win12512unicode(ws,param,49);
      sID=Str2ID(ws,0,SID_ANY_LEN);
      mfree(param);
      return(sID);
    }
  }
  if(!sID)
  {
    win12512unicode(ws,name,49);
    sID=Str2ID(ws,0,wstrlen(ws));
  }
  return(sID);
}

// получить имя жавы
int GetJavaName(BOOK * bk)
{
  wchar_t ws[100];
  TextID2wstr(Book_GetSession(bk)->name,ws,100);
  if (!wstrncmp(ws,L"Foreign app",11))
  {
    return(JavaSession_GetName());
  }
  if (!wstrcmp(ws,L"Java"))
  {
    return(JavaSession_GetName());
  }
  return(0);
}

// создаём список сессий/книг
void CreateBookLst(MyBOOK * myBook)
{
  
  int i,j,k,cnt,fgui;
  int mask=((int)LastExtDB())&0xF8000000;
  BOOK * book;
  UI_APP_SESSION * session;
  SESSION_ITEM *si;
  i=root_list_get_session_count();
  SessoinListsFree((BOOK*)myBook);
  myBook->session_list=List_New();
  myBook->elfs_list=List_New();
  cnt=0;
  int elfs=0;
  for (j=0;j<i;j++)
  {
    si=0;
    session=root_list_get_session(j);
    for (k=0;k<session->listbook->FirstFree;k++)
    {
      book=(BOOK*)ListElement_GetByIndex(session->listbook,k);
      if( (fgui=book->xguilist->guilist->FirstFree)||(((((int)book->onClose)&0xF8000000)!=mask)))
      {
        if ((!isBookManager(book))&&(!isVolumeControllerBook(book))&&(!isRightNowBook(book)))
        {
          
          {
            //                      if (!si)
            {
              char s[100];
              wchar_t ws[100];
              si=new SESSION_ITEM;
              si->book_list=List_New();
              if ((strcmp(book->xbook->name,"CUIDisplayableBook"))&&(NameBookAsSession==0))
              {
                str2wstr(ws,book->xbook->name);
              }
              else
              {
                TextID2wstr(book->xbook->app_session->name,ws,100);
              }
              si->session_name=GetParam(unicode2win1251(s,ws,100));
              si->session=session;
              si->isGuiBook=fgui;
            }
            BOOK_ITEM * b = new BOOK_ITEM;
            b->book=book;
            b->session=session;
            b->book_name=0x6fffffff;
            
            int tmp;
            if ((tmp=GetJavaName(book)))
            {
              TextFree(si->session_name);
              si->session_name=tmp;
            }
            
            
            ListElement_Add(si->book_list,b);
          }
          if ((ElfInBookListEnabled)&&((((int)book->onClose)&0xF8000000)!=mask))
          {
            if (fgui)
            {
              si->book_in_session=si->book_list->FirstFree;
              ListElement_AddtoTop(myBook->session_list,si);
              cnt++;
            }
          }
          if (((((int)book->onClose)&0xF8000000)==mask))
          {
            si->book_in_session=si->book_list->FirstFree;
            ListElement_AddtoTop(myBook->session_list,si);
            cnt++;
          }
          else
          {
            si->book_in_session=si->book_list->FirstFree;
            ListElement_AddtoTop(myBook->elfs_list,si);
            elfs++;
          }
        }
      }
    }
  }
  blistcnt=cnt;
  elistcnt=elfs;
}


// при убийстве какой либо книги
int onRootListChanged(void * r0, BOOK * bk)
{
  if ((!mode_list)&&(!but_list))
  {
    // если ГУИ есть, то убили
    if (bk->gui) GUI_Free((GUI*)bk->gui);
    
    // создали меню
    CreateMenu(0,bk);
  }
  return(0);
};

// если юзер заснул...
int onUserInactivity(void * r0, BOOK * bk)
{
  if (UserInactivityEventEnabled)  CloseMyBook(bk,0);
  
  return(0);
};

// устанавливаем тексты в пунктах меню
int onLBMessage(GUI_MESSAGE * msg)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isBookManager);
  int item;
  SESSION_ITEM * si;
  switch(msg->msg)
  {
  case 1:
    item=GUIonMessage_GetCreatedItemIndex(msg);
    si=(SESSION_ITEM *)ListElement_GetByIndex(myBook->session_list,item);
    SetMenuItemText0(msg,TextCopyId(si->session_name));
    break;
  }
  return(1);
};


int onLBMessage1(GUI_MESSAGE * msg)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isBookManager);
  int item;
  SESSION_ITEM * si;
  switch(msg->msg)
  {
  case 1:
    if (myBook->elfs_list->FirstFree)
    {
      item=GUIonMessage_GetCreatedItemIndex(msg);
      si=(SESSION_ITEM *)ListElement_GetByIndex(myBook->elfs_list,item);
      SetMenuItemText0(msg,TextCopyId(si->session_name));
      if (!si->isGuiBook)
      {
        GUIonMessage_SetItemDisabled(msg,1);
        SetMenuItemText3(msg,STR("Can't set focus to elf without GUI..."));
      }
      break;
    }
  }
  return(1);
};

// взять выбранную книгу из листа книг/эльфов 
BOOK * GetBook(int list)
{
  SESSION_ITEM * si;
  switch (list)
  {
  case BOOKLIST:
    blistpos=GetFocusetListObjectItem(blist);
    si= (SESSION_ITEM*)ListElement_GetByIndex(BookManager_Book->session_list,blistpos);
    break;
  case ELFLIST:
    if (BookManager_Book->elfs_list->FirstFree)
    {
      elistpos=GetFocusetListObjectItem(elist);
      si= (SESSION_ITEM*)ListElement_GetByIndex(BookManager_Book->elfs_list,elistpos);
    }
    else
    {
      return(0);
    }
    break;
  }
  return(((BOOK_ITEM*)(ListElement_GetByIndex(si->book_list,0)))->book);
}

// при выборе пункта
void onEnterPressed(BOOK * book,void * lt)
{
  BookObj_SetFocus(GetBook(BOOKLIST),0);
  CloseMyBook(book,0);
};

// при выборе эльфа
void onEnterPressed1(BOOK * book,void * lt)
{
  BOOK * bk = GetBook(ELFLIST );
  if (bk)
  {
    BookObj_SetFocus(bk,0);
    CloseMyBook(book,0);
  }
};

// при нажатии "С"
void onDelPressed(BOOK * book,void * lt)
{
  BOOK * bk = GetBook(BOOKLIST );
  if (bk)
  {
    if (GetJavaName(bk))
    {
      JavaSession_Manager(0x0E);
    }
    else
    {
      if (Find_StandbyBook()!=bk)
      {
        UI_Event_toSID(RETURN_TO_STANDBY_EVENT,BOOK_GetSessionID(bk));
        UI_Event_toSID(TERMINATE_SESSION_EVENT,BOOK_GetSessionID(bk));
      }
    }
  }
};

// при нажатии "С" на эльфе
void onDelPressed1(BOOK * book,void * lt)
{
  BOOK * bk = GetBook(ELFLIST );
  if (bk)
  {
    {
      if (Find_StandbyBook()!=bk)
      {
        if (CheckEv(bk,RETURN_TO_STANDBY_EVENT))
        {
          UI_Event_toSID(RETURN_TO_STANDBY_EVENT,BOOK_GetSessionID(bk));
        }
        else
        {
          if (CheckEv(bk,ELF_TERMINATE_EVENT))
          {
            UI_Event_toSID(ELF_TERMINATE_EVENT,BOOK_GetSessionID(bk)/*bk->BookID*/);
          }
          else
          {
            MESSAGE(STR("TerminateEvent not supported by elf..."));
          }
        }
      }
    }
  }
};

typedef struct
{
  BOOK * book;
}MSG;

// при нажатии "Автора!" на эльфе
void Author(BOOK * book,void * lt)
{
  
  BOOK * bk = GetBook(ELFLIST );
  if (bk)
  {
    {
      {
        if (CheckEv(bk,ELF_SHOW_INFO_EVENT))
        {
          MSG * msg =new MSG;
          msg->book=(BOOK*)BookManager_Book;
          UI_Event_toSIDwData(ELF_SHOW_INFO_EVENT,BOOK_GetSessionID(bk),msg,(void(*)(void*))mfree_adr());
        }
        else
        {
          MESSAGE(STR("Author unknown"));
        }
      }
    }
  }
};

void Copyright(BOOK * book,void * lt)
{
  blistpos=GetFocusetListObjectItem(blist);
  MESSAGE(COPYRIGHT_STRING);
};

// патченый onKey от своей менюхи...;)
void myOnKey(void *p, int i1, int i2, int i3, int i4)
{
  void (*OldOnKey)(void *, int, int, int, int);
  
  MyBOOK * myBook = (MyBOOK *) FindBook(isBookManager);
  OldOnKey=(void(*)(void *,int,int,int,int))myBook->oldOnKey;
  OldOnKey(p,i1,i2,i3,i4);
  if ((i4==KBD_SHORT_RELEASE)||(i4==KBD_LONG_PRESS))
  {
    if (i1==KEY_DEL)
    {
      onDelPressed((BOOK*)myBook,0);
    }
    if (i1==KEY_DIEZ)
    {
      if (i4==KBD_SHORT_RELEASE)
      {
        BOOK *stby_bk=Find_StandbyBook();
	if (stby_bk) BookObj_SetFocus(stby_bk,0);
      }
      else
      {
	UI_Event(RETURN_TO_STANDBY_EVENT);
      }
      CloseMyBook((BOOK*)myBook,0);
    }
    char * sp;
    get_file(L"shortcuts.ini",&sp);
    char * short_buf=sp;
    if (short_buf)
    {
      char key[20];
      char * param;
      sprintf(key,i4==3?"[S_KEY%d]":"[L_KEY%d]",i1-KEY_DIGITAL_0);
      if (param=manifest_GetParam(short_buf,key,0))
      {
        wchar_t par[256];
        str2wstr(par,param);
        if (strstr(param,"java:"))
        {
          u16 name_len=strstr(param,"//")-param-5;
          u16 vendor_len=strlen(param+7+name_len);
          
          MIDP_DESC * java=new MIDP_DESC;
          java->name=new MIDP_DESC_ITEM;
          java->vendor=new MIDP_DESC_ITEM;
          java->point=0;
          java->name->item_name=new wchar_t[name_len];
          java->name->item_name_len=name_len;
          java->name->const_2=2;
          java->vendor->item_name=new wchar_t[vendor_len];
          java->vendor->item_name_len=vendor_len;
          java->vendor->const_2=2;
          
          wstrncpy(java->name->item_name,par+5,name_len);
          wstrncpy(java->vendor->item_name,par+7+name_len,vendor_len);
          _REQUEST_OAF_START_APPLICATION(ASYNC,0,java,0);
          delete(java->name->item_name);
          delete(java->vendor->item_name);
          delete(java->vendor);
          delete(java->name);
          delete(java);
        }
        else
        {
          if (StandbyOnTop)
          {
            StartAPP(par);
            Show(Find_StandbyBook(),0);
          }
          else
          {
            Shortcut_Run(par);
          }
        }
	mfree(param);
        CloseMyBook((BOOK*)myBook,0);
      }
      mfree(short_buf);
    }
  }
};

int StartElf(wchar_t * path , char * name)
{
  int res=-1;
  wchar_t * elfname = new wchar_t[wstrlen(path)+strlen(name)+1];
  wchar_t * wname = new wchar_t[strlen(name)+1];
  str2wstr(wname,name);
  wstrcpy(elfname,path);
  wstrcat(elfname,wname);
  
  {
    wchar_t *path;
    wchar_t *name;
    int len;
    name=wstrrchr(elfname,'/');
    if (name)
    {
      path=new wchar_t [((len=name-elfname)+1)];
      memcpy(path,elfname,len*sizeof(wchar_t));
      path[len]=0;
      name++;
      if (!isFileExist(path,name,0))
      {
        res=elfload(elfname,0,0,0);
      }
      delete(path);
    }
  }
  delete(elfname);
  delete(wname);
  return(res);
}


// патченый onKey от вкладки "ЭЛЬФЫ"...;)
void myOnKey1(void *p, int i1, int i2, int i3, int i4)
{
  void (*OldOnKey)(void *, int, int, int, int);
  
  MyBOOK * myBook = (MyBOOK *) FindBook(isBookManager);
  OldOnKey=(void(*)(void *,int,int,int,int))myBook->oldOnKey;
  OldOnKey(p,i1,i2,i3,i4);
  if ((i4==KBD_SHORT_RELEASE)||(i4==KBD_LONG_PRESS))
  {
    if (i1==KEY_DEL)
    {
      onDelPressed1((BOOK*)myBook,0);
    }
    if (i1==KEY_DIEZ)
    {
      if (i4==KBD_SHORT_RELEASE)
      {
        BOOK *stby_bk=Find_StandbyBook();
	if (stby_bk) BookObj_SetFocus(stby_bk,0);
      }
      /*
      else
      {
      UI_Event(RETURN_TO_STANDBY_EVENT);
    }
      */
      CloseMyBook((BOOK*)myBook,0);
      
    }
    char * sp;
    get_file(L"shortcuts.ini",&sp);
    char * short_buf=sp;
    if (short_buf)
    {
      char key[20];
      char * param;
      sprintf(key,i4==3?"[ES_KEY%d]":"[EL_KEY%d]",i1-KEY_DIGITAL_0);
      if (param=manifest_GetParam(short_buf,key,0))
      {
	if(StartElf(GetDir(DIR_ELFS | MEM_INTERNAL),param))
          if (StartElf(GetDir(DIR_ELFS | MEM_EXTERNAL),param))
          {
            int ms[3];
            ms[0]=STR("ZBin");
            ms[1]=Str2ID(param,1,strlen(param));
            ms[2]=STR("\n\nnot found..");
            MESSAGE(Str2ID(ms,5,3));
          }
	mfree(param);
        CloseMyBook((BOOK*)myBook,0);
      }
      mfree(short_buf);
    }
  }
};

void onTabSwitch(BOOK * bk,int active_tab)
{
  ActiveTab=active_tab;
}

// создание меню
GUI_TABMENUBAR * CreateGuiList(void * r0, BOOK * bk)
{
  int str_id;
  int p[2];
  GUI_LIST * lo;
  blist =lo =CreateListObject(bk,0);
  elist =CreateListObject(bk,0);
//  bk->gui=(GUI*)lo;
  CreateBookLst((MyBOOK*)bk);
  p[0]=Str2ID (L"Heap : ",0,7);
  p[1]=int2strID (GetFreeBytesOnHeap());
  GuiObject_SetTitleText(lo,Str2ID(p,5,2));
  
  
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)lo,onLBMessage);
  SetNumOfMenuItem(lo,blistcnt);
  if (blistpos>blistcnt)
  {
    SetCursorToItem(lo,blistcnt);
  }
  else
  {
    SetCursorToItem(lo,blistpos);
  }
  
  AddMSGHook(lo,ACTION_BACK, CloseMyBook);
  AddMSGHook(lo,ACTION_LONG_BACK, TerminateManager);
  AddMSGHook(lo,ACTION_SELECT1,onEnterPressed);

  AddMSGHook(lo,0,Shortcuts);
  textidname2id(L"SHC_EDIT_SHORTCUT_TXT",SID_ANY_LEN,&str_id);
  AddCommand(lo,0,str_id);
  AddMSGHook(lo,1,Copyright);
  AddCommand(lo,1,STR("About"));

  ((MyBOOK*)bk)->oldOnKey=(void*)DISP_OBJ_GetOnKey(lo->DISP_OBJ);
  
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC (lo->DISP_OBJ), (DISP_OBJ_ONKEY_METHOD)myOnKey );
  //---------------
  
  
  
  GuiObject_SetTitleText(elist,(STR("Elfs")));
  SetCursorToItem(elist,0);
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)elist,onLBMessage1);
  SetNumOfMenuItem(elist,elistcnt);
  if (elistpos>elistcnt)
  {
    SetCursorToItem(elist,elistcnt);
  }
  else
  {
    SetCursorToItem(elist,elistpos);
  }
  AddMSGHook(elist,ACTION_BACK, CloseMyBook);
  AddMSGHook(elist,ACTION_LONG_BACK, TerminateManager);
  AddMSGHook(elist,0,Shortcuts);
  textidname2id(L"SHC_EDIT_SHORTCUT_TXT",SID_ANY_LEN,&str_id);
  AddCommand(elist,0,str_id);
  
  if (((MyBOOK*)bk)->elfs_list->FirstFree)
  {
    AddMSGHook(elist,ACTION_SELECT1,onEnterPressed1);
    AddMSGHook(elist,1,Author);
    AddCommand(elist,1,STR("Author"));
  }
  else
  {
    ListMenu_SetNoItemText(elist,STR("No elfs in memory"));
  }
 
  ((MyBOOK*)bk)->oldOnKey1=(void*)DISP_OBJ_GetOnKey(elist->DISP_OBJ);
  
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC (elist->DISP_OBJ), (DISP_OBJ_ONKEY_METHOD)myOnKey1 );
  
  
  GUI_TABMENUBAR * tab = CreateTabMenuBar(bk);
  TabMenuBar_SetTabCount(tab,2);
  int tab_pos;
  if (!FirstTab) tab_pos=ActiveTab;
  else tab_pos=FirstTab-1;
  TabMenuBar_SetTabFocused(tab,tab_pos);
  TabMenuBar_AssignGuiObj(tab,0,lo);
  TabMenuBar_AssignGuiObj(tab,1,elist);

  TabMenuBar_SetTabIcon(tab,0,dyn_image[0].ImageID,0);
  TabMenuBar_SetTabIcon(tab,0,dyn_image[1].ImageID,1);
  
  TabMenuBar_SetTabIcon(tab,1,dyn_image[2].ImageID,0);
  TabMenuBar_SetTabIcon(tab,1,dyn_image[3].ImageID,1);
  
  TabMenuBar_SetOnTabSwitch(tab,onTabSwitch);
  
  return(tab);
};

// создание и отображение меню
int CreateMenu(void * r0, BOOK * bk)
{
  bk->gui=(GUI*)CreateGuiList(0,bk);
  ShowWindow(bk->gui);
  return(0);
}

int h2i(char * h)
{
  char c;
  int res=0;
  while((*h++)!='x');
  do
  {
    c=*h++;
    if (c>0x60) c-=0x20; else c-=0x30;
    if (c>9) c-=7;
    res<<=4;
    res|=c;
  }
  while(*h!=';');
  return(res);
}

void SessoinListsFree(BOOK * book)
{
  MyBOOK * myBook=(MyBOOK*)book;
  LIST * lst = myBook->session_list;
  SESSION_ITEM * si;
  
  while (lst->FirstFree)
  {
    int i=0;
    si=(SESSION_ITEM *)ListElement_Remove(lst,0);
    if (ListElement_Prt2NumElement(myBook->elfs_list,si)==0xFFFF)
    {
      // удаляем, если не в списке эльфов
      while (i<si->book_list->FirstFree)
      {
        BOOK_ITEM * bi=(BOOK_ITEM *)ListElement_Remove(si->book_list,i++);
        TextFree(bi->book_name);
        delete(bi);
      }
      TextFree(si->session_name);
      List_Free(si->book_list);
      delete(si);
    }
  }
  List_Free(myBook->session_list);
  
  lst = myBook->elfs_list;
  while (lst->FirstFree)
  {
    int i=0;
    si=(SESSION_ITEM *)ListElement_Remove(lst,0);
    
    while (i<si->book_list->FirstFree)
    {
      BOOK_ITEM * bi=(BOOK_ITEM *)ListElement_Remove(si->book_list,i++);
      TextFree(bi->book_name);
      delete(bi);
    }
    TextFree(si->session_name);
    List_Free(si->book_list);
    delete(si);
  }
  List_Free(myBook->elfs_list);
}

// при закрытии книги
void onMyBookClose(BOOK * book)
{
  MyBOOK * myBook=(MyBOOK*)book;
  // выгрузили файло
  if (myBook->filebuf)
  {
    mfree(myBook->filebuf);
    myBook->filebuf=0;
  }
  if (mode_list)
  {
    GUI_Free((GUI*)mode_list);
    mode_list=0;
  }
  if (but_list)
  {
    GUI_Free((GUI*)but_list);
    but_list=0;
  }
  if (str_inp)
  {
    GUI_Free(str_inp);
    str_inp=0;
  }
  if (buffer)
  {
    delete(buffer);
    buffer=0;
  }
  if (MainMenuID!=-1)
  {
    BOOK * MainMenu=Root_GetBookByID(MainMenuID);
    if (MainMenu) FreeBook(MainMenu);
    MainMenuID=-1;
  }
  // освободили списки книг/сессий
  SessoinListsFree(book);
  
  // выгрузили иконки
  int i;
  for (i=0;i<ICONS_COUNT;i++)
  {
    ImageID_Free(dyn_image[i].ImageID);
  }
  
  CreateDaemon();
}


void CloseMyBook(BOOK * Book, void *)
{
  FreeBook(Book);
  BookManager_Book=0;
};

void TerminateManager(BOOK * Book, void *)
{
  MESSAGE(STR("BookManager\n\nterminated"));
  FreeBook(Book);
  DestroyDaemon();
  ModifyKeyHook(NewKey,0);
  SUBPROC(elf_exit);
};


int get_file(wchar_t * name,char ** buf_set)
{
  int size=0;
  int file;
  char * buf=0;
  FSTAT fstat;
  wchar_t * path = get_path();
  if (isFileExist(path,name,&fstat)==0)
  {
    if ((file=_fopen(path,name,0x1,0x180,0))>=0)
    {
      buf=(char*)malloc(fstat.fsize+1);
      fread(file,buf,fstat.fsize);
      fclose(file);
      size=fstat.fsize;
    }
  }
  buf_set[0]=buf;
  delete(path);
  return(size);
}

// берем значения из ини-файла

void get_iconsID(void)
{
  int i;
  wchar_t imgID;
  wchar_t wstr_name[40];
  wchar_t * path=get_path();
  for (i=0;i<ICONS_COUNT;i++)
  {
    win12512unicode(wstr_name,(char*)img_names[i],strlen(img_names[i]));
    if (ImageID_Get(path,wstr_name,&imgID)<0)
    {
      iconidname2id(id_names[i],SID_ANY_LEN,&imgID);
    }
    dyn_image[i].ImageID=imgID;
  }
}


// собственно старт
__root int CreateBookList(void)
{
  if (!BookManager_Book)
  {
    DestroyDaemon();
    MyBOOK * myBook =BookManager_Book= (MyBOOK *)malloc(sizeof(MyBOOK));
    memset(myBook,0,sizeof(MyBOOK));
    CreateBook(myBook,onMyBookClose,&BookManager_Base_Page,"BookManager",-1,0);
    //
    myBook->session_list=List_New();
    myBook->elfs_list=List_New();
    char * sp;
    get_file(L"bookman.ini",&sp);
    if (myBook->filebuf) delete(myBook->filebuf);
    myBook->filebuf=sp;
    get_iconsID();
    elistpos=0;
    blistpos=0;
    mode_list=0;
    but_list=0;
    buffer=0;
    str_inp=0;
    CreateMenu(0,(BOOK*)myBook);
  }
  return(0);
}


int NewKey(int key, int r1 , int mode)
{
  if ((!mode_list)&&(!but_list))
  {
    if (mode==KeyPressMode)
    {
      if (key==KeyActiv)
      {
        if ((isKeylocked())&&(!Ignore_KeyLock)) return(0);
        BOOK * bk;
        if (!(bk=FindBook(isBookManager)))
        {
          CreateBookList();
          return(-1);
        }
        else
        {
          CloseMyBook(bk,0);
          return(0);
        }
      }
    }
  }
  return(0);
}




int main (void)
{
  if (!FindBook(isBookmanDaemonBook))
  {
    CreateDaemon();
    BookManager_Book=0;
    InitConfig();
    ModifyKeyHook(NewKey,1);
  }
  else MessageBox(0x6fFFFFFF,STR("BookManager is already runed"),0, 1 ,5000,0);
  return(0);
}
