#ifdef NDEBUG

#include "..\include\Lib_Clara.h"

void* operator new(size_t sz){return malloc(sz);};
void* operator new[](size_t sz){return malloc(sz);};
void operator delete(void * p){mfree(p);};
void operator delete[](void * p){mfree(p);};

void* operator new(size_t size, void* p){ return p; }
void* operator new[](size_t size, void* p){ return p; }
void operator delete (void*, void*) { }
void operator delete[] (void*, void*) { }

#else

#include "mem2.h"
#include "..\include\dir.h"
#include "lib_clara_original.h"

int __deleaker_skip;

static char* leaktypes[]={
	"memory/book/gui/gc",
	"strid",
	"iconid",
	"timer",
	"file",
	"hook",
	"dll",
	"gc",
	"gvi",
	"gui",
	"book",
	"process",

	"unallocated"
};

static bool started=false;

static LIST* buffers[trace_typescount];

#define LISTDATACOUNT 3

static int iconid_start=0xE800;
static int iconid_end=0xE8FF;


void trace_init()
{
	for(int i=0;i<trace_typescount;i++)
		buffers[i]=__original_List_Create();
	started=true;
	__deleaker_skip=0;

	int chipid = GetChipID() & CHIPID_MASK;

	if( chipid == CHIPID_DB2000 || chipid == CHIPID_DB2010 )
	{
		int max=0;
		int iconid;
		
		if( iconidname2id( _T("RN_SHORTCUT_LIST_CALL_LIST_ICN"), SID_ANY_LEN, &iconid ) )
			if( max < iconid )
				max = iconid;
		if( iconidname2id( _T("ZERO_ROW_SEPARATOR_ICN"), SID_ANY_LEN, &iconid ) )
			if( max < iconid )
				max = iconid;
		if( iconidname2id( _T("WV_YAHOO_STATUSBAR_SYSTEMMSG_ICN"), SID_ANY_LEN, &iconid ) )
			if( max < iconid )
				max = iconid;

		if( max )
		{
			iconid_start = ( max + 0xFF ) & ~0xFF;
			iconid_end = 0xFFFE; //сомнительно
		}
	}
}

void trace_done()
{
	started=false;
	int f=-1;
	for(int memtype=0;memtype<trace_typescount;memtype++)
	{
		if(buffers[memtype]->FirstFree)
		{
			if(f==-1)
			{
				__original_w_chdir(GetDir(DIR_OTHER|MEM_INTERNAL));
				f=__original_w_fopen(L"memory.txt",WA_Write+WA_Create+WA_Truncate,0x1FF,0);
			}

			char tmp[256];

			__original_w_fwrite(f,tmp,__original_sprintf(tmp,"leak type \"%s\"\n",leaktypes[memtype]));

			for(int j=0;j<buffers[memtype]->FirstFree;j+=LISTDATACOUNT)
			{
				__original_w_fwrite(f,tmp,
									sprintf(tmp,"- %s:%d (%x)\n",
											__original_List_Get(buffers[memtype],j+1),//file
											__original_List_Get(buffers[memtype],j+2),//line
											__original_List_Get(buffers[memtype],j)//value
												)
										);
			}
		}
		__original_List_Destroy(buffers[memtype]);
	}
	if(f!=-1)
		__original_w_fclose(f);
}

void trace_alloc(int mt, void* ptr, const char* file, int line)
{
	if(__deleaker_skip==0)
	{
		__original_List_InsertLast(buffers[mt],ptr);
		__original_List_InsertLast(buffers[mt],(void*)file);
		__original_List_InsertLast(buffers[mt],(void*)line);
	}
	__deleaker_skip=0;
}

void* trace_alloc_ret(int mt, void* ptr, void* badvalue, const char* file, int line)
{
	if( ptr != badvalue )
		trace_alloc( mt, ptr, file, line);
	return ptr;
}


void trace_free(int mt,void* p, const char* file, int line)
{
	if(started)
	{
		bool found=false;
		for(int i=0;i<buffers[mt]->FirstFree;i+=LISTDATACOUNT)
		{
			if(__original_List_Get(buffers[mt],i)==p)
			{
				for(int j=0;j<LISTDATACOUNT;j++)
					__original_List_RemoveAt(buffers[mt],i);
				found=true;
				break;
			}
		}
		if(!found)
		{
			__original_List_InsertLast(buffers[trace_unallocated],(void*)p);
			__original_List_InsertLast(buffers[trace_unallocated],(void*)file);
			__original_List_InsertLast(buffers[trace_unallocated],(void*)line);
		}
	}
}


bool isallocatedstrid(int strid)
{
	return strid!=EMPTY_SID && (strid&0xFFFF0000)!=0;
}

bool isallocatediconid(int iconid)
{
	return iconid >= iconid_start && iconid <= iconid_end;
}

//---------------------------------------------------------------------------





void*  __deleaker_malloc( const char* __file__, int __line__, int size )
{
	void*  ret = __original_malloc(size);
	if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
	return ret;
}

void mfree_(void* p)
{
	mfree(p);
}

void*  __deleaker_mfree_adr( const char* __file__, int __line__ )
{
	return (void*)mfree_;
}

void  __deleaker_mfree( const char* __file__, int __line__, void* p )
{
	trace_free(trace_memory, p, __file__, __line__);
	__original_mfree(p);
}

int  __deleaker_CreateBook( const char* __file__, int __line__, BOOK* pbook, void (*onClose)( BOOK* ), const PAGE_DESC* bp, const char* name, int ParentBookID, APP_DESC* __unknwnargname6 )
{
	trace_free(trace_memory, pbook, __file__, __line__);
	trace_alloc(trace_book, pbook, __file__, __line__);
	return __original_CreateBook(pbook, onClose, bp, name, ParentBookID, __unknwnargname6);
}

