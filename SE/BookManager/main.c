#include "..\include\Lib_Clara.h"
#include "..\include\Dir.h"
#include "..\include\var_arg.h"
#include "..\include\cfg_items.h"
#include "conf_loader.h"
#include "config_data.h"
#include "shortcuts.h"
#include "book_names.h"
#include "main.h"


wchar_t* id_digits[DIGITS_COUNT] =
{
  IDN_DIGIT_0_ICON,
  IDN_DIGIT_1_ICON,
  IDN_DIGIT_2_ICON,
  IDN_DIGIT_3_ICON,
  IDN_DIGIT_4_ICON,
  IDN_DIGIT_5_ICON,
  IDN_DIGIT_6_ICON,
  IDN_DIGIT_7_ICON,
  IDN_DIGIT_8_ICON,
  IDN_DIGIT_9_ICON,
};


wchar_t* id_names[ICONS_COUNT] =
{
  IDN_BOOKS_DEACT_ICON,
  IDN_BOOKS_ACT_ICON,
  IDN_ELFS_DEACT_ICON,
  IDN_ELFS_ACT_ICON
};


const wchar_t* img_digits[DIGITS_COUNT] =
{
  digit0,
  digit1,
  digit2,
  digit3,
  digit4,
  digit5,
  digit6,
  digit7,
  digit8,
  digit9,
};


const wchar_t* img_names[ICONS_COUNT] =
{
  tab1_inact,
  tab1_act,
  tab2_inact,
  tab2_act
};


//============= pages start ======================

const PAGE_MSG bk_main_msglst[] @ "DYN_PAGE" =
{
  PAGE_ENTER_EVENT_TAG, CreateBookList,
  BOOK_DESTROYED_EVENT_TAG, onRootListChanged,
  USER_INACTIVITY_EVENT_TAG, onUserInactivity,
  ACCEPT_EVENT_TAG, RecreateBookList,
  NIL_EVENT_TAG, 0
};

const PAGE_DESC BookManager_Main_Page = { "BookManager_Main_Page", 0, bk_main_msglst };


const PAGE_MSG bk_idle_msglst[] @ "DYN_PAGE" =
{
  PAGE_ENTER_EVENT_TAG, IdlePage_EnterAction,
  NIL_EVENT_TAG, 0
};

const PAGE_DESC BookManager_Idle_Page = { "BookManager_Idle_Page", 0, bk_idle_msglst };


const PAGE_MSG bk_base_msglst[] @ "DYN_PAGE" =
{
  ELF_RECONFIG_EVENT, ReconfigElf,
  NIL_EVENT_TAG, 0
};

const PAGE_DESC BookManager_Base_Page = { "BookManager_Base_Page", 0, bk_base_msglst };

//============= pages end ======================


void elf_exit( void )
{
  trace_done();
  kill_data( &ELF_BEGIN, ( void(*)(void*) )mfree_adr() );
}

// ======================== всякая служебная ерунда start ===========================================

int GetActiveTab( MyBOOK* mbk )
{
  return TabMenuBar_GetFocusedTab( mbk->gui );
}


int isBookManager( BOOK* book )
{
  return 0 == strcmp( book->xbook->name, "BookManager" );
}


int isRSSTickerBook( BOOK* book )
{
  return 0 == strcmp( book->xbook->name, "RSSTicker_Book" );
}


// проверка, обрабатывает ли БЕЙЗ_ПЕЙДЖ книги событие
int CheckEv( BOOK* bk, int ev )
{
  if ( bk->base_page )
  {
    const PAGE_MSG* pm = bk->base_page->msg_lst;
    
    if ( pm )
    {
      while ( pm->msg && pm->proc )
      {
        if ( pm->msg == ev )
        {
          return 1;
        }
        pm++;
      }
    }
  }
  return 0;
}


//Получить пользовательское имя книги
wchar_t * GetUserBookName(wchar_t * ini,wchar_t * orig_name,wchar_t * cur_name)
{
  wchar_t * pos;
  
  if (ini)
  {
    wchar_t uni_pair[MAX_BOOK_NAME_LEN + sizeof(": ")];
    uni_pair[0]=0;
    wstrcpy(uni_pair,orig_name);
    wstrcat(uni_pair,L": ");
    
    pos = wstrwstr(ini,uni_pair);
    
    if (pos)
    {
      swscanf(pos,L"%*[^:]: %l[^\r]",cur_name);
      return pos;
    }
  }
  
  wstrcpy(cur_name,orig_name);
  return 0;
}


// взять имя книги из ини-файла
TEXTID GetBookNameStrID( char* name )
{
  wchar_t cur_name[MAX_BOOK_NAME_LEN+1];
  wchar_t orig_name[MAX_BOOK_NAME_LEN+1];
  
  MyBOOK* mbk = (MyBOOK*) FindBook( isBookManager );
  
  str2wstr(orig_name,name);
  
  GetUserBookName(mbk->ini_buf, orig_name, cur_name);
  
  return TextID_Create(cur_name, ENC_UCS2, TEXTID_ANY_LEN);
}


