#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

#define COPYRIGHT_STRING STR("\nBookManager v2.5\nbuild 040308\nCopyright (c) 2007-2008\nHussein\n\nRespect\nIronMaster,KreN\n\n")
#define MESSAGE(__STR__) MessageBox(0x6fffffff,__STR__,0, 1 ,11000,(BOOK*)BookManager_Book);
#define BOOKLIST 0
#define ELFLIST 1
#define ICONS_COUNT 4


int onUserInactivity(void * r0, BOOK * bk);
int onRootListChanged(void * r0, BOOK * bk);
void onMyBookClose(BOOK *);
void CloseMyBook(BOOK * Book, void *);
int CreateMenu(void * r0, BOOK * bk);
void TerminateManager(BOOK * Book, void *);
int NewKey(int key, int r1 , int mode);
void SessoinListsFree(BOOK * book);

typedef struct
{
  u16 ImageID;
  u16 ImageHandle;
}IMG;

GUI_LIST * blist;
GUI_LIST * elist;
int blistpos=0;
int elistpos=0;
int blistcnt=0;
int elistcnt=0;
int ActiveTab=0;
IMG dyn_image[ICONS_COUNT];

// дэфолтные значения
int KeyPressMode=3;
int KeyActiv=0x1F;
int Ignore_KeyLock=0;
int ElfInBookListEnabled=1;
int UserInactivityEventEnabled=1;
int NameBookAsSession=0;
int StandbyOnTop=0;

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

const PAGE_MSG bk_msglst[] @ "DYN_PAGE"  =
{
  BOOK_DESTROYED_EVENT_TAG,onRootListChanged,
  USER_INACTIVITY_EVENT_TAG,onUserInactivity,
  NIL_EVENT_TAG,          0};

const PAGE_DESC bk = {"BookManager",0,bk_msglst};



void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

//=====================================================================================
u16 RegisterImage(IMG * i , u16 * path, u16 * fname)
{
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
      }
    }
  }
  return(i->ImageID);
};


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

void win12512unicode(u16 *ws, char *s, int len)
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

char *unicode2win1251(char *s, u16 *ws, int len)
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
  u16 ws[50];
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
  u16 ws[100];
  TextID2wstr(Book_GetSession(bk)->name,ws,100);
  if (!wstrncmp(ws,(u16*)L"Foreign app",11))
  {
    return(JavaSession_GetName());
  }
  if (!wstrcmp(ws,(u16*)L"Java"))
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
              u16 ws[100];
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
  ActiveTab=TabMenuBar_GetFocusedTabIndex(bk->gui);
  // если ГУИ есть, то убили
  if (bk->gui) GUI_Free((GUI*)bk->gui);
  
  // создали меню
  CreateMenu(0,bk);
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
    if (myBook->filebuf)
    {
      char key[20];
      char * param;
      sprintf(key,i4==3?"[S_KEY%d]":"[L_KEY%d]",i1-KEY_DIGITAL_0);
      if (param=manifest_GetParam(myBook->filebuf,key,0))
      {
	u16 par[256];
	str2wstr(par,param);
        if (StandbyOnTop)
        {
	  StartAPP(par);
        }
        else
        {
          Shortcut_Run(par);
        }
	mfree(param);
	if (Find_StandbyBook()!=0)Show(Find_StandbyBook(),0);
        CloseMyBook((BOOK*)myBook,0);
      }
    }
  }
};

