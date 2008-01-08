#include "..\\include\Target.h"
#ifndef _LIB_CLARA_H
#define _LIB_CLARA_H

#include "..\\include\Types.h"
#ifndef SWI_HOOK
#include "..\\include\DYN_ext.h"
#include "..\\include\DYN_tag.h"
#include "..\\include\Colors.h"
#endif

extern int *SYNC;
extern int *ASYNC;
extern void *ELF_BEGIN;
#ifndef SWI_HOOK
extern "C" void kill_data(void *p, void (*func_p)(void *));
#else
extern void kill_data(void *p, void (*func_p)(void *));
#endif

#define NULL	0

#define SID_ANY_LEN 0xFFFF
#define STR(__STR__) Str2ID((u16*)(L##__STR__),0,SID_ANY_LEN)

#define MINUTE_TICK_EVENT 999


typedef  unsigned int size_t;

#define u16 unsigned short

// Str2ID conversion flag
/*
#define WSTR_2_ID 0

#define NO_ERROR 0

#define keySELECT 0xFC3
#define keyESC    0xFC1

//#define BOOK_DESTROYED_EVENT 4


//#define RAM_BASE 0x4C04B1CCL

//PhoneBook const

#define ALL_AB_ITEMS            0x20


#define AB_N_FAX                0x08
#define AB_N_OTHER              0x0A
#define AB_EMAIL                0x0B
#define AB_URL                  0x0E
#define AB_INFO                 0x1D
*/

//Key Code
/*
#define KEY_UP                  0x06
#define KEY_DOWN                0x0A
#define KEY_LEFT                0x0C
#define KEY_RIGHT               0x08
#define KEY_ENTER               0x05
#define KEY_ESC                 0x03
#define KEY_DEL                 0x04
#define KEY_LEFT_SOFT           0x01
#define KEY_RIGHT_SOFT          0x02
#define KEY_TASK                0x1C
#define KEY_PLAY                0x20
#define KEY_CAMERA_FOCUS        0x22
#define KEY_CAMERA_SHOT         0x23
#define KEY_VOL_UP              0x0E
#define KEY_VOL_DOWN            0x0F
#define KEY_LENS_COVER_OPEN     0x31
#define KEY_LENS_COVER_CLOSE    0x32
#define KEY_STAR                0x1A
#define KEY_DIEZ                0x1B
#define KEY_POWER               0x00

#define KBD_SHORT_PRESS         0x00
#define KBD_SHORT_RELEASE       0x03
#define KBD_LONG_PRESS          0x01
#define KBD_LONG_RELEASE        0x04
#define KBD_REPEAT              0x02
*/


#pragma diag_suppress=Ta035

#pragma swi_number=0x100
__swi __arm  void IMB(void);

#pragma swi_number=0x101
__swi __arm  void * GetUserData (int size,void (*constr)(void *, void *), void *constr_param);

#pragma swi_number=0x102
__swi __arm  int RemoveUserData(void (*constr)(void *, void *));

#pragma swi_number=0x103
__swi __arm  void *malloc(int size);

#pragma swi_number=0x104
__swi __arm  void mfree(void *p);

#pragma swi_number=0x8104
__swi __arm void *mfree_adr(void);

#pragma swi_number=0x105
__swi __arm  u16 * GetDir (int DirIndex);

#pragma swi_number=0x106
__swi __arm  int fopen (const u16 * fname, int mode, int rights);

#pragma swi_number=0x107
__swi __arm  int ModifyKeyHook(int(*proc)(int,int,int),int mode);

#pragma swi_number=0x108
__swi __arm  void SUBPROC (void(*PROC)(void));


#ifndef SWI_HOOK
#pragma swi_number=0x108
__swi __arm  void SUBPROC (void(*PROC)(int),int p1);

#pragma swi_number=0x108
__swi __arm  void SUBPROC (void(*PROC)(int,void*),int p1 , void * p2);
#endif

#pragma swi_number=0x109
__swi __arm  void MMIPROC (void(*PROC)(void));

#ifndef SWI_HOOK

#pragma swi_number=0x109
__swi __arm  void MMIPROC (void(*PROC)(int),int p1);
#pragma swi_number=0x109
__swi __arm  void MMIPROC (void(*PROC)(int,void*),int p1 , void * p2);
#endif

//#pragma swi_number=0x10A
//__swi __arm  int ModifyOSEHook(int msg , void(*proc)(void * msg),int mode);

//#pragma swi_number=0x10B
//__swi __arm void ELF_RemoveFromList(void *elf_begin);