// получить имя жавы
int GetJavaName( BOOK* bk )
{
  wchar_t ws[100];
  
  TextID_GetWString( BookObj_GetSession( bk )->name, ws, MAXELEMS(ws) );
  
  if ( !wstrncmp( ws, L"Foreign app", 11 ) )
  {
    return JavaSession_GetName();
  }
  
  if ( !wstrcmp( ws, L"Java" ) )
  {
    return JavaSession_GetName();
  }
  
  return EMPTY_TEXTID;
}


//Получить ID жавы для ярлыка
int JavaShortcut_GetID(wchar_t * java_shortcut)
{
  int ret_val = -1;
  wchar_t * hash_name = wstrwstr(java_shortcut,L"//") + 2;
  
  //Find app
  char sp1;
  wchar_t* sp;
  void* JavaDesc;
  JavaDialog_Open( 0, &sp1, &JavaDesc );
  if ( !JavaAppDesc_GetFirstApp( JavaDesc ) )
  {
    int result = 0;
    while ( !result )
    {
      JavaAppDesc_GetJavaAppInfo( JavaDesc, 4, &sp );
      int cmp_res = wstrcmp(hash_name,sp);
      delete(sp);
      if (cmp_res) result = JavaAppDesc_GetNextApp( JavaDesc );
      else
      {
        ret_val = JavaAppDesc_GetJavaAppID( JavaDesc );
        break;
      }
    }
  }
  JavaDialog_Close( sp1 );
  return(ret_val);
}


// взять выбранную книгу из листа книг/эльфов
BOOK* GetBook( int list, BOOK* bk )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  BOOK_LIST_ITEM* elem;
  
  switch ( list )
  {
  case BOOKLIST:
    mbk->blistpos = ListMenu_GetSelectedItem( mbk->blist );
    elem = (BOOK_LIST_ITEM*) List_Get( mbk->books_list, mbk->blistpos );
    break;
    
  case ELFLIST:
    if ( mbk->elfs_list->FirstFree )
    {
      mbk->elistpos = ListMenu_GetSelectedItem( mbk->elist );
      elem = (BOOK_LIST_ITEM*) List_Get( mbk->elfs_list, mbk->elistpos );
    }
    else
    {
      return 0;
    }
    break;
  }
  return (elem->book);
}


// создаём список книг
void CreateBookAndElfsLists( MyBOOK* myBook )
{
  int i, j, k, fgui;
  int books_cnt=0;
  int elfs_cnt=0;
  int mask = ( (int)LastExtDB() )&FLASH_MASK;
  BOOK* book;
  UI_APP_SESSION* session;
  BOOK_LIST_ITEM* elem;
  
  myBook->books_list = List_Create();
  myBook->elfs_list = List_Create();
  
  i = root_list_get_session_count();
  
  for ( j = 0; j < i; j++ )
  {
    elem=0;
    session = root_list_get_session( j );
    
    for ( k = 0; k < session->listbook->FirstFree; k++ )
    {
      book = (BOOK*) List_Get( session->listbook, k );
      
      fgui = book->xguilist->guilist->FirstFree;
      
      if( (fgui) || (ShowNoGUIBooks) || ( ((int)book->onClose)&FLASH_MASK ) != mask )
      {
        if ( !isBookManager( book ) && !IsVolumeControllerBook( book ) && !IsRightNowBook( book ) && !isRSSTickerBook( book ))
        {
          char s[MAX_BOOK_NAME_LEN+1];
          elem = new BOOK_LIST_ITEM;
          elem->book=book;
          
          if ( strcmp( book->xbook->name, JAVA_BOOK_NAME ) )
          {
            char * bn =new char[strlen(book->xbook->name)+1];
            strcpy(bn,book->xbook->name);
            elem->book_name = bn;
          }
          else
          {
            TextID_GetString( BookObj_GetSession(book)->name, s, MAXELEMS(s) );
            char * bn =new char[strlen(s)+1];
            strcpy(bn,s);
            elem->book_name = bn;
          }
          
          elem->isGuiBook = fgui;
          
          TEXTID tmp = GetJavaName( book );
          
          if ( tmp != EMPTY_TEXTID )
          {
            delete(elem->book_name);
            TextID_GetString( tmp, s, MAXELEMS(s) );
            char * java_name =new char[strlen(s)+1];
            strcpy(java_name,s);
            elem->book_name = java_name;
            TextID_Destroy( tmp );
          }
          /*
          if ( ElfInBookListEnabled && ( ((int)book->onClose)&FLASH_MASK ) != mask )
          {
            if ( fgui )
            {
              List_InsertFirst( myBook->books_list, elem );
              books_cnt++;
            }
          }
          */
          if ( ( ((int)book->onClose)&FLASH_MASK ) == mask )
          {
            List_InsertFirst( myBook->books_list, elem );
            books_cnt++;
          }
          else
          {
            List_InsertFirst( myBook->elfs_list, elem );
            elfs_cnt++;
          }
        }
      }
    }
  }
  myBook->blistcnt = books_cnt;
  myBook->elistcnt = elfs_cnt;
}


