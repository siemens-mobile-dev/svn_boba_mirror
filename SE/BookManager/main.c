#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

int onRootListChanged(void * r0, BOOK * bk);
void onMyBookClose(BOOK *);
void CloseMyBook(BOOK * Book, void *);
int CreateMenu(void * r0, BOOK * bk);


typedef struct
{
  BOOK  book;
  char * filebuf;
  void * oldOnKey;
  LIST * session_list;
}MyBOOK;

typedef struct
{
  BOOK * book;
  UI_APP_SESSION * session;
  int book_name;
}BOOK_ITEM;

typedef struct
{
  UI_APP_SESSION * session;
  LIST * book_list;
  int session_name;
  int book_in_session;
}SESSION_ITEM;

// дэфолтные значения
int KeyPressMode=3;
int KeyActiv=0x00;
int KeyTerminate=0xff;
int Ignore_KeyLock=0;
int ItemMode=0;




MyBOOK * bman_book=0;

int isBookManager(BOOK * struc);

const PAGE_MSG bk_msglst[] @ "DYN_PAGE"  = 
{
  BOOK_DESTROYED_EVENT_TAG,onRootListChanged,
  NIL_EVENT_TAG,          0};

const PAGE_DESC bk = {"BookManagerBook",0,bk_msglst};



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
  return(0);
}

// создаём список сессий/книг
void CreateBookLst(MyBOOK * myBook)
{
  int i,j,k,cnt;
  BOOK * book;
  UI_APP_SESSION * session;
  SESSION_ITEM *si;
  i=root_list_get_session_count();
  cnt=0;
  for (j=0;j<i;j++)
  {
    si=0;
    session=root_list_get_session(j);
    for (k=0;k<session->listbook->FirstFree;k++)
    {
      book=(BOOK*)ListElement_GetByIndex(session->listbook,k);
      if( book->xguilist->guilist->FirstFree)
      {
        if ((!isBookManager(book))&&(!isVolumeControllerBook(book))&&(!isRightNowBook(book)))
        {
          if (!si) 
          {
            char s[100];
            u16 ws[100];
            si=new SESSION_ITEM;
            si->book_list=List_New();
            TextID2wstr(session->name,ws,100);
            si->session_name=GetParam(unicode2win1251(s,ws,100));
            si->session=session;
          }
          BOOK_ITEM * b = new BOOK_ITEM;
          b->book=book;
          b->session=session;
          b->book_name=0x6fffffff;
          if (!(b->book_name=GetJavaName(book)))
          {
            b->book_name=GetParam(book->xbook->name);
          }
          ListElement_Add(si->book_list,b);
        }
      }
    }
    if (si)
    {
      
      si->book_in_session=si->book_list->FirstFree;
      ListElement_AddtoTop(myBook->session_list,si);
      cnt++;    
    }
  }
  SetNumOfMenuItem(myBook->book.gui,cnt);
}


// при убийстве какой либо книги
int onRootListChanged(void * r0, BOOK * bk)
{
  // если ГУИ есть, то убили
  if (bk->gui) GUI_Free((GUI*)bk->gui);
  // создали меню
  CreateMenu(0,bk);
  return(0);
};


int SubItem=0;

// устанавливаем тексты в пунктах меню
int onLBMessage(GUI_MESSAGE * msg)
{
  //  static const u16 *sss="jwefg " __DATE__ "sdjkfwe";
  MyBOOK * myBook = (MyBOOK *) FindBook(isBookManager);
  int item;
  SESSION_ITEM * si;
  switch(msg->msg)
  {
    // onCreateListItem
  case 1:
    item=GUIonMessage_GetCreatedItemIndex(msg);
    si=(SESSION_ITEM *)ListElement_GetByIndex(myBook->session_list,item);
    SetMenuItemText0(msg,TextCopyId(si->session_name));
    SetMenuItemText2(msg,Str2ID (L"\nBookManager v2.1\nbuild 140108\nCopyright (c) 2007-2008\nHussein",0,100));
    if (si->book_list->FirstFree>1)
    {
      List2lineSetAsSubitem(msg,0,si->book_in_session);
    }
    else
    {
      SetMenuItemText1(msg,TextCopyId((int)(((BOOK_ITEM*)(ListElement_GetByIndex(si->book_list,0)))->book_name)));
    }
    SubItem=0;
    break;
    // onCreateListSubItem
  case 3:
    item=GUIonMessage_GetCreatedSubItemParrentIndex(msg);
    si=(SESSION_ITEM *)ListElement_GetByIndex(myBook->session_list,item); 
    BOOK_ITEM * bi =(BOOK_ITEM *) ListElement_GetByIndex(si->book_list,SubItem++);
    List2lineSubitemAddText(msg,TextCopyId(bi->book_name));
    break;
  }
  return(1);
};

// при выборе пункта
void onEnterPressed(BOOK * book,void * lt)
{
  MyBOOK * myBook = (MyBOOK *) book;
  SESSION_ITEM * si= (SESSION_ITEM*)ListElement_GetByIndex(myBook->session_list,GetFocusetListObjectItem(book->gui));
  // ставим фокус на верхнюю книгу в сессии
  BookObj_SetFocus(((BOOK_ITEM*)(ListElement_GetByIndex(si->book_list,0)))->book,0);
  CloseMyBook(book,0);
};