#pragma swi_number=0x10C
__swi __arm int ModifyUIHook(int event , int (*PROC)(UI_MESSAGE*),int mode);

//-------------------------------------------------------------------------------------------

#pragma swi_number=0x112
__swi __arm  void memset(void *mem,char chr,int size);

#pragma swi_number=0x113
__swi __arm  int memcpy(void *dest,const void *source,int cnt);

#pragma swi_number=0x114
__swi __arm  int sprintf(char *buf, char *str, ...);

#pragma swi_number=0x115
__swi __arm  int snwprintf(u16* buffer, int size, const u16* fmt, ...);

#pragma swi_number=0x116
//0x204 - append
//0x004 - write begin

__swi __arm  int _fopen(const unsigned short *filpath, const unsigned short *filname,unsigned int mode , unsigned int rights, unsigned int __0);

#pragma swi_number=0x117
__swi __arm  int fclose(int  file);

#pragma swi_number=0x118
__swi __arm  int fread(int file , void *ptr, int size);

#pragma swi_number=0x119
__swi __arm  int fwrite(int file , const void *ptr, int size);

//#pragma swi_number=0x11A
//__swi __arm  int _wstat(short *path, void *statbuf);

//#pragma swi_number=0x11B
//__swi __arm  int fputs(const char *s, int file);

#pragma swi_number=0x11C
__swi __arm  int isFileExist(u16 * pach, u16 *fname , FSTAT * fstat_stuct);

#pragma swi_number=0x11D
__swi __arm  void * AllocDirHandle(u16 * path);

#pragma swi_number=0x11E
__swi __arm  void *GetFname(void *, void*);

#pragma swi_number=0x11F
__swi __arm  void * CreateDataBrowser(void);

#pragma swi_number=0x120
__swi __arm  void DataBrowser_SetStyle(void * db , int style);

#pragma swi_number=0x121
__swi __arm  void DataBrowser_SetHeaderText(void * db , int StrID);

#pragma swi_number=0x122
__swi __arm  void DataBrowser_SetFolders(void * db , int * FolderList);

#pragma swi_number=0x123
__swi __arm  void DataBrowser_SetOKSoftKeyText(void * db , int StrID);

#pragma swi_number=0x124
__swi __arm  void DataBrowser_Show(void * db);

#pragma swi_number=0x125
__swi __arm  void DataBrowser_XXX(void * db);

#pragma swi_number=0x126
__swi __arm  unsigned short * getFileExtention(unsigned short * fnane);

#pragma swi_number=0x127
__swi __arm  int DataBrowser_isFileInListExt(unsigned short * ext_table,unsigned short * patch ,unsigned short * fname );

#pragma swi_number=0x128
__swi __arm  void Timer_ReSet(u16 *timerID ,int time, void (*onTimer)(u16 * unk , void *), void*);

#pragma swi_number=0x129
__swi __arm  u16 Timer_Set(int time, void (*onTimer)(u16 * unk , void *), void*);

#pragma swi_number=0x12A
__swi __arm  void Timer_Kill(u16 *timerID);

#pragma swi_number=0x12B
__swi __arm  int CreateBook(void * mem, void (* onClose)(BOOK*), const PAGE_DESC * bp, const char * name, int __0xFFFFFFFF,int __0);

#pragma swi_number=0x12C
__swi __arm  void BookObj_KillBook(void *Book);

#pragma swi_number=0x12D
__swi __arm  void BookObj_GotoPage(BOOK * book, PAGE_DESC * page);

#pragma swi_number=0x12E
__swi __arm  void BookObj_ReturnPage(BOOK * ,int);

#pragma swi_number=0x12F
__swi __arm  void FreeBook ( void *book);


#pragma swi_number=0x130
__swi __arm  BOOK * FindBook(IS_NEEDED_BOOK);

#pragma swi_number=0x131
__swi __arm  BOOK * Find_StandbyBook(void);

#pragma swi_number=0x132
__swi __arm  void BookObj_SetFocus(void *,int focus);

#pragma swi_number=0x133
__swi __arm  void GuiObject_SetTitleText (void * gui , int StrID);

#pragma swi_number=0x134
__swi __arm  void ShowWindow (void * gui);

#pragma swi_number=0x135
__swi __arm  void SetCursorToItem ( void *obj , int item);

#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI *,int style);

#ifndef SWI_HOOK
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_LIST *,int style);
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_TABMENUBAR *,int style);
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_ONEOFMANY *,int style);
#pragma swi_number=0x136
__swi __arm  void GUI_SetStyle(GUI_NOFMANY *,int style);
#endif

#pragma swi_number=0x137
__swi __arm  int GetCreatedListObjectItemNum ( void *msg);

