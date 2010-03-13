#include "..\include\Lib_Clara.h"
#include "..\include\Dir.h"
#include "..\include\cfg_items.h"
#include "..\include\var_arg.h"
#include "shortcuts.h"
#include "main.h"



wchar_t* get_path()
{
  wchar_t* path = new wchar_t[wstrlen( GetDir( DIR_INI ) ) + 8 + 1];
  wstrcpy( path, GetDir( DIR_INI ) );
  wstrcat( path, L"/bookman" );
  return path;
}


void Shortcut_Append( wchar_t* name_buf, char* mask_buf, wchar_t* path )
{
  int f;
  if ( ( f = _fopen( path, L"shortcuts.ini", 0x108, 0x180, 0 ) ) >= 0 )
  {
    char* temp_buf = new char[strlen( mask_buf ) + wstrlen( name_buf ) + 4];
    strcpy( temp_buf, mask_buf );
    strcat( temp_buf, " " );
    wstr2strn( temp_buf + strlen( temp_buf ), name_buf, wstrlen( name_buf ) );
    strcat( temp_buf, "\r\n" );
    fwrite( f, temp_buf, strlen( temp_buf ) );
    fclose( f );
    delete( temp_buf );
  }
  else
  {
    MessageBox( 0x6fFFFFFF, STR( "Can't open shortcuts.ini" ), 0, 1 , 5000, 0 );
  }
}


void ReWriteShortcut( MyBOOK* mbk, wchar_t* name_buf, char* mask_buf, wchar_t* path )
{
  int f;
  char* pos;
  if ( mbk->shortcuts_buf )
  {
    char* param = manifest_GetParam( mbk->shortcuts_buf, mask_buf, 0 );
    
    int len = wstrlen( name_buf );      //длина старого €рлыка
    char* str_buf = new char[len + 1];
    unicode2win1251( str_buf, name_buf, len );
    int len_prefix = strlen(mask_buf)+1;      //длина префикса с пробелом
    if ( param )
    {
      pos = strstr( mbk->shortcuts_buf, mask_buf );
      if ( ( f = _fopen( path, L"shortcuts.ini", 0x204, 0x180, 0 ) ) >= 0 )
      {
        int len_minus = strlen( param );    //длина старого €рлыка
        fwrite( f, mbk->shortcuts_buf, pos - mbk->shortcuts_buf + len_prefix );     //пишем начало файла
        fwrite( f, str_buf, len );      //пишем новый €рлык
        fwrite( f, pos + len_prefix + len_minus, ( mbk->shortcuts_buf_size - ( pos - mbk->shortcuts_buf + len_prefix ) ) - len_minus );      //пишем остаток файла
        fclose( f );
      }
      else
      {
        MessageBox( 0x6fFFFFFF, STR( "Can't open shortcuts.ini" ), 0, 1 , 5000, mbk );
      }
      mfree( param );
    }
    else
    {
      if ( pos = strstr( mbk->shortcuts_buf, mask_buf ) )
      {
        if ( ( f = _fopen( path, L"shortcuts.ini", 0x204, 0x180, 0 ) ) >= 0 )
        {
          fwrite( f, mbk->shortcuts_buf, pos - mbk->shortcuts_buf + len_prefix );
          fwrite( f, str_buf, len );
          fwrite( f, pos + len_prefix, mbk->shortcuts_buf_size - ( pos - mbk->shortcuts_buf + len_prefix ) );
          fclose( f );
        }
        else
        {
          MessageBox( 0x6fFFFFFF, STR( "Can't open shortcuts.ini" ), 0, 1 , 5000, 0 );
        }
      }
      else
      {
        Shortcut_Append( name_buf, mask_buf, path );
      }
    }
    delete( str_buf );
  }
  else
  {
    Shortcut_Append( name_buf, mask_buf, path );
  }
}

