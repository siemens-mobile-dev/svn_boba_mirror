#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "conf_loader.h"
#include "config_data.h"
#include "bookman_daemon.h"
#include "shortcuts.h"


#define COPYRIGHT_STRING STR("\nBookManager v2.95\nbuild 291008\nCopyright (c) 2007-2008\nHussein\n\nRespect\nIronMaster,KreN\n\n")
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


typedef struct
{
  BOOK  book;
  GUI_TABMENUBAR * gui;
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


typedef struct
{
  int BookID;
}DESTROYBOOK_DATA;


typedef struct
{
  wchar_t ImageID;
}IMG;

MyBOOK * BookManager_Book=0;
IMG dyn_image[ICONS_COUNT];

GUI_LIST * java_list_menu;
LIST * java_list;

GUI_LIST * blist;
GUI_LIST * elist;
GUI_LIST * mode_list;
GUI_LIST * but_list;
//GUI * str_inp;
char * buffer=0;
int buf_size=0;
int MainMenuID=-1;
int blistpos=0;
int elistpos=0;
int blistcnt=0;
int elistcnt=0;
int ActiveTab=0;

int CreateBookList(void * r0, BOOK * bk);
int isBookManager(BOOK * struc);
int onUserInactivity(void * r0, BOOK * bk);
int onRootListChanged(void * r0, BOOK * bk);
void onMyBookClose(BOOK *);
void CloseMyBook(BOOK * Book, void *);
int CreateMenu(int r0, BOOK * bk);
void TerminateManager(BOOK * Book, void *);
int NewKey(int key, int r1 , int mode);
void SessoinListsFree(BOOK * book);
int get_file(wchar_t * fname,char ** buf_set);


const PAGE_MSG bk_main_msglst[] @ "DYN_PAGE"  =
{
  PAGE_ENTER_EVENT_TAG,CreateBookList,
  NIL_EVENT_TAG,0
};

const PAGE_DESC BookManager_Main_Page = {"BookManager_Main_Page",0,bk_main_msglst};


const PAGE_MSG bk_base_msglst[] @ "DYN_PAGE"  =
{
  BOOK_DESTROYED_EVENT_TAG,onRootListChanged,
  USER_INACTIVITY_EVENT_TAG,onUserInactivity,
  NIL_EVENT_TAG,0
};

const PAGE_DESC BookManager_Base_Page = {"BookManager_Base_Page",0,bk_base_msglst};



void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

//=====================================================================================


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

char * unicode2win1251(char *s, wchar_t *ws, int len)
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
      if (!isBookmanDaemonBook(book))
      {
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
    if (((MyBOOK*)bk)->gui) GUI_Free((GUI*)((MyBOOK*)bk)->gui);
    
    // создали меню
    CreateMenu(ActiveTab,bk);
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
    blistpos=ListMenu_GetSelectedItem(blist);
    si= (SESSION_ITEM*)ListElement_GetByIndex(BookManager_Book->session_list,blistpos);
    break;
  case ELFLIST:
    if (BookManager_Book->elfs_list->FirstFree)
    {
      elistpos=ListMenu_GetSelectedItem(elist);
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
        UI_Event_toBookID(RETURN_TO_STANDBY_EVENT,BOOK_GetBookID(bk));
        UI_Event_toBookID(TERMINATE_SESSION_EVENT,BOOK_GetBookID(bk));
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
          UI_Event_toBookID(RETURN_TO_STANDBY_EVENT,BOOK_GetBookID(bk));
        }
        else
        {
          if (CheckEv(bk,ELF_TERMINATE_EVENT))
          {
            UI_Event_toBookID(ELF_TERMINATE_EVENT,BOOK_GetBookID(bk)/*bk->BookID*/);
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
          UI_Event_toBookIDwData(ELF_SHOW_INFO_EVENT,BOOK_GetBookID(bk),msg,(void(*)(void*))mfree_adr());
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
  blistpos=ListMenu_GetSelectedItem(blist);
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
          int id_len=strlen(strstr(param,"ID=")+3);
          wchar_t buf[20];
          str2wstr(buf,strstr(param,"ID=")+3);
          int ID;
          char sp_flag;
          wtoi(buf,id_len,&ID);
          REQUEST_UI_OAF_START_APPLICATION(ASYNC,ID+0x10000,&sp_flag);
        }
        else
        {
          if (StandbyOnTop)
          {
            StartAPP(par);
            BookObj_Show(Find_StandbyBook(),0);
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
      if (!fstat(path,name,0))
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
GUI_TABMENUBAR * CreateGuiList(int tab_pos, BOOK * bk)
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
  
  GUIObject_Softkey_SetAction(lo,ACTION_BACK, CloseMyBook);
  GUIObject_Softkey_SetAction(lo,ACTION_LONG_BACK, TerminateManager);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,onEnterPressed);

  GUIObject_Softkey_SetAction(lo,0,Shortcuts);
  textidname2id(L"SHC_EDIT_SHORTCUT_TXT",SID_ANY_LEN,&str_id);
  GUIObject_Softkey_SetText(lo,0,str_id);
  GUIObject_Softkey_SetAction(lo,1,Copyright);
  GUIObject_Softkey_SetText(lo,1,STR("About"));

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
  GUIObject_Softkey_SetAction(elist,ACTION_BACK, CloseMyBook);
  GUIObject_Softkey_SetAction(elist,ACTION_LONG_BACK, TerminateManager);
  GUIObject_Softkey_SetAction(elist,0,Shortcuts);
  textidname2id(L"SHC_EDIT_SHORTCUT_TXT",SID_ANY_LEN,&str_id);
  GUIObject_Softkey_SetText(elist,0,str_id);
  
  if (((MyBOOK*)bk)->elfs_list->FirstFree)
  {
    GUIObject_Softkey_SetAction(elist,ACTION_SELECT1,onEnterPressed1);
    GUIObject_Softkey_SetAction(elist,1,Author);
    GUIObject_Softkey_SetText(elist,1,STR("Author"));
  }
  else
  {
    ListMenu_SetNoItemText(elist,STR("No elfs in memory"));
  }
 
  ((MyBOOK*)bk)->oldOnKey1=(void*)DISP_OBJ_GetOnKey(elist->DISP_OBJ);
  
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC (elist->DISP_OBJ), (DISP_OBJ_ONKEY_METHOD)myOnKey1 );
  
  
  GUI_TABMENUBAR * tab = CreateTabMenuBar(bk);
  TabMenuBar_SetTabCount(tab,2);
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
int CreateMenu(int tab_pos, BOOK * bk)
{
  ((MyBOOK*)bk)->gui=CreateGuiList(tab_pos,bk);
  ShowWindow(((MyBOOK*)bk)->gui);
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
    if (java_list_menu)
  {
    GUI_Free((GUI*)java_list_menu);
    java_list_menu=0;
  }
  if (java_list)
  {
    List_FreeElements(java_list,elem_filter,elem_free);
    List_Free(java_list);
    java_list=0;
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
/*  if (str_inp)
  {
    GUI_Free(str_inp);
    str_inp=0;
  }*/
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
  // освободили списки книг/сессий
  SessoinListsFree(book);
  
  // выгрузили иконки
  int i;
  for (i=0;i<ICONS_COUNT;i++)
  {
    ImageID_Free(dyn_image[i].ImageID);
  }
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
  FSTAT _fstat;
  wchar_t * path = get_path();
  if (fstat(path,name,&_fstat)==0)
  {
    if ((file=_fopen(path,name,0x1,0x180,0))>=0)
    {
      buf=(char*)malloc(_fstat.fsize+1);
      fread(file,buf,_fstat.fsize);
      fclose(file);
      size=_fstat.fsize;
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
  int _imgID;
  wchar_t wstr_name[40];
  wchar_t * path=get_path();
  for (i=0;i<ICONS_COUNT;i++)
  {
    win12512unicode(wstr_name,(char*)img_names[i],strlen(img_names[i]));
    if (ImageID_Get(path,wstr_name,&imgID)<0)
    {
      iconidname2id(id_names[i],SID_ANY_LEN,&_imgID);
      dyn_image[i].ImageID=_imgID;
    }
    else dyn_image[i].ImageID=imgID;
  }
  delete(path);
}


// собственно старт
__root int CreateBookList(void * r0, BOOK * bk)
{
  if (BookManager_Book)
  {
    MyBOOK * myBook = (MyBOOK*)bk;
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
//    str_inp=0;
    java_list=0;
    java_list_menu=0;
    int tab_pos;
    if (!FirstTab) tab_pos=ActiveTab;
    else tab_pos=FirstTab-1;
    CreateMenu(tab_pos,(BOOK*)myBook);
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
          BookManager_Book = (MyBOOK *)malloc(sizeof(MyBOOK));
          memset(BookManager_Book,0,sizeof(MyBOOK));
          CreateBook(BookManager_Book,onMyBookClose,&BookManager_Base_Page,"BookManager",-1,0);
          BookObj_GotoPage((BOOK*)BookManager_Book,&BookManager_Main_Page);
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
  else
  {
    MessageBox(0x6fFFFFFF,STR("BookManager is already runed"),0, 1 ,5000,0);
    SUBPROC(elf_exit);
  }
  return(0);
}
