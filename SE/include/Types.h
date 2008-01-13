#ifndef _TYPES_H
#define _TYPES_H

#define u16 unsigned short



/*
#ifndef SWI_HOOK
const int _SYNC=0;
const int _ASYNC=0;
#define SYNC &_SYNC
#define ASYNC &_ASYNC
#endif
*/

typedef int STRID;

union SIGNAL;

typedef unsigned char OSPRIORITY;
typedef unsigned char OSBOOLEAN;

typedef unsigned short OSVECTOR;

typedef unsigned long PROCESS;
typedef unsigned long SIGSELECT;
typedef unsigned long OSBUFSIZE;
typedef unsigned long OSUSER;
typedef unsigned long OSERRCODE;
typedef unsigned long OSADDRESS;
typedef unsigned long OSTIME;
typedef unsigned long OSTICK;
typedef unsigned long OSATTREF;
typedef unsigned long OSSEGMENT;

typedef signed long OSFSEMVAL;
typedef signed long OSSEMVAL;

typedef void (OSENTRYPOINT)(void);

typedef OSADDRESS (OSERRH)(OSBOOLEAN, OSERRCODE, OSERRCODE);



struct HELPER_SIGNAL
{
    SIGSELECT signo;
    void(*PROC)(int,void*);
    int   param1;
    void * param2;
};
typedef struct
{
  char mode;
  int   SID;
  unsigned short event;

  void *data;
  void (*mfree)(void*);
}UI_MESSAGE;


typedef struct
{
  int   count;
  int   signals[3];
}MYSIG;

typedef struct
{
  SIGSELECT signo;
  void (* HOOK)(void*);
}OSE_HOOK_ITEM;

typedef struct
{
  int  unk;
  u16 *path;
  u16 *fname;
  char * content_type;
}FILELISTITEM;


typedef struct
{
  int  FlashFolderName;
  int SavedToFlash;
  u16 *FlashPath;
  int unk0;
  int PhoneFolderName;
  int SavedToPhone;
  u16 *PhonePath;
  int unk1;
  int unk_StrID0;
  int unk_StrID1;
  int unk2;
  int unk3;
}DB_EXT_FOLDERS;


typedef struct
{
  u16 *fname;
  u16 *path;
  char *ctype;
  u16 * fname_zero;
  const char * unk1;
  int unk2;
  u16 unk3;
  u16 unk4;
}FILEITEM;

typedef struct
{
  u16 unk;
  u16 FirstFree;
  void **listdata;
}LIST;

typedef struct
{
  LIST * listbook;
  int    name;
  int    unk;
  int    unk1;
  int    app_ID;
  int    unk2;
}LISTBOOK;

typedef struct
{
  int   unk;
  char * name;
  void * app_session;
  int   unk1;
  int   unk2;
  int   unk3;
  int   unk4;
  int   unk5;
  int   unk6;
  int   unk7;
  int   unk8;
}XBOOK;

struct XGUILIST;
struct GUI;

typedef struct
{
  XBOOK * xbook;
  void * onClose;
  int    sessionID;
  void * idle_page;
  void * base_page;
  struct XGUILIST * xguilist;
  struct GUI * gui;
  void * unk3;
  char dummy[0x10];
}BOOK;

typedef struct XGUILIST
{
  BOOK * book;
  LIST * guilist;
}XGUILIST;

typedef struct
{
  LIST * listbook;
  int    name;
  int    W1;
  int    W2;
  int    SessionID;
  int    unk2;
}UI_APP_SESSION;
//-------------------------------------------
typedef struct
{
  u16   year;
  char  mon;
  char  day;
}DATE;

typedef struct
{
  char  hour;
  char  min;
  char  sec;
  char  unk;
}TIME;

typedef struct
{
  DATE  date;
  TIME  time;
}DATETIME;