#pragma swi_number=0x138
__swi __arm  char SetMenuItemText0 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x139
__swi __arm  char SetMenuItemText1 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x13A
__swi __arm  char SetMenuItemText2 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x13B
__swi __arm  char SetMenuItemText3 ( GUI_MESSAGE *, int StrID);

#pragma swi_number=0x13C
__swi __arm  void SetListObjectItemIcon ( void *msg , int align , int iconID);

#pragma swi_number=0x13D
__swi __arm  void SetHeaderText( void *obj , int StrID);

#pragma swi_number=0x13E
__swi __arm  int GetFocusetListObjectItem( void *obj);

#pragma swi_number=0x13F
__swi __arm  void SetMenuItemStyle( void *obj , int style);

#pragma swi_number=0x140
__swi __arm  void putchar( void *xx , int x, int y, int _zero, int zero1 , short wchar);

#pragma swi_number=0x141
__swi __arm  void SetGuiItemText(void *gui,int item,int StrID);

#pragma swi_number=0x142
__swi __arm  void StatusIndication_SetItemText(void *gui,int item,int StrID);

#pragma swi_number=0x143
__swi __arm  void StatusIndication_ShowNotes(int StrID);

#pragma swi_number=0x144
__swi __arm  void * CreateStringInput(int,...);

#pragma swi_number=0x145
__swi __arm  GUI_ONEOFMANY * CreateOneOfMany(BOOK *);

#pragma swi_number=0x146
__swi __arm  void OneOfMany_SetItemCount(GUI_ONEOFMANY * , int count);

#pragma swi_number=0x147
__swi __arm  void OneOfMany_SetChecked(GUI_ONEOFMANY * , int checked);

#pragma swi_number=0x148
__swi __arm  void OneOfMany_SetTexts(GUI_ONEOFMANY * , int * StrID , int Count);

#pragma swi_number=0x149
__swi __arm  int OneOfMany_GetSelected(GUI_ONEOFMANY * rb);

#pragma swi_number=0x14A
__swi __arm  void StatusIndication_Item8_SetText(int StrID);

#pragma swi_number=0x14B
__swi __arm  void AddMSGHook(void * menu,short msg,void (*proc)(BOOK *,void*));//BOOK *,GUI ? *

#pragma swi_number=0x14C
__swi __arm  void AddCommand(void * menu,int cmd , int StrID);

#pragma swi_number=0x14D
__swi __arm  void SoftKey_SetEnable(void *gui,int msg,int ena);

#pragma swi_number=0x14E
__swi __arm  void SoftKey_AddErrorStr(void *gui,int msg,int StrID);

#pragma swi_number=0x14F
__swi __arm  void SoftKey_RemoveItem(void *gui,u16 action);

#pragma swi_number=0x150
__swi __arm  void SoftKey_SetVisible(void *gui,int action,int visible);

#pragma swi_number=0x151
__swi __arm  void SoftKey_SuppressDefaultAction(void *gui,int action);

#pragma swi_number=0x152
__swi __arm  u16* wstrcpy(u16 * dest, u16 * source);

#pragma swi_number=0x153
__swi __arm  u16* wstrncpy(u16 * dest, u16 * source, int maxlen);

#pragma swi_number=0x154
__swi __arm  u16* wstrcat(u16 * wstr, u16 * subwstr);

#pragma swi_number=0x155
__swi __arm  short* wstrncat(short * wstr, short * subwstr , int maxlen);

#pragma swi_number=0x156
__swi __arm  int wstrcmp(unsigned short * wstr1, unsigned short * wstr2);

#pragma swi_number=0x157
__swi __arm  int wstrlen(u16 * wstr);

#pragma swi_number=0x158
__swi __arm  u16 * str2wstr(u16 * wstr,char * str);

#pragma swi_number=0x159
__swi __arm  int strcmp(char * str1, char * str2);

#pragma swi_number=0x15A
__swi __arm  int strlen(char *str);

#pragma swi_number=0x15B
__swi __arm  char * wstr2strn (char * str , u16 * wstr, int maxlen);

#pragma swi_number=0x15C
__swi __arm  int int2strID (int num);

#pragma swi_number=0x15D
__swi __arm  int Str2ID(const void * wstr , int  flag , int len);

#pragma swi_number=0x15E
__swi __arm  void StrID2Str(int StrID,char * str,int maxlen);

#pragma swi_number=0x15F
__swi __arm  void TextID2wstr(int StrID,u16 * str,int maxlen);

#pragma swi_number=0x160
__swi __arm  int TextGetLength(int StrID);