void  __deleaker_FreeBook( const char* __file__, int __line__, BOOK* book )
{
	trace_free(trace_book, book, __file__, __line__);
	__original_FreeBook(book);
}

void  __deleaker_BookObj_KillBook( const char* __file__, int __line__, BOOK* book )
{
	trace_free(trace_book, book, __file__, __line__);
	__original_BookObj_KillBook(book);
}

LIST*  __deleaker_List_Create( const char* __file__, int __line__ )
{
	LIST*  ret = __original_List_Create();
	if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_List_Destroy( const char* __file__, int __line__, LIST* lst )
{
	trace_free(trace_memory, lst, __file__, __line__);
	__original_List_Destroy(lst);
}

char*  __deleaker_manifest_GetParam( const char* __file__, int __line__, const char* buf, const char* param_name, int unk )
{
	char*  ret = __original_manifest_GetParam(buf, param_name, unk);
	if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
	return ret;
}

DIR_HANDLE*  __deleaker_AllocDirHandle( const char* __file__, int __line__, const wchar_t* path )
{
	DIR_HANDLE*  ret = __original_AllocDirHandle(path);
	if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_DestroyDirHandle( const char* __file__, int __line__, DIR_HANDLE* handle )
{
	trace_free(trace_memory, handle, __file__, __line__);
	__original_DestroyDirHandle(handle);
}

int  __deleaker_GUIObject_Create( const char* __file__, int __line__, GUI* __unknwnargname1, void (*GuiDestroy)( GUI* ), void (*DispDescCreate)( DISP_DESC* ), BOOK* __unknwnargname4, void (*DispObjCallBack)( DISP_OBJ*, void* msg, GUI* ), int display, int size_of_gui )
{
	trace_free(trace_memory, __unknwnargname1, __file__, __line__);
	trace_alloc(trace_gui, __unknwnargname1, __file__, __line__);
	return __original_GUIObject_Create(__unknwnargname1, GuiDestroy, DispDescCreate, __unknwnargname4, DispObjCallBack, display, size_of_gui);
}

GUI*  __deleaker_GUIObject_Destroy( const char* __file__, int __line__, GUI* __unknwnargname1 )
{
	//освобождает или просто возвращает?
	trace_free(trace_gui, __unknwnargname1, __file__, __line__);
	return __original_GUIObject_Destroy(__unknwnargname1);
}

int  __deleaker__fopen( const char* __file__, int __line__, const wchar_t* filpath, const wchar_t* filname, unsigned int mode, unsigned int rights, unsigned int __0 )
{
	int  ret = __original__fopen(filpath, filname, mode, rights, __0);
	if(ret!=-1)trace_alloc(trace_file, (void*)ret, __file__, __line__);
	return ret;
}

int  __deleaker_fopen( const char* __file__, int __line__, const wchar_t* fname, int mode, int rights )
{
	int  ret = __original_fopen(fname, mode, rights);
	if(ret!=-1)trace_alloc(trace_file, (void*)ret, __file__, __line__);
	return ret;
}

int  __deleaker_fclose( const char* __file__, int __line__, int file )
{
	trace_free(trace_file, (void*)file, __file__, __line__);
	return __original_fclose(file);
}

int  __deleaker_w_fopen( const char* __file__, int __line__, const wchar_t* name, int attr, int rights, int err )
{
	//тот же trace_file?
	int  ret = __original_w_fopen(name, attr, rights, err);
	if(ret!=-1)trace_alloc(trace_file, (void*)ret, __file__, __line__);
	return ret;
}

int  __deleaker_w_fclose( const char* __file__, int __line__, int f )
{
	trace_free(trace_file, (void*)f, __file__, __line__);
	return __original_w_fclose(f);
}


void  __deleaker_TabMenuBar_SetTabGui( const char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab, GUI* __unknwnargname3 )
{
	trace_free(trace_gui, (void*)__unknwnargname3, __file__, __line__);
	return __original_TabMenuBar_SetTabGui(__unknwnargname1, tab, __unknwnargname3);
}

GUI_LIST*  __deleaker_CreateListMenu( const char* __file__, int __line__, BOOK* __unknwnargname1, int display )
{
	GUI_LIST*  ret = __original_CreateListMenu(__unknwnargname1, display);
	if(ret)trace_alloc(trace_gui, (void*)ret, __file__, __line__);
	return ret;
}

GUI_NOFMANY*  __deleaker_CreateNOfMany( const char* __file__, int __line__, BOOK* book )
{
	GUI_NOFMANY*  ret = __original_CreateNOfMany(book);
	if(ret)trace_alloc(trace_gui, (void*)ret, __file__, __line__);
	return ret;
}

GUI_ONEOFMANY*  __deleaker_CreateOneOfMany( const char* __file__, int __line__, BOOK* book )
{
	GUI_ONEOFMANY*  ret = __original_CreateOneOfMany(book);
	if(ret)trace_alloc(trace_gui, (void*)ret, __file__, __line__);
	return ret;
}

GUI_TABMENUBAR*  __deleaker_CreateTabMenuBar( const char* __file__, int __line__, BOOK* book )
{
	GUI_TABMENUBAR*  ret = __original_CreateTabMenuBar(book);
	if(ret)trace_alloc(trace_gui, (void*)ret, __file__, __line__);
	return ret;
}

GUI*  __deleaker_CreateProgressBar( const char* __file__, int __line__, BOOK* book, int display )
{
	GUI*  ret = __original_CreateProgressBar(book, display);
	if(ret)trace_alloc(trace_gui, (void*)ret, __file__, __line__);
	return ret;
}

STRID  __deleaker_Str2ID( const char* __file__, int __line__, const void* wstr, int flag, int len )
{
	STRID  ret = __original_Str2ID(wstr, flag, len);
	if(isallocatedstrid(ret))
	{
		trace_alloc(trace_strid, (void*)ret, __file__, __line__);
		if(flag==5)
		{
			for(int i=0;i<len;i++)
				trace_free(trace_strid, (void*)((long*)wstr)[i], __file__, __line__);
		}
	}
	return ret;
}

STRID  __deleaker_TextCopyId( const char* __file__, int __line__, STRID __unknwnargname1 )
{
	STRID  ret = __original_TextCopyId(__unknwnargname1);
	if(isallocatedstrid(ret))
		trace_alloc(trace_strid, (void*)ret, __file__, __line__);
	return ret;
}

STRID  __deleaker_int2strID( const char* __file__, int __line__, int num )
{
	STRID  ret = __original_int2strID(num);
	if(isallocatedstrid(ret))
		trace_alloc(trace_strid, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_TextFree( const char* __file__, int __line__, STRID __unknwnargname1 )
{
	trace_free(trace_strid, (void*)__unknwnargname1, __file__, __line__ );
	return __original_TextFree(__unknwnargname1);
}

void  __deleaker_GUIObject_SetTitleText( const char* __file__, int __line__, GUI* __unknwnargname1, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_GUIObject_SetTitleText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_GUIObject_SoftKeys_SetText( const char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	return __original_GUIObject_SoftKeys_SetText(__unknwnargname1, actionID, __unknwnargname3);
}

char  __deleaker_GUIonMessage_SetMenuItemText( const char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_GUIonMessage_SetMenuItemText(msg, __unknwnargname2);
}

char  __deleaker_GUIonMessage_SetMenuItemSecondLineText( const char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_GUIonMessage_SetMenuItemSecondLineText(msg, __unknwnargname2);
}

char  __deleaker_GUIonMessage_SetMenuItemInfoText( const char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_GUIonMessage_SetMenuItemInfoText(msg, __unknwnargname2);
}

char  __deleaker_GUIonMessage_SetMenuItemUnavailableText( const char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_GUIonMessage_SetMenuItemUnavailableText(msg, __unknwnargname2);
}

int  __deleaker_MessageBox( const char* __file__, int __line__, STRID header_text, STRID message_text, wchar_t IconID, int style, int time, BOOK* book )
{
	if(isallocatedstrid(header_text))trace_free(trace_strid, (void*)header_text, __file__, __line__ );
	if(isallocatedstrid(message_text) && message_text!=header_text)trace_free(trace_strid, (void*)message_text, __file__, __line__ );
	if(isallocatediconid(IconID))trace_free(trace_iconid, (void*)IconID, __file__, __line__ );
	return __original_MessageBox(header_text, message_text, IconID, style, time, book);
}

void  __deleaker_BookObj_SoftKeys_SetText( const char* __file__, int __line__, BOOK* book, int actionID, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	return __original_BookObj_SoftKeys_SetText(book, actionID, __unknwnargname3);
}

void  __deleaker_DataBrowserDesc_SetHeaderText( const char* __file__, int __line__, void* DataBrowserDesc, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_DataBrowserDesc_SetHeaderText(DataBrowserDesc, __unknwnargname2);
}

void  __deleaker_DataBrowserDesc_SetOKSoftKeyText( const char* __file__, int __line__, void* DataBrowserDesc, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_DataBrowserDesc_SetOKSoftKeyText(DataBrowserDesc, __unknwnargname2);
}

void  __deleaker_DispObject_SetTitleText( const char* __file__, int __line__, DISP_OBJ* __unknwnargname1, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_DispObject_SetTitleText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_GUIObject_SoftKeys_AddErrorStr( const char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	return __original_GUIObject_SoftKeys_AddErrorStr(__unknwnargname1, actionID, __unknwnargname3);
}

void  __deleaker_ProgressBar_SetText( const char* __file__, int __line__, GUI* __unknwnargname1, STRID text )
{
	if(isallocatedstrid(text))trace_free(trace_strid, (void*)text, __file__, __line__ );
	return __original_ProgressBar_SetText(__unknwnargname1, text);
}

void  __deleaker_StatusIndication_Item8_SetText( const char* __file__, int __line__, STRID __unknwnargname1 )
{
	if(isallocatedstrid(__unknwnargname1))trace_free(trace_strid, (void*)__unknwnargname1, __file__, __line__ );
	return __original_StatusIndication_Item8_SetText(__unknwnargname1);
}

void  __deleaker_StatusIndication_SetItemText( const char* __file__, int __line__, GUI* __unknwnargname1, int item, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	return __original_StatusIndication_SetItemText(__unknwnargname1, item, __unknwnargname3);
}

void  __deleaker_StatusIndication_ShowNotes( const char* __file__, int __line__, STRID __unknwnargname1 )
{
	if(isallocatedstrid(__unknwnargname1))trace_free(trace_strid, (void*)__unknwnargname1, __file__, __line__ );
	return __original_StatusIndication_ShowNotes(__unknwnargname1);
}

void  __deleaker_StringInput_DispObject_SetText( const char* __file__, int __line__, DISP_OBJ* __unknwnargname1, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_StringInput_DispObject_SetText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_GUIObject_SoftKeys_SetInfoText( const char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	return __original_GUIObject_SoftKeys_SetInfoText(__unknwnargname1, actionID, __unknwnargname3);
}

void  __deleaker_ListMenu_SetSecondLineText( const char* __file__, int __line__, GUI_LIST* __unknwnargname1, int elem_num, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	return __original_ListMenu_SetSecondLineText(__unknwnargname1, elem_num, __unknwnargname3);
}

void  __deleaker_GUIonMessage_SubItem_SetText( const char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_GUIonMessage_SubItem_SetText(msg, __unknwnargname2);
}

void  __deleaker_Feedback_SetTextExtended( const char* __file__, int __line__, GUI_FEEDBACK* __unknwnargname1, STRID text, int where )
{
	if(isallocatedstrid(text))trace_free(trace_strid, (void*)text, __file__, __line__ );
	return __original_Feedback_SetTextExtended(__unknwnargname1, text, where);
}

void  __deleaker_GUIObject_SoftKeys_SetTexts( const char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID short_text, STRID full_text )
{
	if(isallocatedstrid(short_text))trace_free(trace_strid, (void*)short_text, __file__, __line__ );
	if(isallocatedstrid(full_text) /*&& StrID1!=StrID*/)trace_free(trace_strid, (void*)full_text, __file__, __line__ );
	return __original_GUIObject_SoftKeys_SetTexts(__unknwnargname1, actionID, short_text, full_text);
}

STRID  __deleaker_PNUM2Name( const char* __file__, int __line__, void* pnum, int isIconNeeded, int only_full_number_flag )
{
	STRID  ret = __original_PNUM2Name(pnum, isIconNeeded, only_full_number_flag);
	if(isallocatedstrid(ret))
		trace_alloc(trace_strid, (void*)ret, __file__, __line__);
	return ret;
}

int  __deleaker_Date2ID( const char* __file__, int __line__, DATE* __unknwnargname1, int TimeFormat, int __unknwnargname3 )
{
	int  ret = __original_Date2ID(__unknwnargname1, TimeFormat, __unknwnargname3);
	if(isallocatedstrid(ret))
		trace_alloc(trace_strid, (void*)ret, __file__, __line__);
	return ret;
}

int  __deleaker_Time2ID( const char* __file__, int __line__, TIME* __unknwnargname1, char TimeFormat, int isSec )
{
	int  ret = __original_Time2ID(__unknwnargname1, TimeFormat, isSec);
	if(isallocatedstrid(ret))
		trace_alloc(trace_strid, (void*)ret, __file__, __line__);
	return ret;
}

STRID  __deleaker_KeyCode2Name( const char* __file__, int __line__, int key_code )
{
	STRID  ret = __original_KeyCode2Name(key_code);
	if(isallocatedstrid(ret))trace_alloc(trace_strid, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_NOfMany_SetTexts( const char* __file__, int __line__, GUI_NOFMANY* __unknwnargname1, STRID* strids, int items_count )
{
	for(int i=0;i<items_count;i++)
		if(isallocatedstrid(strids[i]))trace_free(trace_strid, (void*)strids[i], __file__, __line__ );
	__original_NOfMany_SetTexts(__unknwnargname1, strids, items_count);
}

void  __deleaker_OneOfMany_SetTexts( const char* __file__, int __line__, GUI_ONEOFMANY* __unknwnargname1, STRID* strids, int Count )
{
	for(int i=0;i<Count;i++)
		if(isallocatedstrid(strids[i]))trace_free(trace_strid, (void*)strids[i], __file__, __line__ );
	__original_OneOfMany_SetTexts(__unknwnargname1, strids, Count);
}

void  __deleaker_FeedBack_SetText( const char* __file__, int __line__, GUI_FEEDBACK* __unknwnargname1, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_FeedBack_SetText(__unknwnargname1, __unknwnargname2);
}

int  __deleaker_Gif2ID( const char* __file__, int __line__, u16 IMAGEHANDLE, const wchar_t* path, const wchar_t* fname, wchar_t* ID )
{//проверить код ошибки
	int  ret = __original_Gif2ID(IMAGEHANDLE, path, fname, ID);
	if(ret)trace_alloc(trace_iconid, (void*)*ID, __file__, __line__);
	return ret;
}

void  __deleaker_GUIObject_SetTitleIcon( const char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
	if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
	__original_GUIObject_SetTitleIcon(__unknwnargname1, imageID);
}

void  __deleaker_GUIInput_SetIcon( const char* __file__, int __line__, GUI* __unknwnargname1, wchar_t icon )
{
	if(isallocatediconid(icon))trace_free(trace_iconid, (void*)icon, __file__, __line__ );
	__original_GUIInput_SetIcon(__unknwnargname1, icon);
}

void  __deleaker_ProgressBar_SetIcon( const char* __file__, int __line__, GUI* __unknwnargname1, u16 icon_id )
{
	if(isallocatediconid(icon_id))trace_free(trace_iconid, (void*)icon_id, __file__, __line__ );
	__original_ProgressBar_SetIcon(__unknwnargname1, icon_id);
}

void  __deleaker_GUIonMessage_SetMenuItemIcon( const char* __file__, int __line__, GUI_MESSAGE* msg, int align, wchar_t iconID )
{
	if(isallocatediconid(iconID))trace_free(trace_iconid, (void*)iconID, __file__, __line__ );
	__original_GUIonMessage_SetMenuItemIcon(msg, align, iconID);
}

void  __deleaker_TabMenuBar_SetTabIcon( const char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab, wchar_t icon, int for_state )
{
	if(isallocatediconid(icon))trace_free(trace_iconid, (void*)icon, __file__, __line__ );
	__original_TabMenuBar_SetTabIcon(__unknwnargname1, tab, icon, for_state);
}

void  __deleaker_VCALL_SetNameIcon( const char* __file__, int __line__, void* vc, wchar_t icon )
{
	if(isallocatediconid(icon))trace_free(trace_iconid, (void*)icon, __file__, __line__ );
	__original_VCALL_SetNameIcon(vc, icon);
}

#ifdef __cplusplus
int  __deleaker_ModifyKeyHook( const char* __file__, int __line__, KEYHOOKPROC proc, int mode, LPARAM lparam )
{
	int  ret = __original_ModifyKeyHook(proc, mode, lparam);
	if(mode==0)trace_free(trace_hook, (void*)proc, __file__, __line__ );
	if(mode==1)trace_alloc(trace_hook, (void*)proc, __file__, __line__);
	return ret;
}
int  __deleaker_ModifyKeyHook( const char* __file__, int __line__, int (*proc)( int key, int repeat_count, int mode ), int mode, LPARAM lparam )
{
	int  ret = __original_ModifyKeyHook(proc, mode, lparam);
	if(mode==0)trace_free(trace_hook, (void*)proc, __file__, __line__ );
	if(mode==1)trace_alloc(trace_hook, (void*)proc, __file__, __line__);
	return ret;
}
#else
int  __deleaker_ModifyKeyHook( const char* __file__, int __line__, int (*proc)( int key, int repeat_count, int mode, void* ), int mode, void* lparam )
{
	int  ret = __original_ModifyKeyHook(proc, mode, lparam);
	if(mode==0)trace_free(trace_hook, (void*)proc, __file__, __line__ );
	if(mode==1)trace_alloc(trace_hook, (void*)proc, __file__, __line__);
	return ret;
}
#endif

int  __deleaker_ModifyUIPageHook( const char* __file__, int __line__, int event, int (*PROC)(void *msg, BOOK * book, PAGE_DESC * page_desc, LPARAM ClientData), LPARAM ClientData, int mode )
{
	int  ret = __original_ModifyUIPageHook(event, PROC, ClientData, mode);
	if(mode==0)trace_free(trace_hook, (void*)PROC, __file__, __line__ );
	if(mode==1)trace_alloc(trace_hook, (void*)PROC, __file__, __line__);
	return ret;
}

int  __deleaker_ImageID_Get( const char* __file__, int __line__, const wchar_t* fpath, const wchar_t* fname, wchar_t* imageID )
{
	int  ret = __original_ImageID_Get(fpath, fname, imageID);
	if(isallocatediconid(ret))
		trace_alloc(trace_iconid, (void*)(*imageID), __file__, __line__);
	return ret;
}

int  __deleaker_ImageID_GetIndirect( const char* __file__, int __line__, void* buf_image, int size, int __NULL, wchar_t* image_type, wchar_t* imageID )
{//проверить возвращаемое значение
	int  ret = __original_ImageID_GetIndirect(buf_image, size, __NULL, image_type, imageID);
	trace_free(trace_memory, buf_image, __file__, __line__);
	trace_alloc(trace_iconid, (void*)(*imageID), __file__, __line__);
	return ret;
}

void  __deleaker_ImageID_Free( const char* __file__, int __line__, wchar_t imageID )
{
	trace_free(trace_iconid, (void*)imageID, __file__, __line__);
	__original_ImageID_Free(imageID);
}

GC*  __deleaker_GC_CreateMemoryGC( const char* __file__, int __line__, int xsize, int ysize, int bpp, int unk, void* somefn, int unk2 )
{
	GC*  ret = __original_GC_CreateMemoryGC(xsize, ysize, bpp, unk, somefn, unk2);
	if(ret)trace_alloc(trace_gc, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_GC_FreeGC( const char* __file__, int __line__, GC* gc )
{
	trace_free(trace_gc, gc, __file__, __line__);
	__original_GC_FreeGC(gc);
}

GVI_PEN  __deleaker_GVI_CreateDashedPen( const char* __file__, int __line__, char thikness, int color, int bitmask, int step, int offset )
{
	GVI_PEN  ret = __original_GVI_CreateDashedPen(thikness, color, bitmask, step, offset);
	if(ret)trace_alloc(trace_gvi, (void*)ret, __file__, __line__);
	return ret;
}

GVI_PEN  __deleaker_GVI_CreatePen( const char* __file__, int __line__, char thikness, int color )
{
	GVI_PEN  ret = __original_GVI_CreatePen(thikness, color);
	if(ret)trace_alloc(trace_gvi, (void*)ret, __file__, __line__);
	return ret;
}

GVI_BRUSH  __deleaker_GVI_CreateSolidBrush( const char* __file__, int __line__, int color )
{
	GVI_BRUSH  ret = __original_GVI_CreateSolidBrush(color);
	if(ret)trace_alloc(trace_gvi, (void*)ret, __file__, __line__);
	return ret;
}

GVI_BMP  __deleaker_GVI_CreateBitmap( const char* __file__, int __line__, int xsize, int ysize, int bpp )
{
	GVI_BMP  ret = __original_GVI_CreateBitmap(xsize, ysize, bpp);
	if(ret)trace_alloc(trace_gvi, (void*)ret, __file__, __line__);
	return ret;
}

GVI_GC  __deleaker_GVI_CreateMemoryGC( const char* __file__, int __line__, GVI_BMP bitmap )
{
	GVI_GC  ret = __original_GVI_CreateMemoryGC(bitmap);
	if(ret)trace_alloc(trace_gvi, (void*)ret, __file__, __line__);
	return ret;
}

BOOL  __deleaker_GVI_Delete_GVI_Object( const char* __file__, int __line__, GVI_OBJ* __unknwnargname1 )
{
	trace_free(trace_gvi, *__unknwnargname1, __file__, __line__);
	return __original_GVI_Delete_GVI_Object(__unknwnargname1);
}

void  __deleaker_GVI_DeleteMemoryGC( const char* __file__, int __line__, GVI_GC gc )
{
	trace_free(trace_gvi, gc, __file__, __line__);
	return __original_GVI_DeleteMemoryGC(gc);
}

GUI_FEEDBACK*  __deleaker_TextFeedbackWindow( const char* __file__, int __line__, BOOK* book, int zero )
{
	GUI_FEEDBACK*  ret = __original_TextFeedbackWindow(book, zero);
	if(ret)trace_alloc(trace_gui, (void*)ret, __file__, __line__);
	return ret;
}

void*  __deleaker_DataBrowserDesc_Create( const char* __file__, int __line__ )
{
	void*  ret = __original_DataBrowserDesc_Create();
	if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_DataBrowserDesc_Destroy( const char* __file__, int __line__, void* DataBrowserDesc )
{
	trace_free(trace_memory, DataBrowserDesc, __file__, __line__);
	return __original_DataBrowserDesc_Destroy(DataBrowserDesc);
}

#define CreateStringInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_gui, __original_CreateStringInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateDateInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_gui, __original_CreateDateInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreatePercentInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_gui, __original_CreatePercentInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateStringInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_gui, __original_CreateStringInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateTimeInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_gui, __original_CreateTimeInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateYesNoQuestionVA( a, ... ) (GUI*) trace_alloc_ret( trace_gui, __original_CreateYesNoQuestionVA( a, __VA_ARGS__), NULL, __file__, __line__ )

GUI_FEEDBACK*  __deleaker_CreateMonitorFeedback( const char* __file__, int __line__, STRID __unknwnargname1, BOOK* __unknwnargname2, void (*onbusy)(BOOK*), void (*onedit)(BOOK*), void (*ondelete)(BOOK*) )
{
	GUI_FEEDBACK*  ret = __original_CreateMonitorFeedback(__unknwnargname1, __unknwnargname2, onbusy, onedit, ondelete);
	if(ret)trace_alloc(trace_gui, (void*)ret, __file__, __line__);
	return ret;
}

void*  __deleaker_LoadDLL( const char* __file__, int __line__, wchar_t* DllName )
{
	void*  ret = __original_LoadDLL(DllName);
	if(ret)trace_alloc(trace_dll, (void*)ret, __file__, __line__);
	return ret;
}

int  __deleaker_UnLoadDLL( const char* __file__, int __line__, void* DllData )
{
	trace_free(trace_dll, DllData, __file__, __line__);
	return __original_UnLoadDLL(DllData);
}

void  __deleaker_GUIObject_SetSecondRowTitleText( const char* __file__, int __line__, GUI* __unknwnargname1, STRID __unknwnargname2 )
{
	if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
	return __original_GUIObject_SetSecondRowTitleText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_ListMenu_SetNoItemText( const char* __file__, int __line__, GUI_LIST* __unknwnargname1, STRID str )
{
	if(isallocatedstrid(str))trace_free(trace_strid, (void*)str, __file__, __line__ );
	return __original_ListMenu_SetNoItemText(__unknwnargname1, str);
}

void  __deleaker_TabMenuBar_SetTabTitle( const char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab_num, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	return __original_TabMenuBar_SetTabTitle(__unknwnargname1, tab_num, __unknwnargname3);
}

void  __deleaker_DispObject_SetBackgroundImage( const char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID )
{
	if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
	__original_DispObject_SetBackgroundImage(__unknwnargname1, imageID);
}

void  __deleaker_DispObject_SetCursorImage( const char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID )
{
	if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
	__original_DispObject_SetCursorImage(__unknwnargname1, imageID);
}

void  __deleaker_DispObject_SetTitleBackgroundImage( const char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID )
{
	if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
	__original_DispObject_SetTitleBackgroundImage(__unknwnargname1, imageID);
}

void  __deleaker_GUIObject_SetBackgroundImage( const char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
	if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
	__original_GUIObject_SetBackgroundImage(__unknwnargname1, imageID);
}

void  __deleaker_GUIObject_SetCursorImage( const char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
	if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
	__original_GUIObject_SetCursorImage(__unknwnargname1, imageID);
}

void  __deleaker_GUIObject_SetTitleBackgroundImage( const char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
	if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
	__original_GUIObject_SetTitleBackgroundImage(__unknwnargname1, imageID);
}

STRID  __deleaker_Shortcut_Get_MenuItemName( const char* __file__, int __line__, void* __unknwnargname1 )
{
	STRID  ret = __original_Shortcut_Get_MenuItemName(__unknwnargname1);
	trace_alloc(trace_strid, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_YesNoQuestion_SetIcon( const char* __file__, int __line__, GUI* __unknwnargname1, wchar_t iconId )
{
	if(isallocatediconid(iconId))trace_free(trace_iconid, (void*)iconId, __file__, __line__ );
	__original_YesNoQuestion_SetIcon(__unknwnargname1, iconId);
}

int  __deleaker_ListMenu_SetItemIcon( const char* __file__, int __line__, GUI_LIST* __unknwnargname1, int Item, wchar_t unk_FFFF, int mode, wchar_t ImageID )
{
	//!!!unknown arg unk_FFFF
	if(isallocatediconid(ImageID))trace_free(trace_iconid, (void*)ImageID, __file__, __line__ );
	return __original_ListMenu_SetItemIcon(__unknwnargname1, Item, unk_FFFF, mode, ImageID);
}

int  __deleaker_Shortcut_Get_MenuItemIconID( const char* __file__, int __line__, void* __unknwnargname1 )
{
	int  ret = __original_Shortcut_Get_MenuItemIconID(__unknwnargname1);
	if(isallocatediconid(ret))trace_alloc(trace_iconid, (void*)ret, __file__, __line__);
	return ret;
}

PROCESS  __deleaker_create_process( const char* __file__, int __line__, int proc_type, char* name, OSENTRYPOINT* entrypoint, OSADDRESS stack_size, OSPRIORITY priority, OSTIME timeslice, PROCESS pid_block, void* redir_table, OSVECTOR vector, OSUSER user )
{
	PROCESS  ret = __original_create_process(proc_type, name, entrypoint, stack_size, priority, timeslice, pid_block, redir_table, vector, user);
	trace_alloc(trace_process, (void*)ret, __file__, __line__);
	return ret;
}

void  __deleaker_kill_proc( const char* __file__, int __line__, PROCESS pid )
{
	trace_free(trace_process, (void*)pid, __file__, __line__);
	return __original_kill_proc(pid);
}

void  __deleaker_MediaPlayer_SoftKeys_AddHelpStr( const char* __file__, int __line__, GUI* player_gui, int item, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	__original_MediaPlayer_SoftKeys_AddHelpStr(player_gui, item, __unknwnargname3);
}

void  __deleaker_MediaPlayer_SoftKeys_SetText( const char* __file__, int __line__, GUI* player_gui, int actionID, STRID __unknwnargname3 )
{
	if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
	__original_MediaPlayer_SoftKeys_SetText(player_gui, actionID, __unknwnargname3);
}

//__swi __arm SUB_EXECUTE* BrowserItem_Get_SUB_EXECUTE( BOOK* BrowserItemBook );
//__swi __arm void CoCreateInstance( PUUID cid, PUUID iid, void** pInterface );
//__swi __arm void* CreateMessage( int size, int ev, char* name );
//__swi __arm char* CreateURI( wchar_t* fpath, wchar_t* fname, char* URIScheme );
//__swi __arm void DataBrowserDesc_SetActions( void* DataBrowserDesc, char* actions );
//__swi __arm void DataBrowserDesc_SetFileExtList( void* DataBrowserDesc, const wchar_t* ExtList );
//__swi __arm void DataBrowserDesc_SetFolders( void* DataBrowserDesc, const wchar_t** FolderList );
//__swi __arm void DataBrowserDesc_SetItemFilter( void* DataBrowserDesc, DB_FILE_FILTER );
//__swi __arm void DataBrowserDesc_SetOption( void* DataBrowserDesc, char* option );
//__swi __arm void DataBrowserDesc_SetViewModeAndSortOrder( void* DataBrowserDesc, int view_sort_ID );
//__swi __arm void DataBrowser_Create( void* DataBrowserDesc );
//__swi __arm SUB_EXECUTE* DataBrowser_CreateSubExecute( int BookID, FILEITEM* );
//__swi __arm int DataBrowser_ExecuteSubroutine( SUB_EXECUTE* sub, int action, u16* unk );
//__swi __arm int DirHandle_SetFilterStr( DIR_HANDLE*, const wchar_t* filter );
//__swi __arm FILEITEM* FILEITEM_Create( void );
//__swi __arm FILEITEM* FILEITEM_CreateCopy( FILEITEM* );
//__swi __arm void FILEITEM_Destroy( FILEITEM* );
//__swi __arm int FILEITEM_SetFname( FILEITEM*, const wchar_t* fname );
//__swi __arm int FILEITEM_SetFnameAndContentType( FILEITEM*, const wchar_t* fname );
//__swi __arm int FILEITEM_SetPath( FILEITEM*, const wchar_t* fpath );
//__swi __arm int FILEITEM_SetPathAndContentType( FILEITEM*, const wchar_t* fpath );
//__swi __arm void Feedback_SetKeyHook( GUI_FEEDBACK*, void (*hook)( BOOK* book, int key, int unk, int unk2 ) );
//__swi __arm int FreeMessage( void** Mess );
//__swi __arm FILELISTITEM* GetFname( DIR_HANDLE*, FILELISTITEM* );
//__swi __arm char* GetURIScheme( int schemeID );
//__swi __arm void* GetUserData( int size, void (*constr)( void*, void* ), void* constr_param );
//__swi __arm int JavaAppDesc_Get( int unk1, void** JavaDesc );
//__swi __arm int JavaAppDesc_GetFirstApp( void* JavaDesc );
//__swi __arm int JavaAppDesc_GetJavaAppFullpath( void* JavaDesc, JavaAppFullpath* );
//__swi __arm int JavaAppDesc_GetJavaAppID( void* JavaDesc );
//__swi __arm int JavaAppDesc_GetJavaAppInfo( void* JavaDesc, int ID, wchar_t** wstr );
//__swi __arm int JavaAppDesc_GetNextApp( void* JavaDesc );
//__swi __arm int JavaApp_LogoImageID_Get( wchar_t* fullpath, wchar_t* imageID );
//__swi __arm void JavaDialog_Close( int unk1 );
//__swi __arm int JavaDialog_Open( int unk1, char* unk2, void** JavaDesc );
//__swi __arm int JavaSession_GetName( void );
//__swi __arm int MSG_SendMessage_AddRecipient( void*, wchar_t*, wchar_t*, int, int );
//__swi __arm int MSG_SendMessage_CreateMessage( int, void* );
//__swi __arm int MSG_SendMessage_DestroyMessage( void* );
//__swi __arm int MSG_SendMessage_Start( int, void*, int );
//__swi __arm char* MainInput_getPNUM( GUI* );
//__swi __arm wchar_t* MenuBook_Desktop_GetSelectedItemID( BOOK* MenuBook_Desktop );
//__swi __arm void* MetaData_Desc_Create( wchar_t* path, wchar_t* name );
//__swi __arm void MetaData_Desc_Destroy( void* MetaData_Desc );
//__swi __arm int MetaData_Desc_GetCoverInfo( void* MetaData_Desc, char* cover_type, int* size, int* cover_offset );
//__swi __arm wchar_t* MetaData_Desc_GetTags( void* MetaData_Desc, int tagID );
//__swi __arm int MetaData_Desc_GetTrackNum( void* MetaData_Desc, int __NULL );
//__swi __arm void OSE_GetShell( void** pInterface );
//__swi __arm void ObexSendFile( SEND_OBEX_STRUCT* );
//__swi __arm void PNUM2str( char* str, void* pnum, int len, int max_len );
//__swi __arm void REQUEST_ALARMCLOCKSTATUS_GET( const int* sync, char* alarm_status );
//__swi __arm void REQUEST_ALARMCLOCKTIME_GET( const int* sync, TIME* t );
//__swi __arm void REQUEST_ALARMDATEANDTIMESTATUS_GET( const int* sync, DATETIME* );
//__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_GETHANDLE( const int* sync, u16* IMAGEHANDLE, char* unk );
//__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_REGISTER( const int* sync, u16 ImageHandle, wchar_t* path, wchar_t* fname, int unk, wchar_t* ImageID, char* error );
//__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_UNREGISTER( const int* sync, u16 ImageHandle, u16*, u16*, wchar_t ImageID, int unk_1, char* error );
//__swi __arm int REQUEST_PHONEBOOK_ACCESSSTATUS_TOTAL_GET( const int* sync, int* );
//__swi __arm int REQUEST_PROFILE_GETPROFILENAME( const int* sync, int unk, STRID_DATA*, char* error );
//__swi __arm int RemoveUserData( void (*constr)( void*, void* ) );
//__swi __arm int Request_EventChannel_Subscribe( const int* sync, int mode, int event );
//__swi __arm void SendDispatchMessage( int id, int unk_zero, int size, void* mess );
//__swi __arm void SendMessage( void** signal, int PID );
//__swi __arm void SoftKeys_GetLabel( DISP_OBJ* softkeys, SKLABEL* lbl, int id );
//__swi __arm void* SoundRecorderDesc_Create( void );
//__swi __arm void SoundRecorderDesc_Destroy( void* desc );
//__swi __arm int SoundRecorderDesc_SetBookID( void* desc, int BookID );
//__swi __arm int SoundRecorderDesc_SetFname( void* desc, const wchar_t* fname );
//__swi __arm int SoundRecorderDesc_SetFolder( void* desc, const wchar_t* fpath );
//__swi __arm int SoundRecorderDesc_SetRecordSize( void* desc, const int size );
//__swi __arm int SoundRecorderDesc_SetType( void* desc, const int type );
//__swi __arm int SoundRecorder_Create( void* desc );
//__swi __arm int SoundRecorder_RecordCall( BOOK* OngoingCallBook );
//__swi __arm int SpeedDial_GetPNUM( int _zero, char charter0__9, void* PNUM );
//__swi __arm void Timer_Kill( u16* timerID );
//__swi __arm void Timer_ReSet( u16* timer, int time, void (*onTimer)( u16 timerID, void* ), void* );
//__swi __arm u16 Timer_Set( int time, void (*onTimer)( u16 timerID, void* ), void* );
//__swi __arm void* WaitMessage( void* SIGSEL );
//__swi __arm C_INTERFACE* Window_GetComponentInterface( WINDOW* );
//__swi __arm void free_buf( union SIGNAL** sig );
//__swi __arm int inflate( z_streamp strm, int flush );
//__swi __arm int inflateEnd( z_streamp strm );
//__swi __arm int inflateInit2_( z_streamp strm, int windowBits, const char* version, int stream_size );
//__swi __arm png_infop png_create_info_struct( png_structp png_ptr );
//__swi __arm png_structp png_create_read_struct_2( png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn, png_voidp mem_ptr, png_malloc_ptr malloc_fn, png_free_ptr free_fn );
//__swi __arm void png_destroy_info_struct( png_structp png_ptr, png_infopp info_ptr_ptr );
//__swi __arm void png_destroy_read_struct( png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr );
//__swi __arm png_uint_32 png_get_IHDR( png_structp png_ptr, png_infop info_ptr, png_uint_32* width, png_uint_32* height, int* bit_depth, int* color_type, int* interlace_method, int* compression_method, int* filter_method );
//__swi __arm png_voidp png_get_progressive_ptr( png_structp png_ptr );
//__swi __arm png_uint_32 png_get_rowbytes( png_structp png_ptr, png_infop info_ptr );
//__swi __arm png_uint_32 png_get_valid( png_structp png_ptr, png_infop info_ptr, png_uint_32 flag );
//__swi __arm void png_process_data( png_structp png_ptr, png_infop info_ptr, png_bytep buffer, png_size_t buffer_size );
//__swi __arm void png_progressive_combine_row( png_structp png_ptr, png_bytep old_row, png_bytep new_row );
//__swi __arm void png_read_update_info( png_structp png_ptr, png_infop info_ptr );
//__swi __arm void png_set_filler( png_structp png_ptr, png_uint_32 filler, int flags );
//__swi __arm void png_set_gray_1_2_4_to_8( png_structp png_ptr );
//__swi __arm void png_set_gray_to_rgb( png_structp png_ptr );
//__swi __arm void png_set_palette_to_rgb( png_structp png_ptr );
//__swi __arm void png_set_progressive_read_fn( png_structp png_ptr, png_voidp progressive_ptr, png_progressive_info_ptr info_fn, png_progressive_row_ptr row_fn, png_progressive_end_ptr end_fn );
//__swi __arm void png_set_strip_16( png_structp png_ptr );
//__swi __arm void png_set_tRNS_to_alpha( png_structp png_ptr );
//__swi __arm int png_sig_cmp( png_bytep sig, png_size_t start, png_size_t num_to_check );
//__swi __arm void send( union SIGNAL** sig, PROCESS to );
//__swi __arm int setjmp( jmp_buf jmpbuf );
//__swi __arm int w_dirclose( void* );
//__swi __arm void* w_diropen( const wchar_t* dir );
//__swi __arm wchar_t* w_dirread( void* );

#endif
