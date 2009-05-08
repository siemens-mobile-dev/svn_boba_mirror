#ifndef __INCLUDE_MEM__
#define __INCLUDE_MEM__

#include "..\include\Lib_Clara.h"

#ifdef NDEBUG

#define trace_init()
#define trace_done()

#else
//-DEBUG------------------------------------------------------------------------
#pragma swi_number=0x103
__swi __arm  void *old_malloc_internal(int size);

enum trace_types
{
  trace_memory,
  trace_strid,
  trace_iconid,
//  trace_timer,
  trace_file,
  
  trace_typescount
};

void trace_init();
void trace_done();

int   trace_alloc_i(trace_types mt,int invalid, int i,char* file,int line);
void *trace_alloc(trace_types mt,void* invalid,void* p,char* file,int line);
int   trace_free_i(trace_types mt,int p);
void *trace_free(trace_types mt,void* p);
void *mfree_adr_(void);

//__swi __arm  void * GetUserData (int size,void (*constr)(void *, void *), void *constr_param);
//__swi __arm  int RemoveUserData(void (*constr)(void *, void *));

#define mfree_adr() mfree_adr_()

#define malloc(s)     trace_alloc( trace_memory,NULL, malloc(s),__FILE__,__LINE__ )
#define mfree(p)      mfree( trace_free( trace_memory, p ) )

inline void operator delete(void* p){mfree(p);}
inline void* operator new(size_t size, char *file, int line){ return trace_alloc( trace_memory, NULL, old_malloc_internal(size), file, line); }
inline void* operator new[](size_t size, char *file, int line){ return trace_alloc( trace_memory, NULL, old_malloc_internal(size), file, line);}
#define NEW new (__FILE__, __LINE__)
#define new NEW

#define List_New()    (LIST *) trace_alloc( trace_memory, NULL, List_New(), __FILE__, __LINE__ )
#define List_Free(l)  List_Free( (LIST *) trace_free( trace_memory, l) )

#define FreeBook(b)   FreeBook( (BOOK*) trace_free( trace_memory, b) )

#define manifest_GetParam(a,b,c)  (char*)trace_alloc( trace_memory, NULL, manifest_GetParam(a,b,c), __FILE__, __LINE__ )

#define AllocDirHandle(p) (DIR_HANDLE*)trace_alloc( trace_memory, NULL, AllocDirHandle(p), __FILE__, __LINE__ )
#define DestroyDirHandle(p) DestroyDirHandle((DIR_HANDLE*)trace_free(trace_memory, p) )
#define GUI_Free(p)   GUI_Free((GUI*) trace_free( trace_memory, p ))
/*
#ifdef __cplusplus
GUI* trace_free(trace_types mt,GUI* p);
GUI_LIST* trace_free(trace_types mt,GUI_LIST* p);
GUI_ONEOFMANY* trace_free(trace_types mt,GUI_ONEOFMANY* p);
GUI_NOFMANY* trace_free(trace_types mt,GUI_NOFMANY* p);
GUI_FEEDBACK* trace_free(trace_types mt,GUI_FEEDBACK* p);
#endif
#define GUI_Free(p)   GUI_Free( trace_free( trace_memory, p ))

#define CreateListObject(b,u) (GUI_LIST*)trace_alloc( trace_memory, NULL, CreateListObject(b,u), __FILE__, __LINE__ )
#define CreateNOfMany(b)    (GUI_NOFMANY*)trace_alloc( trace_memory, NULL, CreateNOfMany(b), __FILE__, __LINE__ )
#define CreateOneOfMany(b)  (GUI_ONEOFMANY*)trace_alloc( trace_memory, NULL, CreateOneOfMany(b), __FILE__, __LINE__ )
#define CreateTabMenuBar(b) (GUI_TABMENUBAR*)trace_alloc( trace_memory, NULL, CreateTabMenuBar(b), __FILE__, __LINE__ )
#define CreateProgressBar(b) (GUI*)trace_alloc( trace_memory, NULL, CreateProgressBar(b), __FILE__, __LINE__ )
*/

//__swi __arm GUI* CreateDateInputVA(int zero, ...);
//__swi __arm GUI* CreateStringInput(int,...);
//__swi __arm GUI* CreateTimeInputVA(int zero, ...);
//__swi __arm GUI* CreateYesNoQuestionVA(int zero, ...);
//__swi __arm GUI_FEEDBACK* CreateFeedBack (int,...);

