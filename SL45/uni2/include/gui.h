extern void doIDLE(void);
extern void PleaseWait(int boxed);

//linked list
typedef struct
{
  void *first;
  void *last;
  void (*data_mfree)(void *);
}LLQ;

typedef struct
{
  void *next;
  void *prev;
  void *data;
}EDITQ;

typedef struct
{
  void *next;
  void *prev;
}LLIST;


typedef struct{
  void *next;
  void *prev;
  void /*CSM_DESC*/ *constr;
  int id;
  int state;
  int unk1;
  int unk2;
  LLQ gui_ll;
}CSM_RAM;


typedef struct
{
 int (*onMessage)(CSM_RAM*,GBS_MSG*);
 void (*onCreate)(CSM_RAM*);
 void (*onClose)(CSM_RAM*);
 unsigned int datasize; //Min 0x2C
 unsigned int tablesize;
 const unsigned int far *states;
}CSM_DESC;

typedef struct{
  unsigned int x;
  unsigned int y;
  unsigned int x2;
  unsigned int y2;
}RECT;

//check it
typedef struct
{
  RECT *canvas;
  void *methods;
  void *definition;
  char state;
  char unk2;
  char unk3;
  char unk4;
  int color1; //Параметр GeneralFunc пишется сюда?????
  int color2;
  LLQ item_ll;
  int unk5;
  char unk6;
  char unk7;
  char unk8;
  char unk9;
  int unk10;
  int flag30; //Используется при создании (бывает |=0x02)
}GUI;

//check it
typedef struct
{
  char zero;
  char unk1;
  int  keys;
  GBS_MSG *gbsmsg;
}GUI_MSG;

typedef struct {
 unsigned int x;
 unsigned int y;
 unsigned int w;
 unsigned int h;
 int far* pMenuIconID; //array of numbers in pit, with end as 0
 int lgp_id;
 int end_of_data; // 0x7FFF
}HEADER_DESC;

typedef struct
{
  int far* pIconIDList; //array of numbers in pit, with end as 0
  int lgp_id_small;
  int lgp_id_large;
  int zero;
  int *softkeys; //{6,0x22,0x1D}
  int ItemType;
  int ItemConditionCode;
}MENUITEM_DESC;

typedef struct
{
 void huge* proc;
}MENUITEM_HNDL;

typedef struct
{
 int ShortPressCode;     // code to return when this button pressed for a short period
 int LongPressCode;      // when pressed and held
 int lgp_id;             // string ID from lng resources or -1 (no button), -2 (gray button)
}SOFTKEY_DESC;

typedef struct
{
  SOFTKEY_DESC *desc;
  int n;
}SOFTKEYSTAB;

typedef struct
{
 void huge *pUnk1; //4's handler_info
 void huge *global_hook_proc; //GUI * gui, int cmd // void huge *OnKeyPress; //(void far* data, void far* key_data)
//int type=*(int far*)(key_data+0x02);
//0x01 - RED
//0x17 - RSFT
//0x15 - 0-9,*,#
//.....
//int key=*(int far*)(0x06+*(char far**)(key_data+0x04));
//int state=*(int far*)(0x04+*(char far**)(key_data+0x04)); 
 void huge *pUnk3;
 void huge *OnMessage;
//r14:
//0x01 - OnCreate
//0x03 - OnDestroy
//0x07 - OnCurItemChange
//0x0F - OnShiftMenuDown
//0x10 - OnShiftMenuUp 
 int *softkeys; //{6,0x22,0x1D}, mb default for all items, if item.softkeys==NULL
 SOFTKEYSTAB *softkeystab; //вариант софткеев?
 int flag1;
#define md_flag0        0x01 //(1) Обычное меню (0) Анимированное меню. Иконки - из pIconIDList для каждого пункта меню.  (Отображение должно начинаться со второго пункта меню, или должен быть установлен бит 10)
#define md_flag1        0x02 //? меню не прорисовывается и не управляется стрелками
#define md_flag2        0x04
#define md_flag3        0x08 //выделять пункт рамкой (0) или инверсией (1)
#define md_flag4        0x10
#define md_flag5        0x20
#define md_flag6        0x40 //с полосой прокрутки (0) или без (1)
#define md_flag7        0x80 //1 - разрешён режим больших букв
#define md_flag8        0x100 //выравнивание и иконки пунктов меню слева (0) или справа (1)
#define md_flag9        0x200 //показывать номер выдел.пункта в правом верх. углу (1) или нет (0)
#define md_flagA        0x400 //(0) рисовать разделитель в меню с более чем 3-мя пунктами (1)  не рисовать разделитель в меню с более чем 3-мя пунктами
#define md_flagB        0x800
 int zeroflag2;
 void huge *itemproc; //Called when draw item// void huge* OnAppearItem; //ItemIconHandler
//r14 - item number
 MENUITEM_DESC* items; //Table of items desc;
 MENUITEM_HNDL* procs;
 int n_items;
}MENU_DESC;


typedef struct {
 int type;
 void far* Zero1;
 int Zero2;
 int Zero3;
 char flag;//2
 char len;
 void far* Zero4;
 WSHDR far* ws_hdr;
 int Zero5;
}INPUT_DATA;


extern void ClearInputData(INPUT_DATA far*);
extern void SetInputData(INPUT_DATA far*,WSHDR far*,int maxlen);
extern void SetInputType(INPUT_DATA far*, int type); 
//0-no show
//1-only show
//2-digit,*,#
//3-text
//6-only show cursor ?
//5-number(invert, fulllen)
//6-number with dot(on *)
//7-"<text>" not editable
//8-invert,  not editable
extern void AddInputToWSTR(WSHDR far*,INPUT_DATA far*);