#pragma swi_number=0x161
__swi __arm  void TextFree(int StrID);

#pragma swi_number=0x162
__swi __arm  int AB_DEFAULTNBR_GET(int rec_num, void * mem_0x30, int * unk);

#pragma swi_number=0x163
__swi __arm  int AB_READSTRING(void * mem_0x30,int rec_num, int rec_item_num);

#pragma swi_number=0x164
__swi __arm  int AB_READPHONENBR(void * mem_0x30,int rec_num, int rec_item_num);

#pragma swi_number=0x165
__swi __arm  int AB_GETNBROFITEMS(int get_from,int unk_0);

#pragma swi_number=0x166
__swi __arm  int PNUM_len(void *pnum);

#pragma swi_number=0x167
__swi __arm  void PNUM2str(char *str,void *pnum,int len, int max_len);


//#pragma swi_number=0x168
//__swi __arm  void * ListElement_GetByNum(LIST *lst,int num_element);

#pragma swi_number=0x168
__swi __arm  DISP_OBJ * GUIObj_GetDISPObj (void * gui);

#pragma swi_number=0x169
__swi __arm  LIST *List_New(void);

#pragma swi_number=0x16A
__swi __arm  void List_Free(LIST *lst);

#pragma swi_number=0x16B
__swi __arm  void ListElement_AddtoTop(LIST *lst,void *newElement);

#pragma swi_number=0x16C
__swi __arm  int Gif2ID(u16 IMAGEHANDLE,u16 * path, u16 * fname,u16 * ID);

#pragma swi_number=0x16D
__swi __arm  int REQUEST_IMAGEHANDLER_INTERNAL_GETHANDLE(const int * __zero,u16 * IMAGEHANDLE,u16 * unk);

#pragma swi_number=0x16E
__swi __arm  void REQUEST_DATEANDTIME_GET(const int * __zero , DATETIME *dt);

#pragma swi_number=0x16F
__swi __arm  void IndicationDevice_Backlight_FadeToLevel(int unk_zero ,int bl_level);

#pragma swi_number=0x170
__swi __arm  int GetFreeBytesOnHeap(void);

#pragma swi_number=0x171
__swi __arm  void Hide(void * udata);

#pragma swi_number=0x172
__swi __arm  void Show(void * udata);

#pragma swi_number=0x173
__swi __arm  void StartAPP (u16 * appname);

//#pragma swi_number=0x174
//__swi __arm  void CreateEvent(int event);

#pragma swi_number=0x174
__swi __arm  void ListMenu_SetOnMessages (GUI_LIST * , void * onMessage);

#pragma swi_number=0x175
__swi __arm  char * manifest_GetParam(char *buf,char *param_name,int unk);

#pragma swi_number=0x176
__swi __arm  int lseek(int file,int offset,int mode);

#pragma swi_number=0x8177
__swi __arm  int get_VBUFFER(void);

#pragma swi_number=0x178
__swi __arm  void SetLampLevel (int level);

#pragma swi_number=0x179
__swi __arm  void * ListElement_Remove(LIST *lst,int index);

#pragma swi_number=0x17A
__swi __arm  int GetCurrentPID(void);

#pragma swi_number=0x17B
__swi __arm  int ListElement_Prt2NumElement(LIST *lst,void *ptr);


#pragma swi_number=0x17C
__swi   __arm void *alloc(int size,int signo);
#pragma swi_number=0x17D
__swi  __arm void * recive(const SIGSELECT *sigsel);
#pragma swi_number=0x17E
__swi   __arm void send(union SIGNAL **sig,int PID);
#pragma swi_number=0x17F
__swi   __arm int sender(void **sig);
#pragma swi_number=0x180
__swi   __arm void free_buf(void **sig);
#pragma swi_number=0x181
__swi   __arm int create_process(int proc_type,char *name,void (*entrypoint)(void),int stack_size,int priority,int timeslice,int pid_block,void *redir_table,int vector,int user);
#pragma swi_number=0x182
__swi   __arm void start (int pid);
#pragma swi_number=0x183
__swi   __arm int get_ptype(int pid);
#pragma swi_number=0x184
__swi   __arm int current_process (void);
#pragma swi_number=0x185
__swi   __arm int delay (int timeout);

#pragma swi_number=0x1A0
__swi __arm  char* strcpy(char * dest, char * source);