void WriteShortcut( wchar_t* name_buf )
{
  MyBOOK* mbk = (MyBOOK*) FindBook( isBookManager );
  wchar_t* path = get_path();
  char mask_buf[10];
  
  int dig_num = ListMenu_GetSelectedItem( mbk->but_list );
  
  if ( !ListMenu_GetSelectedItem( mbk->mode_list ) )
  {
    if ( !mbk->ActiveTAB )
      sprintf( mask_buf, "[S_KEY%d]", dig_num );
    else
      sprintf( mask_buf, "[ES_KEY%d]", dig_num );
    
    ReWriteShortcut( mbk, name_buf, mask_buf, path );
  }
  else
  {
    if ( !mbk->ActiveTAB )
      sprintf( mask_buf, "[L_KEY%d]", dig_num );
    else
      sprintf( mask_buf, "[EL_KEY%d]", dig_num );
    
    ReWriteShortcut( mbk, name_buf, mask_buf, path );
  }
  delete( path );
}

void onShortcutSet( BOOK* MainMenu, GUI* )
{
  wchar_t* name_buf = MenuBook_Desktop_GetSelectedItemID( MainMenu );
  if ( name_buf )
  {
    WriteShortcut( name_buf );
    mfree( name_buf );
  }
  FreeBook( MainMenu );
  BookObj_ReturnPage( FindBook( isBookManager ), ACCEPT_EVENT );
}


int onPrevious_MainMenu_DB( void* data, BOOK* book )
{
  BookObj_ReturnPage( book, NIL_EVENT );
  return 1;
}


int onCancel_MainMenu_DB( void* data, BOOK* book )
{
  BookObj_ReturnPage( book, CANCEL_EVENT );
  return 1;
}


int CreateMainMenu( void* data, BOOK* book )
{
  BOOK* MainMenu = MenuBook_Desktop( 1, BookObj_GetBookID( book ) );
  if ( MainMenu )
  {
    ( (MyBOOK*) book )->MainMenuID = BookObj_GetBookID( MainMenu );
    BookObj_SoftKeys_SetAction( MainMenu, 0x0, onShortcutSet );
    int str_id;
    textidname2id( L"SHC_SET_SHORTCUT_SK", SID_ANY_LEN, &str_id );
    BookObj_SoftKeys_SetText( MainMenu, 0x0, str_id );
    return 1;
  }
  BookObj_ReturnPage( book, ACCEPT_EVENT );
  return 0;
}

int DB_Filter( const wchar_t* ext_table, const wchar_t* path , const wchar_t* name )
{
  FSTAT _fstat;
  
  if ( DataBrowser_isFileInListExt( ext_table, path, name ) )
    return 1;
  
  fstat( path, name, &_fstat );
  
  return 0 != ( _fstat.unk1 & 0x10000 );
}


int onAccept_DB( void* data, BOOK* book )
{
  wchar_t* path_ptr = wstrwstr( FILEITEM_GetPath( ( FILEITEM* )data ), L"ZBin/" );
  wchar_t* elf_buf = new wchar_t[wstrlen( path_ptr + 4 ) + wstrlen( FILEITEM_GetFname( ( FILEITEM* )data ) ) + 1];
  elf_buf[0] = 0;
  
  if ( path_ptr )
    wstrcpy( elf_buf, path_ptr + 4 );
  
  wstrcat( elf_buf, L"/" );
  wstrcat( elf_buf, FILEITEM_GetFname( ( FILEITEM* )data ) );
  WriteShortcut( elf_buf );
  delete( elf_buf );
  BookObj_ReturnPage( book, ACCEPT_EVENT );
  return 0;
}

int CreateDB( void* data, BOOK* book )
{
  void* DB_Desc = DataBrowserDesc_Create();
  const wchar_t* folder_list[3];
  folder_list[0] = GetDir( DIR_ELFS | MEM_INTERNAL );
  folder_list[1] = GetDir( DIR_ELFS | MEM_EXTERNAL );
  folder_list[2] = 0;
  DataBrowserDesc_SetHeaderText( DB_Desc, STR( "ZBin" ) );
  DataBrowserDesc_SetBookID( DB_Desc, BookObj_GetBookID( book ) );
  DataBrowserDesc_SetFolders( DB_Desc, folder_list );
  DataBrowserDesc_SetFileExtList( DB_Desc, L"*.elf" );
  DataBrowserDesc_SetItemFilter( DB_Desc, DB_Filter );
  DataBrowserDesc_SetFoldersNumber( DB_Desc, 2 );
  DataBrowserDesc_SetSelectAction( DB_Desc, 1 );
  DataBrowser_Create( DB_Desc );
  DataBrowserDesc_Destroy( DB_Desc );
  return 0;
}

