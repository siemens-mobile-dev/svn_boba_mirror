#define NULL	0

// kbd msg:
#define KEY_DOWN        0x193
#define KEY_UP          0x194
#define LONG_PRESS      0x195
// 196 - sent 1 second after last key release, but not always?
// 197 - ? unknown kbd msg
// 198 - ? unknown kbd msg

// For kbd msg:
// submsg - scan Code:
#define LEFT_SOFT       0x01
#define RIGHT_SOFT      0x04
#define GREEN_BUTTON    0x0B
#define RED_BUTTON      0x0C
#define VOL_UP_BUTTON   0x14
#define VOL_DOWN_BUTTON 0x15
#define UP_BUTTON       0x3B
#define DOWN_BUTTON     0x3C
#define LEFT_BUTTON     0x3D
#define RIGHT_BUTTON    0x3E
#define ENTER_BUTTON	0x1A
#define INTERNET_BUTTON     0x11
// '*', '#', '0'-'9'

typedef struct
{
  short pid_from;
  short msg;
  int submess;
  void *data0;
  void *data1;
}GBS_MSG;

typedef struct
{
  char unk1;
  char dummy;
  short unk2;
  GBS_MSG *msg;
} GUI_MSG;

typedef struct
{
  void *next;
  void *prev;
  void /*CSM_DESC*/ *constr;
  int id;
  int state;
  int unk1;
  int unk2;
  void *gui_first;
  void *gui_last;
  void *memfree;
}CSM_RAM;

typedef struct
{
  int (*onMessage)(CSM_RAM*,GBS_MSG*);
  void (*onCreate)(CSM_RAM*);
  void (*onClose)(CSM_RAM*);
  int datasize;
  int statesize;
  const int *states;
}CSM_DESC;

typedef struct
{
 int under_idle_id;
 int idle_id;
 void *csm_q;
} CSMROOT;

typedef struct
{
 short x;
 short y;
 short x2;
 short y2;
}RECT;

typedef struct
{
 char dummy[0x24];
}DRWOBJ;

typedef struct
{
 unsigned short *wsbody;
 void *(*ws_malloc)(void);
 void (*ws_mfree)(void *);
 int unk;
 int maxlen;
}WSHDR;

typedef struct
{
 void *unk;
 int unk2;
}GBSTMR;

typedef struct
{
 RECT rc;
 int *icon;
 int lgp_id;
 int lgp_null; //0x7FFFFFFF
}HEADER_DESC;

typedef struct
{
 int *icon;
 int lgp_id_small;
 int lgp_id_large;
 int zero;
 int *softkeys; //{6,0x22,0x1D}
 int flag1; //0
 int flag2; //0x59D
}MENUITEM_DESC;



typedef struct
{
 short key1;
 short key2;
 int lgp_id;
}SOFTKEY_DESC;

typedef struct
{
 SOFTKEY_DESC *desc;
 int n;
}SOFTKEYSTAB;

typedef struct
{
 int flag; //0,8 etc
 void *onKey;		//GUI * gui , GUI_MSG * msg
 void *global_hook_proc; //GUI * gui, int cmd
 void *proc3;
 int *softkeys; //{6,0x22,0x1D}, mb default for all items, if item.softkeys==NULL
 SOFTKEYSTAB *softkeystab;
 int flags2; //2
 void *itemproc; //Called when draw item
 MENUITEM_DESC * items; //Table of items desc;
 void ** procs; //Table of procs when item selected
 int n_items; //Number of items
}MENU_DESC;

//Constants
//Seek
#define S_SET 0
#define S_CUR 1
#define S_END 2

//Permissions
#define P_WRITE 0x100
#define P_READ 0x80

//Access
#define A_ReadOnly 0
#define A_WriteOnly 1
#define A_ReadWrite 2
#define A_NoShare 4
#define A_Append 8
#define A_Exclusive 0x10
#define A_MMCStream 0x20
#define A_Create 0x100
#define A_Truncate 0x200
#define A_FailCreateOnExist 0x400
#define A_FailOnReopen 0x800

#define A_TXT 0x4000
#define A_BIN 0x8000



extern __arm void *malloc (unsigned int size);
extern __arm void mfree (void *);

extern __arm void *memcpy(void *, void const *, int);
extern __arm void zeromem(void *, int);



// File library
extern __arm int fopen(const char *cFileName,unsigned int iFileFlags,unsigned int iFileMode,unsigned int *ErrorNumber);
extern __arm void fclose(int FileHandler,unsigned int *ErrorNumber);
extern __arm int fread(int FileHandler,void *cBuffer,int iByteCount,unsigned int *ErrorNumber);
extern __arm int fwrite(int FileHandler,char *cBuffer,int iByteCount,unsigned int *ErrorNumber);
extern __arm int fflush(int FileHandler,int *ErrorNumber);
extern __arm int lseek(int FileHandler,unsigned int offset,unsigned int origin,unsigned int *ErrorNumber,unsigned int *ErrorNumber2);

