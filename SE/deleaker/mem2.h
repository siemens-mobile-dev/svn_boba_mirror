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
void * __deleaker_malloc(int size, char* __file__,  int __line__);
void  __deleaker_mfree(void *p, char* __file__,  int __line__);

//delete из-за delete[] макросом не переопределить, поэтому только
//включением оптимизации с галкой "function inlining"
#pragma inline=forced
inline void operator delete(void* p){ __deleaker_mfree(p, __FILE__, __LINE__); }
#pragma inline=forced
inline void operator delete[](void* p){ __deleaker_mfree(p, __FILE__, __LINE__); }

inline void* operator new(size_t size, char *file, int line){ return  __deleaker_malloc(size, file, line ); }
inline void* operator new[](size_t size, char *file, int line){ return __deleaker_malloc( size, file, line ); }

#define NEW new (__FILE__, __LINE__)
#define new NEW
/*
#pragma inline=forced
inline void* operator new(size_t size){ return  __deleaker_malloc(size, __FILE__, __LINE__ ); }
#pragma inline=forced
inline void* operator new[](size_t size){ return __deleaker_malloc( size, __FILE__, __LINE__ ); }
*/

#define malloc(size) __deleaker_malloc(size, __FILE__,  __LINE__)
void*  __deleaker_malloc(int size, char* __file__,  int __line__);
#define mfree_adr() __deleaker_mfree_adr(__FILE__,  __LINE__)
void*  __deleaker_mfree_adr(char* __file__,  int __line__);
#define mfree(p) __deleaker_mfree(p, __FILE__,  __LINE__)
void  __deleaker_mfree(void* p, char* __file__,  int __line__);
#define FreeBook(book) __deleaker_FreeBook(book, __FILE__,  __LINE__)
void  __deleaker_FreeBook(BOOK* book, char* __file__,  int __line__);
#define BookObj_KillBook(book) __deleaker_BookObj_KillBook(book, __FILE__,  __LINE__)
void  __deleaker_BookObj_KillBook(BOOK* book, char* __file__,  int __line__);
#define List_Create() __deleaker_List_Create(__FILE__,  __LINE__)
LIST*  __deleaker_List_Create(char* __file__,  int __line__);
#define List_Destroy(lst) __deleaker_List_Destroy(lst, __FILE__,  __LINE__)
void  __deleaker_List_Destroy(LIST* lst, char* __file__,  int __line__);
#define manifest_GetParam(buf, param_name, unk) __deleaker_manifest_GetParam(buf, param_name, unk, __FILE__,  __LINE__)
char*  __deleaker_manifest_GetParam(const char* buf, const char* param_name, int unk, char* __file__,  int __line__);
#define AllocDirHandle(path) __deleaker_AllocDirHandle(path, __FILE__,  __LINE__)
DIR_HANDLE*  __deleaker_AllocDirHandle(const wchar_t* path, char* __file__,  int __line__);
#define DestroyDirHandle(handle) __deleaker_DestroyDirHandle(handle, __FILE__,  __LINE__)
void  __deleaker_DestroyDirHandle(DIR_HANDLE* handle, char* __file__,  int __line__);
#define GUIObject_Destroy(__unknwnargname1) __deleaker_GUIObject_Destroy(__unknwnargname1, __FILE__,  __LINE__)
GUI*  __deleaker_GUIObject_Destroy(GUI* __unknwnargname1, char* __file__,  int __line__);
#define _fopen(filpath, filname, mode, rights, __0) __deleaker__fopen(filpath, filname, mode, rights, __0, __FILE__,  __LINE__)
int  __deleaker__fopen(const wchar_t* filpath, const wchar_t* filname, unsigned int mode, unsigned int rights, unsigned int __0, char* __file__,  int __line__);
#define fopen(fname, mode, rights) __deleaker_fopen(fname, mode, rights, __FILE__,  __LINE__)
int  __deleaker_fopen(const wchar_t* fname, int mode, int rights, char* __file__,  int __line__);
#define fclose(file) __deleaker_fclose(file, __FILE__,  __LINE__)
int  __deleaker_fclose(int file, char* __file__,  int __line__);
#define w_fopen(name, attr, rights, err) __deleaker_w_fopen(name, attr, rights, err, __FILE__,  __LINE__)
int  __deleaker_w_fopen(const wchar_t* name, int attr, int rights, int err, char* __file__,  int __line__);
#define w_fclose(f) __deleaker_w_fclose(f, __FILE__,  __LINE__)
int  __deleaker_w_fclose(int f, char* __file__,  int __line__);
#define TabMenuBar_SetTabGui(__unknwnargname1, tab, __unknwnargname3) __deleaker_TabMenuBar_SetTabGui(__unknwnargname1, tab, __unknwnargname3, __FILE__,  __LINE__)
void  __deleaker_TabMenuBar_SetTabGui(GUI_TABMENUBAR* __unknwnargname1, int tab, GUI* __unknwnargname3, char* __file__,  int __line__);
#define CreateListMenu(__unknwnargname1, display) __deleaker_CreateListMenu(__unknwnargname1, display, __FILE__,  __LINE__)
GUI_LIST*  __deleaker_CreateListMenu(BOOK* __unknwnargname1, int display, char* __file__,  int __line__);
#define CreateNOfMany(book) __deleaker_CreateNOfMany(book, __FILE__,  __LINE__)
GUI_NOFMANY*  __deleaker_CreateNOfMany(BOOK* book, char* __file__,  int __line__);
#define CreateOneOfMany(book) __deleaker_CreateOneOfMany(book, __FILE__,  __LINE__)
GUI_ONEOFMANY*  __deleaker_CreateOneOfMany(BOOK* book, char* __file__,  int __line__);
#define CreateTabMenuBar(book) __deleaker_CreateTabMenuBar(book, __FILE__,  __LINE__)
GUI_TABMENUBAR*  __deleaker_CreateTabMenuBar(BOOK* book, char* __file__,  int __line__);
#define CreateProgressBar(book, display) __deleaker_CreateProgressBar(book, display, __FILE__,  __LINE__)
GUI*  __deleaker_CreateProgressBar(BOOK* book, int display, char* __file__,  int __line__);
#define Str2ID(wstr, flag, len) __deleaker_Str2ID(wstr, flag, len, __FILE__,  __LINE__)
STRID  __deleaker_Str2ID(const void* wstr, int flag, int len, char* __file__,  int __line__);
#define TextCopyId(__unknwnargname1) __deleaker_TextCopyId(__unknwnargname1, __FILE__,  __LINE__)
int  __deleaker_TextCopyId(STRID __unknwnargname1, char* __file__,  int __line__);
#define int2strID(num) __deleaker_int2strID(num, __FILE__,  __LINE__)
STRID  __deleaker_int2strID(int num, char* __file__,  int __line__);
#define TextFree(__unknwnargname1) __deleaker_TextFree(__unknwnargname1, __FILE__,  __LINE__)
void  __deleaker_TextFree(STRID __unknwnargname1, char* __file__,  int __line__);
#define GUIObject_SetTitleText(__unknwnargname1, __unknwnargname2) __deleaker_GUIObject_SetTitleText(__unknwnargname1, __unknwnargname2, __FILE__,  __LINE__)
void  __deleaker_GUIObject_SetTitleText(GUI* __unknwnargname1, STRID __unknwnargname2, char* __file__,  int __line__);
#define GUIObject_SoftKeys_SetText(__unknwnargname1, actionID, __unknwnargname3) __deleaker_GUIObject_SoftKeys_SetText(__unknwnargname1, actionID, __unknwnargname3, __FILE__,  __LINE__)
void  __deleaker_GUIObject_SoftKeys_SetText(GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3, char* __file__,  int __line__);
#define GUIonMessage_SetMenuItemText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemText(msg, __unknwnargname2, __FILE__,  __LINE__)
char  __deleaker_GUIonMessage_SetMenuItemText(GUI_MESSAGE* msg, STRID __unknwnargname2, char* __file__,  int __line__);
#define GUIonMessage_SetMenuItemSecondLineText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemSecondLineText(msg, __unknwnargname2, __FILE__,  __LINE__)
char  __deleaker_GUIonMessage_SetMenuItemSecondLineText(GUI_MESSAGE* msg, STRID __unknwnargname2, char* __file__,  int __line__);
#define GUIonMessage_SetMenuItemInfoText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemInfoText(msg, __unknwnargname2, __FILE__,  __LINE__)
char  __deleaker_GUIonMessage_SetMenuItemInfoText(GUI_MESSAGE* msg, STRID __unknwnargname2, char* __file__,  int __line__);
#define GUIonMessage_SetMenuItemUnavailableText(msg, __unknwnargname2) __deleaker_GUIonMessage_SetMenuItemUnavailableText(msg, __unknwnargname2, __FILE__,  __LINE__)
char  __deleaker_GUIonMessage_SetMenuItemUnavailableText(GUI_MESSAGE* msg, STRID __unknwnargname2, char* __file__,  int __line__);
#define MessageBox(header_text, message_text, IconID, style, time, book) __deleaker_MessageBox(header_text, message_text, IconID, style, time, book, __FILE__,  __LINE__)
int  __deleaker_MessageBox(STRID header_text, STRID message_text, wchar_t IconID, int style, int time, BOOK* book, char* __file__,  int __line__);
#define BookObj_SoftKeys_SetText(book, actionID, __unknwnargname3) __deleaker_BookObj_SoftKeys_SetText(book, actionID, __unknwnargname3, __FILE__,  __LINE__)
void  __deleaker_BookObj_SoftKeys_SetText(BOOK* book, int actionID, STRID __unknwnargname3, char* __file__,  int __line__);
#define DataBrowserDesc_SetHeaderText(DataBrowserDesc, __unknwnargname2) __deleaker_DataBrowserDesc_SetHeaderText(DataBrowserDesc, __unknwnargname2, __FILE__,  __LINE__)
void  __deleaker_DataBrowserDesc_SetHeaderText(void* DataBrowserDesc, STRID __unknwnargname2, char* __file__,  int __line__);
#define DataBrowserDesc_SetOKSoftKeyText(DataBrowserDesc, __unknwnargname2) __deleaker_DataBrowserDesc_SetOKSoftKeyText(DataBrowserDesc, __unknwnargname2, __FILE__,  __LINE__)
void  __deleaker_DataBrowserDesc_SetOKSoftKeyText(void* DataBrowserDesc, STRID __unknwnargname2, char* __file__,  int __line__);
#define DispObject_SetTitleText(__unknwnargname1, __unknwnargname2) __deleaker_DispObject_SetTitleText(__unknwnargname1, __unknwnargname2, __FILE__,  __LINE__)
void  __deleaker_DispObject_SetTitleText(DISP_OBJ* __unknwnargname1, STRID __unknwnargname2, char* __file__,  int __line__);
#define GUIObject_SoftKeys_AddErrorStr(__unknwnargname1, actionID, __unknwnargname3) __deleaker_GUIObject_SoftKeys_AddErrorStr(__unknwnargname1, actionID, __unknwnargname3, __FILE__,  __LINE__)
void  __deleaker_GUIObject_SoftKeys_AddErrorStr(GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3, char* __file__,  int __line__);
#define ProgressBar_SetText(__unknwnargname1, text) __deleaker_ProgressBar_SetText(__unknwnargname1, text, __FILE__,  __LINE__)
void  __deleaker_ProgressBar_SetText(GUI* __unknwnargname1, STRID text, char* __file__,  int __line__);
#define StatusIndication_Item8_SetText(__unknwnargname1) __deleaker_StatusIndication_Item8_SetText(__unknwnargname1, __FILE__,  __LINE__)
void  __deleaker_StatusIndication_Item8_SetText(STRID __unknwnargname1, char* __file__,  int __line__);
#define StatusIndication_SetItemText(__unknwnargname1, item, __unknwnargname3) __deleaker_StatusIndication_SetItemText(__unknwnargname1, item, __unknwnargname3, __FILE__,  __LINE__)
void  __deleaker_StatusIndication_SetItemText(GUI* __unknwnargname1, int item, STRID __unknwnargname3, char* __file__,  int __line__);
#define StatusIndication_ShowNotes(__unknwnargname1) __deleaker_StatusIndication_ShowNotes(__unknwnargname1, __FILE__,  __LINE__)
void  __deleaker_StatusIndication_ShowNotes(STRID __unknwnargname1, char* __file__,  int __line__);
#define StringInput_DispObject_SetText(__unknwnargname1, __unknwnargname2) __deleaker_StringInput_DispObject_SetText(__unknwnargname1, __unknwnargname2, __FILE__,  __LINE__)
void  __deleaker_StringInput_DispObject_SetText(DISP_OBJ* __unknwnargname1, STRID __unknwnargname2, char* __file__,  int __line__);
#define GUIObject_SoftKeys_SetInfoText(__unknwnargname1, actionID, __unknwnargname3) __deleaker_GUIObject_SoftKeys_SetInfoText(__unknwnargname1, actionID, __unknwnargname3, __FILE__,  __LINE__)
void  __deleaker_GUIObject_SoftKeys_SetInfoText(GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3, char* __file__,  int __line__);
#define ListMenu_SetSecondLineText(__unknwnargname1, elem_num, __unknwnargname3) __deleaker_ListMenu_SetSecondLineText(__unknwnargname1, elem_num, __unknwnargname3, __FILE__,  __LINE__)
void  __deleaker_ListMenu_SetSecondLineText(GUI_LIST* __unknwnargname1, int elem_num, STRID __unknwnargname3, char* __file__,  int __line__);
#define GUIonMessage_SubItem_SetText(msg, __unknwnargname2) __deleaker_GUIonMessage_SubItem_SetText(msg, __unknwnargname2, __FILE__,  __LINE__)
void  __deleaker_GUIonMessage_SubItem_SetText(GUI_MESSAGE* msg, STRID __unknwnargname2, char* __file__,  int __line__);
#define Feedback_SetTextExtended(__unknwnargname1, text, where) __deleaker_Feedback_SetTextExtended(__unknwnargname1, text, where, __FILE__,  __LINE__)
void  __deleaker_Feedback_SetTextExtended(GUI_FEEDBACK* __unknwnargname1, STRID text, int where, char* __file__,  int __line__);
#define GUIObject_SoftKeys_SetTexts(__unknwnargname1, actionID, short_text, full_text) __deleaker_GUIObject_SoftKeys_SetTexts(__unknwnargname1, actionID, short_text, full_text, __FILE__,  __LINE__)
void  __deleaker_GUIObject_SoftKeys_SetTexts(GUI* __unknwnargname1, u16 actionID, STRID short_text, STRID full_text, char* __file__,  int __line__);
#define PNUM2Name(pnum, isIconNeeded, only_full_number_flag) __deleaker_PNUM2Name(pnum, isIconNeeded, only_full_number_flag, __FILE__,  __LINE__)
STRID  __deleaker_PNUM2Name(void* pnum, int isIconNeeded, int only_full_number_flag, char* __file__,  int __line__);
#define Date2ID(__unknwnargname1, TimeFormat, __unknwnargname3) __deleaker_Date2ID(__unknwnargname1, TimeFormat, __unknwnargname3, __FILE__,  __LINE__)
int  __deleaker_Date2ID(DATE* __unknwnargname1, int TimeFormat, int __unknwnargname3, char* __file__,  int __line__);
#define Time2ID(__unknwnargname1, TimeFormat, isSec) __deleaker_Time2ID(__unknwnargname1, TimeFormat, isSec, __FILE__,  __LINE__)
int  __deleaker_Time2ID(TIME* __unknwnargname1, char TimeFormat, int isSec, char* __file__,  int __line__);
#define KeyCode2Name(key_code) __deleaker_KeyCode2Name(key_code, __FILE__,  __LINE__)
STRID  __deleaker_KeyCode2Name(int key_code, char* __file__,  int __line__);
#define NOfMany_SetTexts(__unknwnargname1, strids, items_count) __deleaker_NOfMany_SetTexts(__unknwnargname1, strids, items_count, __FILE__,  __LINE__)
void  __deleaker_NOfMany_SetTexts(GUI_NOFMANY* __unknwnargname1, STRID* strids, int items_count, char* __file__,  int __line__);
#define OneOfMany_SetTexts(__unknwnargname1, strids, Count) __deleaker_OneOfMany_SetTexts(__unknwnargname1, strids, Count, __FILE__,  __LINE__)
void  __deleaker_OneOfMany_SetTexts(GUI_ONEOFMANY* __unknwnargname1, STRID* strids, int Count, char* __file__,  int __line__);
#define FeedBack_SetText(__unknwnargname1, __unknwnargname2) __deleaker_FeedBack_SetText(__unknwnargname1, __unknwnargname2, __FILE__,  __LINE__)
void  __deleaker_FeedBack_SetText(GUI_FEEDBACK* __unknwnargname1, STRID __unknwnargname2, char* __file__,  int __line__);
#define Gif2ID(IMAGEHANDLE, path, fname, ID) __deleaker_Gif2ID(IMAGEHANDLE, path, fname, ID, __FILE__,  __LINE__)
int  __deleaker_Gif2ID(u16 IMAGEHANDLE, const wchar_t* path, const wchar_t* fname, wchar_t* ID, char* __file__,  int __line__);
#define GUIObject_SetTitleIcon(__unknwnargname1, imageID) __deleaker_GUIObject_SetTitleIcon(__unknwnargname1, imageID, __FILE__,  __LINE__)
void  __deleaker_GUIObject_SetTitleIcon(GUI* __unknwnargname1, wchar_t imageID, char* __file__,  int __line__);
#define GUIInput_SetIcon(__unknwnargname1, icon) __deleaker_GUIInput_SetIcon(__unknwnargname1, icon, __FILE__,  __LINE__)
void  __deleaker_GUIInput_SetIcon(GUI* __unknwnargname1, wchar_t icon, char* __file__,  int __line__);
#define ProgressBar_SetIcon(__unknwnargname1, icon_id) __deleaker_ProgressBar_SetIcon(__unknwnargname1, icon_id, __FILE__,  __LINE__)
void  __deleaker_ProgressBar_SetIcon(GUI* __unknwnargname1, u16 icon_id, char* __file__,  int __line__);
#define GUIonMessage_SetMenuItemIcon(msg, align, iconID) __deleaker_GUIonMessage_SetMenuItemIcon(msg, align, iconID, __FILE__,  __LINE__)
void  __deleaker_GUIonMessage_SetMenuItemIcon(GUI_MESSAGE* msg, int align, wchar_t iconID, char* __file__,  int __line__);
#define TabMenuBar_SetTabIcon(__unknwnargname1, tab, icon, for_state) __deleaker_TabMenuBar_SetTabIcon(__unknwnargname1, tab, icon, for_state, __FILE__,  __LINE__)
void  __deleaker_TabMenuBar_SetTabIcon(GUI_TABMENUBAR* __unknwnargname1, int tab, wchar_t icon, int for_state, char* __file__,  int __line__);
#define VCALL_SetNameIcon(vc, icon) __deleaker_VCALL_SetNameIcon(vc, icon, __FILE__,  __LINE__)
void  __deleaker_VCALL_SetNameIcon(void* vc, wchar_t icon, char* __file__,  int __line__);
#define ModifyKeyHook(proc, mode) __deleaker_ModifyKeyHook(proc, mode, __FILE__,  __LINE__)
int  __deleaker_ModifyKeyHook(int (*proc)( int, int, int ), int mode, char* __file__,  int __line__);
#define ModifyUIHook(event, PROC, mode) __deleaker_ModifyUIHook(event, PROC, mode, __FILE__,  __LINE__)
int  __deleaker_ModifyUIHook(int event, int (*PROC)( UI_MESSAGE* ), int mode, char* __file__,  int __line__);
#define ImageID_Get(fpath, fname, imageID) __deleaker_ImageID_Get(fpath, fname, imageID, __FILE__,  __LINE__)
int  __deleaker_ImageID_Get(const wchar_t* fpath, const wchar_t* fname, wchar_t* imageID, char* __file__,  int __line__);
#define ImageID_GetIndirect(buf_image, size, __NULL, image_type, imageID) __deleaker_ImageID_GetIndirect(buf_image, size, __NULL, image_type, imageID, __FILE__,  __LINE__)
int  __deleaker_ImageID_GetIndirect(void* buf_image, int size, int __NULL, wchar_t* image_type, wchar_t* imageID, char* __file__,  int __line__);
#define ImageID_Free(imageID) __deleaker_ImageID_Free(imageID, __FILE__,  __LINE__)
void  __deleaker_ImageID_Free(wchar_t imageID, char* __file__,  int __line__);
#define GC_CreateMemoryGC(xsize, ysize, bpp, unk, somefn, unk2) __deleaker_GC_CreateMemoryGC(xsize, ysize, bpp, unk, somefn, unk2, __FILE__,  __LINE__)
GC*  __deleaker_GC_CreateMemoryGC(int xsize, int ysize, int bpp, int unk, void* somefn, int unk2, char* __file__,  int __line__);
#define GC_FreeGC(gc) __deleaker_GC_FreeGC(gc, __FILE__,  __LINE__)
void  __deleaker_GC_FreeGC(GC* gc, char* __file__,  int __line__);
#define GVI_CreateDashedPen(thikness, color, bitmask, step, offset) __deleaker_GVI_CreateDashedPen(thikness, color, bitmask, step, offset, __FILE__,  __LINE__)
GVI_PEN  __deleaker_GVI_CreateDashedPen(char thikness, int color, int bitmask, int step, int offset, char* __file__,  int __line__);
#define GVI_CreatePen(thikness, color) __deleaker_GVI_CreatePen(thikness, color, __FILE__,  __LINE__)
GVI_PEN  __deleaker_GVI_CreatePen(char thikness, int color, char* __file__,  int __line__);
#define GVI_CreateSolidBrush(color) __deleaker_GVI_CreateSolidBrush(color, __FILE__,  __LINE__)
GVI_BRUSH  __deleaker_GVI_CreateSolidBrush(int color, char* __file__,  int __line__);
#define GVI_CreateBitmap(xsize, ysize, bpp) __deleaker_GVI_CreateBitmap(xsize, ysize, bpp, __FILE__,  __LINE__)
GVI_BMP  __deleaker_GVI_CreateBitmap(int xsize, int ysize, int bpp, char* __file__,  int __line__);
#define GVI_CreateMemoryGC(bitmap) __deleaker_GVI_CreateMemoryGC(bitmap, __FILE__,  __LINE__)
GVI_GC  __deleaker_GVI_CreateMemoryGC(GVI_BMP bitmap, char* __file__,  int __line__);
#define GVI_Delete_GVI_Object(__unknwnargname1) __deleaker_GVI_Delete_GVI_Object(__unknwnargname1, __FILE__,  __LINE__)
BOOL  __deleaker_GVI_Delete_GVI_Object(GVI_OBJ* __unknwnargname1, char* __file__,  int __line__);
#define GVI_DeleteMemoryGC(gc) __deleaker_GVI_DeleteMemoryGC(gc, __FILE__,  __LINE__)
void  __deleaker_GVI_DeleteMemoryGC(GVI_GC gc, char* __file__,  int __line__);
#define TextFeedbackWindow(book, zero) __deleaker_TextFeedbackWindow(book, zero, __FILE__,  __LINE__)
GUI_FEEDBACK*  __deleaker_TextFeedbackWindow(BOOK* book, int zero, char* __file__,  int __line__);
#define DataBrowserDesc_Create() __deleaker_DataBrowserDesc_Create(__FILE__,  __LINE__)
void*  __deleaker_DataBrowserDesc_Create(char* __file__,  int __line__);
#define DataBrowserDesc_Destroy(DataBrowserDesc) __deleaker_DataBrowserDesc_Destroy(DataBrowserDesc, __FILE__,  __LINE__)
void  __deleaker_DataBrowserDesc_Destroy(void* DataBrowserDesc, char* __file__,  int __line__);
#define CreateMonitorFeedback(__unknwnargname1, __unknwnargname2, onbusy, onedit, ondelete) __deleaker_CreateMonitorFeedback(__unknwnargname1, __unknwnargname2, onbusy, onedit, ondelete, __FILE__,  __LINE__)
GUI_FEEDBACK*  __deleaker_CreateMonitorFeedback(STRID __unknwnargname1, BOOK* __unknwnargname2, void (*onbusy)(BOOK*), void (*onedit)(BOOK*), void (*ondelete)(BOOK*), char* __file__,  int __line__);

#endif
#endif
