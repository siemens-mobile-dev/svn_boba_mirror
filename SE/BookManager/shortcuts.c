#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\cfg_items.h"
#include "..\\include\var_arg.h"
#include "main.h"


typedef struct
{
  wchar_t * name;
  wchar_t * fullpath;
  int appID;
  wchar_t imageID;
}java_list_elem;


#define ActiveTab GetActiveTab()

int get_file(wchar_t * fname,char ** buf_set);
int CreateButtonList(void *data, BOOK * book);

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
          int len=wstrlen(name_buf);
          int len_minus=strlen(param);
          char * str_buf=new char[len+1];
          unicode2win1251(str_buf,name_buf,len);
          fwrite(f,buffer,pos-buffer+9+ActiveTab); //(9+ActiveTab) - длина префикса кнопки
          fwrite(f,str_buf,len);
          fwrite(f,pos+9+ActiveTab+len_minus,(buf_size-(pos-buffer+9+ActiveTab))-len_minus);
          delete(str_buf);
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
          int len=wstrlen(name_buf);
          char * str_buf=new char[len+1];
          unicode2win1251(str_buf,name_buf,len);
          fwrite(f,buffer,pos-buffer+9+ActiveTab);
          fwrite(f,str_buf,len);
          fwrite(f,pos+9+ActiveTab,buf_size-(pos-buffer+9+ActiveTab));
          delete(str_buf);
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
  int dig_num=ListMenu_GetSelectedItem(but_list);
  if (!ListMenu_GetSelectedItem(mode_list))
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

void onShortcutSet(BOOK * MainMenu, GUI* )
{
  wchar_t*name_buf=MenuBook_Desktop_GetSelectedItemID(MainMenu);
  if (name_buf)
  {
    WriteShortcut(name_buf);
    mfree(name_buf);
  }
  FreeBook(MainMenu);
  BookObj_ReturnPage(FindBook(isBookManager),ACCEPT_EVENT);
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
  if (MainMenu=MenuBook_Desktop(1,BOOK_GetBookID(book)))
  {
    MainMenuID=BOOK_GetBookID(MainMenu);
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
  FSTAT _fstat;
  if (DataBrowser_isFileInListExt(ext_table,path,name)) return(1);
  fstat(path,name,&_fstat);
  if ((_fstat.unk1&0x10000)) return(1);
  return(0);
}


int onAccept_DB(void * data, BOOK * book)
{
  wchar_t * path_ptr=wstrwstr( FILEITEM_GetPath((FILEITEM*)data), L"ZBin/");
  wchar_t * elf_buf=new wchar_t[wstrlen(path_ptr+4)+wstrlen( FILEITEM_GetFname((FILEITEM*)data) )+1];
  elf_buf[0]=0;
  if (path_ptr) wstrcpy(elf_buf,path_ptr+4);
  wstrcat(elf_buf,L"/");
  wstrcat(elf_buf, FILEITEM_GetFname((FILEITEM*)data) );
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
  DataBrowserDesc_SetBookID(DB_Desc,BOOK_GetBookID(book));
  DataBrowserDesc_SetFolders(DB_Desc,folder_list);
  DataBrowserDesc_SetFileExtList(DB_Desc,L"*.elf");
  DataBrowserDesc_SetItemFilter(DB_Desc,DB_Filter);
  DataBrowserDesc_SetFoldersNumber(DB_Desc,2);
  DataBrowserDesc_SetSelectAction(DB_Desc,1);
  DataBrowser_Create(DB_Desc);
  DataBrowserDesc_Destroy(DB_Desc);
  return(0);
}

/*
void onPrevious_SI(BOOK * book)
{
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}

void onCancel_SI(BOOK * book)
{
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
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
    int dig_num=ListMenu_GetSelectedItem(but_list);
    if (!ListMenu_GetSelectedItem(mode_list)) sprintf(mask_buf,"[S_KEY%d]",dig_num);
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
*/

void onEnter_JavaList(BOOK * book, GUI* )
{
  java_list_elem * elem=(java_list_elem *)ListElement_GetByIndex(java_list,ListMenu_GetSelectedItem(java_list_menu));
  int java_buf_len=wstrlen(elem->name)+40;
  wchar_t * java_buf=new wchar_t[java_buf_len];
  snwprintf(java_buf,java_buf_len,L"java:%ls//ID=%d",elem->name,elem->appID);

  WriteShortcut(java_buf);
  delete(java_buf);
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}


void ExitJavaList(BOOK * book, GUI* )
{
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
}


void DestroyJavaList(BOOK * book, GUI* )
{
  BookObj_ReturnPage(book,NIL_EVENT);
}


void elem_free(void * elem)
{
  java_list_elem * lm=(java_list_elem *)elem;
  if (lm->name) delete(lm->name);
  if (lm->fullpath) delete(lm->fullpath);
  if (lm->imageID) ImageID_Free(lm->imageID);
  delete(lm);
}

int elem_filter(void * elem)
{
  if (elem) return(1);
  return(0);
}


int onExit_JavaList(void *data, BOOK * book)
{
  if (java_list_menu)
  {
    GUI_Free(java_list_menu);
    java_list_menu=0;
  }
  if (java_list)
  {
    List_FreeElements(java_list,elem_filter,elem_free);
    List_Free(java_list);
    java_list=0;
  }
  return(0);
}


int java_list_callback(GUI_MESSAGE * msg)
{
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    java_list_elem * elem=(java_list_elem*)ListElement_GetByIndex(java_list,GUIonMessage_GetCreatedItemIndex(msg));
    SetMenuItemText0(msg,Str2ID(elem->name,0,SID_ANY_LEN));
    JavaApp_LogoImageID_Get(elem->fullpath,&elem->imageID);
    SetListObjectItemIcon(msg,0,elem->imageID);
  }
  return(1);
}