void DestroyBookAndElfsLists( MyBOOK* myBook )
{
  // MyBOOK* myBook = (MyBOOK*) book;
  LIST* lst = myBook->books_list;
  BOOK_LIST_ITEM* elem;
  
  if ( lst )
  {
    while ( lst->FirstFree )
    {
      elem = (BOOK_LIST_ITEM*) List_RemoveAt( lst, 0 );
      if ( List_IndexOf( myBook->elfs_list, elem ) == LIST_ERROR )
      {
        // удаляем, если не в списке эльфов
        delete(elem->book_name);
        delete(elem);
      }
    }
    
    List_Destroy( myBook->books_list );
    myBook->books_list=0;
  }
  
  lst = myBook->elfs_list;
  if ( lst )
  {
    while ( lst->FirstFree )
    {
      elem = (BOOK_LIST_ITEM*) List_RemoveAt( lst, 0 );
      delete(elem->book_name);
      delete(elem);
    }
    List_Destroy( myBook->elfs_list );
    myBook->elfs_list=0;
  }
}


wchar_t* get_path()
{
  wchar_t* path = new wchar_t[wstrlen( GetDir( DIR_INI ) ) + sizeof("/bookman")];
  wstrcpy( path, GetDir( DIR_INI ) );
  wstrcat( path, L"/bookman" );
  return path;
}


int get_file( wchar_t* name, void** buf_set )
{
  int size = 0;
  int file;
  void* buf = 0;
  FSTAT _fstat;
  
  wchar_t* path = get_path();
  
  if ( fstat( path, name, &_fstat ) == 0 )
  {
    if ( ( file = _fopen( path, name, FSX_O_RDONLY, FSX_S_IREAD|FSX_S_IWRITE, 0 ) ) >= 0 )
    {
      buf = malloc( _fstat.fsize+1 );
      fread( file, buf, _fstat.fsize );
      fclose( file );
      size = _fstat.fsize;
    }
  }
  
  *buf_set = buf;
  delete( path );
  return size;
}


void get_iconsID( MyBOOK* mbk )
{
  int i;
  IMAGEID imgID;
  int _imgID;
  wchar_t wstr_path[100];
  
  for ( i = 0;i < ICONS_COUNT;i++ )
  {
    iconidname2id(id_names[i],TEXTID_ANY_LEN,&_imgID);
    mbk->tabs_image[i].ImageID = _imgID;
  }
  for ( i = 0;i < DIGITS_COUNT;i++ )
  {
    iconidname2id(id_digits[i],TEXTID_ANY_LEN,&_imgID);
    mbk->digs_image[i].ImageID = _imgID;
  }
  for ( i = 0;i < ICONS_COUNT;i++ )
  {
    wchar_t * wstr_name = wstrrchr(img_names[i],L'/');
    if ( wstr_name++ )
    {
      int path_len = wstr_name-img_names[i]-1;
      wstrncpy(wstr_path,img_names[i],path_len);
      wstr_path[path_len]=0;
      if (ImageID_Get( wstr_path, wstr_name, &imgID ) >= 0 )
      {
        mbk->tabs_image[i].ImageID=imgID;
      }
    }
  }
  for ( i = 0;i < DIGITS_COUNT;i++ )
  {
    wchar_t * wstr_name = wstrrchr(img_digits[i],L'/');
    if ( wstr_name++ )
    {
      int path_len = wstr_name-img_digits[i]-1;
      wstrncpy(wstr_path,img_digits[i],path_len);
      wstr_path[path_len]=0;
      if (ImageID_Get( wstr_path, wstr_name, &imgID ) >= 0 )
      {
        mbk->digs_image[i].ImageID=imgID;
      }
    }
  }
}


//Загрузить ини файлы
void LoadBookNames(MyBOOK * mbk)
{
  if ( mbk->ini_buf )
  {
    delete (mbk->ini_buf);
    mbk->ini_buf=0;
  }
  void* sp;
  mbk->ini_buf_size = get_file( INI_BOOK_NAMES, &sp );
  mbk->ini_buf = (wchar_t*)sp;
}


//Загрузить ини файлы
void LoadShortcuts(MyBOOK * mbk)
{
  if ( mbk->shortcuts_buf )
  {
    delete (mbk->shortcuts_buf);
    mbk->shortcuts_buf=0;
  }
  void* sp;
  mbk->shortcuts_buf_size = get_file( INI_SHORTCUTS, &sp );
  mbk->shortcuts_buf = (char*)sp;
}

// ======================== всякая служебная ерунда end ===========================================


// при убийстве какой либо книги
int onRootListChanged( void* r0, BOOK* bk )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  if ( !mbk->mode_list && !mbk->but_list )
  {
    // создали меню
    mbk->ActiveTAB=GetActiveTab(mbk);
    CreateBookManagerGUI( mbk );
  }
  return 0;
}


// если юзер заснул...
int onUserInactivity( void* r0, BOOK* bk )
{
  if ( UserInactivityEventEnabled )
    CloseMyBook( bk, 0 );
  return 0;
}


// устанавливаем тексты в пунктах меню
int onCallback_Books( GUI_MESSAGE* msg )
{
  MyBOOK* myBook = (MyBOOK*) GUIonMessage_GetBook( msg );
  int item;
  BOOK_LIST_ITEM* elem;
  
  switch( GUIonMessage_GetMsg( msg ) )
  {
  case LISTMSG_GetItem:
    item = GUIonMessage_GetCreatedItemIndex( msg );
    elem = (BOOK_LIST_ITEM*) List_Get( myBook->books_list, item );
    GUIonMessage_SetMenuItemText( msg, GetBookNameStrID(elem->book_name) );
    
    if ( !elem->isGuiBook )
    {
      GUIonMessage_SetItemDisabled( msg, TRUE );
      GUIonMessage_SetMenuItemUnavailableText( msg, STR( "Can't set focus to book without GUI..." ) );
    }
    break;
  case LISTMSG_HighlightChanged:
    item = GUIonMessage_GetSelectedItem( msg );
    RefreshBookSoftkeys( myBook, item );
    break;
  }
  
  return 1;
}


