#ifdef NDEBUG

#include "..\include\Lib_Clara.h"

void* operator new(size_t sz){return malloc(sz);};
void* operator new[](size_t sz){return malloc(sz);};
void operator delete(void * p){mfree(p);};

#else

#include "mem2.h"
#include "..\include\dir.h"
#include "lib_clara_original.h"


enum trace_types
{
  trace_memory,
  trace_strid,
  trace_iconid,
  trace_timer,
  trace_file,
  trace_hook,
  trace_dll,

  trace_unallocated,

  trace_typescount
};

static char* leaktypes[]={
  "memory/book/gui/gc",
  "strid",
  "iconid",
  "timer",
  "file",
  "hook",
  "dll",

  "unallocated"
};

static bool started=false;

static LIST* buffers[trace_typescount];

#define LISTDATACOUNT 3


void trace_init()
{
  for(int i=0;i<trace_typescount;i++)
    buffers[i]=__original_List_Create();
  started=true;
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

void trace_alloc(int mt, void* ptr, char *file, int line)
{
    __original_List_InsertLast(buffers[mt],ptr);
    __original_List_InsertLast(buffers[mt],file);
    __original_List_InsertLast(buffers[mt],(void*)line);
}

void* trace_alloc_ret(int mt, void* ptr, void* badvalue,char *file, int line)
{
  if( ptr != badvalue )
     trace_alloc( mt, ptr, file, line);
  return ptr;
}


void trace_free(int mt,void* p, char* file, int line)
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
      __original_List_InsertLast(buffers[trace_unallocated],file);
      __original_List_InsertLast(buffers[trace_unallocated],(void*)line);
    }
  }
}


bool isallocatedstrid(int strid)
{
  return strid!=EMPTY_SID && (strid&0xFFFF0000)!=0;
}

bool isallocatediconid(int strid)
{
//  return strid!=0xffff && (strid&0xFFFF0000)!=0;
  return false;//!!!
}

//---------------------------------------------------------------------------





