#ifndef _MAIN_H_
#define _MAIN_H_

#define COPYRIGHT_STRING STR( "\nBookManager v3.1\nbuild 030310\nCopyright (c) 2007-2008\nHussein\n\nRespect\nIronMaster, KreN\n\n" )
#define ICONS_COUNT 4
#define BOOKLIST 0
#define ELFLIST 1
#define ICONS_COUNT 4
#define DIGITS_COUNT 10


#define IDN_BOOKS_DEACT_ICON _T( "RN_TAB_SESSION_MANAGER_DESELECTED_ICN" )
#define IDN_BOOKS_ACT_ICON _T( "RN_TAB_SESSION_MANAGER_SELECTED_ICN" )
#define IDN_ELFS_DEACT_ICON _T( "RN_TAB_BOOKMARKS_DESELECTED_ICN" )
#define IDN_ELFS_ACT_ICON _T( "RN_TAB_BOOKMARKS_SELECTED_ICN" )

#define MAX_BOOK_NAME_LEN 50

#define JAVA_BOOK_NAME "CUIDisplayableBook"

#define FLASH_MASK 0xF8000000


typedef struct
{
  wchar_t ImageID;
}IMG;

typedef struct _MYBOOK : BOOK
{
  GUI_TABMENUBAR* gui;
  wchar_t* ini_buf;
  int ini_buf_size;
  char* shortcuts_buf;
  int shortcuts_buf_size;
  DISP_OBJ_ONKEY_METHOD oldOnKey;
  DISP_OBJ_ONKEY_METHOD oldOnKey1;
  LIST* books_list;
  LIST* elfs_list;
  LIST* java_list;
  GUI_LIST* java_list_menu;
  GUI_LIST* blist;
  GUI_LIST* elist;
  GUI_LIST* mode_list;
  GUI_LIST* but_list;
  GUI* YesNoQuestion;
  GUI* StringInput;
  IMG tabs_image[ICONS_COUNT];
  IMG digs_image[DIGITS_COUNT];
  int MainMenuID;
  wchar_t blistpos;
  wchar_t elistpos;
  wchar_t blistcnt;
  wchar_t elistcnt;
  wchar_t ActiveTAB;
  char isA2;
}MyBOOK;


typedef struct
{
  BOOK* book;
  UI_APP_SESSION* session;
  int book_name;
  
}BOOK_ITEM;


typedef struct
{
  BOOK * book;
  char * book_name;
  int isGuiBook;
}BOOK_LIST_ITEM;


typedef struct
{
  int BookID;
}DESTROYBOOK_DATA;


typedef struct
{
  BOOK* book;
}MSG;


int CreateBookList( void* r0, BOOK* bk );
int RecreateBookList( void* r0, BOOK* bk );
int isBookManager( BOOK* struc );
int onUserInactivity( void* r0, BOOK* bk );
int onRootListChanged( void* r0, BOOK* bk );
void onMyBookClose( BOOK* );
void CloseMyBook( BOOK* Book, GUI* );
int CreateMenu( int r0, BOOK* bk );
void PreTerminateManager( BOOK* Book, GUI* );
int NewKey( int key, int r1, int mode, LPARAM, DISP_OBJ* );
void SessoinListsFree( MyBOOK* book );
int get_file( wchar_t* fname, void** buf_set );
void RefreshElfSoftkeys( MyBOOK* mbk, int item );
int GetActiveTab(MyBOOK*);
int GetBookNameStrID( char* name);
wchar_t * GetUserBookName(wchar_t * ini,wchar_t * orig_name,wchar_t * cur_name);
wchar_t* get_path();


#endif