int onCallback_Elfs( GUI_MESSAGE* msg )
{
  MyBOOK* myBook = (MyBOOK*) GUIonMessage_GetBook( msg );
  int item;
  BOOK_LIST_ITEM* elem;
  
  switch( GUIonMessage_GetMsg( msg ) )
  {
  case LISTMSG_GetItem:
    if ( myBook->elfs_list->FirstFree )
    {
      item = GUIonMessage_GetCreatedItemIndex( msg );
      elem = (BOOK_LIST_ITEM*)List_Get( myBook->elfs_list, item );
      GUIonMessage_SetMenuItemText( msg, GetBookNameStrID(elem->book_name) );
      
      if ( !elem->isGuiBook )
      {
        GUIonMessage_SetItemDisabled( msg, TRUE );
        GUIonMessage_SetMenuItemUnavailableText( msg, STR( "Can't set focus to elf without GUI..." ) );
      }
    }
    break;
  case LISTMSG_HighlightChanged:
    item = GUIonMessage_GetSelectedItem( msg );
    RefreshElfSoftkeys( myBook, item );
    break;
  }
  return 1;
}


// при выборе пункта
void onEnterPressed_Books( BOOK* book, GUI* lt )
{
  BookObj_SetFocus( GetBook( BOOKLIST, book ), 0 );
  CloseMyBook( book, 0 );
}


// при выборе эльфа
void onEnterPressed_Elfs( BOOK* book, GUI* lt )
{
  BOOK* bk = GetBook( ELFLIST, book );
  if ( bk )
  {
    BookObj_SetFocus( bk, 0 );
    CloseMyBook( book, 0 );
  }
}


// при нажатии "С"
void onDelPressed_Books( BOOK* book, GUI* gui )
{
  BOOK* bk = GetBook( BOOKLIST, book );
  if ( bk )
  {
    TEXTID tmp = GetJavaName( bk );
    if ( tmp != EMPTY_TEXTID )
    {
      JavaSession_Manager( 0x0E );
      TextID_Destroy( tmp );
    }
    else
    {
      BOOK * sby_book=Find_StandbyBook();
      if (!DestroyMethod && bk!=sby_book) FreeBook(bk);
      else
      {
        int parentID;
        do
        {
          parentID=bk->xbook->parent_BookID;
          if (bk!=sby_book) FreeBook(bk);
          if (parentID!=-1) bk=FindBookByID(parentID);
        }
        while(parentID!=-1);
        //UI_Event_toBookID( RETURN_TO_STANDBY_EVENT, BookObj_GetBookID( bk ) );
        //UI_Event_toBookID( TERMINATE_SESSION_EVENT, BookObj_GetBookID( bk ) );
      }
    }
  }
}


// при нажатии "С" на эльфе
void onDelPressed_Elfs( BOOK* book, GUI* gui )
{
  BOOK* bk = GetBook( ELFLIST, book );
  if ( bk && bk !=  Find_StandbyBook())
  {
    if ( CheckEv( bk, RETURN_TO_STANDBY_EVENT ) )
    {
      UI_Event_toBookID( RETURN_TO_STANDBY_EVENT, BookObj_GetBookID( bk ) );
    }
    else
    {
      if ( CheckEv( bk, ELF_TERMINATE_EVENT ) )
      {
        UI_Event_toBookID( ELF_TERMINATE_EVENT, BookObj_GetBookID( bk )/* bk->BookID*/ );
      }
      else
      {
        MessageBox(EMPTY_TEXTID,STR("TerminateEvent not supported by elf..."),NOIMAGE,1,3000,book);
      }
    }
  }
}


// при нажатии "Автора!" на эльфе
void Author( BOOK* book, GUI* lt )
{
  BOOK* bk = GetBook( ELFLIST, book );
  if ( bk )
  {
    if ( CheckEv( bk, ELF_SHOW_INFO_EVENT ) )
    {
      MSG* msg = new MSG;
      msg->book = book;
      UI_Event_toBookIDwData( ELF_SHOW_INFO_EVENT, BookObj_GetBookID( bk ), msg, ( void(*)( void*) )mfree_adr() );
    }
    else
    {
      MessageBox(EMPTY_TEXTID,STR("Author unknown"),NOIMAGE,1,3000,book);
    }
  }
}


void Copyright( BOOK* book, GUI* lt )
{
  MyBOOK* mbk = (MyBOOK*) book;
  mbk->blistpos = ListMenu_GetSelectedItem( mbk->blist );
  MessageBox(EMPTY_TEXTID,COPYRIGHT_STRING,NOIMAGE,1,3000,book);
}