//#define Timer_Set(a,b,c) (u16)trace_alloc_i(trace_timer, 0, Timer_Set(a,b,c),__FILE__,__LINE__ )
//#define Timer_Kill(t) Timer_Kill( trace_free_p16( trace_timer, t ) )

#define _fopen(a,b,c,d,e) trace_alloc_i(trace_file, -1, _fopen(a,b,c,d,e), __FILE__,__LINE__ )
#define fclose(a)     fclose(trace_free_i(trace_file,a))

//????
#define w_fopen(n,a,r,e) trace_alloc_i(trace_file, -1, w_fopen(n,a,r,e), __FILE__, __LINE__ )
#define w_fclose(a)     w_fclose(trace_free_i(trace_file,a))

//__swi __arm ACTION* ActionCreate(int (*PROC)(void* msg,BOOK*), int BookID, u16 event, APP_DESC* app_desc, PAGE_DESC* pag_desc);
//__swi __arm char SetMenuItemText0 ( GUI_MESSAGE* , int StrID);
//__swi __arm char SetMenuItemText1 ( GUI_MESSAGE* , int StrID);
//__swi __arm char SetMenuItemText2 ( GUI_MESSAGE* , int StrID);
//__swi __arm char SetMenuItemText3 ( GUI_MESSAGE* , int StrID);
//__swi __arm FILEITEM* FILEITEM_Create(void);
//__swi __arm FILEITEM* FILEITEM_CreateCopy (FILEITEM* );
//__swi __arm int AB_NAME_ITEM2StrID(void* ab_name);
//__swi __arm int AB_NUM_ITEM2StrID(void* ab_num_item);
//__swi __arm int CreateSMSCont (int, void*);
//__swi __arm int CteateNewMessage (int,void*,int);
//__swi __arm int Date2ID(DATE* , int TimeFormat,int);
//__swi __arm int fopen (const wchar_t* fname, int mode, int rights);
//__swi __arm int FreeMessage(void** Mess);
//__swi __arm int FreeSMSCont (void*);
//__swi __arm int Gif2ID(u16 IMAGEHANDLE,const wchar_t* path, const wchar_t* fname,wchar_t* ID);
//__swi __arm int ImageID_Get(const wchar_t* fpath,const wchar_t* fname,wchar_t* imageID);
//__swi __arm int ImageID_GetIndirect(void* buf_image,int size,int __NULL,wchar_t* image_type,wchar_t* imageID);
//__swi __arm int int2strID (int num);
//__swi __arm int JavaDialog_Open(int unk1,char* unk2,void** JavaDesc);
//__swi __arm int ModifyKeyHook(int(*proc)(int,int,int),int mode);
//__swi __arm int ModifyUIHook(int event , int (*PROC)(UI_MESSAGE*),int mode);
//__swi __arm int PrepareSMSCont (void*,wchar_t*,wchar_t*,int,int);
//__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_GETHANDLE(const int* __zero,u16* IMAGEHANDLE,char* unk);
//__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_REGISTER(const int* __zero,u16 ImageHandle,wchar_t* path,wchar_t* fname,int unk, wchar_t* ImageID ,char* error);
//__swi __arm int REQUEST_IMAGEHANDLER_INTERNAL_UNREGISTER (const int* __zero,u16 ImageHandle,u16* ,u16* ,wchar_t ImageID, int unk_1,char* error);
//__swi __arm int Str2ID(const void* wstr , int flag , int len);
//__swi __arm int TextCopyId(int);
//__swi __arm int Time2ID(TIME* , char TimeFormat,int);
//__swi __arm PROCESS create_process(int proc_type,char* name,OSENTRYPOINT* entrypoint,OSADDRESS stack_size,OSPRIORITY priority,OSTIME timeslice,PROCESS pid_block,void* redir_table,OSVECTOR vector,OSUSER user);
//__swi __arm SUB_EXECUTE* BrowserItem_Get_SUB_EXECUTE(BOOK* BrowserItemBook);
//__swi __arm SUB_EXECUTE* DataBrowser_CreateSubExecute(int BookID, FILEITEM* f);
//__swi __arm union SIGNAL* alloc(OSBUFSIZE size,SIGSELECT signo);
//__swi __arm union SIGNAL* receive(const SIGSELECT* sigsel);
//__swi __arm void BookObj_CallPage(BOOK* book, const PAGE_DESC* page);
//__swi __arm void BookObj_GotoPage(BOOK* book, const PAGE_DESC* page);
//__swi __arm void BookObj_KillBook(void* Book);
//__swi __arm void BookObj_ReturnPage(BOOK* ,int);
//__swi __arm void BookObj_Softkey_SetText(BOOK* ,int actionID,int StrID);
//__swi __arm void DataBrowserDesc_Destroy(void* db);
//__swi __arm void DataBrowserDesc_SetHeaderText(void* db , int StrID);
//__swi __arm void DataBrowserDesc_SetOKSoftKeyText(void* db , int StrID);
//__swi __arm void DispObject_KillRefreshTimer(DISP_OBJ* );
//__swi __arm void DispObject_SetRefreshTimer(DISP_OBJ* ,int time);
//__swi __arm void FeedBack_SetText(GUI_FEEDBACK*,int);
//__swi __arm void FILEITEM_Destroy(FILEITEM* );
//__swi __arm void free_buf(union SIGNAL** sig);
//__swi __arm void GuiObject_SetBackgroundImage(void* gui, wchar_t imageID);
//__swi __arm void GUIObject_SetHeaderText(void* gui , int StrID);
//__swi __arm void GuiObject_SetHighlightImage(void* gui, wchar_t imageID);
//__swi __arm void GuiObject_SetTitleImage(void* gui, wchar_t imageID);
//__swi __arm void GuiObject_SetTitleText (void* gui , int StrID);
//__swi __arm void GUIObject_SoftKey_AddErrorStr(void* gui,u16 actionID,int StrID);
//__swi __arm void GUIObject_Softkey_SetText(void* gui,u16 actionID , int StrID);
//__swi __arm void ImageID_Free(wchar_t imageID);
//__swi __arm void InputFeedback_SetIcon(GUI* gui, wchar_t icon);
//__swi __arm void JavaDialog_Close(int unk1);
//__swi __arm void kill_proc(PROCESS pid);
//__swi __arm void List2lineSubitemAddText(void* msg,int StrID);
//__swi __arm void ListMenu_SetNoItemText(GUI_LIST* , int str);
//__swi __arm void ListMenu_SetSecondLineText(GUI_LIST* ,int elem_num,int StrID);
//__swi __arm void List_FreeElements(LIST* ,int (*cmp_proc)(void* elem_from_list),void (*freefunc)(void* elem_from_list));
//__swi __arm void MetaData_Desc_Destroy(void* MetaData_Desc);
//__swi __arm void NOfMany_SetTexts(GUI_NOFMANY* , int* StrIDtable, int items_count);
//__swi __arm void OneOfMany_SetTexts(GUI_ONEOFMANY* , int* StrID , int Count);
//__swi __arm void ProgressBar_SetIcon(GUI*,u16 icon_id);
//__swi __arm void ProgressBar_SetText(GUI*,STRID text);
//__swi __arm void send(union SIGNAL** sig,PROCESS to);
//__swi __arm void SetListObjectItemIcon ( void* msg , int align , wchar_t iconID);
//__swi __arm void SoftKeys_SetText(void* gui,int msg,int StrID,int StrID1);
//__swi __arm void SoftKey_AddHelpStr(void* gui,int msg,int StrID);
//__swi __arm void StatusIndication_Item8_SetText(int StrID);
//__swi __arm void StatusIndication_SetItemText(void* gui,int item,int StrID);
//__swi __arm void StatusIndication_ShowNotes(int StrID);
//__swi __arm void StringInput_DispObject_SetText(DISP_OBJ* ,int StrID);
//__swi __arm void TabMenuBar_SetTabIcon(GUI_TABMENUBAR* ,int tab, wchar_t icon , int for_state);
//__swi __arm void TextFree(int StrID);
//__swi __arm void VCALL_SetNameIcon (void* vc , wchar_t icon);
//__swi __arm void* CreateMessage(int size,int ev,char* name);
//__swi __arm void* DataBrowserDesc_Create(void);
//__swi __arm void* FILEITEM_Destroy_adr (void);
//__swi __arm void* MetaData_Desc_Create(wchar_t* path,wchar_t* name);
//__swi __arm void GuiObject_SetTitleIcon(void* gui, wchar_t imageID);

/*
__swi __arm char* GetIMSI(void);
*/

#endif

#endif