typedef struct
{
  int unk;
  int unk1;
  int fsize;
  int unk3;
  int unk4;
  int unk5;
  int unk6;
}FSTAT;


typedef struct
{
  u16 unk;
  char num[40];//2
  char unk1; //43
  char plus;
  char unk2;
  char len;
  // и еще какоето гавно....
  char xx[0x200];
}AB_NUM_ITEM;
/*
typedef struct
{
  u16 len;
  u16 unk;
  char str[];
}AB_STR_ITEM;
*/






typedef struct
{
  int msg;
#ifndef SWI_HOOK
  int (*proc)(void * ,BOOK*);
#else
  int (*proc);

#endif
}PAGE_MSG;

typedef struct
{
  const char * name;
  int unk_NULL;
  const PAGE_MSG * msg_lst;
}PAGE_DESC;



typedef struct
{
  int flag;
  int unk_NULL1;
  int unk_NULL2;
  int HeaderStrID;
  int unk_NULL3;
  int (*onMessage)(void *);
}PAGE_MENU_ITEMS_DESC;

typedef struct
{
  int flag;
  int unk_NULL1;
  int unk_NULL2;
  int HeaderStrID;
  int unk_NULL3;
  int (*onMessage)(void *);
  const PAGE_MENU_ITEMS_DESC * const *items;
}PAGE_MENU_DESC;

typedef struct
{
  int msg;
  int submsg;
  int param1;
  void * param2;
}HELPER_MSG;


typedef struct
{
  unsigned short event;
//  int (* HOOK)(UI_MESSAGE*, BOOK *);
  int (* HOOK)(UI_MESSAGE*);
}UI_HOOK_ITEM;

typedef struct
{
  int (*PROC)(UI_MESSAGE*,BOOK*);
  int SessionID;
  unsigned short event;
  void * APP_DESC;
  void * PAGE_DESC;
}ACTION;
/*
typedef struct
{
  char ** content_type;
  int ext_list;
  void (*PROC)(int*);
  int u3;
  u16* u4;
  int u5;
  int u6;
}DB_EXT;
*/

typedef struct
{
  int cmd;
  int (*PROC)(void*);
  int StrID;
  int (*PROC1)(void * , void *);
}FILESUBROUTINE;


typedef struct
{
  char **content_type;
  u16 **ext_list;
  void (*GetMethods)( FILESUBROUTINE **);
  int u1;
  const DB_EXT_FOLDERS * dbf;
  int u2;
  int u3;
}DB_EXT;

typedef struct
{
  LIST *UserDataList;
  LIST *gKbdHookList;
  LIST *elflist;
  int HPID;
  LIST *UIHookList;
  LIST	*OseHookList;
  DB_EXT **dbe;
  FILESUBROUTINE* elf_ext_m;
}EP_DATA;

typedef struct
{
  int base;
  int size;
  u16 * fname;
}ELF_DESC;

typedef int (*IS_NEEDED_BOOK)(BOOK *);

typedef struct
{
#ifndef SWI_HOOK
  int (*SigHandler)(SIGNAL **sig);
#endif
  PAGE_DESC     *default_page;
  char  * name;
  int (*MsgHandler)(void*msg);
  char * (*GetMsgName)(int);
}APP_DESC;

typedef struct
{
  char  msg;
  BOOK * book;


}GUI_MESSAGE;

struct DISP_DESC;

struct WINDOW;


typedef struct SYS_WINDOW
{
  int   magic;
  u16   x1;
  u16   x2;
  u16   y1;
  u16   y2;
}SYS_WINDOW;


typedef struct WINDOW
{
  struct SYS_WINDOW * swin;
  int unk_int;
  void * unk_win1;
  u16   x1;
  u16   x2;
  u16   y1;
  u16   y2;
  char  isVisible;
  char  unk_char;
  u16   unk_u16;
  u16   unk_u161;
  u16   unk_u162;
  struct DISP_DESC * desc;
}WINDOW;