int StartElf(u16 * path , char * name)
{
  int res=-1;
  u16 * elfname = new u16[wstrlen(path)+strlen(name)+1];
  u16 * wname = new u16[strlen(name)+1];
  str2wstr(wname,name);
  wstrcpy(elfname,path);
  wstrcat(elfname,wname);
  
  {
    u16 *path;
    u16 *name;
    int len;
    name=wstrrchr(elfname,'/');
    if (name)
    {
      path=new u16 [((len=name-elfname)+1)];
      memcpy(path,elfname,len*sizeof(u16));
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
    if (myBook->filebuf)
    {
      char key[20];
      char * param;
      sprintf(key,i4==3?"[ES_KEY%d]":"[EL_KEY%d]",i1-KEY_DIGITAL_0);
      if (param=manifest_GetParam(myBook->filebuf,key,0))
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
    }
  }
};

// создание меню
GUI_TABMENUBAR * CreateGuiList(void * r0, BOOK * bk)
{
  int p[2];
  GUI_LIST * lo;
  blist =lo =CreateListObject(bk,0);
  elist =CreateListObject(bk,0);
  bk->gui=(GUI*)lo;
  CreateBookLst((MyBOOK*)bk);
  p[0]=Str2ID (L"Heap : ",0,7);
  p[1]=int2strID (GetFreeBytesOnHeap());
  GuiObject_SetTitleText(lo,Str2ID(p,5,2));
  
  
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)lo,(void*)onLBMessage);
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
  
  AddMSGHook(lo,1,Copyright);
  AddCommand(lo,1,STR("About"));

  ((MyBOOK*)bk)->oldOnKey=(void*)DISP_OBJ_GetOnKey(lo->DISP_OBJ);
  
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC (lo->DISP_OBJ), (DISP_OBJ_ONKEY_METHOD)myOnKey );
  //---------------
  
  
  
  GuiObject_SetTitleText(elist,(STR("Elfs")));
  SetCursorToItem(elist,0);
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)elist,(void*)onLBMessage1);
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
  
  if (((MyBOOK*)bk)->elfs_list->FirstFree)
  {
    AddMSGHook(elist,1,Author);
    AddCommand(elist,1,STR("Author"));
    AddMSGHook(elist,ACTION_SELECT1,onEnterPressed1);
  }
  else
  {
    ListMenu_SetNoItemText(elist,STR("No elfs in memory"));
  }
  
  ((MyBOOK*)bk)->oldOnKey1=(void*)DISP_OBJ_GetOnKey(elist->DISP_OBJ);
  
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC (elist->DISP_OBJ), (DISP_OBJ_ONKEY_METHOD)myOnKey1 );
  
  
  GUI_TABMENUBAR * tab = CreateTabMenuBar(bk);
  TabMenuBar_SetTabCount(tab,2);
  TabMenuBar_SetTabFocused(tab,ActiveTab);
  TabMenuBar_AssignGuiObj(tab,0,lo);
  TabMenuBar_AssignGuiObj(tab,1,elist);

  TabMenuBar_SetTabIcon(tab,0,dyn_image[0].ImageID,0);
  TabMenuBar_SetTabIcon(tab,0,dyn_image[1].ImageID,1);
  
  TabMenuBar_SetTabIcon(tab,1,dyn_image[2].ImageID,0);
  TabMenuBar_SetTabIcon(tab,1,dyn_image[3].ImageID,1);
  
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
  char error_code;
  MyBOOK * myBook=(MyBOOK*)book;
  // выгрузили файло
  if (myBook->filebuf)
  {
    mfree(myBook->filebuf);
    myBook->filebuf=0;
  }
  // освободили списки книг/сессий
  SessoinListsFree(book);
  
  // выгрузили иконки
  int i;
  for (i=0;i<ICONS_COUNT;i++)
    REQUEST_IMAGEHANDLER_INTERNAL_UNREGISTER(SYNC,dyn_image[i].ImageHandle,0,0,dyn_image[i].ImageID,1,&error_code);
}


void CloseMyBook(BOOK * Book, void *)
{
  ActiveTab=TabMenuBar_GetFocusedTabIndex(Book->gui);
  FreeBook(Book);
  BookManager_Book=0;
};

void TerminateManager(BOOK * Book, void *)
{
  MESSAGE(STR("BookManager\n\nterminated"));
  FreeBook(Book);
  ModifyKeyHook(NewKey,0);
  SUBPROC(elf_exit);
};

// берем значения из ини-файла
char * get_ini_key(int full_init)
{
  int file;
  char * buf=0;
  FSTAT fstat;
  u16 * path = new u16[wstrlen(GetDir(DIR_INI))+wstrlen((u16*)L"/bookman")+1];
  wstrcpy(path,GetDir(DIR_INI));
  wstrcat(path,(u16*)L"/bookman");
  if (isFileExist(path,(u16*)L"bookman.ini",&fstat)==0)
  {
    if ((file=_fopen(path,(u16*)L"bookman.ini",0x001,0x180,0))>=0)
    {
      buf=(char*)malloc(fstat.fsize+1);
      fread(file,buf,fstat.fsize);
      char * param;
      if (param=manifest_GetParam(buf,"[START_KEY]",0))
      {
        KeyActiv=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[KEY_MODE]",0))
      {
        KeyPressMode=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[IGNORE_KEYLOCK]",0))
      {
        Ignore_KeyLock=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[UIEE]",0))
      {
        UserInactivityEventEnabled=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[NBAS]",0))
      {
        NameBookAsSession=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[SOT]",0))
      {
        StandbyOnTop=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[EIBL]",0))
      {
        ElfInBookListEnabled=h2i(param);
        mfree(param);
      }
      if (full_init)
      {
        // если full_init то регистрируем иконки и не освобождаем загруженный ини
        int i;
        for (i=0;i<ICONS_COUNT;i++)
        {
          char key[50];
          u16 ws[100];
          sprintf(key,"[ICON%.02d]",i);
          
          if (param=manifest_GetParam(buf,key,0))
          {
            str2wstr(ws,param);
            // грузим иконки
            RegisterImage(&dyn_image[i] ,path, ws);
            mfree(param);
          }
        }
      }
      else
      {
        mfree(buf);
        buf=0;
      }
    }
    fclose(file);
  }
  delete(path);
  return(buf);
}

// собственно старт
__root int CreateBookList(void)
{
  if (!BookManager_Book)
  {
    MyBOOK * myBook =BookManager_Book= (MyBOOK *)malloc(sizeof(MyBOOK));
    memset(myBook,0,sizeof(MyBOOK));
    CreateBook(myBook,onMyBookClose,&bk,"BookManager",-1,0);
    //
    myBook->session_list=List_New();
    myBook->elfs_list=List_New();
    myBook->filebuf=get_ini_key(1);
    elistpos=0;
    blistpos=0;
    CreateMenu(0,(BOOK*)myBook);
  }
  return(0);
}

int NewKey(int key, int r1 , int mode)
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
  return(0);
}




int main (void)
{
  BookManager_Book=0;
  get_ini_key(0);
  ModifyKeyHook(NewKey,1);
  return(0);
}