/*
void onPrevious_SI( BOOK* book )
{
BookObj_ReturnPage( book, ACCEPT_EVENT );
}

void onCancel_SI( BOOK* book )
{
BookObj_ReturnPage( book, NIL_EVENT );
BookObj_ReturnPage( book, NIL_EVENT );
BookObj_ReturnPage( book, NIL_EVENT );
}

int onExit_SI( void* data, BOOK* book )
{
if ( str_inp )
{
GUIObject_Destroy( str_inp );
str_inp = 0;
}
return 0;
}

void onAccept_SI( BOOK* book, wchar_t* string, int len )
{
WriteShortcut( string );
BookObj_ReturnPage( book, ACCEPT_EVENT );
}


int CreateSI( void* data, BOOK* book )
{
int str_id;
int editable_strID = 0x6FFFFFFF;
if ( shortcuts_buf )
{
char mask_buf[10];
int dig_num = ListMenu_GetSelectedItem( but_list );
if ( !ListMenu_GetSelectedItem( mode_list ) ) sprintf( mask_buf, "[S_KEY%d]", dig_num );
else sprintf( mask_buf, "[L_KEY%d]", dig_num );
char* param;
if ( param = manifest_GetParam( shortcuts_buf, mask_buf, 0 ) )
{
editable_strID = Str2ID( param, 6, SID_ANY_LEN );
mfree( param );
}
}
textidname2id( L"SHC_SET_SHORTCUT_SK", SID_ANY_LEN, &str_id );
if ( str_inp ) GUIObject_Destroy( str_inp );
str_inp = CreateStringInputVA( 0,
VAR_PREV_ACTION_PROC( onPrevious_SI ),
VAR_LONG_BACK_PROC( onCancel_SI ),
VAR_BOOK( book ),
VAR_OK_PROC( onAccept_SI ),
VAR_STRINP_FIXED_TEXT( str_id ),
VAR_STRINP_TEXT( editable_strID ),
VAR_STRINP_MIN_LEN( 0 ),
VAR_STRINP_MAX_LEN( 200 ),
0 );
return 0;
}
*/

void onEnter_JavaList( BOOK* book, GUI* )
{
  MyBOOK* mbk = (MyBOOK*) book;
  java_list_elem* elem = (java_list_elem*) List_Get( mbk->java_list, ListMenu_GetSelectedItem( mbk->java_list_menu ) );
  int java_buf_len = wstrlen( elem->name ) + 40;
  wchar_t* java_buf = new wchar_t[java_buf_len];
  snwprintf( java_buf, java_buf_len, L"java:%ls//ID=%d", elem->name, elem->appID );
  
  WriteShortcut( java_buf );
  delete( java_buf );
  BookObj_ReturnPage( book, ACCEPT_EVENT );
}


void JavaList_Cancel_Action( BOOK* book, GUI* )
{
  BookObj_ReturnPage( book, CANCEL_EVENT );
}


void JavaList_Prev_Action( BOOK* book, GUI* )
{
  BookObj_ReturnPage( book, NIL_EVENT );
}


void elem_free( void* elem )
{
  java_list_elem* lm = (java_list_elem*) elem;
  
  if ( lm->name )
    delete( lm->name );
  
  if ( lm->fullpath )
    delete( lm->fullpath );
  
  if ( lm->imageID )
    ImageID_Free( lm->imageID );
  
  delete( lm );
}

int elem_filter( void* elem )
{
  return elem!= NULL;
}