// патченый onKey от своей менюхи...; )
void myOnKey_Books( DISP_OBJ* p, int keyID, int i2, int i3, int press_mode )
{
  MyBOOK* mbk = (MyBOOK*) FindBook( isBookManager );
  
  mbk->oldOnKey( p, keyID, i2, i3, press_mode );
  
  if ( press_mode == KBD_SHORT_RELEASE || press_mode == KBD_LONG_PRESS )
  {
    if ( keyID == KEY_DIEZ )
    {
      if ( press_mode == KBD_SHORT_RELEASE )
      {
        BOOK* stby_bk = Find_StandbyBook();
        if ( stby_bk ) BookObj_SetFocus( stby_bk, 0 );
      }
      else
      {
        UI_Event( RETURN_TO_STANDBY_EVENT );
      }
      CloseMyBook( mbk, 0 );
    }
    
    if ( mbk->shortcuts_buf )
    {
      char key[20];
      sprintf( key, press_mode == 3?"[S_KEY%d]":"[L_KEY%d]", keyID-KEY_DIGITAL_0 );
      
      char* param = manifest_GetParam( mbk->shortcuts_buf, key, 0 );
      if ( param  )
      {
        wchar_t par[256];
        str2wstr( par, param );
        
        if ( strstr( param, "java:" ) )
        {
          char sp_flag;
          REQUEST_UI_OAF_START_APPLICATION( ASYNC, JavaShortcut_GetID(par)+0x10000, &sp_flag );
        }
        else
        {
          if ( StandbyOnTop )
          {
            StartAPP( par );
            BookObj_Show( Find_StandbyBook(), 0 );
          }
          else
          {
            Shortcut_Run( par );
          }
        }
        
        mfree( param );
        CloseMyBook( mbk, 0 );
      }
    }
  }
}


int StartElf( wchar_t* path, char* name )
{
  int res = -1;
  wchar_t* elfname = new wchar_t[wstrlen( path )+strlen( name )+1];
  wchar_t* wname = new wchar_t[strlen( name )+1];
  str2wstr( wname, name );
  wstrcpy( elfname, path );
  wstrcat( elfname, wname );
  
  {
    wchar_t* path;
    wchar_t* name;
    int len;
    name = wstrrchr( elfname, '/' );
    if ( name )
    {
      path = new wchar_t[ ( len = name-elfname ) + 1 ];
      memcpy( path, elfname, len * sizeof(wchar_t) );
      path[len] = 0;
      name++;
      if ( !fstat( path, name, 0 ) )
      {
        res = elfload( elfname, 0, 0, 0 );
      }
      delete( path );
    }
  }
  delete( elfname );
  delete( wname );
  return res;
}


// патченый onKey от вкладки "ЭЛЬФЫ"...; )
void myOnKey_Elfs( DISP_OBJ* p, int keyID, int i2, int i3, int press_mode )
{
  MyBOOK* mbk = (MyBOOK*) FindBook( isBookManager );
  
  mbk->oldOnKey1( p, keyID, i2, i3, press_mode );
  
  if ( press_mode == KBD_SHORT_RELEASE || press_mode == KBD_LONG_PRESS )
  {
    if ( keyID == KEY_DIEZ )
    {
      if ( press_mode == KBD_SHORT_RELEASE )
      {
        BOOK* stby_bk = Find_StandbyBook();
        if ( stby_bk )
          BookObj_SetFocus( stby_bk, 0 );
      }

      CloseMyBook( mbk, 0 );
      
    }
    
    if ( mbk->shortcuts_buf )
    {
      char key[20];
      sprintf( key, press_mode == 3 ? "[ES_KEY%d]" : "[EL_KEY%d]", keyID - KEY_DIGITAL_0 );
      
      char* param = manifest_GetParam( mbk->shortcuts_buf, key, 0 );
      if ( param )
      {
        if( StartElf( GetDir( DIR_ELFS | MEM_INTERNAL ), param ) < 0 )
          if ( StartElf( GetDir( DIR_ELFS | MEM_EXTERNAL ), param ) < 0 )
          {
            int ms[3];
            ms[0] = STR( "ZBin" );
            ms[1] = TextID_Create( param, ENC_LAT1, strlen( param ) );
            ms[2] = STR( "\n\nnot found.." );
            MessageBox( EMPTY_TEXTID,TextID_Create(ms,ENC_TEXTID,3),NOIMAGE,1,3000,FindBook(isBookManager));
          }
        
        mfree( param );
        CloseMyBook( mbk, 0 );
      }
    }
  }
}


void RefreshElfSoftkeys( MyBOOK* mbk, int item )
{
  if ( mbk->elfs_list->FirstFree )
  {
    BOOK_LIST_ITEM* elem = (BOOK_LIST_ITEM*) List_Get( mbk->elfs_list, item );
    if ( !elem->isGuiBook )
    {
      GUIObject_SoftKeys_SetVisible( mbk->elist, ACTION_SELECT1, FALSE );
    }
    else
    {
      GUIObject_SoftKeys_SetVisible( mbk->elist, ACTION_SELECT1, TRUE );
    }
  }
  else
  {
    ListMenu_SetNoItemText( mbk->elist, STR( "No elfs in memory" ) );
    GUIObject_SoftKeys_SetVisible( mbk->elist, ACTION_SELECT1, 0 );
    GUIObject_SoftKeys_SetVisible( mbk->elist, 1, 0 );
    GUIObject_SoftKeys_SetVisible( mbk->elist, 2, 0 );
  }
}


