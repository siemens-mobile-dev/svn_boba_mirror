#include "..\include\Lib_Clara.h"
#include "..\include\Dir.h"
#include "..\include\var_arg.h"
#include "conf_loader.h"
#include "config_data.h"
#include "bookman_daemon.h"
#include "shortcuts.h"
#include "main.h"
#include "book_names.h"

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



wchar_t* id_names[ICONS_COUNT] =
{
	IDN_BOOKS_DEACT_ICON,
	IDN_BOOKS_ACT_ICON,
	IDN_ELFS_DEACT_ICON,
	IDN_ELFS_ACT_ICON
};


const wchar_t* img_names[ICONS_COUNT] =
{
	tab1_inact,
	tab1_act,
	tab2_inact,
	tab2_act
};


const PAGE_MSG bk_main_msglst[] @ "DYN_PAGE" =
{
	PAGE_ENTER_EVENT_TAG, CreateBookList,
	ACCEPT_EVENT_TAG, RecreateBookList,
	NIL_EVENT_TAG, 0
};

const PAGE_DESC BookManager_Main_Page = { "BookManager_Main_Page", 0, bk_main_msglst };


const PAGE_MSG bk_base_msglst[] @ "DYN_PAGE" =
{
	BOOK_DESTROYED_EVENT_TAG, onRootListChanged,
	USER_INACTIVITY_EVENT_TAG, onUserInactivity,
	NIL_EVENT_TAG, 0
};

const PAGE_DESC BookManager_Base_Page = { "BookManager_Base_Page", 0, bk_base_msglst };


void elf_exit( void )
{
	kill_data( &ELF_BEGIN, ( void(*)(void*) )mfree_adr() );
}

// =====================================================================================

int GetActiveTab( MyBOOK* mbk )
{
	return TabMenuBar_GetFocusedTab( mbk->gui );
}

int isBookManager( BOOK* struc )
{
	return struc->onClose == onMyBookClose;
}

void win12512unicode( wchar_t* ws, char* s, int len )
{
	int c;
	
	while( ( c = *s++ ) && len-- > 0 )
	{
		if ( c == 0xA8 )
			c = 0x401;
		if ( c == 0xAA )
			c = 0x404;
		if ( c == 0xAF )
			c = 0x407;
		if ( c == 0xB8 )
			c = 0x451;
		if ( c == 0xBA )
			c = 0x454;
		if ( c == 0xBF )
			c = 0x457;
		if ( c >= 0xC0 && c < 0x100 )
			c += 0x350;
		*ws++ = c;
	}
	*ws = 0;
}

char* unicode2win1251( char* s, wchar_t* ws, int len )
{
	char* d = s;
	int c;
	
	while( ( c = *ws++ ) && len-- > 0 )
	{
		if ( c == 0x401 )
			c = 0xA8;
		if ( c == 0x404 )
			c = 0xAA;
		if ( c == 0x407 )
			c = 0xAF;
		if ( c == 0x451 )
			c = 0xB8;
		if ( c == 0x454 )
			c = 0xBA;
		if ( c == 0x457 )
			c = 0xBF;
		if ( c >= 0x410 && c < 0x450 )
			c -= 0x350;
		*s++ = c;
	}
	
	*s = 0;
	
	return d;
}

int w1251toUNICODE ( u16* wstr )
{
	u16* ws = wstr;
	u16 c;
	
	while( *ws )
	{
		c = *ws;
		if ( c == 0xA8 )
			c = 0x401;
		if ( c == 0xAA )
			c = 0x404;
		if ( c == 0xAF )
			c = 0x407;
		if ( c == 0xB8 )
			c = 0x451;
		if ( c == 0xBA )
			c = 0x454;
		if ( c == 0xBF )
			c = 0x457;
		if ( c >= 0xC0 && c < 0x100 )
			c += 0x350;
		*ws++ = c;
	}
	return Str2ID( wstr, 0, SID_ANY_LEN );
};

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