int onExit_JavaList( void* data, BOOK* book )
{
  MyBOOK* mbk = (MyBOOK*) book;
  
  if ( mbk->java_list_menu )
  {
    GUIObject_Destroy( mbk->java_list_menu );
    mbk->java_list_menu = 0;
  }
  
  if ( mbk->java_list )
  {
    List_DestroyElements( mbk->java_list, elem_filter, elem_free );
    List_Destroy( mbk->java_list );
    mbk->java_list = 0;
  }
  return 0;
}


int java_list_callback( GUI_MESSAGE* msg )
{
  MyBOOK* mbk = (MyBOOK*) GUIonMessage_GetBook( msg );
  
  switch( GUIonMessage_GetMsg( msg ) )
  {
  case LISTMSG_GetItem:
    java_list_elem* elem = (java_list_elem*) List_Get( mbk->java_list, GUIonMessage_GetCreatedItemIndex( msg ) );
    GUIonMessage_SetMenuItemText( msg, Str2ID( elem->name, 0, SID_ANY_LEN ) );
    JavaApp_LogoImageID_Get( elem->fullpath, &elem->imageID );
    GUIonMessage_SetMenuItemIcon( msg, 0, elem->imageID );
  }
  return 1;
}


java_list_elem* CreateElem( void* JavaDesc )
{
  java_list_elem* elem = new java_list_elem;
  wchar_t* sp;
  JavaAppDesc_GetJavaAppInfo( JavaDesc, 0, &sp );
  elem->name = sp;
  JavaAppDesc_GetJavaAppInfo( JavaDesc, 5, &sp );
  elem->fullpath = sp;
  elem->appID = JavaAppDesc_GetJavaAppID( JavaDesc );
  return elem;
}


int CreateJavaList( void* data, BOOK* book )
{
  MyBOOK* mbk = (MyBOOK*) book;
  
  if ( mbk->java_list )
  {
    List_DestroyElements( mbk->java_list, elem_filter, elem_free );
    List_Destroy( mbk->java_list );
  }
  
  mbk->java_list = List_Create();
  char sp1;
  void* JavaDesc;
  JavaDialog_Open( 0, &sp1, &JavaDesc );
  
  if ( !JavaAppDesc_GetFirstApp( JavaDesc ) )
  {
    int result = 0;
    while ( !result )
    {
      List_InsertLast( mbk->java_list, CreateElem( JavaDesc ) );
      result = JavaAppDesc_GetNextApp( JavaDesc );
    }
  }
  
  JavaDialog_Close( sp1 );
  
  if ( mbk->java_list_menu )
    GUIObject_Destroy( mbk->java_list_menu );
  
  mbk->java_list_menu = CreateListMenu( book, 0 );
  GUIObject_SetTitleText( mbk->java_list_menu, STR( "Java" ) );
  ListMenu_SetItemCount( mbk->java_list_menu, mbk->java_list->FirstFree );
  ListMenu_SetOnMessage( mbk->java_list_menu, java_list_callback );
  ListMenu_SetCursorToItem( mbk->java_list_menu, 0 );
  GUIObject_SoftKeys_SetAction( mbk->java_list_menu, ACTION_BACK, JavaList_Prev_Action );
  GUIObject_SoftKeys_SetAction( mbk->java_list_menu, ACTION_LONG_BACK, JavaList_Cancel_Action );
  GUIObject_SoftKeys_SetAction( mbk->java_list_menu, ACTION_SELECT1, onEnter_JavaList );
  GUIObject_Show( mbk->java_list_menu );
  return 0;
}


const PAGE_MSG SelectShortcut_PageEvents[]@ "DYN_PAGE" = {
  PAGE_ENTER_EVENT_TAG, CreateMainMenu,
  PREVIOUS_EVENT_TAG, onPrevious_MainMenu_DB,
  CANCEL_EVENT_TAG, onCancel_MainMenu_DB,
  NIL_EVENT_TAG, 0
};

PAGE_DESC SelectShortcut_page = { "BookManager_ChangeShortcuts_SelectShortcut_Page", 0, SelectShortcut_PageEvents };