//GBS library
extern __arm GBS_StartTimer(GBSTMR *tmr,int time,int msg,int unk,int cepid);
extern __arm GBS_StartTimerProc(GBSTMR *tmr,int time,void *proc);
extern __arm GBS_SendMessage(int cepid, int msg, ...); //int submsg, void *data1, void *data2

//GSM library
extern __thumb int isCallActive(void);
extern __thumb int getConvTime(void);
//CSM library
extern CSMROOT CSM_root;
extern __thumb int CreateCSM(const CSM_DESC*,void *,int);
extern __thumb void *FindCSMbyID(int id);
extern __thumb void IDLECSM_onCreate(CSM_RAM*);

//GUI library
extern __thumb void DirectRedrawGUI(void);
extern __thumb void DirectRedrawGUI_ID(int id);
extern __thumb void PendedRedrawGUI(void);
extern __thumb void CreateMenu(int isSmall,int unk,MENU_DESC const *,HEADER_DESC const *,int unk1,int items_count,int unk2,int unk3);
extern __thumb void *GetDataOfItemByID(void *, int);
extern __thumb void SetMenuItemIcon(void *GUI,int item,int icon);


//Draw objects library
extern __thumb void DrawObject(DRWOBJ *);
extern __thumb void SetPropTo_Obj1(DRWOBJ *drwobj,void *rect,int rect_flag,WSHDR *wshdr,int font,int text_flag);
extern __thumb void StoreXYWHtoRECT(RECT *rect,int x,int y,int w,int h);
extern __thumb void FreeDrawObject_subobj(DRWOBJ *drwobj);
extern __thumb void SetColor(DRWOBJ *drwobj, int color1, int color2);
extern __thumb void DrawFrameInNativeMenu(int x1,int y1,int x2,int y2,int x_round,int y_round,int flags,int pen,int brush);
extern __thumb int  GetPaletteAdrByColorIndex(int ColorIndex) ;
extern __thumb void DrawString(WSHDR *WSHDR,int x1,int y1,int x2,int y2,int font,int text_attribute,int Pen,int Brush);
//Strings library
extern __thumb char *strcpy(char *, const char *);
extern __thumb char *strstr(const char *, const char *);
extern __thumb int strlen(char *);
extern __thumb void strcat(char * , const char *);
extern __thumb int sprintf (char *s,const char *format,...);

//Wide strings library
extern __thumb WSHDR *AllocWS(int);
extern __thumb void FreeWS(WSHDR *);
extern __thumb WSHDR *CreateLocalWS(WSHDR *wshdr,unsigned short *wsbody,int len);
extern __thumb int wstrlen(WSHDR *);
extern __thumb WSHDR *wstrcpy(WSHDR *, WSHDR *);
extern __thumb WSHDR *wstrncpy(WSHDR *, WSHDR *, int);
extern __thumb WSHDR *wstrcpy_safe(WSHDR *, WSHDR *);
extern __thumb WSHDR *wstrcat(WSHDR *, WSHDR *);
extern __thumb WSHDR *cutwstr(WSHDR *, int);
extern	__thumb	void wsaddwchar (void *WSHDR,int wchar);
extern __thumb int wsprintf(WSHDR *, const char *format, ...);
extern __thumb int ws2ascii(char * dest, WSHDR * wssource, int wslen);
extern __thumb void asciz2wstr(WSHDR * , char *);
// -----------

extern __arm int getDialStringLen(void *);
extern __thumb WSHDR* getDialString(void *);
extern __thumb void openAB(void);
extern __thumb void CreateEditViewCSM(int , int);
extern __thumb void Open_NewSMS_Dialog(WSHDR*, char*, int,int,int);
extern __thumb int GetCurrMenuItem(void * GUI);
extern __thumb void SetNMenuItems(void *GUI, int n_items);
extern __thumb int MakeVoiceCallTo(char * phonenum, int , int);
extern __arm void StartMidlet(char *);


//Adress book library
extern __thumb void UnPackABrecord(void *, void *, int ,int);
extern __thumb void FreeUnPackABrecord(void * , void (*)(void *));
extern __thumb void ABnumber2str(void * , char *, int , int);


/*
//Files library
#pragma swi_number=0x0010
__swi int fopen(const char * cFileName, unsigned int iFileFlags, unsigned int iFileMode, int *ErrorNumber);

#pragma swi_number=0x0011
__swi int fread(int FileHandler, void *cBuffer, int iByteCount, int *ErrorNumber);

#pragma swi_number=0x0012
__swi int fwrite(int FileHandler, const char * cBuffer, int iByteCount, int *ErrorNumber);

#pragma swi_number=0x0013
__swi int fclose(int FileHandler, int *ErrorNumber);

#pragma swi_number=0x0014
__swi int fflush(int FileHandler, int *ErrorNumber);

#pragma swi_number=0x0015
__swi unsigned int lseek(int FileHandler, unsigned int offset, unsigned int origin, int *ErrorNumber, int *ErrorNumber2);
*/