//====================================================================================
// Dialogs
//====================================================================================
// kbd msg:
#define KEY_DOWN        0xE9
#define KEY_UP          0xEA 
#define LONG_PRESS      0xEB
#define KEY_LONG        0xEB
// EC - sent 1 second after last key release, but not always?
// ED - ? unknown kbd msg
// EE - ? unknown kbd msg

// For kbd msg:
// P0 - scan Code:
#define LEFT_SOFT       0x01
#define RIGHT_SOFT      0x04
#define RECORD_BUTTON   0x06
#define GREEN_BUTTON    0x0B
#define RED_BUTTON      0x0C
#define VOL_UP_BUTTON   0x0D
#define VOL_DOWN_BUTTON 0x0E
#define PTT_BUTTON      0x3A
#define UP_BUTTON       0x3B
#define DOWN_BUTTON     0x3C
#define LEFT_BUTTON     0x3D
#define RIGHT_BUTTON    0x3E
#define PLAY_BUTTON     0x3F
// '*', '#', '0'-'9'
// P1 - 0 or garbage
// P2 - scan code (if >80h, the key is releeased, if 7B - long press)
// P3 - 
// P4 - scan code with high byte == ??? or FF
// P5 - unknown


typedef struct 
{
 int (*onMessage)(CSM_RAM*,GBS_MSG*);
 void huge* onInit;
 void huge* onExit1;
 void huge* onExit2;
 void huge* onRun;
 unsigned int buf_size1; //0x10 
 unsigned int dummy1;    //0xC4 
}DLG_DESC;

typedef struct 
{
 int Buff[15];
}DLG_BUFF;

// MsgBox
typedef struct
{
 int Flag1;              // 4000 or 4003 or 0
 int Flag2;      // 0
 int Flag3;              // 22h - underlined caption, or 2 - not, 2000h - ???
 int TimeToKeep; // in milliseconds or 0 - infinite
 int unk1;    //0
 int DialogTitle;        // 7FFF - none
 void far *pNull1;       // 0
 int (*BtnHndlProc)(int P1, int P2, int Key);
/* Key:
        for softkeys it is a ShortPressCode/LongPressCode, other keys:
        52 - button released or play button
        46 - record
        12 - "+" or up
        13 - "-" or down
        0c - green
        0a - red
        14 - left
        15 - long press left
        17 - right
        18 - long press right
        2e - numeric keys
        2F - * or #
*/
 void far *pNull2;       // 0 // Init/TimerProc?
 SOFTKEY_DESC far *ButtonDefinitions;        // 4 ButtonDef's
 int Flag4; //20006 - Question; 20007, 20010
 int unk2;  //0
 int unk3;  //0
}MSGBOX_DESC;

extern void CreateModalDialog_0_0(const DLG_DESC far*, DLG_BUFF far* Buff); //ShowDialog = CreateModalDialog(xx,0,0)
extern void CreateModalDialog    (const DLG_DESC far*, DLG_BUFF far* Buff, int flag1,int ID);



extern void ShowMessageDlg                (const MSGBOX_DESC far*, const int far* StringIds );
extern void ShowMessageDlgWithIcon        (const MSGBOX_DESC far*, const int far*);
extern void ShowMessageWithIconAndNoBorder(const MSGBOX_DESC far*, const int far*);

extern void DoBack(int Unk); // 0xA - exit from dialog to previous screen
extern void DoBack0A(void);

extern int  CreateCSM(const CSM_DESC far*, void far* data ,int flag);
extern void CloseCSM(int id);
extern CSM_RAM* FindCSMbyID (int id);


extern int GetBottomCSM_ID(void);
extern int IDLEDIALOG_ID;

extern int  CreateESTR_Dialog(char far* data, char far* WinHeader);
extern void DisplayMessageBoxV(unsigned int t,unsigned int idx);

extern char far* GetCSMPointer(CSM_RAM* data);

typedef struct {
 int flag1;
 int flag2;
 void huge* do1; //  void (*global_hook_proc)(GUI *gui, int cmd);
 void huge* onKey; //0xDB9118 - SendMessage(pid_MMI,0xAC,0x17,WSHDR far*)   int  (*onKey)(GUI *gui, GUI_MSG *msg);
 void huge* do3;
 char far* zero1;
  SOFTKEYSTAB *softkeystab;
 unsigned int x; //0
 unsigned int y; //15
 unsigned int w; //101
 unsigned int h; //55
 int flag3; //font?
 int flag4;
 int Zero1;
 int Zero2;
 int Zero3;
 int Zero4;
 int Zero5;
 int Zero6;
} INPUTDIA_DESC;

extern CreateInputTextDialog(INPUTDIA_DESC far* inputdia_desc, HEADER_DESC far* header, WSHDR far*, int flag);

extern void GeneralFunc(int Unk);
extern int DoEscape(void);


#define menu_full       0x00
#define menu_options    0x01
extern void CreateMenu02(int Style, int Zero1, int Zero2, MENU_DESC*, HEADER_DESC*, char far* Zero3,  char far* Zero4, void far* procZero5, char far* Zero6);

extern void SetMenuItemIcon(char far* Unk1, int Item, int CSMData1, int CSMData2);
extern void RefreshMenuItem(char far* Unk1);
extern int  GetCurrentMenuItem(char far* Unk1);



/*
typedef struct{
  char unk1[20];
  unsigned int file_size;
  char unk2[16];
  char folder_name[129];
  char file_name[200];
}DIR_ENTRY;


//-----------------------------------------

typedef struct
{
  char dummy[40];
  WSHDR *pWS;
}EDITCONTROL;
*/