const PAGE_MSG SelectElf_PageEvents[]@ "DYN_PAGE" = {
  PAGE_ENTER_EVENT_TAG, CreateDB,
  ACCEPT_EVENT_TAG, onAccept_DB,
  PREVIOUS_EVENT_TAG, onPrevious_MainMenu_DB,
  CANCEL_EVENT_TAG, onCancel_MainMenu_DB,
  NIL_EVENT_TAG, 0
};

PAGE_DESC SelectElf_page = { "BookManager_ChangeShortcuts_SelectElf_Page", 0, SelectElf_PageEvents };

/*
const PAGE_MSG EditShortcut_PageEvents[]@ "DYN_PAGE" = {
PAGE_ENTER_EVENT_TAG, CreateSI,
PAGE_EXIT_EVENT_TAG, onExit_SI,
NIL_EVENT_TAG, 0
};

PAGE_DESC ChangeShortcuts_Edit_page = { "BookManager_ChangeShortcuts_Edit_Page", 0, EditShortcut_PageEvents };
*/

const PAGE_MSG SetJava_PageEvents[]@ "DYN_PAGE" = {
  PAGE_ENTER_EVENT_TAG, CreateJavaList,
  PAGE_EXIT_EVENT_TAG, onExit_JavaList,
  NIL_EVENT_TAG, 0
};

PAGE_DESC ChangeShortcuts_SetJava_page = { "BookManager_ChangeShortcuts_SetJava_Page", 0, SetJava_PageEvents };


void But_SetJava( BOOK* book, GUI* )
{
  BookObj_CallPage( book, &ChangeShortcuts_SetJava_page );
}


void DestroyButList( BOOK* book, GUI* )
{
  BookObj_ReturnPage( book, PREVIOUS_EVENT );
}


void * SHORTCUT_DESC_Init(char * param)
{
  SHORTCUT_DESC * w_buf = new SHORTCUT_DESC;
  memset( w_buf, 0, sizeof(w_buf) );
  w_buf->shortcut_state = SC_State_Present;
  
  if ( strlen( param ) < 0x32 )
  {
    str2wstr( w_buf->name, param );
  }
  else
  {
    char temp_buf[0x32];
    strncpy( temp_buf, param, MAXELEMS(temp_buf) - 1 );
    temp_buf[ MAXELEMS(temp_buf) - 1 ] = 0;
    str2wstr( w_buf->name, temp_buf );
  }
  if (!wstrcmp(w_buf->name,L"MainMenu")) w_buf->shortcut_state = SC_State_MainMenu;
  return(w_buf);
}


void * SHORTCUT_DESC_A2_Init(char * param)
{
  SHORTCUT_DESC_A2 * w_buf = new SHORTCUT_DESC_A2;
  memset( w_buf, 0, sizeof(w_buf) );
  w_buf->shortcut_state = SC_State_Present;
  str2wstr( w_buf->name, param );
  if (!wstrcmp(w_buf->name,L"MainMenu")) w_buf->name_type = SC_Name_MainMenu;
  else w_buf->name_type = SC_Name_StandName;
  
  return(w_buf);
}