// взять значение из ини-файла
int GetParam( char* name )
{
	wchar_t ws[50];
	int sID = 0;
	
	MyBOOK* myBook = (MyBOOK*) FindBook( isBookManager );
	
	if ( myBook->ini_buf )
	{
		char* param = manifest_GetParam( myBook->ini_buf, name, 0 );
		if ( param )
		{
			win12512unicode( ws, param, MAXELEMS(ws)-1 );
			sID = Str2ID( ws, 0, SID_ANY_LEN );
			mfree( param );
			return sID;
		}
	}
	
	if( !sID )
	{
		sID = Str2ID( name, 6, strlen(name) );
	}
	return sID;
}

// получить имя жавы
int GetJavaName( BOOK* bk )
{
	wchar_t ws[100];
	
	TextID2wstr( BookObj_GetSession( bk )->name, ws, MAXELEMS(ws) );
	
	if ( !wstrncmp( ws, L"Foreign app", 11 ) )
	{
		return JavaSession_GetName();
	}
	
	if ( !wstrcmp( ws, L"Java" ) )
	{
		return JavaSession_GetName();
	}
	
	return 0;
}

int isRSSTickerBook( BOOK* book )
{
	return 0 == strcmp( book->xbook->name, "RSSTicker_Book" );
}

// создаём список книг
void CreateBookLst( MyBOOK* myBook )
{
	int i, j, k, fgui;
	int books_cnt=0;
	int elfs_cnt=0;
	int mask = ( (int)LastExtDB() )&0xF8000000;
	BOOK* book;
	UI_APP_SESSION* session;
	BOOK_LIST_ITEM* elem;
	
	SessoinListsFree( myBook );
	
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
			if (!isBookmanDaemonBook(book))
			{
				if( (fgui = book->xguilist->guilist->FirstFree) || ( ((int)book->onClose)&0xF8000000 ) != mask )
				{
					if ( !isBookManager( book ) && !isVolumeControllerBook( book ) && !isRightNowBook( book ) && !isRSSTickerBook( book ))
					{
						char s[50];
						elem = new BOOK_LIST_ITEM;
						elem->book=book;
						if ( strcmp( book->xbook->name, "CUIDisplayableBook" ) )
						{
							char * bn =new char[strlen(book->xbook->name)+1];
							strcpy(bn,book->xbook->name);
							elem->book_name = bn;
						}
						else
						{
							StrID2Str( BookObj_GetSession(book)->name, s, MAXELEMS(s), 0 );
							char * bn =new char[strlen(s)+1];
							strcpy(bn,s);
							elem->book_name = bn;
						}
						elem->isGuiBook = fgui;
						
						int tmp = GetJavaName( book );
						if ( tmp )
						{
							delete(elem->book_name);
							StrID2Str( tmp, s, MAXELEMS(s), 0 );
							char * java_name =new char[strlen(s)+1];
							strcpy(java_name,s);
							elem->book_name = java_name;
						}
						if ( ElfInBookListEnabled && ( ((int)book->onClose)&0xF8000000 ) != mask )
						{
							if ( fgui )
							{
								List_InsertFirst( myBook->books_list, elem );
								books_cnt++;
							}
						}
						
						if ( ( ((int)book->onClose)&0xF8000000 ) == mask )
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
	}
	myBook->blistcnt = books_cnt;
	myBook->elistcnt = elfs_cnt;
}


void SessoinListsFree( MyBOOK* myBook )
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
	}
}


// при убийстве какой либо книги
int onRootListChanged( void* r0, BOOK* bk )
{
	MyBOOK* mbk = (MyBOOK*) bk;
	if ( !mbk->mode_list && !mbk->but_list )
	{
		// создали меню
		mbk->ActiveTAB=GetActiveTab(mbk);
		CreateMenu( mbk->ActiveTAB, bk );
	}
	return 0;
};

// если юзер заснул...
int onUserInactivity( void* r0, BOOK* bk )
{
	if ( UserInactivityEventEnabled )
		CloseMyBook( bk, 0 );
	return 0;
};