#pragma swi_number=0x1A1
__swi __arm  void *CreateMessage(int size,int ev,char *name);
#pragma swi_number=0x1A2
__swi __arm  void SendMessage(void **signal,int PID);
#pragma swi_number=0x1A3
__swi __arm  void * WaitMessage(void *SIGSEL);
#pragma swi_number=0x1A4
__swi __arm  int FreeMessage(void **Mess);
#pragma swi_number=0x1A5
__swi __arm  void SendDispatchMessage(int id, int unk_zero, int size,void *mess);

#pragma swi_number=0x1A6
__swi __arm  char * UIEventName(int event);




#pragma swi_number=0x1A8
__swi __arm  void  UI_Event(int event);
#pragma swi_number=0x1A9
__swi __arm  void  UI_Event_wData(int event,void *message,void (*mfree)(void*));
#pragma swi_number=0x1AA
__swi __arm  void  UI_Event_toSID(int event,int SessionID);
#pragma swi_number=0x1AB
__swi __arm  void  UI_Event_toSIDwData(void *event,int SedionID,void *message,void (*mfree)(void*));
#pragma swi_number=0x1AC
__swi __arm  int ListElement_Find(LIST *lst,void *element, int (*cmp_proc)(void *,void *));


#pragma swi_number=0x1AD
__swi __arm  void * ListElement_GetByIndex(LIST * , int index);
#pragma swi_number=0x1AE
__swi __arm  u16* wstrrchr(u16*wstr, u16 wchar);
#pragma swi_number=0x1AF
__swi __arm  void BookObj_CallSubroutine(BOOK * , void * SubProcDesc);
#pragma swi_number=0x1B0
__swi __arm  void ListElement_Add(LIST *lst,void *newElement);

#pragma swi_number=0x1B1
__swi __arm  void debug_printf(char * fmt,...);

#pragma swi_number=0x1B2
__swi __arm  int PlayFile(u16 * path, u16 * fname);



#pragma swi_number=0x81B8
__swi __arm IS_NEEDED_BOOK isMediaPlayerVideoBook(void);
//#pragma swi_number=0x81B9
//__swi __arm void * get_APP_DESC_TABLE(void);



/*
06CC: FFFFFFFF 4D03D444 ; 1B3: int GetChargeBatteryInPercent(BOOK *standby , int *level);
06D0: FFFFFFFF E13E1545 ; 1B4: int REQUEST_PROFILE_GETACTIVEPROFILE(const int *__zero , int *level);
06D4: FFFFFFFF 3D3F1545 ; 1B5: void REQUEST_PROFILE_SETACTIVEPROFILE(const int *__zero , int NUMprof);
06D8: FFFFFFFF ADA9EA44 ; 1B6: void SetBrightnessOfScreen(int br);
06DC: FFFFFFFF 7DD9D344 ; 1B7: BOOK * Find_KeylockBook(void);
06E0: FFFFFFFF 9199F644 ; 81B8: IS_NEEDED_BOOK isMediaPlayerVideoBook(void);
06E4: FFFFFFFF 49B3F644 ; 1B9: void VideoOnWholeScreen(BOOK * MediaPlayer_Video_Book);
06E8: FFFFFFFF EDE5D344 ; 1BA: void ShuttingDown(void);
06EC: FFFFFFFF 714FF844 ; 1BB: void Vibra(int t1, int t2, int t3);
06F0: FFFFFFFF C9910B45 ; 1BC: char GetDateFormat(const int *zero, char *level);
06F4: FFFFFFFF 69920B45 ; 1BD: char GetTimeFormat(const int *zero, char *level);
06F8: FFFFFFFF CD92D244 ; 1BE: int Date2ID(int *, char DateFormat);
06FC: FFFFFFFF E591D244 ; 1BF: int Time2ID(int *, char TimeFormat);





*/

#pragma swi_number=0x1BF
__swi __arm int Time2ID(TIME *, char TimeFormat,int);

#pragma swi_number=0x1C0
__swi __arm  GUI_LIST * CreateListObject(BOOK * , void * unk);
#pragma swi_number=0x1C1
__swi __arm  void SetNumOfMenuItem(void * , int  item_count);

#ifndef SWI_HOOK
#pragma swi_number=0x81C2
__swi __arm LIST ** ROOT_APP(void);
#endif
#pragma swi_number=0x1C3
__swi __arm  void SoftKey_AddHelpStr(void *gui,int msg,int StrID);
#pragma swi_number=0x1C4
__swi __arm  void SoftKey_SetItemAsSubItem(void *gui,int item,int subitem);
#pragma swi_number=0x1C5
__swi __arm  int REQUEST_SYSTEM_SHUTDOWN(void);
#pragma swi_number=0x1C6
__swi __arm  int REQUEST_SYSTEM_RESTART(void);
#pragma swi_number=0x1C7
__swi __arm  void SoftKeys_SetText(void *gui,int msg,int StrID,int StrID1);