void RefreshBookSoftkeys( MyBOOK* mbk, int item )
{
  BOOK_LIST_ITEM* elem = (BOOK_LIST_ITEM*) List_Get( mbk->books_list, item );
  if ( !elem->isGuiBook )
  {
    GUIObject_SoftKeys_SetVisible( mbk->blist, ACTION_SELECT1, FALSE );
  }
  else
  {
    GUIObject_SoftKeys_SetVisible( mbk->blist, ACTION_SELECT1, TRUE );
  }
}


// создание меню
void CreateBookManagerGUI( MyBOOK* mbk )
{
  int str_id;
  int list_pos;
  
  DestroyBookAndElfsLists( mbk );
  CreateBookAndElfsLists( mbk );
  
  if ( !mbk->gui )
  {
    mbk->gui = CreateTabMenuBar( mbk );
    TabMenuBar_SetTabCount( mbk->gui, 2 );
    
    TabMenuBar_SetTabIcon( mbk->gui, 0, mbk->tabs_image[0].ImageID, 0 );
    TabMenuBar_SetTabIcon( mbk->gui, 0, mbk->tabs_image[1].ImageID, 1 );
    
    TabMenuBar_SetTabIcon( mbk->gui, 1, mbk->tabs_image[2].ImageID, 0 );
    TabMenuBar_SetTabIcon( mbk->gui, 1, mbk->tabs_image[3].ImageID, 1 );
  }
  
  if ( mbk->blist )
  {
    list_pos = ListMenu_GetSelectedItem(mbk->blist);
    ListMenu_DestroyItems(mbk->blist);
    ListMenu_SetItemCount( mbk->blist, mbk->blistcnt );
    ListMenu_SetCursorToItem( mbk->blist, list_pos );
    RefreshBookSoftkeys( mbk, ListMenu_GetSelectedItem(mbk->blist) );
  }
  else
  {
    mbk->blist = CreateListMenu( mbk, 0 );
    ListMenu_SetOnMessage( mbk->blist, onCallback_Books );
    ListMenu_SetItemCount( mbk->blist, mbk->blistcnt );
    if ( mbk->blistpos > mbk->blistcnt )
    {
      ListMenu_SetCursorToItem( mbk->blist, mbk->blistcnt );
    }
    else
    {
      ListMenu_SetCursorToItem( mbk->blist, mbk->blistpos );
    }
    
    GUIObject_SoftKeys_SetAction( mbk->blist, ACTION_BACK, CloseMyBook );
    GUIObject_SoftKeys_SetAction( mbk->blist, ACTION_LONG_BACK, PreTerminateManager );
    GUIObject_SoftKeys_SetAction( mbk->blist, ACTION_SELECT1, onEnterPressed_Books );
    GUIObject_SoftKeys_SetAction( mbk->blist, ACTION_DELETE, onDelPressed_Books );
    GUIObject_SoftKeys_SetVisible(mbk->blist, ACTION_DELETE, FALSE);
    
    GUIObject_SoftKeys_SetAction( mbk->blist, 0, Shortcuts );
    textidname2id( L"SHC_EDIT_SHORTCUT_TXT", TEXTID_ANY_LEN, &str_id );
    GUIObject_SoftKeys_SetText( mbk->blist, 0, str_id );
    GUIObject_SoftKeys_SetAction( mbk->blist, 1, BookNames );
    textidname2id( L"DB_RENAME_TXT", TEXTID_ANY_LEN, &str_id );
    GUIObject_SoftKeys_SetText( mbk->blist, 1, str_id );
    GUIObject_SoftKeys_SetAction( mbk->blist, 2, Copyright );
    GUIObject_SoftKeys_SetText( mbk->blist, 2, STR( "About" ) );
    
    mbk->oldOnKey = DispObject_GetOnKey( GUIObject_GetDispObject( mbk->blist ) );
    
    DISP_DESC_SetOnKey( DispObject_GetDESC ( GUIObject_GetDispObject( mbk->blist ) ), myOnKey_Books );
    
    TabMenuBar_SetTabGui( mbk->gui, 0, mbk->blist );

    TEXTID p[2] = { TextID_Create ( L"Heap : ", ENC_UCS2, 7 ), TextID_CreateIntegerID ( GetFreeBytesOnHeap() ) };
    TabMenuBar_SetTabTitle( mbk->gui, 0, TextID_Create( p, ENC_TEXTID, 2 ) );	
  }
  //---------------
  
  if ( mbk->elist )
  {
    list_pos = ListMenu_GetSelectedItem(mbk->elist);
    ListMenu_DestroyItems(mbk->elist);
    ListMenu_SetItemCount( mbk->elist, mbk->elistcnt );
    ListMenu_SetCursorToItem( mbk->elist, list_pos );
    RefreshElfSoftkeys( mbk, ListMenu_GetSelectedItem(mbk->elist) );
  }
  else
  {
    mbk->elist = CreateListMenu( mbk, 0 );
    ListMenu_SetCursorToItem( mbk->elist, 0 );
    ListMenu_SetOnMessage( mbk->elist, onCallback_Elfs );
    ListMenu_SetItemCount( mbk->elist, mbk->elistcnt );
    if ( mbk->elistpos > mbk->elistcnt )
    {
      ListMenu_SetCursorToItem( mbk->elist, mbk->elistcnt );
    }
    else
    {
      ListMenu_SetCursorToItem( mbk->elist, mbk->elistpos );
    }
    
    GUIObject_SoftKeys_SetAction( mbk->elist, ACTION_BACK, CloseMyBook );
    GUIObject_SoftKeys_SetAction( mbk->elist, ACTION_LONG_BACK, PreTerminateManager );
    GUIObject_SoftKeys_SetAction( mbk->elist, ACTION_SELECT1, onEnterPressed_Elfs );
    GUIObject_SoftKeys_SetAction( mbk->elist, ACTION_DELETE, onDelPressed_Elfs );
    GUIObject_SoftKeys_SetVisible(mbk->elist, ACTION_DELETE, FALSE);
    
    GUIObject_SoftKeys_SetAction( mbk->elist, 0, Shortcuts );
    textidname2id( L"SHC_EDIT_SHORTCUT_TXT", TEXTID_ANY_LEN, &str_id );
    GUIObject_SoftKeys_SetText( mbk->elist, 0, str_id );
    GUIObject_SoftKeys_SetAction( mbk->elist, 1, BookNames );
    textidname2id( L"DB_RENAME_TXT", TEXTID_ANY_LEN, &str_id );
    GUIObject_SoftKeys_SetText( mbk->elist, 1, str_id );
    GUIObject_SoftKeys_SetAction( mbk->elist, 2, Author );
    GUIObject_SoftKeys_SetText( mbk->elist, 2, STR( "Author" ) );
    
    mbk->oldOnKey1 = DispObject_GetOnKey( GUIObject_GetDispObject( mbk->elist ) );
    
    DISP_DESC_SetOnKey( DispObject_GetDESC ( GUIObject_GetDispObject( mbk->elist ) ), myOnKey_Elfs );
    
    RefreshElfSoftkeys( mbk, ListMenu_GetSelectedItem(mbk->elist) );
    
    TabMenuBar_SetTabGui( mbk->gui, 1, mbk->elist );
    TabMenuBar_SetTabTitle( mbk->gui, 1, STR( "Elfs" ) );
  }
  
  TabMenuBar_SetFocusedTab( mbk->gui, mbk->ActiveTAB );
  
  GUIObject_Show(mbk->gui);
}