// устанавливаем тексты в пунктах меню
int onLBMessage( GUI_MESSAGE* msg )
{
	MyBOOK* myBook = (MyBOOK*) GUIonMessage_GetBook( msg );
	int item;
	BOOK_LIST_ITEM* elem;
	
	switch( GUIonMessage_GetMsg( msg ) )
	{
	case LISTMSG_GetItem:
		item = GUIonMessage_GetCreatedItemIndex( msg );
		elem = (BOOK_LIST_ITEM*) List_Get( myBook->books_list, item );
		GUIonMessage_SetMenuItemText( msg, GetParam(elem->book_name) );
		break;
	}
	
	return 1;
};


int onLBMessage1( GUI_MESSAGE* msg )
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
			GUIonMessage_SetMenuItemText( msg, GetParam(elem->book_name) );
			
			if ( !elem->isGuiBook )
			{
				GUIonMessage_SetItemDisabled( msg, 1 );
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
};

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

// при выборе пункта
void onEnterPressed( BOOK* book, GUI* lt )
{
	BookObj_SetFocus( GetBook( BOOKLIST, book ), 0 );
	CloseMyBook( book, 0 );
};

// при выборе эльфа
void onEnterPressed1( BOOK* book, GUI* lt )
{
	BOOK* bk = GetBook( ELFLIST, book );
	if ( bk )
	{
		BookObj_SetFocus( bk, 0 );
		CloseMyBook( book, 0 );
	}
};

// при нажатии "С"
void onDelPressed( BOOK* book, void* lt )
{
	BOOK* bk = GetBook( BOOKLIST, book );
	if ( bk )
	{
		if ( GetJavaName( bk ) )
		{
			JavaSession_Manager( 0x0E );
		}
		else
		{
			if ( Find_StandbyBook() != bk )
			{
				UI_Event_toBookID( RETURN_TO_STANDBY_EVENT, BookObj_GetBookID( bk ) );
				UI_Event_toBookID( TERMINATE_SESSION_EVENT, BookObj_GetBookID( bk ) );
			}
		}
	}
};

// при нажатии "С" на эльфе
void onDelPressed1( BOOK* book, void* lt )
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
				MessageBox(EMPTY_SID,STR("TerminateEvent not supported by elf..."),NOIMAGE,1,3000,book);
			}
		}
	}
};

typedef struct
{
	BOOK* book;
}MSG;

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
			MessageBox(EMPTY_SID,STR("Author unknown"),NOIMAGE,1,3000,book);
		}
	}
};

void Copyright( BOOK* book, GUI* lt )
{
	MyBOOK* mbk = (MyBOOK*) book;
	mbk->blistpos = ListMenu_GetSelectedItem( mbk->blist );
	MessageBox(EMPTY_SID,COPYRIGHT_STRING,NOIMAGE,1,3000,book);
};

