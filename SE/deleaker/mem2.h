#ifndef __INCLUDE_MEM__
#define __INCLUDE_MEM__

#include "..\include\Lib_Clara.h"

#ifdef NDEBUG

#define trace_init()
#define trace_done()

#else

void trace_init();
void trace_done();
void trace_alloc(int mt, void* ptr,char *file, int line);
void* trace_alloc_ret(int mt, void* ptr, void* badvalue,char *file, int line);
void trace_free(int mt,void* p, char* file, int line);

//эти нужны обязательно
void * __deleaker_malloc( char* __file__, int __line__, int size );
void  __deleaker_mfree( char* __file__, int __line__, void *p );

//delete из-за delete[] макросом не переопределить, поэтому только
//включением оптимизации с галкой "function inlining"
#pragma inline=forced
inline void operator delete(void* p){ __deleaker_mfree( __FILE__, __LINE__, p ); }
#pragma inline=forced
inline void operator delete[](void* p){ __deleaker_mfree( __FILE__, __LINE__, p ); }

inline void* operator new(size_t size, char *file, int line){ return  __deleaker_malloc( file, line, size ); }
inline void* operator new[](size_t size, char *file, int line){ return __deleaker_malloc( file, line, size ); }

#define NEW new (__FILE__, __LINE__)
#define new NEW
/*
#pragma inline=forced
inline void* operator new(size_t size){ return  __deleaker_malloc(size, __FILE__, __LINE__ ); }
#pragma inline=forced
inline void* operator new[](size_t size){ return __deleaker_malloc( size, __FILE__, __LINE__ ); }
*/