java_list_elem * CreateElem(void * JavaDesc)
{
  java_list_elem * elem=new java_list_elem;
  wchar_t * sp;
  JavaAppDesc_GetJavaAppInfo(JavaDesc,0,&sp);
  elem->name=sp;
  JavaAppDesc_GetJavaAppInfo(JavaDesc,5,&sp);
  elem->fullpath=sp;
  elem->appID=JavaAppDesc_GetJavaAppID(JavaDesc);
  return(elem);
}


int CreateJavaList(void *data, BOOK * book)
{
  if (java_list)
  {
    List_FreeElements(java_list,elem_filter,elem_free);
    List_Free(java_list);
  }
  java_list=List_New();
  char sp1;
  void * JavaDesc;
  JavaDialog_Open(0,&sp1,&JavaDesc);
  if (!JavaAppDesc_GetFirstApp(JavaDesc))
  {
    int result=0;
    while (!result)
    {
      ListElement_Add(java_list,CreateElem(JavaDesc));
      result=JavaAppDesc_GetNextApp(JavaDesc);
    }
  }
  JavaDialog_Close(sp1);
  if (java_list_menu) GUI_Free(java_list_menu);
  java_list_menu=CreateListObject(book,0);
  GuiObject_SetTitleText(java_list_menu,STR("Java"));
  SetNumOfMenuItem(java_list_menu,java_list->FirstFree);
  OneOfMany_SetonMessage(java_list_menu,java_list_callback);
  SetCursorToItem(java_list_menu,0);
  GUIObject_Softkey_SetAction(java_list_menu,ACTION_BACK,DestroyJavaList);
  GUIObject_Softkey_SetAction(java_list_menu,ACTION_LONG_BACK,ExitJavaList);
  GUIObject_Softkey_SetAction(java_list_menu,ACTION_SELECT1,onEnter_JavaList);
  ShowWindow(java_list_menu);
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

/*
const PAGE_MSG EditShortcut_PageEvents[]@ "DYN_PAGE" ={
  PAGE_ENTER_EVENT_TAG,CreateSI,
  PAGE_EXIT_EVENT_TAG,onExit_SI,
  NIL_EVENT_TAG,0
};

PAGE_DESC ChangeShortcuts_Edit_page ={"BookManager_ChangeShortcuts_Edit_Page",0,EditShortcut_PageEvents};
*/

const PAGE_MSG SetJava_PageEvents[]@ "DYN_PAGE" ={
  PAGE_ENTER_EVENT_TAG,CreateJavaList,
  PAGE_EXIT_EVENT_TAG,onExit_JavaList,
  NIL_EVENT_TAG,0
};

PAGE_DESC ChangeShortcuts_SetJava_page ={"BookManager_ChangeShortcuts_SetJava_Page",0,SetJava_PageEvents};


void But_SetJava(BOOK * book, GUI* )
{
  BookObj_CallPage(book,&ChangeShortcuts_SetJava_page);
}


void DestroyButList(BOOK * book, GUI* )
{
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}



int but_list_callback(GUI_MESSAGE * msg)
{
  int str_id;
  int icon_id;
  int count;
  char * param=0;
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    int item_num=GUIonMessage_GetCreatedItemIndex(msg);
    char mask_buf[10];
    textidname2id(L"SHC_NONE_NAME_TXT",SID_ANY_LEN,&str_id);
    icon_id=0xFFFF;
    if (buffer)
    {
      if (!ListMenu_GetSelectedItem(mode_list))
      {
        if (!ActiveTab) sprintf(mask_buf,"[S_KEY%d]",item_num);
        else sprintf(mask_buf,"[ES_KEY%d]",item_num);
      }
      else
      {
        if (!ActiveTab) sprintf(mask_buf,"[L_KEY%d]",item_num);
        else sprintf(mask_buf,"[EL_KEY%d]",item_num);
      }
      if (param=manifest_GetParam(buffer,mask_buf,0))
      {
        if (strlen(param))
        {
          if (strstr(param,"java:"))
          {
            str_id=Str2ID(param+5,6,strstr(param,"//")-(param+5));
            iconidname2id(L"DB_LIST_JAVA_ICN",SID_ANY_LEN,&icon_id);
          }
          else
          {
            SHORTCUT * w_buf =new(SHORTCUT);
            memset(w_buf,0,0x32);
            w_buf->shortcut_state=0x2;
            if (strlen(param)<0x32) str2wstr(w_buf->name,param);
            else
            {
              char temp_buf[0x32];
              strncpy(temp_buf,param,0x31);
              temp_buf[0x31]=0;
              str2wstr(w_buf->name,temp_buf);
            }
            str_id=Shortcut_Get_MenuItemName(w_buf);
            icon_id=Shortcut_Get_MenuItemIconID(w_buf);
            if (icon_id==0xFFFF) iconidname2id(L"RN_VERT_MY_SHORTCUTS_ICN",SID_ANY_LEN,&icon_id);
            delete(w_buf);
          }
        }
      }
    }
    if (str_id==0x6FFFFFFF) str_id=Str2ID(param,6,SID_ANY_LEN);
    int strID_array[6];
    strID_array[0]=int2strID(item_num);
    strID_array[1]=0x7800003A;
    strID_array[2]=0x78000020;
    if (icon_id!=0xFFFF)
    {
      strID_array[3]=icon_id+0x78000000;
      strID_array[4]=0x78000020;
      strID_array[5]=str_id;
      count=6;
    }
    else
    {
      strID_array[3]=str_id;
      count=4;
    }
    SetMenuItemText0(msg,Str2ID(strID_array,5,count));
  }
  if (param) mfree(param);
  return(1);
}