int but_list_callback( GUI_MESSAGE* msg )
{
  int str_id=0x6FFFFFFF;
  int icon_id=0xFFFF;
  char* param = 0;
  MyBOOK* mbk = (MyBOOK*) GUIonMessage_GetBook( msg );
  
  switch( GUIonMessage_GetMsg( msg ) )
  {
  case LISTMSG_GetItem:
    int item_num = GUIonMessage_GetCreatedItemIndex( msg );
    char mask_buf[10];
    
    if ( mbk->shortcuts_buf )
    {
      if ( !ListMenu_GetSelectedItem( mbk->mode_list ) )
      {
        if ( !mbk->ActiveTAB )
          sprintf( mask_buf, "[S_KEY%d]", item_num );
        else
          sprintf( mask_buf, "[ES_KEY%d]", item_num );
      }
      else
      {
        if ( !mbk->ActiveTAB )
          sprintf( mask_buf, "[L_KEY%d]", item_num );
        else
          sprintf( mask_buf, "[EL_KEY%d]", item_num );
      }
      if ( param = manifest_GetParam( mbk->shortcuts_buf, mask_buf, 0 ) )
      {
        if ( strlen( param ) )
        {
          if ( strstr( param, "java:" ) )
          {
            str_id = Str2ID( param + 5, 6, strstr( param, "//" ) - ( param + 5 ) );
            iconidname2id( L"DB_LIST_JAVA_ICN", SID_ANY_LEN, &icon_id );
          }
          else
          {
            void * w_buf;
            if (mbk->isA2) w_buf = SHORTCUT_DESC_A2_Init(param);
            else w_buf = SHORTCUT_DESC_Init(param);
            
            str_id = Shortcut_Get_MenuItemName( w_buf );
            icon_id = Shortcut_Get_MenuItemIconID( w_buf );
            
            if ( icon_id == 0xFFFF ) iconidname2id( L"RN_VERT_MY_SHORTCUTS_ICN", SID_ANY_LEN, &icon_id );
            
            delete( w_buf );
            
            if ( str_id == 0x6FFFFFFF ) str_id = Str2ID( param, 6, SID_ANY_LEN );
          }
        }
        delete(param);
      }
      else
      {
        textidname2id( L"SHC_NONE_NAME_TXT", SID_ANY_LEN, &str_id );
      }
    }
    else
    {
      textidname2id( L"SHC_NONE_NAME_TXT", SID_ANY_LEN, &str_id );
    }
    
    GUIonMessage_SetMenuItemText( msg, str_id );
    GUIonMessage_SetMenuItemIcon(msg, 0, mbk->digs_image[item_num].ImageID );
    GUIonMessage_SetMenuItemIcon(msg, 2, icon_id );
    break;
  }

  return 1;
}

/*
void But_EditShortcut( BOOK* book, void* )
{
BookObj_CallPage( book, &ChangeShortcuts_Edit_page );
}
*/

void But_SetMM( BOOK* book, GUI* )
{
  wchar_t name_buf[20];
  wstrcpy( name_buf, L"MainMenu" );
  WriteShortcut( name_buf );
  CreateButtonList( 0, book );
}

void But_onEnter( BOOK* book, GUI* )
{
  if ( !((MyBOOK*)book)->ActiveTAB )
    BookObj_CallPage( book, &SelectShortcut_page );
  else
    BookObj_CallPage( book, &SelectElf_page );
}


int ExitButtonList( void* data, BOOK* book )
{
  MyBOOK* mbk = (MyBOOK*) book;
  if ( mbk->but_list )
  {
    GUIObject_Destroy( mbk->but_list );
    mbk->but_list = 0;
  }
  return 0;
}


void CancelButtonList( BOOK* book, GUI* )
{
  BookObj_ReturnPage( book, CANCEL_EVENT );
}


int DeleteShortcut( MyBOOK* mbk, char* mask_buf, int f )
{
  char* pos;
  char* param = 0;
  int res = 0;
  
  if ( mbk->shortcuts_buf )
  {
    if ( param = manifest_GetParam( mbk->shortcuts_buf, mask_buf, 0 ) )
    {
      int len_minus = strlen( param );
      if ( len_minus > 0 )
      {
        pos = strstr( mbk->shortcuts_buf, mask_buf );
        int len_prefix = strlen(mask_buf)+1;
        fwrite( f, mbk->shortcuts_buf, pos - mbk->shortcuts_buf + len_prefix );
        fwrite( f, pos + len_prefix + len_minus, mbk->shortcuts_buf_size - ( pos - mbk->shortcuts_buf + len_prefix ) - len_minus );
        res = 1;
      }
    }
    if ( param ) mfree( param );
  }
  return res;
}