void*  __deleaker_malloc( char* __file__, int __line__, int size )
{
  void*  ret = __original_malloc(size);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

void mfree_(void* p)
{
  mfree(p);
}

void*  __deleaker_mfree_adr( char* __file__, int __line__ )
{
  return (void*)mfree_;
}

void  __deleaker_mfree( char* __file__, int __line__, void* p )
{
  trace_free(trace_memory, p, __file__, __line__);
  __original_mfree(p);
}

void  __deleaker_FreeBook( char* __file__, int __line__, BOOK* book )
{
  trace_free(trace_memory, book, __file__, __line__);
  __original_FreeBook(book);
}

void  __deleaker_BookObj_KillBook( char* __file__, int __line__, BOOK* book )
{
  trace_free(trace_memory, book, __file__, __line__);
  __original_BookObj_KillBook(book);
}

LIST*  __deleaker_List_Create( char* __file__, int __line__ )
{
  LIST*  ret = __original_List_Create();
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

void  __deleaker_List_Destroy( char* __file__, int __line__, LIST* lst )
{
  trace_free(trace_memory, lst, __file__, __line__);
  __original_List_Destroy(lst);
}

char*  __deleaker_manifest_GetParam( char* __file__, int __line__, const char* buf, const char* param_name, int unk )
{
  char*  ret = __original_manifest_GetParam(buf, param_name, unk);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

DIR_HANDLE*  __deleaker_AllocDirHandle( char* __file__, int __line__, const wchar_t* path )
{
  DIR_HANDLE*  ret = __original_AllocDirHandle(path);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

void  __deleaker_DestroyDirHandle( char* __file__, int __line__, DIR_HANDLE* handle )
{
  trace_free(trace_memory, handle, __file__, __line__);
  __original_DestroyDirHandle(handle);
}

GUI*  __deleaker_GUIObject_Destroy( char* __file__, int __line__, GUI* __unknwnargname1 )
{
  //освобождает или просто возвращает?
  trace_free(trace_memory, __unknwnargname1, __file__, __line__);
  return __original_GUIObject_Destroy(__unknwnargname1);
}

int  __deleaker__fopen( char* __file__, int __line__, const wchar_t* filpath, const wchar_t* filname, unsigned int mode, unsigned int rights, unsigned int __0 )
{
  int  ret = __original__fopen(filpath, filname, mode, rights, __0);
  if(ret!=-1)trace_alloc(trace_file, (void*)ret, __file__, __line__);
  return ret;
}

int  __deleaker_fopen( char* __file__, int __line__, const wchar_t* fname, int mode, int rights )
{
  int  ret = __original_fopen(fname, mode, rights);
  if(ret!=-1)trace_alloc(trace_file, (void*)ret, __file__, __line__);
  return ret;
}

int  __deleaker_fclose( char* __file__, int __line__, int file )
{
  trace_free(trace_file, (void*)file, __file__, __line__);
  return __original_fclose(file);
}

int  __deleaker_w_fopen( char* __file__, int __line__, const wchar_t* name, int attr, int rights, int err )
{
  //тот же trace_file?
  int  ret = __original_w_fopen(name, attr, rights, err);
  if(ret!=-1)trace_alloc(trace_file, (void*)ret, __file__, __line__);
  return ret;
}

int  __deleaker_w_fclose( char* __file__, int __line__, int f )
{
  trace_free(trace_file, (void*)f, __file__, __line__);
  return __original_w_fclose(f);
}


void  __deleaker_TabMenuBar_SetTabGui( char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab, GUI* __unknwnargname3 )
{
  trace_free(trace_memory, (void*)__unknwnargname3, __file__, __line__);
  return __original_TabMenuBar_SetTabGui(__unknwnargname1, tab, __unknwnargname3);
}

GUI_LIST*  __deleaker_CreateListMenu( char* __file__, int __line__, BOOK* __unknwnargname1, int display )
{
  GUI_LIST*  ret = __original_CreateListMenu(__unknwnargname1, display);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GUI_NOFMANY*  __deleaker_CreateNOfMany( char* __file__, int __line__, BOOK* book )
{
  GUI_NOFMANY*  ret = __original_CreateNOfMany(book);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GUI_ONEOFMANY*  __deleaker_CreateOneOfMany( char* __file__, int __line__, BOOK* book )
{
  GUI_ONEOFMANY*  ret = __original_CreateOneOfMany(book);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GUI_TABMENUBAR*  __deleaker_CreateTabMenuBar( char* __file__, int __line__, BOOK* book )
{
  GUI_TABMENUBAR*  ret = __original_CreateTabMenuBar(book);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GUI*  __deleaker_CreateProgressBar( char* __file__, int __line__, BOOK* book, int display )
{
  GUI*  ret = __original_CreateProgressBar(book, display);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

//что у strid функций при ошибке?
STRID  __deleaker_Str2ID( char* __file__, int __line__, const void* wstr, int flag, int len )
{
  STRID  ret = __original_Str2ID(wstr, flag, len);
  trace_alloc(trace_strid, (void*)ret, __file__, __line__);
  if(flag==5)
  {
    for(int i=0;i<len;i++)
      trace_free(trace_strid, (void*)((long*)wstr)[i], __file__, __line__);
  }
  return ret;
}

STRID  __deleaker_TextCopyId( char* __file__, int __line__, STRID __unknwnargname1 )
{
  STRID  ret = __original_TextCopyId(__unknwnargname1);
  trace_alloc(trace_strid, (void*)ret, __file__, __line__);
  return ret;
}

STRID  __deleaker_int2strID( char* __file__, int __line__, int num )
{
  STRID  ret = __original_int2strID(num);
  trace_alloc(trace_strid, (void*)ret, __file__, __line__);
  return ret;
}

void  __deleaker_TextFree( char* __file__, int __line__, STRID __unknwnargname1 )
{
  trace_free(trace_strid, (void*)__unknwnargname1, __file__, __line__ );
  return __original_TextFree(__unknwnargname1);
}

void  __deleaker_GUIObject_SetTitleText( char* __file__, int __line__, GUI* __unknwnargname1, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_GUIObject_SetTitleText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_GUIObject_SoftKeys_SetText( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 )
{
  if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
  return __original_GUIObject_SoftKeys_SetText(__unknwnargname1, actionID, __unknwnargname3);
}

char  __deleaker_GUIonMessage_SetMenuItemText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_GUIonMessage_SetMenuItemText(msg, __unknwnargname2);
}

char  __deleaker_GUIonMessage_SetMenuItemSecondLineText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_GUIonMessage_SetMenuItemSecondLineText(msg, __unknwnargname2);
}

char  __deleaker_GUIonMessage_SetMenuItemInfoText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_GUIonMessage_SetMenuItemInfoText(msg, __unknwnargname2);
}

char  __deleaker_GUIonMessage_SetMenuItemUnavailableText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_GUIonMessage_SetMenuItemUnavailableText(msg, __unknwnargname2);
}

int  __deleaker_MessageBox( char* __file__, int __line__, STRID header_text, STRID message_text, wchar_t IconID, int style, int time, BOOK* book )
{
  if(isallocatedstrid(header_text))trace_free(trace_strid, (void*)header_text, __file__, __line__ );
  if(isallocatedstrid(message_text) && message_text!=header_text)trace_free(trace_strid, (void*)message_text, __file__, __line__ );
  if(isallocatediconid(IconID))trace_free(trace_iconid, (void*)IconID, __file__, __line__ );
  return __original_MessageBox(header_text, message_text, IconID, style, time, book);
}

void  __deleaker_BookObj_SoftKeys_SetText( char* __file__, int __line__, BOOK* book, int actionID, STRID __unknwnargname3 )
{
  if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
  return __original_BookObj_SoftKeys_SetText(book, actionID, __unknwnargname3);
}

void  __deleaker_DataBrowserDesc_SetHeaderText( char* __file__, int __line__, void* DataBrowserDesc, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_DataBrowserDesc_SetHeaderText(DataBrowserDesc, __unknwnargname2);
}

void  __deleaker_DataBrowserDesc_SetOKSoftKeyText( char* __file__, int __line__, void* DataBrowserDesc, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_DataBrowserDesc_SetOKSoftKeyText(DataBrowserDesc, __unknwnargname2);
}

void  __deleaker_DispObject_SetTitleText( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_DispObject_SetTitleText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_GUIObject_SoftKeys_AddErrorStr( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 )
{
  if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
  return __original_GUIObject_SoftKeys_AddErrorStr(__unknwnargname1, actionID, __unknwnargname3);
}

void  __deleaker_ProgressBar_SetText( char* __file__, int __line__, GUI* __unknwnargname1, STRID text )
{
  if(isallocatedstrid(text))trace_free(trace_strid, (void*)text, __file__, __line__ );
  return __original_ProgressBar_SetText(__unknwnargname1, text);
}

void  __deleaker_StatusIndication_Item8_SetText( char* __file__, int __line__, STRID __unknwnargname1 )
{
  if(isallocatedstrid(__unknwnargname1))trace_free(trace_strid, (void*)__unknwnargname1, __file__, __line__ );
  return __original_StatusIndication_Item8_SetText(__unknwnargname1);
}

void  __deleaker_StatusIndication_SetItemText( char* __file__, int __line__, GUI* __unknwnargname1, int item, STRID __unknwnargname3 )
{
  if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
  return __original_StatusIndication_SetItemText(__unknwnargname1, item, __unknwnargname3);
}

void  __deleaker_StatusIndication_ShowNotes( char* __file__, int __line__, STRID __unknwnargname1 )
{
  if(isallocatedstrid(__unknwnargname1))trace_free(trace_strid, (void*)__unknwnargname1, __file__, __line__ );
  return __original_StatusIndication_ShowNotes(__unknwnargname1);
}

void  __deleaker_StringInput_DispObject_SetText( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_StringInput_DispObject_SetText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_GUIObject_SoftKeys_SetInfoText( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID __unknwnargname3 )
{
  if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
  return __original_GUIObject_SoftKeys_SetInfoText(__unknwnargname1, actionID, __unknwnargname3);
}

void  __deleaker_ListMenu_SetSecondLineText( char* __file__, int __line__, GUI_LIST* __unknwnargname1, int elem_num, STRID __unknwnargname3 )
{
  if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
  return __original_ListMenu_SetSecondLineText(__unknwnargname1, elem_num, __unknwnargname3);
}

void  __deleaker_GUIonMessage_SubItem_SetText( char* __file__, int __line__, GUI_MESSAGE* msg, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_GUIonMessage_SubItem_SetText(msg, __unknwnargname2);
}

void  __deleaker_Feedback_SetTextExtended( char* __file__, int __line__, GUI_FEEDBACK* __unknwnargname1, STRID text, int where )
{
  if(isallocatedstrid(text))trace_free(trace_strid, (void*)text, __file__, __line__ );
  return __original_Feedback_SetTextExtended(__unknwnargname1, text, where);
}

void  __deleaker_GUIObject_SoftKeys_SetTexts( char* __file__, int __line__, GUI* __unknwnargname1, u16 actionID, STRID short_text, STRID full_text )
{
  if(isallocatedstrid(short_text))trace_free(trace_strid, (void*)short_text, __file__, __line__ );
  if(isallocatedstrid(full_text) /*&& StrID1!=StrID*/)trace_free(trace_strid, (void*)full_text, __file__, __line__ );
  return __original_GUIObject_SoftKeys_SetTexts(__unknwnargname1, actionID, short_text, full_text);
}

STRID  __deleaker_PNUM2Name( char* __file__, int __line__, void* pnum, int isIconNeeded, int only_full_number_flag )
{
  STRID  ret = __original_PNUM2Name(pnum, isIconNeeded, only_full_number_flag);
  trace_alloc(trace_strid, (void*)ret, __file__, __line__);
  return ret;
}

int  __deleaker_Date2ID( char* __file__, int __line__, DATE* __unknwnargname1, int TimeFormat, int __unknwnargname3 )
{
  int  ret = __original_Date2ID(__unknwnargname1, TimeFormat, __unknwnargname3);
  trace_alloc(trace_strid, (void*)ret, __file__, __line__);
  return ret;
}

int  __deleaker_Time2ID( char* __file__, int __line__, TIME* __unknwnargname1, char TimeFormat, int isSec )
{
  int  ret = __original_Time2ID(__unknwnargname1, TimeFormat, isSec);
  trace_alloc(trace_strid, (void*)ret, __file__, __line__);
  return ret;
}

STRID  __deleaker_KeyCode2Name( char* __file__, int __line__, int key_code )
{
  STRID  ret = __original_KeyCode2Name(key_code);
  if(isallocatedstrid(ret))trace_alloc(trace_strid, (void*)ret, __file__, __line__);
  return ret;
}

void  __deleaker_NOfMany_SetTexts( char* __file__, int __line__, GUI_NOFMANY* __unknwnargname1, STRID* strids, int items_count )
{
  for(int i=0;i<items_count;i++)
    if(isallocatedstrid(strids[i]))trace_free(trace_strid, (void*)strids[i], __file__, __line__ );
  __original_NOfMany_SetTexts(__unknwnargname1, strids, items_count);
}

void  __deleaker_OneOfMany_SetTexts( char* __file__, int __line__, GUI_ONEOFMANY* __unknwnargname1, STRID* strids, int Count )
{
  for(int i=0;i<Count;i++)
    if(isallocatedstrid(strids[i]))trace_free(trace_strid, (void*)strids[i], __file__, __line__ );
  __original_OneOfMany_SetTexts(__unknwnargname1, strids, Count);
}

void  __deleaker_FeedBack_SetText( char* __file__, int __line__, GUI_FEEDBACK* __unknwnargname1, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_FeedBack_SetText(__unknwnargname1, __unknwnargname2);
}

int  __deleaker_Gif2ID( char* __file__, int __line__, u16 IMAGEHANDLE, const wchar_t* path, const wchar_t* fname, wchar_t* ID )
{
  int  ret = __original_Gif2ID(IMAGEHANDLE, path, fname, ID);
  if(ret)trace_alloc(trace_iconid, (void*)*ID, __file__, __line__);
  return ret;
}

void  __deleaker_GUIObject_SetTitleIcon( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
  if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
  __original_GUIObject_SetTitleIcon(__unknwnargname1, imageID);
}

void  __deleaker_GUIInput_SetIcon( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t icon )
{
  if(isallocatediconid(icon))trace_free(trace_iconid, (void*)icon, __file__, __line__ );
  __original_GUIInput_SetIcon(__unknwnargname1, icon);
}

void  __deleaker_ProgressBar_SetIcon( char* __file__, int __line__, GUI* __unknwnargname1, u16 icon_id )
{
  if(isallocatediconid(icon_id))trace_free(trace_iconid, (void*)icon_id, __file__, __line__ );
  __original_ProgressBar_SetIcon(__unknwnargname1, icon_id);
}

void  __deleaker_GUIonMessage_SetMenuItemIcon( char* __file__, int __line__, GUI_MESSAGE* msg, int align, wchar_t iconID )
{
  if(isallocatediconid(iconID))trace_free(trace_iconid, (void*)iconID, __file__, __line__ );
  __original_GUIonMessage_SetMenuItemIcon(msg, align, iconID);
}

void  __deleaker_TabMenuBar_SetTabIcon( char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab, wchar_t icon, int for_state )
{
  if(isallocatediconid(icon))trace_free(trace_iconid, (void*)icon, __file__, __line__ );
  __original_TabMenuBar_SetTabIcon(__unknwnargname1, tab, icon, for_state);
}

void  __deleaker_VCALL_SetNameIcon( char* __file__, int __line__, void* vc, wchar_t icon )
{
  if(isallocatediconid(icon))trace_free(trace_iconid, (void*)icon, __file__, __line__ );
  __original_VCALL_SetNameIcon(vc, icon);
}

#ifdef __cplusplus
int  __deleaker_ModifyKeyHook( char* __file__, int __line__, KEYHOOKPROC proc, int mode, LPARAM lparam )
{
  int  ret = __original_ModifyKeyHook(proc, mode, lparam);
  if(mode==0)trace_free(trace_hook, (void*)proc, __file__, __line__ );
  if(mode==1)trace_alloc(trace_hook, (void*)proc, __file__, __line__);
  return ret;
}
int  __deleaker_ModifyKeyHook( char* __file__, int __line__, int (*proc)( int, int, int ), int mode, LPARAM lparam )
{
  int  ret = __original_ModifyKeyHook(proc, mode, lparam);
  if(mode==0)trace_free(trace_hook, (void*)proc, __file__, __line__ );
  if(mode==1)trace_alloc(trace_hook, (void*)proc, __file__, __line__);
  return ret;
}
#else
int  __deleaker_ModifyKeyHook( char* __file__, int __line__, int (*proc)( int, int, int, void* ), int mode, void* lparam )
{
  int  ret = __original_ModifyKeyHook(proc, mode, lparam);
  if(mode==0)trace_free(trace_hook, (void*)proc, __file__, __line__ );
  if(mode==1)trace_alloc(trace_hook, (void*)proc, __file__, __line__);
  return ret;
}
#endif

int  __deleaker_ModifyUIHook( char* __file__, int __line__, int event, int (*PROC)( UI_MESSAGE* ), int mode )
{
  int  ret = __original_ModifyUIHook(event, PROC, mode);
  if(mode==0)trace_free(trace_hook, (void*)PROC, __file__, __line__ );
  if(mode==1)trace_alloc(trace_hook, (void*)PROC, __file__, __line__);
  return ret;
}

int  __deleaker_ImageID_Get( char* __file__, int __line__, const wchar_t* fpath, const wchar_t* fname, wchar_t* imageID )
{
  int  ret = __original_ImageID_Get(fpath, fname, imageID);
  trace_alloc(trace_iconid, (void*)(*imageID), __file__, __line__);
  return ret;
}

int  __deleaker_ImageID_GetIndirect( char* __file__, int __line__, void* buf_image, int size, int __NULL, wchar_t* image_type, wchar_t* imageID )
{
  int  ret = __original_ImageID_GetIndirect(buf_image, size, __NULL, image_type, imageID);
  trace_free(trace_memory, buf_image, __file__, __line__);
  trace_alloc(trace_iconid, (void*)(*imageID), __file__, __line__);
  return ret;
}

void  __deleaker_ImageID_Free( char* __file__, int __line__, wchar_t imageID )
{
  trace_free(trace_iconid, (void*)imageID, __file__, __line__);
  __original_ImageID_Free(imageID);
}

GC*  __deleaker_GC_CreateMemoryGC( char* __file__, int __line__, int xsize, int ysize, int bpp, int unk, void* somefn, int unk2 )
{
  GC*  ret = __original_GC_CreateMemoryGC(xsize, ysize, bpp, unk, somefn, unk2);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

void  __deleaker_GC_FreeGC( char* __file__, int __line__, GC* gc )
{
  trace_free(trace_memory, gc, __file__, __line__);
  __original_GC_FreeGC(gc);
}

GVI_PEN  __deleaker_GVI_CreateDashedPen( char* __file__, int __line__, char thikness, int color, int bitmask, int step, int offset )
{
  GVI_PEN  ret = __original_GVI_CreateDashedPen(thikness, color, bitmask, step, offset);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GVI_PEN  __deleaker_GVI_CreatePen( char* __file__, int __line__, char thikness, int color )
{
  GVI_PEN  ret = __original_GVI_CreatePen(thikness, color);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GVI_BRUSH  __deleaker_GVI_CreateSolidBrush( char* __file__, int __line__, int color )
{
  GVI_BRUSH  ret = __original_GVI_CreateSolidBrush(color);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GVI_BMP  __deleaker_GVI_CreateBitmap( char* __file__, int __line__, int xsize, int ysize, int bpp )
{
  GVI_BMP  ret = __original_GVI_CreateBitmap(xsize, ysize, bpp);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

GVI_GC  __deleaker_GVI_CreateMemoryGC( char* __file__, int __line__, GVI_BMP bitmap )
{
  GVI_GC  ret = __original_GVI_CreateMemoryGC(bitmap);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

BOOL  __deleaker_GVI_Delete_GVI_Object( char* __file__, int __line__, GVI_OBJ* __unknwnargname1 )
{
  trace_free(trace_memory, *__unknwnargname1, __file__, __line__);
  return __original_GVI_Delete_GVI_Object(__unknwnargname1);
}

void  __deleaker_GVI_DeleteMemoryGC( char* __file__, int __line__, GVI_GC gc )
{
  trace_free(trace_memory, gc, __file__, __line__);
  return __original_GVI_DeleteMemoryGC(gc);
}

GUI_FEEDBACK*  __deleaker_TextFeedbackWindow( char* __file__, int __line__, BOOK* book, int zero )
{
  GUI_FEEDBACK*  ret = __original_TextFeedbackWindow(book, zero);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

void*  __deleaker_DataBrowserDesc_Create( char* __file__, int __line__ )
{
  void*  ret = __original_DataBrowserDesc_Create();
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

void  __deleaker_DataBrowserDesc_Destroy( char* __file__, int __line__, void* DataBrowserDesc )
{
  trace_free(trace_memory, DataBrowserDesc, __file__, __line__);
  return __original_DataBrowserDesc_Destroy(DataBrowserDesc);
}

#define CreateStringInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_memory, __original_CreateStringInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateDateInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_memory, __original_CreateDateInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreatePercentInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_memory, __original_CreatePercentInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateStringInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_memory, __original_CreateStringInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateTimeInputVA( a, ... ) (GUI*) trace_alloc_ret( trace_memory, __original_CreateTimeInputVA( a, __VA_ARGS__), NULL, __file__, __line__ )

#define CreateYesNoQuestionVA( a, ... ) (GUI*) trace_alloc_ret( trace_memory, __original_CreateYesNoQuestionVA( a, __VA_ARGS__), NULL, __file__, __line__ )

GUI_FEEDBACK*  __deleaker_CreateMonitorFeedback( char* __file__, int __line__, STRID __unknwnargname1, BOOK* __unknwnargname2, void (*onbusy)(BOOK*), void (*onedit)(BOOK*), void (*ondelete)(BOOK*) )
{
  GUI_FEEDBACK*  ret = __original_CreateMonitorFeedback(__unknwnargname1, __unknwnargname2, onbusy, onedit, ondelete);
  if(ret)trace_alloc(trace_memory, (void*)ret, __file__, __line__);
  return ret;
}

void*  __deleaker_LoadDLL( char* __file__, int __line__, wchar_t* DllName )
{
  void*  ret = __original_LoadDLL(DllName);
  if(ret)trace_alloc(trace_dll, (void*)ret, __file__, __line__);
  return ret;
}

int  __deleaker_UnLoadDLL( char* __file__, int __line__, void* DllData )
{
  trace_free(trace_dll, DllData, __file__, __line__);
  return __original_UnLoadDLL(DllData);
}

void  __deleaker_GUIObject_SetSecondRowTitleText( char* __file__, int __line__, GUI* __unknwnargname1, STRID __unknwnargname2 )
{
  if(isallocatedstrid(__unknwnargname2))trace_free(trace_strid, (void*)__unknwnargname2, __file__, __line__ );
  return __original_GUIObject_SetSecondRowTitleText(__unknwnargname1, __unknwnargname2);
}

void  __deleaker_ListMenu_SetNoItemText( char* __file__, int __line__, GUI_LIST* __unknwnargname1, STRID str )
{
  if(isallocatedstrid(str))trace_free(trace_strid, (void*)str, __file__, __line__ );
  return __original_ListMenu_SetNoItemText(__unknwnargname1, str);
}

void  __deleaker_TabMenuBar_SetTabTitle( char* __file__, int __line__, GUI_TABMENUBAR* __unknwnargname1, int tab_num, STRID __unknwnargname3 )
{
  if(isallocatedstrid(__unknwnargname3))trace_free(trace_strid, (void*)__unknwnargname3, __file__, __line__ );
  return __original_TabMenuBar_SetTabTitle(__unknwnargname1, tab_num, __unknwnargname3);
}

void  __deleaker_DispObject_SetBackgroundImage( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID )
{
  if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
  __original_DispObject_SetBackgroundImage(__unknwnargname1, imageID);
}

void  __deleaker_DispObject_SetCursorImage( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID )
{
  if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
  __original_DispObject_SetCursorImage(__unknwnargname1, imageID);
}

void  __deleaker_DispObject_SetTitleBackgroundImage( char* __file__, int __line__, DISP_OBJ* __unknwnargname1, wchar_t imageID )
{
  if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
  __original_DispObject_SetTitleBackgroundImage(__unknwnargname1, imageID);
}

void  __deleaker_GUIObject_SetBackgroundImage( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
  if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
  __original_GUIObject_SetBackgroundImage(__unknwnargname1, imageID);
}

void  __deleaker_GUIObject_SetCursorImage( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
  if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
  __original_GUIObject_SetCursorImage(__unknwnargname1, imageID);
}

void  __deleaker_GUIObject_SetTitleBackgroundImage( char* __file__, int __line__, GUI* __unknwnargname1, wchar_t imageID )
{
  if(isallocatediconid(imageID))trace_free(trace_iconid, (void*)imageID, __file__, __line__ );
  __original_GUIObject_SetTitleBackgroundImage(__unknwnargname1, imageID);
}
//__swi __arm ACTION* ActionCreate( int (*PROC)( void* msg, BOOK* ), int BookID, u16 event, APP_DESC* app_desc, PAGE_DESC* pag_desc );
//__swi __arm SUB_EXECUTE* BrowserItem_Get_SUB_EXECUTE( BOOK* BrowserItemBook );
//__swi __arm void* CallID_GetCallStatusDesc( int CallID );
//__swi __arm wchar_t* CallStatusDesc_GetName( void* CallStatusDesc );
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
//__swi __arm DB_FILE_FILTER DataBrowser_isFileInListExt_adr( void );
//__swi __arm int DirHandle_SetFilterStr( DIR_HANDLE*, const wchar_t* filter );
//__swi __arm void* DispObject_SoftKeys_GetParams( DISP_OBJ* );
//__swi __arm FILEITEM* FILEITEM_Create( void );
//__swi __arm FILEITEM* FILEITEM_CreateCopy( FILEITEM* );
//__swi __arm void FILEITEM_Destroy( FILEITEM* );
//__swi __arm int FILEITEM_SetFname( FILEITEM*, const wchar_t* fname );
//__swi __arm int FILEITEM_SetFnameAndContentType( FILEITEM*, const wchar_t* fname );
//__swi __arm int FILEITEM_SetPath( FILEITEM*, const wchar_t* fpath );
//__swi __arm int FILEITEM_SetPathAndContentType( FILEITEM*, const wchar_t* fpath );
//__swi __arm void Feedback_SetKeyHook( GUI_FEEDBACK*, void (*hook)( BOOK* book, int key, int unk, int unk2 ) );
//__swi __arm void Feedback_SetManualScrollingText( DISP_OBJ* feedback_disp_obj, int state );
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
//__swi __arm void JavaSession_Manager( int cmd );
//__swi __arm void ListMenu_DestroyItems( GUI_LIST* );
//__swi __arm int ListMenu_GetItemCount( GUI_LIST* );
//__swi __arm int ListMenu_SetItemIcon( GUI_LIST*, int Item, wchar_t unk_FFFF, int mode, wchar_t ImageID );
//__swi __arm int MSG_SendMessage_AddRecipient( void*, wchar_t*, wchar_t*, int, int );
//__swi __arm int MSG_SendMessage_CreateMessage( int, void* );
//__swi __arm int MSG_SendMessage_DestroyMessage( void* );
//__swi __arm int MSG_SendMessage_Start( int, void*, int );
//__swi __arm char* MainInput_getPNUM( GUI* );
//__swi __arm void MediaPlayer_SoftKeys_AddHelpStr( GUI* player_gui, int item, STRID );
//__swi __arm void MediaPlayer_SoftKeys_SetText( GUI* player_gui, int actionID, STRID );
//__swi __arm BOOK* MenuBook_Desktop( int mode, int BookID );
//__swi __arm wchar_t* MenuBook_Desktop_GetSelectedItemID( BOOK* MenuBook_Desktop );
//__swi __arm void* MetaData_Desc_Create( wchar_t* path, wchar_t* name );
//__swi __arm void MetaData_Desc_Destroy( void* MetaData_Desc );
//__swi __arm int MetaData_Desc_GetCoverInfo( void* MetaData_Desc, char* cover_type, int* size, int* cover_offset );
//__swi __arm wchar_t* MetaData_Desc_GetTags( void* MetaData_Desc, int tagID );
//__swi __arm int MetaData_Desc_GetTrackNum( void* MetaData_Desc, int __NULL );
//__swi __arm int ModifyUIPageHook( int event, int (*PROC)( void* msg, BOOK* book ), int book_id, int mode );
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
//__swi __arm int Request_ICA_ShutdownAllConnections( const int* sync );
//__swi __arm void SendDispatchMessage( int id, int unk_zero, int size, void* mess );
//__swi __arm void SendMessage( void** signal, int PID );
//__swi __arm int Shortcut_Get_MenuItemIconID( void* );
//__swi __arm int Shortcut_Get_MenuItemName( void* );
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
//__swi __arm int StringInput_GetStringAndLen( GUI*, wchar_t**, u16* );
//__swi __arm void Timer_Kill( u16* timerID );
//__swi __arm void Timer_ReSet( u16* timer, int time, void (*onTimer)( u16 timerID, void* ), void* );
//__swi __arm u16 Timer_Set( int time, void (*onTimer)( u16 timerID, void* ), void* );
//__swi __arm void* WaitMessage( void* SIGSEL );
//__swi __arm C_INTERFACE* Window_GetComponentInterface( WINDOW* );
//__swi __arm void YesNoQuestion_SetIcon( GUI*, wchar_t iconId );
//__swi __arm PROCESS create_process( int proc_type, char* name, OSENTRYPOINT* entrypoint, OSADDRESS stack_size, OSPRIORITY priority, OSTIME timeslice, PROCESS pid_block, void* redir_table, OSVECTOR vector, OSUSER user );
//__swi __arm void free_buf( union SIGNAL** sig );
//__swi __arm int get_CellData( PLMN_LAC_DESC * desc1, RAT_CI_DESC * desc2, char * CSReg );
//__swi __arm int inflate( z_streamp strm, int flush );
//__swi __arm int inflateEnd( z_streamp strm );
//__swi __arm int inflateInit2_( z_streamp strm, int windowBits, const char* version, int stream_size );
//__swi __arm void kill_proc( PROCESS pid );
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