/*
void But_EditShortcut(BOOK * book, void *)
{
  BookObj_CallPage(book,&ChangeShortcuts_Edit_page);
}
*/

void But_SetMM(BOOK * book, GUI* )
{
  wchar_t name_buf[20];
  wstrcpy(name_buf,L"MainMenu");
  WriteShortcut(name_buf);
  CreateButtonList(0,book);
}

void But_onEnter(BOOK * book, GUI* )
{
  if (!ActiveTab) BookObj_CallPage(book,&SelectShortcut_page);
  else BookObj_CallPage(book,&SelectElf_page);
}


int ExitButtonList(void *data, BOOK * book)
{
  if (but_list)
  {
    GUI_Free(but_list);
    but_list=0;
  }
  return(0);
}


void CancelButtonList(BOOK * book, GUI* )
{
  BookObj_ReturnPage(book,NIL_EVENT);
  BookObj_ReturnPage(book,NIL_EVENT);
}


int DeleteShortcut(char * mask_buf,int f)
{
  char * pos;
  char * param=0;
  int res=0;
  if (buffer)
  {
    if (param=manifest_GetParam(buffer,mask_buf,0))
    {
      int len_minus=strlen(param);
      if (len_minus>0)
      {
        pos=strstr(buffer,mask_buf);
        char * new_buf=new char[buf_size-len_minus];
        memcpy(new_buf,buffer,pos-buffer+9+ActiveTab);
        memcpy(new_buf+(pos-buffer+9+ActiveTab),pos+9+ActiveTab+len_minus,buf_size-(pos-buffer+9+ActiveTab)-len_minus);
        fwrite(f,new_buf,buf_size-len_minus);
        delete(new_buf);
        res=1;
      }
    }
    if (param) mfree(param);
  }
  return(res);
}