// при нажатии "С"
void onDelPressed(BOOK * book,void * lt)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isBookManager);
  SESSION_ITEM * si= (SESSION_ITEM*)ListElement_GetByIndex(myBook->session_list,GetFocusetListObjectItem(book->gui));
  BOOK *bk =(BOOK*)(((BOOK_ITEM*)(ListElement_GetByIndex(si->book_list,0)))->book);
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
        UI_Event_toSID(RETURN_TO_STANDBY_EVENT,bk->BookID);
        UI_Event_toSID(TERMINATE_SESSION_EVENT,bk->BookID);
      }
    }
  }
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
	StartAPP(par);
	mfree(param);
	if (Find_StandbyBook()!=0)Show(Find_StandbyBook());
        CloseMyBook((BOOK*)myBook,0);
      }
    }
  }
};

// создание меню
GUI_LIST * CreateGuiList(void * r0, BOOK * bk)
{
  int p[2];
  GUI_LIST * lo;
  lo =CreateListObject(bk,0);
  bk->gui=(GUI*)lo;
  p[0]=Str2ID (L"Heap : ",0,7);
  p[1]=int2strID (GetFreeBytesOnHeap());
  GuiObject_SetTitleText(lo,Str2ID(p,5,2));
  CreateBookLst((MyBOOK*)bk);
  
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)lo,(void*)onLBMessage);
  SetCursorToItem(lo,0);
  SetMenuItemStyle(lo,ItemMode);
  
  AddMSGHook(lo,ACTION_BACK, CloseMyBook);
  AddMSGHook(lo,ACTION_SELECT1,onEnterPressed);
  
  
  ((MyBOOK*)bk)->oldOnKey=(void*)DISP_OBJ_GetOnKey(lo->DISP_OBJ);
  
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC (lo->DISP_OBJ), (DISP_OBJ_ONKEY_METHOD)myOnKey );
  return(lo);
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

// при закрытии книги
void onMyBookClose(BOOK * book)
{
  MyBOOK * myBook=(MyBOOK*)book;
  if (myBook->filebuf)
  {
    mfree(myBook->filebuf);
    myBook->filebuf=0;
  }
  LIST * lst = myBook->session_list;
  SESSION_ITEM * si;
  
  while (lst->FirstFree)
  {
    int i=0;
    si=(SESSION_ITEM *)ListElement_Remove(lst,0);
    
    while (i<si->book_list->FirstFree)
    {
      BOOK_ITEM * bi=(BOOK_ITEM *)ListElement_GetByIndex(si->book_list,i++);
      TextFree(bi->book_name);
      delete(bi);
    }
    TextFree(si->session_name);
    List_Free(si->book_list);
    delete(si);
  }
  List_Free(myBook->session_list);
}


void CloseMyBook(BOOK * Book, void *)
{
  FreeBook(Book);
  bman_book=0;
};

// собственно старт
__root int CreateBookList(void)
{
  int file;
  
  //  u16 f[100];
  if (!bman_book)
  {
    MyBOOK * myBook =bman_book= (MyBOOK *)malloc(sizeof(MyBOOK));
    memset(myBook,0,sizeof(MyBOOK));
    CreateBook(myBook,onMyBookClose,&bk,"BookManager",-1,0);
    //
    myBook->session_list=List_New();
    myBook->filebuf=0;
    // открываем для чтения bookman.ini
    if ((file=_fopen(GetDir(DIR_INI),(u16*)L"bookman.ini",0x001,0x180,0))>=0)
    {
      FSTAT fstat;
      // определяем его размер
      if (isFileExist(GetDir(DIR_INI),(u16*)L"bookman.ini",&fstat)==0)
      {
        // выделяем память
	myBook->filebuf=(char*)malloc(fstat.fsize+1);
        // чтитаем его
	fread(file,(char*)myBook->filebuf,fstat.fsize);
      }
      // закрываем
      fclose(file);
    }
    CreateMenu(0,(BOOK*)myBook);
  }
  return(0);
}

int NewKey(int key, int r1 , int mode)
{
  if (mode==KeyPressMode)
  {
    if (key==KeyTerminate)
    {
      if ((isKeylocked())&&(!Ignore_KeyLock)) return(0);
      ModifyKeyHook(NewKey,0);
      SUBPROC(elf_exit);
      return(0);
    }
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

// берем значения из ини-файла
void get_ini_key(void)
{
  int file;
  char * buf;
  if ((file=_fopen(GetDir(DIR_INI),(u16*)L"bookman.ini",0x001,0x180,0))>=0)
  {
    FSTAT fstat;
    if (isFileExist(GetDir(DIR_INI),(u16*)L"bookman.ini",&fstat)==0)
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
      if (param=manifest_GetParam(buf,"[TERMINATE_KEY]",0))
      {
        KeyTerminate=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[IGNORE_KEYLOCK]",0))
      {
        Ignore_KeyLock=h2i(param);
        mfree(param);
      }
      if (param=manifest_GetParam(buf,"[EXT_MODE]",0))
      {
        if(h2i(param))  ItemMode=3;
        mfree(param);
      }
      mfree(buf);
    }
    fclose(file);
  }
}


int main (void)
{
  bman_book=0;
  get_ini_key();
  ModifyKeyHook(NewKey,1);
  return(0);
}