void DestroyIniBuffers(MyBOOK* myBook)
{
  if ( myBook->ini_buf )
  {
    delete myBook->ini_buf;
    myBook->ini_buf=0;
  }
  if ( myBook->shortcuts_buf )
  {
    delete myBook->shortcuts_buf;
    myBook->shortcuts_buf=0;
  }
}


void DestroyAllGUIs(MyBOOK* myBook)
{
  if ( myBook->java_list_menu )
  {
    GUIObject_Destroy( myBook->java_list_menu );
    myBook->java_list_menu=0;
    List_DestroyElements( myBook->java_list, elem_filter, elem_free );
    List_Destroy( myBook->java_list );
    myBook->java_list = 0;
  }
  
  if ( myBook->mode_list )
  {
    GUIObject_Destroy( myBook->mode_list );
    myBook->mode_list=0;
  }
  
  if ( myBook->but_list )
  {
    GUIObject_Destroy( myBook->but_list );
    myBook->but_list=0;
  }
  
  if ( myBook->YesNoQuestion )
  {
    GUIObject_Destroy( myBook->YesNoQuestion );
    myBook->YesNoQuestion=0;
  }
  
  if ( myBook->blist )
  {
    GUIObject_Destroy( myBook->blist );
    myBook->blist=0;
    myBook->blistpos=0;
  }
  
  if ( myBook->elist )
  {
    GUIObject_Destroy( myBook->elist );
    myBook->elist=0;
    myBook->elistpos=0;
  }
  
  if ( myBook->gui )
  {
    GUIObject_Destroy( myBook->gui );
    myBook->gui=0;
  }
  
  if ( myBook->MainMenuID != -1 )
  {
    BOOK* MainMenu = FindBookByID( myBook->MainMenuID );
    
    if ( MainMenu )
      FreeBook( MainMenu );
    
    myBook->MainMenuID = -1;
  }
}


void DestroyIcons(MyBOOK* myBook)
{
  int i;
  
  for ( i = 0;i < ICONS_COUNT;i++ )
  {
    ImageID_Free( myBook->tabs_image[i].ImageID );
  }
  
  for ( i = 0;i < DIGITS_COUNT;i++ )
  {
    ImageID_Free( myBook->digs_image[i].ImageID );
  }
}


// при закрытии книги
void onMyBookClose( BOOK* book )
{
  MyBOOK* myBook = (MyBOOK*) book;
  
  DestroyIniBuffers(myBook);
  
  DestroyAllGUIs(myBook);
  
  DestroyBookAndElfsLists( myBook );
  
  DestroyIcons(myBook);
}


void ReturnMyBook( BOOK* bk, GUI* )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  GUIObject_Destroy( mbk->YesNoQuestion );
  mbk->YesNoQuestion = 0;
  mbk->ActiveTAB=GetActiveTab(mbk);
  CreateBookManagerGUI( mbk );
}


void CloseMyBook( BOOK* bk, GUI* )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  
  mbk->ActiveTAB=GetActiveTab(mbk);
  BookObj_GotoPage( mbk, &BookManager_Idle_Page);
}