#pragma swi_number=0x1C8
__swi __arm  int isRightNowBook(BOOK *);
#pragma swi_number=0x1C9
__swi __arm  int isVolumeControllerBook(BOOK *);


#pragma swi_number=0x1CA
__swi __arm  GUI_TABMENUBAR * CreateTabMenuBar(BOOK *);
#pragma swi_number=0x1CB
__swi __arm  void TabMenuBar_SetTabCount(GUI_TABMENUBAR * , int count);
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI *);

#ifndef SWI_HOOK
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI_LIST *);
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI_ONEOFMANY *);
#pragma swi_number=0x1CC
__swi __arm  void TabMenuBar_AssignGuiObj(GUI_TABMENUBAR * , int tab , GUI_NOFMANY *);
#endif

#pragma swi_number=0x1CD
__swi __arm  void TabMenuBar_SetTabIcon(GUI_TABMENUBAR *,int tab, u16 icon , int for_state);

#pragma swi_number=0x1CE
__swi __arm  void  GUI_Free(GUI*);

#pragma swi_number=0x1CF
__swi __arm  void GUI_SetIcon(GUI* , int icon);

#pragma swi_number=0x1D0
__swi __arm int StringInput_GetStringAndLen (GUI*,u16**,u16*);

//#pragma swi_number=0x1D3


#pragma swi_number=0x1D4
__swi __arm  GUI_NOFMANY * CreateNOfMany(BOOK *);

#pragma swi_number=0x1D5
__swi __arm  void NOfMany_SetTexts(GUI_NOFMANY * , int * StrIDtable, int items_count);

#pragma swi_number=0x1D6
__swi __arm  void NOfMany_SetChecked(GUI_NOFMANY * , u16 * checked_table, int items_count);

#pragma swi_number=0x1D7
__swi __arm  void NOfMany_SetCursor(GUI_NOFMANY * , int item);

#pragma swi_number=0x1D8
__swi __arm  int NOfMany_GetCheckedCount(GUI_NOFMANY *);

#pragma swi_number=0x1D9
__swi __arm  int NOfMany_GetChecked(GUI_NOFMANY * , u16 * buffer );

#pragma swi_number=0x1DA
__swi __arm  void NOfMany_SetonMessage(GUI_NOFMANY * , void * proc );

#pragma swi_number=0x1E0
__swi __arm  void OneOfMany_SetFocused(GUI_ONEOFMANY * , int item);
#pragma swi_number=0x1E1
__swi __arm  void OneOfMany_SetonMessage(GUI_ONEOFMANY * , void * proc);



#pragma swi_number=0x1E2
__swi __arm GUI_FEEDBACK * CreateFeedBack (int,...);
#pragma swi_number=0x1E3
__swi __arm void  FeedBack_SetText(GUI_FEEDBACK*,int);
#pragma swi_number=0x1E4
__swi __arm int GetBattaryState(const int * , BATT *);
#pragma swi_number=0x1E5
__swi __arm void * get_DisplayGC (void);
#pragma swi_number=0x1E6
__swi __arm void GetGuiRect(void*,RECT*);
#pragma swi_number=0x1E7
__swi __arm  void get_GC_RECT(void*,RECT*);
#pragma swi_number=0x1E8
__swi __arm char get_GC_xx(void * GC);
#pragma swi_number=0x1E9
__swi __arm void *set_GC_xx(void * GC, int);
#pragma swi_number=0x1EA
__swi __arm void GC_validate_RECT(void * GC , RECT*);
#pragma swi_number=0x1EB
__swi __arm void addGui2book(BOOK*,GUI*);
#pragma swi_number=0x1EC
__swi __arm void DrawRect(int x1, int y1,int x2 ,int y2,int pen_color,int brush_color);
#pragma swi_number=0x1ED
__swi __arm void DrawString(int StrID,int align,int x1,int y1,int x2,int y2,int unk,int unk1,int pen_color ,int brush_color);
#pragma swi_number=0x1EE
__swi __arm  void addGui2XGuiList (void* xguilist , GUI*);
#pragma swi_number=0x1EF
__swi __arm void InvalidateRect(DISP_OBJ *,RECT*);
#pragma swi_number=0x1F0
__swi __arm int CreateObject(GUI*,void (*)(DISP_DESC *),void (*)(DISP_DESC *),BOOK *,int,int);
#pragma swi_number=0x1F1
__swi __arm void SetFont(int);
#pragma swi_number=0x81F2
__swi __arm DISP_OBJ ** StatusRow_p(void);