void But_onDelete(BOOK * book, GUI* )
{
   int f;
   int res;
   wchar_t * path = get_path();
   if ((f=_fopen(path,L"shortcuts.ini",0x204,0x180,0))>=0)
   {
     char mask_buf[10];
     int dig_num=ListMenu_GetSelectedItem(but_list);
     if (!ListMenu_GetSelectedItem(mode_list))
     {
       if (!ActiveTab) sprintf(mask_buf,"[S_KEY%d]",dig_num);
       else sprintf(mask_buf,"[ES_KEY%d]",dig_num);
       res=DeleteShortcut(mask_buf,f);
     }
     else
     {
       if (!ActiveTab) sprintf(mask_buf,"[L_KEY%d]",dig_num);
       else sprintf(mask_buf,"[EL_KEY%d]",dig_num);
       res=DeleteShortcut(mask_buf,f);
     }
     fclose(f);
     if (res) CreateButtonList(0,book);
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
    but_pos=ListMenu_GetSelectedItem(but_list);
    GUI_Free(but_list);
  }
  but_list=CreateListObject(book,0);
  GuiObject_SetTitleText(but_list,STR("Buttons"));
  SetNumOfMenuItem(but_list,10);
  OneOfMany_SetonMessage(but_list,but_list_callback);
  SetCursorToItem(but_list,but_pos);
  GUIObject_Softkey_SetAction(but_list,ACTION_BACK,DestroyButList);
  GUIObject_Softkey_SetAction(but_list,ACTION_LONG_BACK,CancelButtonList);
  GUIObject_Softkey_SetAction(but_list,ACTION_SELECT1,But_onEnter);
  GUIObject_Softkey_SetAction(but_list,ACTION_DELETE,But_onDelete);
  GUIObject_SoftKey_SetVisible(but_list,ACTION_DELETE,0);
  if (!ActiveTab)
  {
    GUIObject_Softkey_SetAction(but_list,0,But_SetMM);
    textidname2id(L"SHC_SET_MAINMENU_TXT",SID_ANY_LEN,&str_id);
    GUIObject_Softkey_SetText(but_list,0,str_id);
    GUIObject_Softkey_SetAction(but_list,1,But_SetJava);
    GUIObject_Softkey_SetText(but_list,1,STR("Java"));
    /*
    GUIObject_Softkey_SetAction(but_list,2,But_EditShortcut);
    textidname2id(L"CALE_EDIT_EVENT_TXT",SID_ANY_LEN,&str_id);
    GUIObject_Softkey_SetText(but_list,2,str_id);
    */
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


void DestroyModeList(BOOK * book, GUI* )
{
  BookObj_ReturnPage(book,ACCEPT_EVENT);
}


void onEnter_ModeList(BOOK * book, GUI* )
{
  BookObj_CallPage(book,&ChangeShortcuts_Buttons_page);
}


int list_callback(GUI_MESSAGE * msg)
{
  switch( GUIonMessage_GetMsg(msg) )
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
    mode_list_pos=ListMenu_GetSelectedItem(mode_list);
    GUI_Free(mode_list);
  }
  mode_list=CreateListObject(book,0);
  GuiObject_SetTitleText(mode_list,STR("Press Mode"));
  SetNumOfMenuItem(mode_list,2);
  OneOfMany_SetonMessage( mode_list,list_callback);
  SetCursorToItem(mode_list,mode_list_pos);
  GUIObject_Softkey_SetAction(mode_list,ACTION_BACK,DestroyModeList);
  GUIObject_Softkey_SetAction(mode_list,ACTION_LONG_BACK,DestroyModeList);
  GUIObject_Softkey_SetAction(mode_list,ACTION_SELECT1,onEnter_ModeList);
  ShowWindow(mode_list);
  return(0);
}


int ExitShortcutsSet(void *data, BOOK *)
{
  if (mode_list)
  {
    GUI_Free(mode_list);
    mode_list=0;
  }
  if (but_list)
  {
    GUI_Free(but_list);
    but_list=0;
  }
  if (buffer)
  {
    delete(buffer);
    buffer=0;
  }
  if (MainMenuID!=-1)
  {
    BOOK * MainMenu=FindBookByID(MainMenuID);
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


void Shortcuts(BOOK * book, GUI* )
{
  BookObj_CallPage(book,&ChangeShortcuts_page);
}