void TerminateManager( BOOK* Book, GUI* )
{
  MessageBox( EMPTY_TEXTID,STR("BookManager\n\nterminated"),NOIMAGE,1,3000,0);
  FreeBook( Book );
  MODIFYKEYHOOK( NewKey, KEY_HOOK_REMOVE, NULL );
  SUBPROC( elf_exit );
}


void PreTerminateManager( BOOK* bk, GUI* )
{
  MyBOOK* mbk = (MyBOOK*) bk;

  mbk->YesNoQuestion = CreateYesNoQuestionVA( 0,
                                             VAR_BOOK( mbk ),
                                             VAR_YESNO_PRE_QUESTION( TextID_Create( "Exit command selected", ENC_LAT1, TEXTID_ANY_LEN ) ),
                                             VAR_YESNO_QUESTION( TextID_Create( "Unload?", ENC_LAT1, TEXTID_ANY_LEN ) ),
                                             0 );
  GUIObject_SoftKeys_SetAction( mbk->YesNoQuestion, ACTION_YES, TerminateManager );
  GUIObject_SoftKeys_SetAction( mbk->YesNoQuestion, ACTION_NO, CloseMyBook );
  GUIObject_SoftKeys_SetAction( mbk->YesNoQuestion, ACTION_BACK, ReturnMyBook );
}


int RecreateBookList( void* r0, BOOK* bk )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  CreateBookManagerGUI( mbk );
  return(0);
}


// собственно старт
__root int CreateBookList( void* r0, BOOK* bk )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  
  get_iconsID( mbk );

  if ( FirstTab )
    mbk->ActiveTAB = FirstTab-1;
  
  CreateBookManagerGUI( mbk );
  return 0;
}


int IdlePage_EnterAction( void* r0, BOOK* bk )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  
  DestroyAllGUIs(mbk);
  DestroyBookAndElfsLists(mbk);
  DestroyIcons(mbk);
  
  BookObj_WindowSetWantsFocus(mbk,0,FALSE);
  BookObj_Hide(mbk,0);
  return 0;
}


int ReconfigElf( void* mess, BOOK* book )
{
  RECONFIG_EVENT_DATA* reconf = ( RECONFIG_EVENT_DATA* )mess;
  
  if ( !wstrcmpi( reconf->path, successed_config_path ) && !wstrcmpi( reconf->name, successed_config_name ) )
  {
    InitConfig();
    return 1;
  }
  return 0;
}


int NewKey( int key, int rep_count, int mode, MyBOOK * mbk, DISP_OBJ* )
{
  if ( mode == KeyPressMode )
  {
    if ( key == KeyActiv )
    {
      if ( isKeylocked() && !Ignore_KeyLock )
        return 0;
      
      if ( Display_GetTopBook(0)!=mbk )
      {
        BookObj_WindowSetWantsFocus(mbk,0,TRUE);
        BookObj_SetFocus(mbk,0);
        BookObj_GotoPage( mbk, &BookManager_Main_Page );
        return -1;
      }
      else
      {
        if ( !mbk->mode_list && !mbk->YesNoQuestion && !mbk->StringInput )
        {
          CloseMyBook( mbk, 0 );
          return 0;
        }
        return -1;
      }
    }
  }
  return 0;
}


MyBOOK * CreateBookManagerBook()
{
  MyBOOK* mbk = (MyBOOK*) malloc( sizeof( MyBOOK ) );
  memset( mbk, 0, sizeof( MyBOOK ) );
  CreateBook( mbk, onMyBookClose, &BookManager_Base_Page, "BookManager", -1, 0 );
  //Init flags
  mbk->gui = 0;
  mbk->ini_buf=0;
  mbk->ini_buf_size=0;
  mbk->shortcuts_buf = 0;
  mbk->shortcuts_buf_size = 0;
  mbk->books_list = 0;
  mbk->elfs_list = 0;
  mbk->java_list = 0;
  mbk->java_list_menu = 0;
  mbk->blist = 0;
  mbk->elist = 0;
  mbk->mode_list = 0;
  mbk->but_list = 0;
  mbk->YesNoQuestion = 0;
  mbk->StringInput = 0;
  mbk->MainMenuID = -1;
  mbk->blistpos = 0;
  mbk->elistpos = 0;
  mbk->blistcnt = 0;
  mbk->elistcnt = 0;
  mbk->ActiveTAB = 0;
  mbk->isA2 = 0;
  //End init
  int platform=GetChipID()&CHIPID_MASK;
  if (platform==CHIPID_DB3150||platform==CHIPID_DB3200||platform==CHIPID_DB3210||platform==CHIPID_DB3350)
    mbk->isA2 = 1;
  
  LoadBookNames(mbk);
  LoadShortcuts(mbk);
  
  BookObj_GotoPage( mbk, &BookManager_Idle_Page);
  
  return mbk;
}


int main ( void )
{
  if ( !FindBook( isBookManager ) )
  {
	trace_init(L"bookmanmem.txt");
    InitConfig();
    MODIFYKEYHOOK( NewKey, KEY_HOOK_ADD, CreateBookManagerBook() );
  }
  else
  {
    MessageBox( EMPTY_TEXTID, STR( "BookManager is already runed" ), NOIMAGE, 1, 5000, 0 );
    SUBPROC( elf_exit );
  }
  return 0;
}