void But_onDelete( BOOK* book, GUI* )
{
  MyBOOK* mbk = (MyBOOK*) book;
  int f;
  int res;
  wchar_t* path = get_path();
  if ( ( f = _fopen( path, L"shortcuts.ini", 0x204, 0x180, 0 ) ) >= 0 )
  {
    char mask_buf[10];
    int dig_num = ListMenu_GetSelectedItem( mbk->but_list );
    
    if ( !ListMenu_GetSelectedItem( mbk->mode_list ) )
    {
      if ( !mbk->ActiveTAB )
        sprintf( mask_buf, "[S_KEY%d]", dig_num );
      else
        sprintf( mask_buf, "[ES_KEY%d]", dig_num );
      
      res = DeleteShortcut( mbk, mask_buf, f );
    }
    else
    {
      if ( !mbk->ActiveTAB )
        sprintf( mask_buf, "[L_KEY%d]", dig_num );
      else
        sprintf( mask_buf, "[EL_KEY%d]", dig_num );
      
      res = DeleteShortcut( mbk, mask_buf, f );
    }
    
    fclose( f );
    if ( res )
      CreateButtonList( 0, book );
  }
  else
  {
    MessageBox( 0x6fFFFFFF, STR( "Can't open shortcuts.ini" ), 0, 1 , 5000, 0 );
  }
  delete( path );
}




int CreateButtonList( void* data, BOOK* book )
{
  MyBOOK* mbk = (MyBOOK*) book;
  int str_id;
  char* sp;
  mbk->shortcuts_buf_size = get_file( L"shortcuts.ini", &sp );
  
  if ( mbk->shortcuts_buf )
    delete mbk->shortcuts_buf;
  
  mbk->shortcuts_buf = sp;
  int but_pos = 0;
  
  if ( mbk->but_list )
  {
    but_pos = ListMenu_GetSelectedItem( mbk->but_list );
    GUIObject_Destroy( mbk->but_list );
  }
  
  mbk->but_list = CreateListMenu( book, 0 );
  GUIObject_SetTitleText( mbk->but_list, STR( "Buttons" ) );
  ListMenu_SetItemCount( mbk->but_list, 10 );
  ListMenu_SetOnMessage( mbk->but_list, but_list_callback );
  ListMenu_SetCursorToItem( mbk->but_list, but_pos );
  GUIObject_SoftKeys_SetAction( mbk->but_list, ACTION_BACK, DestroyButList );
  GUIObject_SoftKeys_SetAction( mbk->but_list, ACTION_LONG_BACK, CancelButtonList );
  GUIObject_SoftKeys_SetAction( mbk->but_list, ACTION_SELECT1, But_onEnter );
  GUIObject_SoftKeys_SetAction( mbk->but_list, ACTION_DELETE, But_onDelete );
  GUIObject_SoftKeys_SetVisible( mbk->but_list, ACTION_DELETE, 0 );
  
  if ( !mbk->ActiveTAB )
  {
    GUIObject_SoftKeys_SetAction( mbk->but_list, 0, But_SetMM );
    textidname2id( L"SHC_SET_MAINMENU_TXT", SID_ANY_LEN, &str_id );
    GUIObject_SoftKeys_SetText( mbk->but_list, 0, str_id );
    GUIObject_SoftKeys_SetAction( mbk->but_list, 1, But_SetJava );
    GUIObject_SoftKeys_SetText( mbk->but_list, 1, STR( "Java" ) );
    /*
    GUIObject_SoftKeys_SetAction( but_list, 2, But_EditShortcut );
    textidname2id( L"CALE_EDIT_EVENT_TXT", SID_ANY_LEN, &str_id );
    GUIObject_SoftKeys_SetText( but_list, 2, str_id );
    */
  }
  
  GUIObject_Show( mbk->but_list );
  return 0;
}


int ButtonList_Cancel_Event_Action( void* data, BOOK* book )
{
  BookObj_ReturnPage( book, CANCEL_EVENT );
  return(1);
}


const PAGE_MSG ChangeShortcuts_Buttons_PageEvents[]@ "DYN_PAGE" = {
  PAGE_ENTER_EVENT_TAG, CreateButtonList,
  ACCEPT_EVENT_TAG, CreateButtonList,
  CANCEL_EVENT_TAG, ButtonList_Cancel_Event_Action,
  PAGE_EXIT_EVENT_TAG, ExitButtonList,
  NIL_EVENT_TAG, 0
};