#pragma swi_number=0x1F3
__swi __arm int root_list_get_session_count(void);
#pragma swi_number=0x1F4
__swi __arm UI_APP_SESSION *root_list_get_session(int num_session);
#pragma swi_number=0x1F5
__swi __arm BOOK *SESSION_GetTopBook(UI_APP_SESSION *);

//15.11.07
#pragma swi_number=0x1F6
__swi __arm int MainInput_getVisible(GUI *);
#pragma swi_number=0x1F7
__swi __arm int MainInput_strlen(GUI *);
#pragma swi_number=0x1F8
__swi __arm char * MainInput_getPNUM(GUI *);
#pragma swi_number=0x1F9
__swi __arm int MainInput_isPlus(GUI *);

#pragma swi_number=0x1FA
__swi __arm int BOOK_GetSessionID(BOOK * );
#pragma swi_number=0x1FB
__swi __arm int wstrncmp(u16*,u16*,int);
#pragma swi_number=0x1FC
__swi __arm  int DISP_OBJ_GetAbsoluteXPos(DISP_OBJ *);
#pragma swi_number=0x1FD
__swi __arm int DISP_OBJ_GetAbsoluteYPos(DISP_OBJ *);
#pragma swi_number=0x1FE
__swi __arm WINDOW * DISP_OBJ_GetWindow(DISP_OBJ *);
#pragma swi_number=0x1FF
__swi __arm int GetThemeColor(int , int);
#pragma swi_number=0x200
__swi __arm int REQUEST_SETTING_SILENCE_SET(const int * __zero,u16 profile,u16 value);
#pragma swi_number=0x201
__swi __arm int REQUEST_SETTING_SILENCE_GET(const int * __zero,u16 profile, char * silence_mode);

//
#pragma swi_number=0x202
__swi __arm void VCALL_Init (void* vc);
#pragma swi_number=0x203
__swi __arm void VCALL_SetName (void * vc , u16 * name ,char name_len);
#pragma swi_number=0x204
__swi __arm void VCALL_SetNameIcon (void * vc , u16 icon);
#pragma swi_number=0x205
__swi __arm void VCALL_SetNumber (void * vc , u16 * number , char num_len);
#pragma swi_number=0x206
__swi __arm void VCALL_SetHZ1 (void * vc, int , u16 );
#pragma swi_number=0x207
__swi __arm void VCALL_SetHZ2 (void * vc , u16 );
#pragma swi_number=0x208
__swi __arm void MakeVoiceCall (int SessioID , void * vc ,int flag);

#pragma swi_number=0x209
__swi __arm int isDirectory (u16 * path ,u16 * filename , int * error );
#pragma swi_number=0x20A
__swi __arm void FileDelete (u16 * path ,u16 * filename , int * error );
#pragma swi_number=0x20B
__swi __arm void SetFocus (GUI * , int);

#pragma swi_number=0x20C
__swi __arm int CreateSMSCont (int, void*);
#pragma swi_number=0x20D
__swi __arm int FreeSMSCont (void*);
#pragma swi_number=0x20E
__swi __arm int PrepareSMSCont (void*,u16*,u16*,int,int);
#pragma swi_number=0x20F
__swi __arm int CteateNewMessage (int,void*,int);
#pragma swi_number=0x8210
__swi __arm  void *  get_APP_DESC_TABLE(void);


#pragma swi_number=0x211
__swi __arm  C_INTERFACE *Window_GetComponentInterface(WINDOW*);

#pragma swi_number=0x212
__swi __arm  int AB_POSITIONNBR_GET(void * ab_name,char * ,u16 * ,void * ab_num);
#pragma swi_number=0x213
__swi __arm  int AB_NAME_ITEM2StrID(void * ab_name);
#pragma swi_number=0x214
__swi __arm  int AB_NUM_ITEM2StrID(void * ab_num_item);
#pragma swi_number=0x215
__swi __arm  int GetABRecNum(AB_GET_STRUCT *);
#pragma swi_number=0x8216
__swi __arm  AB_ITEM_DESC * get_AB_ITEMS_DESC();


#pragma swi_number=0x217
__swi __arm  GUI * SBY_GetMainInput(BOOK * StandBy);              // !!!!!  не настоящий !!!!!!
#pragma swi_number=0x218
__swi __arm  GUI * SBY_GetStatusIndication(BOOK * StandBy);       // !!!!!  не настоящий !!!!!!

#pragma swi_number=0x219
__swi __arm  int REQUEST_PHONEBOOK_ACCESSSTATUS_TOTAL_GET(const int * __zero, int *);