typedef struct
{
  void * parrent;
  struct DISP_DESC * desc;
  struct WINDOW *window;
  int Win_ID;
  int COLOR1;
  int COLOR2;
  char  font;
  char  font1;
  char  unk;
  char  unk1;
  char dummy[0x88];
  char	* animation;
  char dummy1[20];
  char dummy2[20];
}DISP_OBJ;

typedef struct GUI
{
  DISP_OBJ * DISP_OBJ;
  BOOK * book;
  void * unk;
  void * unk1;
}GUI;

typedef struct
{
  DISP_OBJ * DISP_OBJ;
  BOOK * book;
  void * unk;
  int (* onMessage)(GUI_MESSAGE*);
}GUI_LIST;

typedef struct
{
  DISP_OBJ * DISP_OBJ;
  BOOK * book;
  void * unk;
}GUI_TABMENUBAR;

typedef struct
{
  DISP_OBJ * DISP_OBJ;
  BOOK * book;
  void * unk;
  int (* onMessage)(GUI_MESSAGE*);
}GUI_ONEOFMANY;

typedef struct
{
  DISP_OBJ * DISP_OBJ;
  BOOK * book;
  void * unk;
  int (* onMessage)(GUI_MESSAGE*);
}GUI_NOFMANY;


typedef struct
{
  DISP_OBJ * DISP_OBJ;
  BOOK * book;
  void * unk;
  int (* onMessage)(GUI_MESSAGE*);
}GUI_FEEDBACK;



typedef struct DISP_DESC
{
char *name        ;
u16 DataSize    ;
u16 field_6     ;
void (*Constructor)(DISP_OBJ *);
void (*onCreate)(DISP_OBJ *);
void (*onClose)(DISP_OBJ *);
void (*onRedraw)(DISP_OBJ *);
void (*field_18)(DISP_OBJ *);
void (*field_1C)(DISP_OBJ *);
void (*onKey)(DISP_OBJ *,int scan_code,int unk,int repeat_num,int key_mode);
void (*field_24)(DISP_OBJ *);
void (*field_28)(DISP_OBJ *);
void (*field_2C)(DISP_OBJ *);
void (*field_30)(DISP_OBJ *);
void (*field_34)(DISP_OBJ *);
void (*field_38)(DISP_OBJ *);
void (*field_39)(DISP_OBJ *);
void (*field_3A)(DISP_OBJ *);
char field_3B;
}DISP_DESC;

typedef struct
{
  u16 x1;
  u16 x2;
  u16 y1;
  u16 y2;
}RECT;


typedef struct
{
  char ChargingState;
  char ChargingMethod;
  int BatteryVoltage;
  int ChargerVoltage;
  int ChargerCurrent;
  int CurrentToBattery;
  int BatteryTemperature;
  int SystemTemperature;
  int RemainingCapacity;
  char RemainingCapacityInPercent;
  int PowerDissipation;
  int NrOfChargingCycles;
  int NrOfSecondsOnSafetyTimer;
}BATT;

typedef struct
{
  int magic;
  u16 x1;
  u16 x2;
  u16 y1;
  u16 y2;
}C_INTERFACE;

typedef struct
{
  u16 unk_02;
  int rec_num;
}AB_GET_STRUCT;

typedef struct
{
#ifdef DB2020
  u16 num;
  u16 tab;
  u16 num_in_tab;
  u16 icon;
  int name;
  u16 field_C;
  u16 big_icon;
  u16 field_10;
  u16 ID;  
#else
  u16 num;
  u16 field_2;
  u16 field_4;
  u16 icon;
  int name;
  u16 field_C;
  u16 field_E;
  u16 field_10;
  char field_12;
  char field_13;
  char field_14;
  char field_15;
  u16 field_16l;
  char ID;
  char field_19;
  char field_1A;
  char field_1B;
#endif
}AB_ITEM_DESC;


#endif