#define malloc(size) __deleaker_malloc( __FILE__,  __LINE__, size )
void*  __deleaker_malloc( char* __file__, int __line__, int size );
#define mfree_adr() __deleaker_mfree_adr( __FILE__,  __LINE__ )
void*  __deleaker_mfree_adr( char* __file__, int __line__ );
#define mfree(p) __deleaker_mfree( __FILE__,  __LINE__, p )
void  __deleaker_mfree( char* __file__, int __line__, void* p );
#define FreeBook(book) __deleaker_FreeBook( __FILE__,  __LINE__, book )
void  __deleaker_FreeBook( char* __file__, int __line__, BOOK* book );
#define BookObj_KillBook(book) __deleaker_BookObj_KillBook( __FILE__,  __LINE__, book )
void  __deleaker_BookObj_KillBook( char* __file__, int __line__, BOOK* book );
#define List_Create() __deleaker_List_Create( __FILE__,  __LINE__ )
LIST*  __deleaker_List_Create( char* __file__, int __line__ );
#define List_Destroy(lst) __deleaker_List_Destroy( __FILE__,  __LINE__, lst )
void  __deleaker_List_Destroy( char* __file__, int __line__, LIST* lst );
#define manifest_GetParam(buf, param_name, unk) __deleaker_manifest_GetParam( __FILE__,  __LINE__, buf, param_name, unk )
char*  __deleaker_manifest_GetParam( char* __file__, int __line__, const char* buf, const char* param_name, int unk );
#define AllocDirHandle(path) __deleaker_AllocDirHandle( __FILE__,  __LINE__, path )
DIR_HANDLE*  __deleaker_AllocDirHandle( char* __file__, int __line__, const wchar_t* path );
#define DestroyDirHandle(handle) __deleaker_DestroyDirHandle( __FILE__,  __LINE__, handle )
void  __deleaker_DestroyDirHandle( char* __file__, int __line__, DIR_HANDLE* handle );
#define GUIObject_Destroy(__unknwnargname1) __deleaker_GUIObject_Destroy( __FILE__,  __LINE__, __unknwnargname1 )
GUI*  __deleaker_GUIObject_Destroy( char* __file__, int __line__, GUI* __unknwnargname1 );
#define _fopen(filpath, filname, mode, rights, __0) __deleaker__fopen( __FILE__,  __LINE__, filpath, filname, mode, rights, __0 )
int  __deleaker__fopen( char* __file__, int __line__, const wchar_t* filpath, const wchar_t* filname, unsigned int mode, unsigned int rights, unsigned int __0 );
#define fopen(fname, mode, rights) __deleaker_fopen( __FILE__,  __LINE__, fname, mode, rights )
int  __deleaker_fopen( char* __file__, int __line__, const wchar_t* fname, int mode, int rights );
#define fclose(file) __deleaker_fclose( __FILE__,  __LINE__, file )
int  __deleaker_fclose( char* __file__, int __line__, int file );
#define w_fopen(name, attr, rights, err) __deleaker_w_fopen( __FILE__,  __LINE__, name, attr, rights, err )
int  __deleaker_w_fopen( char* __file__, int __line__, const wchar_t* name, int attr, int rights, int err );
#define w_fclose(f) __deleaker_w_fclose( __FILE__,  __LINE__, f )
int  __deleaker_w_fclose( char* __file__, int __line__, int f );
#define TabMenuBar_SetTabGui(__unknwnargname1, tab, __unknwnargname3) __deleaker_TabMenuBar_SetTabGui( __FILE__,  __LINE__, __unknwnargname1, tab, __unknwnargname3 )
void  __deleaker_TabMenuBar_SetTabGui( char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab, GUI* __unknwnargname3 );
#define CreateListMenu(__unknwnargname1, display) __deleaker_CreateListMenu( __FILE__,  __LINE__, __unknwnargname1, display )
GUI_LIST*  __deleaker_CreateListMenu( char* __file__, int __line__, BOOK* __unknwnargname1, int display );
#define CreateNOfMany(book) __deleaker_CreateNOfMany( __FILE__,  __LINE__, book )
GUI_NOFMANY*  __deleaker_CreateNOfMany( char* __file__, int __line__, BOOK* book );
#define CreateOneOfMany(book) __deleaker_CreateOneOfMany( __FILE__,  __LINE__, book )
GUI_ONEOFMANY*  __deleaker_CreateOneOfMany( char* __file__, int __line__, BOOK* book );
#define CreateTabMenuBar(book) __deleaker_CreateTabMenuBar( __FILE__,  __LINE__, book )
GUI_TABMENUBAR*  __deleaker_CreateTabMenuBar( char* __file__, int __line__, BOOK* book );
#define CreateProgressBar(book, display) __deleaker_CreateProgressBar( __FILE__,  __LINE__, book, display )
GUI*  __deleaker_CreateProgressBar( char* __file__, int __line__, BOOK* book, int display );
#define Str2ID(wstr, flag, len) __deleaker_Str2ID( __FILE__,  __LINE__, wstr, flag, len )
STRID  __deleaker_Str2ID( char* __file__, int __line__, const void* wstr, int flag, int len );
#define TextCopyId(__unknwnargname1) __deleaker_TextCopyId( __FILE__,  __LINE__, __unknwnargname1 )
STRID  __deleaker_TextCopyId( char* __file__, int __line__, STRID __unknwnargname1 );
#define int2strID(num) __deleaker_int2strID( __FILE__,  __LINE__, num )
STRID  __deleaker_int2strID( char* __file__, int __line__, int num );
#define TextFree(__unknwnargname1) __deleaker_TextFree( __FILE__,  __LINE__, __unknwnargname1 )
void  __deleaker_TextFree( char* __file__, int __line__, STRID __unknwnargname1 );
#define GUIObject_SetTitleText(__unknwnargname1, __unknwnargname2) __deleaker_GUIObject_SetTitleText( __FILE__,  __LINE__, __unknwnargname1, __unknwnargname2 )
void  __deleaker_GUIObject_SetTitleText( char* __file__, int __line__, GUI* __unknwnargname1, STRID __unknwnargname2 );
#define GUIObject_SoftKeys_SetText(__unknwnargname1, actionID, __unknwnargname3) __deleaker_GUIObject_SoftKeys_SetText( __FILE__,  __LINE__, __unknwnargname1, actionID, __unknwnargname3 )
void  __deleaker_GUIObject_SoftKeys_SetText( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 );
#define GUIonMessage_SetMenuItemText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemText( __FILE__,  __LINE__, msg, __unknwnargname2 )
char  __deleaker_GUIonMessage_SetMenuItemText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 );
#define GUIonMessage_SetMenuItemSecondLineText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemSecondLineText( __FILE__,  __LINE__, msg, __unknwnargname2 )
char  __deleaker_GUIonMessage_SetMenuItemSecondLineText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 );
#define GUIonMessage_SetMenuItemInfoText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemInfoText( __FILE__,  __LINE__, msg, __unknwnargname2 )
char  __deleaker_GUIonMessage_SetMenuItemInfoText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 );
#define GUIonMessage_SetMenuItemUnavailableText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemUnavailableText( __FILE__,  __LINE__, msg, __unknwnargname2 )
char  __deleaker_GUIonMessage_SetMenuItemUnavailableText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 );
#define MessageBox(header_text, message_text, IconID, style, time, book) __deleaker_MessageBox( __FILE__,  __LINE__, header_text, message_text, IconID, style, time, book )
int  __deleaker_MessageBox( char* __file__, int __line__, STRID header_text, STRID message_text, wchar_t IconID, int style, int time, BOOK* book );
#define BookObj_SoftKeys_SetText(book, actionID, __unknwnargname3) __deleaker_BookObj_SoftKeys_SetText( __FILE__,  __LINE__, book, actionID, __unknwnargname3 )
void  __deleaker_BookObj_SoftKeys_SetText( char* __file__, int __line__, BOOK* book, int actionID, STRID __unknwnargname3 );
#define DataBrowserDesc_SetHeaderText(DataBrowserDesc, __unknwnargname2) __deleaker_DataBrowserDesc_SetHeaderText( __FILE__,  __LINE__, DataBrowserDesc, __unknwnargname2 )
void  __deleaker_DataBrowserDesc_SetHeaderText( char* __file__, int __line__, void* DataBrowserDesc, STRID __unknwnargname2 );
#define DataBrowserDesc_SetOKSoftKeyText(DataBrowserDesc, __unknwnargname2) __deleaker_DataBrowserDesc_SetOKSoftKeyText( __FILE__,  __LINE__, DataBrowserDesc, __unknwnargname2 )
void  __deleaker_DataBrowserDesc_SetOKSoftKeyText( char* __file__, int __line__, void* DataBrowserDesc, STRID __unknwnargname2 );
#define DispObject_SetTitleText(__unknwnargname1, __unknwnargname2) __deleaker_DispObject_SetTitleText( __FILE__,  __LINE__, __unknwnargname1, __unknwnargname2 )
void  __deleaker_DispObject_SetTitleText( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, STRID __unknwnargname2 );
#define GUIObject_SoftKeys_AddErrorStr(__unknwnargname1, actionID, __unknwnargname3) __deleaker_GUIObject_SoftKeys_AddErrorStr( __FILE__,  __LINE__, __unknwnargname1, actionID, __unknwnargname3 )
void  __deleaker_GUIObject_SoftKeys_AddErrorStr( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 );
#define ProgressBar_SetText(__unknwnargname1, text) __deleaker_ProgressBar_SetText( __FILE__,  __LINE__, __unknwnargname1, text )
void  __deleaker_ProgressBar_SetText( char* __file__, int __line__, GUI* __unknwnargname1, STRID text );
#define StatusIndication_Item8_SetText(__unknwnargname1) __deleaker_StatusIndication_Item8_SetText( __FILE__,  __LINE__, __unknwnargname1 )
void  __deleaker_StatusIndication_Item8_SetText( char* __file__, int __line__, STRID __unknwnargname1 );
#define StatusIndication_SetItemText(__unknwnargname1, item, __unknwnargname3) __deleaker_StatusIndication_SetItemText( __FILE__,  __LINE__, __unknwnargname1, item, __unknwnargname3 )
void  __deleaker_StatusIndication_SetItemText( char* __file__, int __line__, GUI* __unknwnargname1, int item, STRID __unknwnargname3 );
#define StatusIndication_ShowNotes(__unknwnargname1) __deleaker_StatusIndication_ShowNotes( __FILE__,  __LINE__, __unknwnargname1 )
void  __deleaker_StatusIndication_ShowNotes( char* __file__, int __line__, STRID __unknwnargname1 );
#define StringInput_DispObject_SetText(__unknwnargname1, __unknwnargname2) __deleaker_StringInput_DispObject_SetText( __FILE__,  __LINE__, __unknwnargname1, __unknwnargname2 )
void  __deleaker_StringInput_DispObject_SetText( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, STRID __unknwnargname2 );
#define GUIObject_SoftKeys_SetInfoText(__unknwnargname1, actionID, __unknwnargname3) __deleaker_GUIObject_SoftKeys_SetInfoText( __FILE__,  __LINE__, __unknwnargname1, actionID, __unknwnargname3 )
void  __deleaker_GUIObject_SoftKeys_SetInfoText( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 );
#define ListMenu_SetSecondLineText(__unknwnargname1, elem_num, __unknwnargname3) __deleaker_ListMenu_SetSecondLineText( __FILE__,  __LINE__, __unknwnargname1, elem_num, __unknwnargname3 )
void  __deleaker_ListMenu_SetSecondLineText( char* __file__, int __line__, GUI_LIST* __unknwnargname1, int elem_num, STRID __unknwnargname3 );
#define GUIonMessage_SubItem_SetText(msg, __unknwnargname2) __deleaker_GUIonMessage_SubItem_SetText( __FILE__,  __LINE__, msg, __unknwnargname2 )
void  __deleaker_GUIonMessage_SubItem_SetText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 );
#define Feedback_SetTextExtended(__unknwnargname1, text, where) __deleaker_Feedback_SetTextExtended( __FILE__,  __LINE__, __unknwnargname1, text, where )
void  __deleaker_Feedback_SetTextExtended( char* __file__, int __line__, GUI_FEEDBACK* __unknwnargname1, STRID text, int where );
#define GUIObject_SoftKeys_SetTexts(__unknwnargname1, actionID, short_text, full_text) __deleaker_GUIObject_SoftKeys_SetTexts( __FILE__,  __LINE__, __unknwnargname1, actionID, short_text, full_text )
void  __deleaker_GUIObject_SoftKeys_SetTexts( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID short_text, STRID full_text );
#define PNUM2Name(pnum, isIconNeeded, only_full_number_flag) __deleaker_PNUM2Name( __FILE__,  __LINE__, pnum, isIconNeeded, only_full_number_flag )
STRID  __deleaker_PNUM2Name( char* __file__, int __line__, void* pnum, int isIconNeeded, int only_full_number_flag );
#define Date2ID(__unknwnargname1, TimeFormat, __unknwnargname3) __deleaker_Date2ID( __FILE__,  __LINE__, __unknwnargname1, TimeFormat, __unknwnargname3 )
int  __deleaker_Date2ID( char* __file__, int __line__, DATE* __unknwnargname1, int TimeFormat, int __unknwnargname3 );
#define Time2ID(__unknwnargname1, TimeFormat, isSec) __deleaker_Time2ID( __FILE__,  __LINE__, __unknwnargname1, TimeFormat, isSec )
int  __deleaker_Time2ID( char* __file__, int __line__, TIME* __unknwnargname1, char TimeFormat, int isSec );
#define KeyCode2Name(key_code) __deleaker_KeyCode2Name( __FILE__,  __LINE__, key_code )
STRID  __deleaker_KeyCode2Name( char* __file__, int __line__, int key_code );
#define NOfMany_SetTexts(__unknwnargname1, strids, items_count) __deleaker_NOfMany_SetTexts( __FILE__,  __LINE__, __unknwnargname1, strids, items_count )
void  __deleaker_NOfMany_SetTexts( char* __file__, int __line__, GUI_NOFMANY* __unknwnargname1, STRID* strids, int items_count );
#define OneOfMany_SetTexts(__unknwnargname1, strids, Count) __deleaker_OneOfMany_SetTexts( __FILE__,  __LINE__, __unknwnargname1, strids, Count )
void  __deleaker_OneOfMany_SetTexts( char* __file__, int __line__, GUI_ONEOFMANY* __unknwnargname1, STRID* strids, int Count );
#define FeedBack_SetText(__unknwnargname1, __unknwnargname2) __deleaker_FeedBack_SetText( __FILE__,  __LINE__, __unknwnargname1, __unknwnargname2 )
void  __deleaker_FeedBack_SetText( char* __file__, int __line__, GUI_FEEDBACK* __unknwnargname1, STRID __unknwnargname2 );
#define Gif2ID(IMAGEHANDLE, path, fname, ID) __deleaker_Gif2ID( __FILE__,  __LINE__, IMAGEHANDLE, path, fname, ID )
int  __deleaker_Gif2ID( char* __file__, int __line__, u16 IMAGEHANDLE, const wchar_t* path, const wchar_t* fname, wchar_t* ID );
#define GUIObject_SetTitleIcon(__unknwnargname1, imageID) __deleaker_GUIObject_SetTitleIcon( __FILE__,  __LINE__, __unknwnargname1, imageID )
void  __deleaker_GUIObject_SetTitleIcon( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID );
#define GUIInput_SetIcon(__unknwnargname1, icon) __deleaker_GUIInput_SetIcon( __FILE__,  __LINE__, __unknwnargname1, icon )
void  __deleaker_GUIInput_SetIcon( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t icon );
#define ProgressBar_SetIcon(__unknwnargname1, icon_id) __deleaker_ProgressBar_SetIcon( __FILE__,  __LINE__, __unknwnargname1, icon_id )
void  __deleaker_ProgressBar_SetIcon( char* __file__, int __line__, GUI* __unknwnargname1, u16 icon_id );
#define GUIonMessage_SetMenuItemIcon(msg, align, iconID) __deleaker_GUIonMessage_SetMenuItemIcon( __FILE__,  __LINE__, msg, align, iconID )
void  __deleaker_GUIonMessage_SetMenuItemIcon( char* __file__, int __line__, GUI_MESSAGE* msg, int align, wchar_t iconID );
#define TabMenuBar_SetTabIcon(__unknwnargname1, tab, icon, for_state) __deleaker_TabMenuBar_SetTabIcon( __FILE__,  __LINE__, __unknwnargname1, tab, icon, for_state )
void  __deleaker_TabMenuBar_SetTabIcon( char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab, wchar_t icon, int for_state );
#define VCALL_SetNameIcon(vc, icon) __deleaker_VCALL_SetNameIcon( __FILE__,  __LINE__, vc, icon )
void  __deleaker_VCALL_SetNameIcon( char* __file__, int __line__, void* vc, wchar_t icon );
#ifdef __cplusplus
#define ModifyKeyHook(proc, mode, lparam) __deleaker_ModifyKeyHook( __FILE__,  __LINE__, proc, mode, lparam )
int  __deleaker_ModifyKeyHook( char* __file__, int __line__, KEYHOOKPROC proc, int mode, LPARAM lparam );
int  __deleaker_ModifyKeyHook( char* __file__, int __line__, int (*proc)( int, int, int ), int mode, LPARAM lparam );
#else
int  __deleaker_ModifyKeyHook( char* __file__, int __line__, int (*proc)( int, int, int, void* ), int mode, void* lparam );
#endif
#define ModifyUIHook(event, PROC, mode) __deleaker_ModifyUIHook( __FILE__,  __LINE__, event, PROC, mode )
int  __deleaker_ModifyUIHook( char* __file__, int __line__, int event, int (*PROC)( UI_MESSAGE* ), int mode );
#define ImageID_Get(fpath, fname, imageID) __deleaker_ImageID_Get( __FILE__,  __LINE__, fpath, fname, imageID )
int  __deleaker_ImageID_Get( char* __file__, int __line__, const wchar_t* fpath, const wchar_t* fname, wchar_t* imageID );
#define ImageID_GetIndirect(buf_image, size, __NULL, image_type, imageID) __deleaker_ImageID_GetIndirect( __FILE__,  __LINE__, buf_image, size, __NULL, image_type, imageID )
int  __deleaker_ImageID_GetIndirect( char* __file__, int __line__, void* buf_image, int size, int __NULL, wchar_t* image_type, wchar_t* imageID );
#define ImageID_Free(imageID) __deleaker_ImageID_Free( __FILE__,  __LINE__, imageID )
void  __deleaker_ImageID_Free( char* __file__, int __line__, wchar_t imageID );
#define GC_CreateMemoryGC(xsize, ysize, bpp, unk, somefn, unk2) __deleaker_GC_CreateMemoryGC( __FILE__,  __LINE__, xsize, ysize, bpp, unk, somefn, unk2 )
GC*  __deleaker_GC_CreateMemoryGC( char* __file__, int __line__, int xsize, int ysize, int bpp, int unk, void* somefn, int unk2 );
#define GC_FreeGC(gc) __deleaker_GC_FreeGC( __FILE__,  __LINE__, gc )
void  __deleaker_GC_FreeGC( char* __file__, int __line__, GC* gc );
#define GVI_CreateDashedPen(thikness, color, bitmask, step, offset) __deleaker_GVI_CreateDashedPen( __FILE__,  __LINE__, thikness, color, bitmask, step, offset )
GVI_PEN  __deleaker_GVI_CreateDashedPen( char* __file__, int __line__, char thikness, int color, int bitmask, int step, int offset );
#define GVI_CreatePen(thikness, color) __deleaker_GVI_CreatePen( __FILE__,  __LINE__, thikness, color )
GVI_PEN  __deleaker_GVI_CreatePen( char* __file__, int __line__, char thikness, int color );
#define GVI_CreateSolidBrush(color) __deleaker_GVI_CreateSolidBrush( __FILE__,  __LINE__, color )
GVI_BRUSH  __deleaker_GVI_CreateSolidBrush( char* __file__, int __line__, int color );
#define GVI_CreateBitmap(xsize, ysize, bpp) __deleaker_GVI_CreateBitmap( __FILE__,  __LINE__, xsize, ysize, bpp )
GVI_BMP  __deleaker_GVI_CreateBitmap( char* __file__, int __line__, int xsize, int ysize, int bpp );
#define GVI_CreateMemoryGC(bitmap) __deleaker_GVI_CreateMemoryGC( __FILE__,  __LINE__, bitmap )
GVI_GC  __deleaker_GVI_CreateMemoryGC( char* __file__, int __line__, GVI_BMP bitmap );
#define GVI_Delete_GVI_Object(__unknwnargname1) __deleaker_GVI_Delete_GVI_Object( __FILE__,  __LINE__, __unknwnargname1 )
BOOL  __deleaker_GVI_Delete_GVI_Object( char* __file__, int __line__, GVI_OBJ* __unknwnargname1 );
#define GVI_DeleteMemoryGC(gc) __deleaker_GVI_DeleteMemoryGC( __FILE__,  __LINE__, gc )
void  __deleaker_GVI_DeleteMemoryGC( char* __file__, int __line__, GVI_GC gc );
#define TextFeedbackWindow(book, zero) __deleaker_TextFeedbackWindow( __FILE__,  __LINE__, book, zero )
GUI_FEEDBACK*  __deleaker_TextFeedbackWindow( char* __file__, int __line__, BOOK* book, int zero );
#define DataBrowserDesc_Create() __deleaker_DataBrowserDesc_Create( __FILE__,  __LINE__ )
void*  __deleaker_DataBrowserDesc_Create( char* __file__, int __line__ );
#define DataBrowserDesc_Destroy(DataBrowserDesc) __deleaker_DataBrowserDesc_Destroy( __FILE__,  __LINE__, DataBrowserDesc )
void  __deleaker_DataBrowserDesc_Destroy( char* __file__, int __line__, void* DataBrowserDesc );
#define CreateMonitorFeedback(__unknwnargname1, __unknwnargname2, onbusy, onedit, ondelete) __deleaker_CreateMonitorFeedback( __FILE__,  __LINE__, __unknwnargname1, __unknwnargname2, onbusy, onedit, ondelete )
GUI_FEEDBACK*  __deleaker_CreateMonitorFeedback( char* __file__, int __line__, STRID __unknwnargname1, BOOK* __unknwnargname2, void (*onbusy)(BOOK*), void (*onedit)(BOOK*), void (*ondelete)(BOOK*) );
#define LoadDLL(DllName) __deleaker_LoadDLL( __FILE__,  __LINE__, DllName )
void*  __deleaker_LoadDLL( char* __file__, int __line__, wchar_t* DllName );
#define UnLoadDLL(DllData) __deleaker_UnLoadDLL( __FILE__,  __LINE__, DllData )
int  __deleaker_UnLoadDLL( char* __file__, int __line__, void* DllData );
#define GUIObject_SetSecondRowTitleText(__unknwnargname1, __unknwnargname2) __deleaker_GUIObject_SetSecondRowTitleText( __FILE__,  __LINE__, __unknwnargname1, __unknwnargname2 )
void  __deleaker_GUIObject_SetSecondRowTitleText( char* __file__, int __line__, GUI* __unknwnargname1, STRID __unknwnargname2 );
#define ListMenu_SetNoItemText(__unknwnargname1, str) __deleaker_ListMenu_SetNoItemText( __FILE__,  __LINE__, __unknwnargname1, str )
void  __deleaker_ListMenu_SetNoItemText( char* __file__, int __line__, GUI_LIST* __unknwnargname1, STRID str );
#define TabMenuBar_SetTabTitle(__unknwnargname1, tab_num, __unknwnargname3) __deleaker_TabMenuBar_SetTabTitle( __FILE__,  __LINE__, __unknwnargname1, tab_num, __unknwnargname3 )
void  __deleaker_TabMenuBar_SetTabTitle( char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab_num, STRID __unknwnargname3 );
#define DispObject_SetBackgroundImage(__unknwnargname1, imageID) __deleaker_DispObject_SetBackgroundImage( __FILE__,  __LINE__, __unknwnargname1, imageID )
void  __deleaker_DispObject_SetBackgroundImage( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID );
#define DispObject_SetCursorImage(__unknwnargname1, imageID) __deleaker_DispObject_SetCursorImage( __FILE__,  __LINE__, __unknwnargname1, imageID )
void  __deleaker_DispObject_SetCursorImage( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID );
#define DispObject_SetTitleBackgroundImage(__unknwnargname1, imageID) __deleaker_DispObject_SetTitleBackgroundImage( __FILE__,  __LINE__, __unknwnargname1, imageID )
void  __deleaker_DispObject_SetTitleBackgroundImage( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID );
#define GUIObject_SetBackgroundImage(__unknwnargname1, imageID) __deleaker_GUIObject_SetBackgroundImage( __FILE__,  __LINE__, __unknwnargname1, imageID )
void  __deleaker_GUIObject_SetBackgroundImage( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID );
#define GUIObject_SetCursorImage(__unknwnargname1, imageID) __deleaker_GUIObject_SetCursorImage( __FILE__,  __LINE__, __unknwnargname1, imageID )
void  __deleaker_GUIObject_SetCursorImage( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID );
#define GUIObject_SetTitleBackgroundImage(__unknwnargname1, imageID) __deleaker_GUIObject_SetTitleBackgroundImage( __FILE__,  __LINE__, __unknwnargname1, imageID )
void  __deleaker_GUIObject_SetTitleBackgroundImage( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID );

#endif
#endif