#pragma swi_number=0x821A
__swi __arm  int * PID_MMI(void);

#pragma swi_number=0x21D
__swi __arm DISP_OBJ* DISPBASE_GetFocused(int display);
#pragma swi_number=0x21E
__swi __arm char DISP_OBJ_getVisible(DISP_OBJ*);


typedef int (*DISP_OBJ_ONCREATE_METHOD)(DISP_OBJ *);
typedef void (*DISP_OBJ_ONCLOSE_METHOD)(DISP_OBJ *);
typedef void (*DISP_OBJ_ONREDRAW_METHOD)(DISP_OBJ *,int ,int,int);
typedef void (*DISP_OBJ_ONKEY_METHOD)(DISP_OBJ *,int ,int,int,int);


typedef void (*DISP_OBJ_METHOD)(DISP_OBJ *);

#pragma swi_number=0x21F
__swi __arm DISP_DESC* DISP_OBJ_GetDESC (DISP_OBJ*);                          // !!!!!  не настоящий !!!!!!

#pragma swi_number=0x220
__swi __arm void DISP_DESC_SetName (DISP_DESC* , char * name);
#pragma swi_number=0x221
__swi __arm char* DISP_OBJ_GetName (DISP_OBJ *);
#pragma swi_number=0x222
__swi __arm void DISP_DESC_SetSize (DISP_DESC* , u16 size);
#pragma swi_number=0x223
__swi __arm u16 DISP_DESC_GetSize (DISP_OBJ *);
//
//
#pragma swi_number=0x224
__swi __arm void DISP_DESC_SetOnCreate (DISP_DESC* , DISP_OBJ_ONCREATE_METHOD);
#pragma swi_number=0x225
__swi __arm DISP_OBJ_ONCREATE_METHOD DISP_OBJ_GetOnCreate (DISP_OBJ *);
#pragma swi_number=0x226
__swi __arm void DISP_DESC_SetOnClose (DISP_DESC* , DISP_OBJ_ONCLOSE_METHOD);
#pragma swi_number=0x227
__swi __arm DISP_OBJ_ONCLOSE_METHOD DISP_OBJ_GetOnClose (DISP_OBJ *);
#pragma swi_number=0x228
__swi __arm void DISP_DESC_SetOnRedraw (DISP_DESC* , DISP_OBJ_ONREDRAW_METHOD);
#pragma swi_number=0x229
__swi __arm DISP_OBJ_ONREDRAW_METHOD DISP_OBJ_GetOnRedraw (DISP_OBJ *);
#pragma swi_number=0x22A
__swi __arm void DISP_DESC_SetOnKey (DISP_DESC* , DISP_OBJ_ONKEY_METHOD);
#pragma swi_number=0x22B
__swi __arm DISP_OBJ_ONKEY_METHOD DISP_OBJ_GetOnKey (DISP_OBJ *);



#pragma swi_number=0x22C
__swi __arm void DISP_DESC_SetMethod03 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x22D
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod03 (DISP_OBJ *);
#pragma swi_number=0x22E
__swi __arm void DISP_DESC_SetMethod04 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x22F
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod04 (DISP_OBJ *);
#pragma swi_number=0x230
__swi __arm void DISP_DESC_SetMethod05 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x231
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod05 (DISP_OBJ *);
#pragma swi_number=0x232
__swi __arm void DISP_DESC_SetMethod06 (DISP_DESC* ,DISP_OBJ_METHOD);
#pragma swi_number=0x233
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod06 (DISP_OBJ *);
#pragma swi_number=0x234
__swi __arm void DISP_DESC_SetMethod07 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x235
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod07 (DISP_OBJ *);
#pragma swi_number=0x236
__swi __arm void DISP_DESC_SetMethod08 (DISP_DESC* ,DISP_OBJ_METHOD);
#pragma swi_number=0x237
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod08 (DISP_OBJ *);
#pragma swi_number=0x238
__swi __arm void DISP_DESC_SetMethod09 (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x239
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod09 (DISP_OBJ *);
#pragma swi_number=0x23A
__swi __arm void DISP_DESC_SetMethod0A (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x23B
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod0A (DISP_OBJ *);
#pragma swi_number=0x23C
__swi __arm void DISP_DESC_SetMethod0B (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x23D
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod0B (DISP_OBJ *);
#pragma swi_number=0x23E
__swi __arm void DISP_DESC_SetMethod0C (DISP_DESC* ,void (*)(void*));
#pragma swi_number=0x23F
__swi __arm DISP_OBJ_METHOD DISP_OBJ_GetMethod0C (DISP_OBJ *);








#endif