// патченый onKey от своей менюхи...; )
void myOnKey( DISP_OBJ* p, int keyID, int i2, int i3, int press_mode )
{
	MyBOOK* mbk = (MyBOOK*) FindBook( isBookManager );
	
	mbk->oldOnKey( p, keyID, i2, i3, press_mode );
	
	if ( press_mode == KBD_SHORT_RELEASE || press_mode == KBD_LONG_PRESS )
	{
		if ( keyID == KEY_DEL )
		{
			onDelPressed( mbk, 0 );
		}
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
					int id_len = strlen( strstr( param, "ID=" )+3 );
					wchar_t buf[20];
					str2wstr( buf, strstr( param, "ID=" )+3 );
					int ID;
					char sp_flag;
					wtoi( buf, id_len, &ID );
					REQUEST_UI_OAF_START_APPLICATION( ASYNC, ID+0x10000, &sp_flag );
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
};

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
void myOnKey1( DISP_OBJ* p, int keyID, int i2, int i3, int press_mode )
{
	MyBOOK* mbk = (MyBOOK*) FindBook( isBookManager );
	
	mbk->oldOnKey1( p, keyID, i2, i3, press_mode );
	
	if ( press_mode == KBD_SHORT_RELEASE || press_mode == KBD_LONG_PRESS )
	{
		if ( keyID == KEY_DEL )
		{
			onDelPressed1( mbk, 0 );
		}
		if ( keyID == KEY_DIEZ )
		{
			if ( press_mode == KBD_SHORT_RELEASE )
			{
				BOOK* stby_bk = Find_StandbyBook();
				if ( stby_bk )
					BookObj_SetFocus( stby_bk, 0 );
			}
			/*
			else
			{
			UI_Event( RETURN_TO_STANDBY_EVENT );
		}
			*/
			CloseMyBook( mbk, 0 );
			
		}
		
		if ( mbk->shortcuts_buf )
		{
			char key[20];
			sprintf( key, press_mode == 3 ? "[ES_KEY%d]" : "[EL_KEY%d]", keyID - KEY_DIGITAL_0 );
			
			char* param = manifest_GetParam( mbk->shortcuts_buf, key, 0 );
			if ( param )
			{
				if( StartElf( GetDir( DIR_ELFS | MEM_INTERNAL ), param ) )
					if ( StartElf( GetDir( DIR_ELFS | MEM_EXTERNAL ), param ) )
					{
						int ms[3];
						ms[0] = STR( "ZBin" );
						ms[1] = Str2ID( param, 1, strlen( param ) );
						ms[2] = STR( "\n\nnot found.." );
						MessageBox( EMPTY_SID,Str2ID(ms,5,3),NOIMAGE,1,3000,FindBook(isBookManager));
					}
				
				mfree( param );
				CloseMyBook( mbk, 0 );
			}
		}
	}
};

/*
void onTabSwitch( BOOK* bk, int active_tab )
{
ActiveTab = active_tab;
}
*/

void RefreshElfSoftkeys( MyBOOK* mbk, int item )
{
	if ( mbk->elfs_list->FirstFree )
	{
		BOOK_LIST_ITEM* elem = (BOOK_LIST_ITEM*) List_Get( mbk->elfs_list, item );
		if ( !elem->isGuiBook )
		{
			GUIObject_SoftKeys_SetVisible( mbk->elist, ACTION_SELECT1, 0 );
		}
		else
		{
			GUIObject_SoftKeys_SetVisible( mbk->elist, ACTION_SELECT1, 1 );
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

// создание меню
void CreateGuiList( int tab_pos, BOOK* bk )
{
	MyBOOK* mbk = (MyBOOK*) bk;
	int str_id;
	int p[2];
	int list_pos;
	
	CreateBookLst( mbk );
	p[0] = Str2ID ( L"Heap : ", 0, 7 );
	p[1] = int2strID ( GetFreeBytesOnHeap() );
	
	if ( !mbk->gui )
	{
		mbk->gui = CreateTabMenuBar( bk );
		TabMenuBar_SetTabCount( mbk->gui, 2 );
		
		TabMenuBar_SetTabIcon( mbk->gui, 0, mbk->tabs_image[0].ImageID, 0 );
		TabMenuBar_SetTabIcon( mbk->gui, 0, mbk->tabs_image[1].ImageID, 1 );
		
		TabMenuBar_SetTabIcon( mbk->gui, 1, mbk->tabs_image[2].ImageID, 0 );
		TabMenuBar_SetTabIcon( mbk->gui, 1, mbk->tabs_image[3].ImageID, 1 );
		
		TabMenuBar_SetTabTitle( mbk->gui, 0, Str2ID( p, 5, 2 ) );
		TabMenuBar_SetTabTitle( mbk->gui, 1, STR( "Elfs" ) );
	}
	
	if ( mbk->blist )
	{
		list_pos = ListMenu_GetSelectedItem(mbk->blist);
		ListMenu_DestroyItems(mbk->blist);
		ListMenu_SetItemCount( mbk->blist, mbk->blistcnt );
		ListMenu_SetCursorToItem( mbk->blist, list_pos );
		GUIObject_SoftKeys_SetVisible(mbk->blist, ACTION_SELECT1, 1);
	}
	else
	{
		mbk->blist = CreateListMenu( bk, 0 );
		ListMenu_SetOnMessage( mbk->blist, onLBMessage );
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
		GUIObject_SoftKeys_SetAction( mbk->blist, ACTION_SELECT1, onEnterPressed );
		
		GUIObject_SoftKeys_SetAction( mbk->blist, 0, Shortcuts );
		textidname2id( L"SHC_EDIT_SHORTCUT_TXT", SID_ANY_LEN, &str_id );
		GUIObject_SoftKeys_SetText( mbk->blist, 0, str_id );
		GUIObject_SoftKeys_SetAction( mbk->blist, 1, BookNames );
		textidname2id( L"DB_RENAME_TXT", SID_ANY_LEN, &str_id );
		GUIObject_SoftKeys_SetText( mbk->blist, 1, str_id );
		GUIObject_SoftKeys_SetAction( mbk->blist, 2, Copyright );
		GUIObject_SoftKeys_SetText( mbk->blist, 2, STR( "About" ) );
		
		mbk->oldOnKey = DispObject_GetOnKey( GUIObject_GetDispObject( mbk->blist ) );
		
		DISP_DESC_SetOnKey( DispObject_GetDESC ( GUIObject_GetDispObject( mbk->blist ) ), myOnKey );
		
		TabMenuBar_SetTabGui( mbk->gui, 0, mbk->blist );
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
		mbk->elist = CreateListMenu( bk, 0 );
		ListMenu_SetCursorToItem( mbk->elist, 0 );
		ListMenu_SetOnMessage( mbk->elist, onLBMessage1 );
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
		GUIObject_SoftKeys_SetAction( mbk->elist, 0, Shortcuts );
		textidname2id( L"SHC_EDIT_SHORTCUT_TXT", SID_ANY_LEN, &str_id );
		GUIObject_SoftKeys_SetText( mbk->elist, 0, str_id );
		GUIObject_SoftKeys_SetAction( mbk->elist, ACTION_SELECT1, onEnterPressed1 );
		GUIObject_SoftKeys_SetAction( mbk->elist, 1, BookNames );
		textidname2id( L"DB_RENAME_TXT", SID_ANY_LEN, &str_id );
		GUIObject_SoftKeys_SetText( mbk->elist, 1, str_id );
		GUIObject_SoftKeys_SetAction( mbk->elist, 2, Author );
		GUIObject_SoftKeys_SetText( mbk->elist, 2, STR( "Author" ) );
		
		mbk->oldOnKey1 = DispObject_GetOnKey( GUIObject_GetDispObject( mbk->elist ) );
		
		DISP_DESC_SetOnKey( DispObject_GetDESC ( GUIObject_GetDispObject( mbk->elist ) ), myOnKey1 );
		
		RefreshElfSoftkeys( mbk, ListMenu_GetSelectedItem(mbk->elist) );
		
		TabMenuBar_SetTabGui( mbk->gui, 1, mbk->elist );
	}
	
	TabMenuBar_SetFocusedTab( mbk->gui, tab_pos );
	
	GUIObject_Show(mbk->gui);
};

void LoadIniFiles(MyBOOK * mbk)
{
	if ( mbk->shortcuts_buf )
	{
		delete (mbk->shortcuts_buf);
		mbk->shortcuts_buf=0;
	}
	if ( mbk->ini_buf )
	{
		delete (mbk->ini_buf);
		mbk->ini_buf=0;
	}
	char* sp;
	mbk->ini_buf_size = get_file( L"bookman.ini", &sp );
	mbk->ini_buf = sp;
	mbk->shortcuts_buf_size = get_file( L"shortcuts.ini", &sp );
	mbk->shortcuts_buf = sp;
}

// создание и отображение меню
int CreateMenu( int tab_pos, BOOK* bk )
{
	MyBOOK* mbk = (MyBOOK*) bk;
	
	LoadIniFiles(mbk);
	
	CreateGuiList( tab_pos, bk );
	
	return 0;
}

int h2i( char* h )
{
	char c;
	int res = 0;
	
	while( *h++ != 'x' );
	
	do
	{
		c = *h++;
		if ( c > 0x60 )
			c -= 0x20;
		else
			c -= 0x30;
		
		if ( c > 9 )
			c -= 7;
		
		res <<= 4;
		res |= c;
	} while( *h != ';' );
	
	return res;
}


// при закрытии книги
void onMyBookClose( BOOK* book )
{
	MyBOOK* myBook = (MyBOOK*) book;
	DaemonBook * dbk = (DaemonBook*)FindBook(isBookmanDaemonBook);
	dbk->ActiveTAB = GetActiveTab( myBook );
	// выгрузили файло
	if ( myBook->ini_buf )
	{
		mfree( myBook->ini_buf );
		myBook->ini_buf = 0;
	}
	if ( myBook->java_list_menu )
	{
		GUIObject_Destroy( myBook->java_list_menu );
		myBook->java_list_menu = 0;
	}
	if ( myBook->java_list )
	{
		List_DestroyElements( myBook->java_list, elem_filter, elem_free );
		List_Destroy( myBook->java_list );
		myBook->java_list = 0;
	}
	if ( myBook->mode_list )
	{
		GUIObject_Destroy( myBook->mode_list );
		myBook->mode_list = 0;
	}
	if ( myBook->but_list )
	{
		GUIObject_Destroy( myBook->but_list );
		myBook->but_list = 0;
	}
	/* if ( str_inp )
	{
	GUIObject_Destroy( str_inp );
	str_inp = 0;
}*/
	if ( myBook->shortcuts_buf )
	{
		delete( myBook->shortcuts_buf );
		myBook->shortcuts_buf = 0;
	}
	if ( myBook->MainMenuID != -1 )
	{
		BOOK* MainMenu = FindBookByID( myBook->MainMenuID );
		
		if ( MainMenu )
			FreeBook( MainMenu );
		
		myBook->MainMenuID = -1;
	}
	// освободили списки книг/сессий
	SessoinListsFree( myBook );
	
	//убили гуи
	if ( myBook->gui ) GUIObject_Destroy( myBook->gui );
	
	// выгрузили иконки
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


void ReturnMyBook( BOOK* bk, GUI* )
{
	MyBOOK* mbk = (MyBOOK*) bk;
	GUIObject_Destroy( mbk->YesNoQuestion );
	mbk->YesNoQuestion = 0;
	mbk->ActiveTAB=GetActiveTab(mbk);
	CreateMenu( mbk->ActiveTAB, bk );
};


void CloseMyBook( BOOK* Book, GUI* )
{
	FreeBook( Book );
};


void TerminateManager( BOOK* Book, GUI* )
{
	MessageBox( EMPTY_SID,STR("BookManager\n\nterminated"),NOIMAGE,1,3000,0);
	FreeBook( Book );
	DestroyDaemon();
	ModifyKeyHook( NewKey, 0 );
	SUBPROC( elf_exit );
};


void PreTerminateManager( BOOK* bk, GUI* )
{
	MyBOOK* mbk = (MyBOOK*) bk;
	//  int pre_quest;
	//  int quest;
	//  textidname2id(L"MSG_UI_EXIT_EDITOR_SAVE_CHANGESQ_TXT",SID_ANY_LEN,&pre_quest);
	//  textidname2id(L"CONTINUE_TXT",SID_ANY_LEN,&pre_quest);
	mbk->YesNoQuestion = CreateYesNoQuestionVA( 0,
											   VAR_BOOK( mbk ),
											   VAR_YESNO_PRE_QUESTION( Str2ID( "Exit command selected", 6, SID_ANY_LEN ) ),
											   VAR_YESNO_QUESTION( Str2ID( "Unload?", 6, SID_ANY_LEN ) ),
											   0 );
	GUIObject_SoftKeys_SetAction( mbk->YesNoQuestion, ACTION_YES, TerminateManager );
	GUIObject_SoftKeys_SetAction( mbk->YesNoQuestion, ACTION_NO, CloseMyBook );
	GUIObject_SoftKeys_SetAction( mbk->YesNoQuestion, ACTION_BACK, ReturnMyBook );
}


int get_file( wchar_t* name, char** buf_set )
{
	int size = 0;
	int file;
	char* buf = 0;
	FSTAT _fstat;
	
	wchar_t* path = get_path();
	
	if ( fstat( path, name, &_fstat ) == 0 )
	{
		if ( ( file = _fopen( path, name, 0x1, 0x180, 0 ) ) >= 0 )
		{
			buf = (char*) malloc( _fstat.fsize+1 );
			fread( file, buf, _fstat.fsize );
			fclose( file );
			size = _fstat.fsize;
		}
	}
	
	buf_set[0] = buf;
	delete( path );
	return size;
}

// берем значения из ини-файла

void get_iconsID( MyBOOK* mbk )
{
	int i;
	wchar_t imgID;
	int _imgID;
	wchar_t wstr_path[100];
	
	for ( i = 0;i < ICONS_COUNT;i++ )
	{
		iconidname2id(id_names[i],SID_ANY_LEN,&_imgID);
		mbk->tabs_image[i].ImageID = _imgID;
	}
	for ( i = 0;i < DIGITS_COUNT;i++ )
	{
		iconidname2id(id_digits[i],SID_ANY_LEN,&_imgID);
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


int RecreateBookList( void* r0, BOOK* bk )
{
	MyBOOK* mbk = (MyBOOK*) bk;
	CreateMenu( mbk->ActiveTAB, bk );
	return(0);
}

// собственно старт
__root int CreateBookList( void* r0, BOOK* bk )
{
	MyBOOK* mbk = (MyBOOK*) bk;
	
	get_iconsID( mbk );
	// str_inp = 0;
	int tab_pos;
	
	if ( !FirstTab )
	{
		DaemonBook * dbk = (DaemonBook*)FindBook(isBookmanDaemonBook);
		tab_pos = dbk->ActiveTAB;
	}
	else
		tab_pos = FirstTab-1;
	
	CreateMenu( tab_pos, bk );
	return 0;
}


int NewKey( int key, int r1, int mode )
{
	BOOK* bk = FindBook( isBookManager );
	MyBOOK* mbk = (MyBOOK*) bk;
	if ( mode == KeyPressMode )
	{
		if ( key == KeyActiv )
		{
			if ( isKeylocked() && !Ignore_KeyLock )
				return 0;
			
			if ( !bk )
			{
				MyBOOK* BookManager_Book = (MyBOOK*) malloc( sizeof( MyBOOK ) );
				memset( BookManager_Book, 0, sizeof( MyBOOK ) );
				CreateBook( BookManager_Book, onMyBookClose, &BookManager_Base_Page, "BookManager", -1, 0 );
				MyBOOK* mbk = (MyBOOK*) BookManager_Book;
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
				int platform=GetChipID();
				platform=platform&0xFF;
				if (platform==PLATFORM_DB3150_1||platform==PLATFORM_DB3150_2||platform==PLATFORM_DB3200_1||platform==PLATFORM_DB3200_2||platform==PLATFORM_DB3210_1||platform==PLATFORM_DB3210_2||platform==PLATFORM_DB3350) mbk->isA2 = 1;
				BookObj_GotoPage( BookManager_Book, &BookManager_Main_Page );
				return -1;
			}
			else
			{
				if ( !mbk->mode_list && !mbk->YesNoQuestion && !mbk->StringInput )
				{
					CloseMyBook( bk, 0 );
					return 0;
				}
				return -1;
			}
		}
	}
	return 0;
}


int main ( void )
{
	if ( !FindBook( isBookmanDaemonBook ) )
	{
		CreateDaemon();
		InitConfig();
		ModifyKeyHook( NewKey, 1 );
	}
	else
	{
		MessageBox( EMPTY_SID, STR( "BookManager is already runed" ), NOIMAGE, 1, 5000, 0 );
		SUBPROC( elf_exit );
	}
	return 0;
}