PAGE_DESC ChangeShortcuts_Buttons_page = { "BookManager_ChangeShortcuts_Buttons_Page", 0, ChangeShortcuts_Buttons_PageEvents };


void ModeList_Cancel_Action( BOOK* book, GUI* )
{
  BookObj_ReturnPage( book, ACCEPT_EVENT );
}


void ModeList_Prev_Action( BOOK* book, GUI* )
{
  BookObj_ReturnPage( book, ACCEPT_EVENT );
}


void onEnter_ModeList( BOOK* book, GUI* )
{
  BookObj_CallPage( book, &ChangeShortcuts_Buttons_page );
}


int list_callback( GUI_MESSAGE* msg )
{
  switch( GUIonMessage_GetMsg( msg ) )
  {
  case LISTMSG_GetItem:
    if ( GUIonMessage_GetCreatedItemIndex( msg ) )
    {
      GUIonMessage_SetMenuItemText( msg, STR( "Long" ) );
    }
    else
    {
      GUIonMessage_SetMenuItemText( msg, STR( "Short" ) );
    }
  }
  return 1;
}


int CreateModeList( void* data, BOOK* book )
{
  MyBOOK* mbk = (MyBOOK*) book;
  int mode_list_pos = 0;
  if ( mbk->mode_list )
  {
    mode_list_pos = ListMenu_GetSelectedItem( mbk->mode_list );
    GUIObject_Destroy( mbk->mode_list );
  }
  mbk->mode_list = CreateListMenu( book, 0 );
  GUIObject_SetTitleText( mbk->mode_list, STR( "Press Mode" ) );
  ListMenu_SetItemCount( mbk->mode_list, 2 );
  ListMenu_SetOnMessage( mbk->mode_list, list_callback );
  ListMenu_SetCursorToItem( mbk->mode_list, mode_list_pos );
  GUIObject_SoftKeys_SetAction( mbk->mode_list, ACTION_BACK, ModeList_Prev_Action );
  GUIObject_SoftKeys_SetAction( mbk->mode_list, ACTION_LONG_BACK, ModeList_Cancel_Action );
  GUIObject_SoftKeys_SetAction( mbk->mode_list, ACTION_SELECT1, onEnter_ModeList );
  GUIObject_Show( mbk->mode_list );
  return 0;
}


int ModeList_CancelEvent_Action( void* data, BOOK* bk )
{
  BookObj_ReturnPage( bk, ACCEPT_EVENT );
  return(0);
}

int ExitShortcutsSet( void* data, BOOK* bk )
{
  MyBOOK* mbk = (MyBOOK*) bk;
  if ( mbk->mode_list )
  {
    GUIObject_Destroy( mbk->mode_list );
    mbk->mode_list = 0;
  }
  if ( mbk->but_list )
  {
    GUIObject_Destroy( mbk->but_list );
    mbk->but_list = 0;
  }
  if ( mbk->MainMenuID!= - 1 )
  {
    BOOK* MainMenu = FindBookByID( mbk->MainMenuID );
    if ( MainMenu ) FreeBook( MainMenu );
    mbk->MainMenuID = - 1;
  }
  return 0;
}


const PAGE_MSG ChangeShortcuts_PageEvents[]@ "DYN_PAGE" = {
  PAGE_ENTER_EVENT_TAG, CreateModeList,
  CANCEL_EVENT_TAG, ModeList_CancelEvent_Action,
  PAGE_EXIT_EVENT_TAG, ExitShortcutsSet,
  NIL_EVENT_TAG, 0
};

PAGE_DESC ChangeShortcuts_page = { "BookManager_ChangeShortcuts_Main_Page", 0, ChangeShortcuts_PageEvents };


void Shortcuts( BOOK* book, GUI* )
{
  MyBOOK * mbk = (MyBOOK*)book;
  mbk->ActiveTAB=GetActiveTab(mbk);
  BookObj_CallPage( book, &ChangeShortcuts_page );
}
