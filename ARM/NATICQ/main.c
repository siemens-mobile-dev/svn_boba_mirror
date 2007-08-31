#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "../inc/pnglist.h"
#include "NatICQ.h"
#include "history.h"
#include "conf_loader.h"
#include "mainmenu.h"
#include "main.h"
#include "language.h"
#include "../inc/xtask_ipc.h"
#include "smiles.h"
#include "naticq_ipc.h"

#ifndef NEWSGOLD
#define SEND_TIMER
#endif

extern volatile int total_smiles;
extern volatile int total_xstatuses;
extern volatile int xstatuses_load;

#define USE_MLMENU

#define TMR_SECOND 216

//IPC
const char ipc_my_name[32]="NatICQ";
const char ipc_xtask_name[]=IPC_XTASK_NAME;
IPC_REQ gipc;

int maincsm_id;
int maingui_id;

void SMART_REDRAW(void)
{
  int f;
  LockSched();
  f=IsGuiOnTop(maingui_id);
  UnlockSched();
  if (f) REDRAW();
}

//По 10 секунд
#define ACTIVE_TIME 360

//Максимальное количество сообщений в логе
#define MAXLOGMSG (20)

// Строковые описания статусов

const char S_OFFLINE[]="Offline";
const char S_INVISIBLE[]="Invisible";
const char S_AWAY[]="Away";
const char S_NA[]="N/A";
const char S_OCCUPIED[]="Occupied";
const char S_DND[]="DND";
const char S_ONLINE[]="Online";
const char S_FFC[]="FFC";

volatile int SENDMSGCOUNT;

int IsActiveUp=0;

int Is_Vibra_Enabled;
unsigned int Is_Sounds_Enabled;
int Is_Show_Offline;
int Is_Show_Groups;

int S_ICONS[TOTAL_ICONS+1];

#define EOP -10
int CurrentStatus;
int CurrentXStatus;

WSHDR *ews;

extern const unsigned int UIN;
extern const char PASS[];

static const char * const icons_names[TOTAL_ICONS]=
{
  "offline.png",
  "invisible.png",
  "away.png",
  "na.png",
  "occupied.png",
  "dnd.png",
  "depression.png",
  "evil.png",
  "home.png",
  "lunch.png",
  "work.png",
  "online.png",
  "ffc.png",
  "message.png",
  "unknown.png",
  "groupon.png",
  "groupoff.png",
  "null.png",
  "addcont.png",
  "head.png",
  "logo.png",
  "ping.png",
  "settings.png"
};

extern const char ICON_PATH[];

char *MakeGlobalString(const char *first, int breakchar, const char *last)
{
  int l;
  char *p;
  p=malloc((l=strlen(first))+strlen(last)+2);
  strcpy(p,first);
  p[l++]=breakchar;
  strcpy(p+l,last);
  return p;
}

void setup_ICONS(void)
{
  int i=0;
  do
  {
    if (!S_ICONS[i]) S_ICONS[i]=(int)MakeGlobalString(ICON_PATH,'\\',icons_names[i]);
    i++;
  }
  while(i<TOTAL_ICONS);
  return;
}

void free_ICONS(void)
{
  int i=0;
  do
  {
    mfree((void*)S_ICONS[i]);
    S_ICONS[i]=0;
    i++;
  }
  while(i<TOTAL_ICONS);
  return;
}

extern const unsigned int IDLEICON_X;
extern const unsigned int IDLEICON_Y;

extern const unsigned int I_COLOR;
extern const unsigned int TO_COLOR;

extern const unsigned int ED_FONT_SIZE;

const char percent_t[]="%t";
const char percent_d[]="%d";
const char empty_str[]="";
const char I_str[]="I";

char logmsg[256];

//Illumination by BoBa 19.04.2007
///////////
extern const unsigned int ILL_DISP_RECV;
extern const unsigned int ILL_KEYS_RECV;
extern const unsigned int ILL_DISP_SEND;
extern const unsigned int ILL_KEYS_SEND;
extern const unsigned int ILL_SEND_TMR;
extern const unsigned int ILL_SEND_FADE;
extern const unsigned int ILL_RECV_TMR;
extern const unsigned int ILL_RECV_FADE;
extern const unsigned int ILL_OFF_FADE;

GBSTMR tmr_illumination;

void IlluminationOff(){
  SetIllumination(0,1,0,ILL_OFF_FADE);
  SetIllumination(1,1,0,ILL_OFF_FADE);
}

void IlluminationOn(const int disp, const int key, const int tmr, const int fade){
  GBS_DelTimer(&tmr_illumination);
  SetIllumination(0,1,disp,fade);
  SetIllumination(1,1,key,fade);
  GBS_StartTimerProc(&tmr_illumination,tmr*216,IlluminationOff);
}

volatile int silenthide;    //by BoBa 25.06.07
///////////

//===================================================================
extern S_SMILES *s_top;
extern DYNPNGICONLIST *SmilesImgList;
extern DYNPNGICONLIST *XStatusesImgList;

//=============================Проигрывание звука=======================
extern const char sndStartup[];
extern const char sndSrvMsg[];
extern const char sndGlobal[];
extern const char sndMsg[];
extern const char sndMsgSent[];
extern const unsigned int sndVolume;


void Play(const char *fname)
{
  if ((!IsCalling())&&Is_Sounds_Enabled)
  {
    FSTATS fstats;
    unsigned int err;
    if (GetFileStats(fname,&fstats,&err)!=-1)
    {
      PLAYFILE_OPT _sfo1;
      WSHDR* sndPath=AllocWS(128);
      WSHDR* sndFName=AllocWS(128);
      char s[128];
      const char *p=strrchr(fname,'\\')+1;
      str_2ws(sndFName,p,128);
      strncpy(s,fname,p-fname);
      s[p-fname]='\0';
      str_2ws(sndPath,s,128);
      
      zeromem(&_sfo1,sizeof(PLAYFILE_OPT));
      _sfo1.repeat_num=1;
      _sfo1.time_between_play=0;
      _sfo1.play_first=0;
      _sfo1.volume=sndVolume;
#ifdef NEWSGOLD
      _sfo1.unk6=1;
      _sfo1.unk7=1;
      _sfo1.unk9=2;
      PlayFile(0x10, sndPath, sndFName, GBS_GetCurCepid(), MSG_PLAYFILE_REPORT, &_sfo1);
#else
#ifdef X75
      _sfo1.unk4=0x80000000;
      _sfo1.unk5=1;
      PlayFile(0xC, sndPath, sndFName, 0,GBS_GetCurCepid(), MSG_PLAYFILE_REPORT, &_sfo1);
#else
      _sfo1.unk5=1;
      PlayFile(0xC, sndPath, sndFName, GBS_GetCurCepid(), MSG_PLAYFILE_REPORT, &_sfo1);
#endif
#endif
      FreeWS(sndPath);
      FreeWS(sndFName);
    }
  }
}

GBSTMR tmr_vibra;
volatile int vibra_count;

void start_vibra(void)
{
  void stop_vibra(void);
  if((Is_Vibra_Enabled)&&(!IsCalling()))
  {
    extern const unsigned int vibraPower;
    SetVibration(vibraPower);
    GBS_StartTimerProc(&tmr_vibra,TMR_SECOND>>1,stop_vibra);
  }
}

void stop_vibra(void)
{
  SetVibration(0);
  if (--vibra_count)
  {
    GBS_StartTimerProc(&tmr_vibra,TMR_SECOND>>1,start_vibra);
  }
}

void ChangeVibra(void)
{
  if (!(Is_Vibra_Enabled=!(Is_Vibra_Enabled)))
    ShowMSG(1,(int)LG_MSGVIBRADIS);
  else
    ShowMSG(1,(int)LG_MSGVIBRAENA);
}

void ChangeSound(void)
{
  if (!(Is_Sounds_Enabled=!(Is_Sounds_Enabled)))
    ShowMSG(1,(int)LG_MSGSNDDIS);
  else
    ShowMSG(1,(int)LG_MSGSNDENA);
}

//===================================================================
//Templates
char *templates_chars; //Собственно файл
char **templates_lines; //Массив указателей на строки

void FreeTemplates(void)
{
  if (templates_lines) mfree(templates_lines);
  if (templates_chars) mfree(templates_chars);
  templates_lines=NULL;
  templates_chars=NULL;
}

extern const char TEMPLATES_PATH[];

int LoadTemplates(unsigned int uin)
{
  FSTATS stat;
  char fn[256];
  int f;
  unsigned int ul;
  int i;
  int fsize;
  char *p;
  char *pp;
  int c;
  FreeTemplates();
  strcpy(fn,TEMPLATES_PATH);
  i=strlen(fn);
  sprintf(fn+i,"\\%d.txt",uin);
  if (GetFileStats(fn,&stat,&ul)==-1) goto L1;
  if ((fsize=stat.size)<=0) goto L1;
  if ((f=fopen(fn,A_ReadOnly+A_BIN,P_READ,&ul))==-1)
  {
  L1:
    strcpy(fn+i,"\\0.txt");
    if (GetFileStats(fn,&stat,&ul)==-1) return 0;
    if ((fsize=stat.size)<=0) return 0;
    f=fopen(fn,A_ReadOnly+A_BIN,P_READ,&ul);
  }
  if (f==-1) return 0;
  p=templates_chars=malloc(fsize+1);
  p[fread(f,p,fsize,&ul)]=0;
  fclose(f,&ul);
  i=0;
  pp=p;
  for(;;)
  {
    c=*p;
    if (c<32)
    {
      if (pp&&(pp!=p))
      {
	templates_lines=realloc(templates_lines,(i+1)*sizeof(char *));
	templates_lines[i++]=pp;
      }
      pp=NULL;
      if (!c) break;
      *p=0;
    }
    else
    {
      if (pp==NULL) pp=p;
    }
    p++;
  }
  return i;
}


//===================================================================

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;



int RXstate=EOP; //-sizeof(RXpkt)..-1 - receive header, 0..RXpkt.data_len - receive data

char *msg_buf;
TPKT RXbuf;
TPKT TXbuf;

int connect_state=0;

int sock=-1;

volatile unsigned long TOTALRECEIVED;
volatile unsigned long TOTALSENDED;

volatile int sendq_l=0; //Длинна очереди для send
volatile void *sendq_p=NULL; //указатель очереди

volatile int is_gprs_online=1;

GBSTMR reconnect_tmr;

extern void kill_data(void *p,void (*func_p)(void *));

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

int total_unread;

//===============================================================================================
#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}

#pragma inline
void patch_header(const HEADER_DESC* head)
{
  ((HEADER_DESC*)head)->rc.x=0;
  ((HEADER_DESC*)head)->rc.y=YDISP;
  ((HEADER_DESC*)head)->rc.x2=ScreenW()-1;
  ((HEADER_DESC*)head)->rc.y2=HeaderH()+YDISP-1;
}
#pragma inline
void patch_input(const INPUTDIA_DESC* inp)
{
  ((INPUTDIA_DESC*)inp)->rc.x=0;
  ((INPUTDIA_DESC*)inp)->rc.y=HeaderH()+1+YDISP;
  ((INPUTDIA_DESC*)inp)->rc.x2=ScreenW()-1;
  ((INPUTDIA_DESC*)inp)->rc.y2=ScreenH()-SoftkeyH()-1;
}
//===============================================================================================

volatile CLIST *cltop;

volatile unsigned int GROUP_CACHE; //Текущая группа для добавления

volatile int contactlist_menu_id;

GBSTMR tmr_active;

volatile int edchat_id;

//Применяется для добавления сообщений
//CLIST *edcontact;
//void *edgui_data;

static int prev_clmenu_itemcount;

char clm_hdr_text[48];
static const char def_clm_hdr_text[] = LG_CLTITLE;
static const char key_clm_hdr_text[] = LG_CLT9INP;

static const HEADER_DESC contactlist_menuhdr = {0, 0, 0, 0, S_ICONS+ICON_HEAD, (int)clm_hdr_text, LGP_NULL};
static const int menusoftkeys[] = {0,1,2};
static const SOFTKEY_DESC menu_sk[] =
{
  {0x0018, 0x0000, (int)LG_OPTIONS},
  {0x0001, 0x0000, (int)LG_CLOSE},
  {0x003D, 0x0000, (int)LGP_DOIT_PIC}
};

char clmenu_sk_r[16];
static const char def_clmenu_sk_r[] = LG_CLOSE;
static const char key_clmenu_sk_r[] = LG_CLEAR;

static const SOFTKEY_DESC clmenu_sk[]=
{
  {0x0018, 0x0000, (int)LG_OPTIONS},
  {0x0001, 0x0000, (int)clmenu_sk_r},
  {0x003D, 0x0000, (int)LGP_DOIT_PIC}
};

const SOFTKEYSTAB menu_skt =
{
  menu_sk, 0
};

static const SOFTKEYSTAB clmenu_skt =
{
  clmenu_sk, 0
};

void contactlist_menu_ghook(void *data, int cmd);
int contactlist_menu_onkey(void *data, GUI_MSG *msg);
void contactlist_menu_iconhndl(void *data, int curitem, void *unk);

#ifdef USE_MLMENU
static const ML_MENU_DESC contactlist_menu=
{
  8,contactlist_menu_onkey,contactlist_menu_ghook,NULL,
  menusoftkeys,
  &clmenu_skt,
  0x11, //+0x400
  contactlist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0,   //n
  1 //Добавочных строк  
};
#else
static const MENU_DESC contactlist_menu=
{
  8,(void *)contactlist_menu_onkey,(void*)contactlist_menu_ghook,NULL,
  menusoftkeys,
  &clmenu_skt,
  0x11, //+0x400
  (void *)contactlist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};
#endif

//GBSTMR tmr_ping;
int tenseconds_to_ping;

LOGQ *NewLOGQ(const char *s)
{
  LOGQ *p=malloc(sizeof(LOGQ)+1+strlen(s));
  zeromem(p,sizeof(LOGQ));
  strcpy(p->text,s);
  return p;
}

LOGQ *LastLOGQ(LOGQ **pp)
{
  LOGQ *q=*pp;
  if (q)
  {
    while(q->next) q=q->next;
  }
  return(q);
}

//Уничтожить лог
void FreeLOGQ(LOGQ **pp)
{
  LOGQ *p=*pp;
  *pp=NULL; //Сразу обрежем
  while(p)
  {
    LOGQ *np=p->next;
    mfree(p);
    p=np;
  }
}

//Удалить элемент из лога
void RemoveLOGQ(LOGQ **queue, LOGQ *p)
{
  LOGQ *q=(LOGQ *)queue;
  LOGQ *qp;
  while(qp=q->next)
  {
    if (qp==p)
    {
      q->next=p->next;
      break;
    }
    q=qp;
  }
  mfree(p); //Если элемент вообще не в логе
}

//Добавить элемент в лог
int AddLOGQ(LOGQ **queue, LOGQ *p)
{
  LOGQ *q=(LOGQ *)queue;
  LOGQ *qp;
  int i=0;
  while(qp=q->next) {q=qp;i++;}
  p->next=NULL;
  q->next=p;
  return(i+1); //Теперь всего в логе элементов
}

void FreeXText(CLIST *t)
{
  if (t->xtext) {mfree(t->xtext);t->xtext=NULL;}
}

//Уничтожить список
void FreeCLIST(void)
{
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  while(cl)
  {
    CLIST *p;
    if (cl->log) FreeLOGQ(&cl->log);
    if (cl->answer) mfree(cl->answer);
    if (cl->xtext) mfree(cl->xtext);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

int GetIconIndex(CLIST *t)
{
  unsigned short s;
  if (t)
  {
    s=t->state;
    if (t->isgroup) return(IS_GROUP);
    if (t->isunread)
      return(IS_MSG);
    else
    {
      if (s==0xFFFF) return(IS_OFFLINE);
      if ((s&0xF000)==0x2000) return (IS_LUNCH);
      if ((s&0xF000)==0x3000) return (IS_EVIL);
      if ((s&0xF000)==0x4000) return (IS_DEPRESSION);
      if ((s&0xF000)==0x5000) return (IS_HOME);
      if ((s&0xF000)==0x6000) return (IS_WORK);
      if (s & 0x0020) return(IS_FFC);
      if (s & 0x0001) return(IS_AWAY);
      if (s & 0x0005) return(IS_NA);
      if (s & 0x0011) return(IS_OCCUPIED);
      if (s & 0x0013) return(IS_DND);
      if (s & 0x0002) return(IS_FFC);
      if (s & 0x0100) return(IS_INVISIBLE);
    }
  }
  else
  {
    return(IS_UNKNOWN);
  }
  return(IS_ONLINE);
}

CLIST *FindContactByUin(unsigned int uin)
{
  CLIST *t;
  t=(CLIST *)cltop;
  while(t)
  {
    if ((t->uin==uin)&&(!t->isgroup)) break;
    t=t->next;
  }
  return(t);
}

CLIST *FindGroupByID(unsigned int grp)
{
  CLIST *t;
  t=(CLIST *)cltop;
  while(t)
  {
    if ((t->group==grp)&&(t->isgroup)) break;
    t=t->next;
  }
  return(t);
}

LOGQ *FindContactLOGQByAck(TPKT *p)
{
  CLIST *t;
  LockSched();
  t=FindContactByUin(p->pkt.uin);
  UnlockSched();
  unsigned int id=*((unsigned short*)(p->data));
  LOGQ *q;
  if (!t) return NULL;
  LockSched();
  q=t->log;
  while(q)
  {
    if (q->ID==id) break;
    q=q->next;
  }
  UnlockSched();
  return q;
}

//Ключи для поиска по T9
static const char table_T9Key[256]=
"11111111111111111111111111111111"
"10001**0***0000*012345678900***0"
"0222333444555666777788899991*110"
"122233344455566677778889999111*1"
"11111111111111111111111111111111"
"11111111311111111111111131111111"
"22223333444455566677778888899999"
"22223333444455566677778888899999";

char ContactT9Key[32];

int strcmp_nocase(const char *s, const char *d)
{
  int cs;
  int ds;
  do
  {
    cs=*s++;
    if (cs&0x40) cs&=0xDF;
    ds=*d++;
    if (ds&0x40) ds&=0xDF;
    cs-=ds;
    if (cs) break;
  }
  while(ds);
  return(cs);
}

int CompareContacts(CLIST *t, CLIST *p)
{
  int c;
  /*  int i1=t->isactive?1:0;
  int i2=p->isactive?1:0;
  if ((c=i1-i2))
  {
  return(c);
}*/
  if ((c=p->group-t->group))
  {
    return(c);
  }
  
  extern const int SORT_CLIST;
  int ip=GetIconIndex(p);
  int it=GetIconIndex(t);
  switch (SORT_CLIST)
  {
  case 0: // by name
    if ((ip>IS_OFFLINE)&&(ip<IS_MSG)&&
	(it>IS_OFFLINE)&&(it<IS_MSG)) break;
  case 1: // by status
    if ((c=ip-it))
    {
      return(c);
    }
    break;
  }
  
  return(strcmp_nocase(t->name,p->name));
}

CLIST *FindContactByNS(int *i, int si, int act_flag, CLIST *search_contact)
{
  CLIST *t;
  t=(CLIST *)cltop;
  char *s;
  char *d;
  int c;
  int grp_id=0;
  int grp_dis=0;
  while(t)
  {
    if (t->isgroup)
    {
      grp_id=t->group;
      grp_dis=t->state;
    }
    if (act_flag<2)
    {
      if ((act_flag)&&(!t->isactive)) goto L_NOT9;
      if ((!act_flag)&&(t->isactive)) goto L_NOT9;
    }
    if ((si==IS_ANY)||(GetIconIndex(t)==si))
    {
      s=ContactT9Key;
      if ((!Is_Show_Groups)&&(t->isgroup)) goto L_NOT9;
      
      if ((!t->isgroup)&&(t->group==grp_id)&&(grp_dis)&&(!(*s))) goto L_NOT9;
      
      if (!Is_Show_Offline) // by Seklth 13.06.2007
        if ((!t->isgroup)&&(!t->isactive)&&(t->state==0xFFFF)&&(t->uin!=UIN)&&(!(*s))) 
          if (!t->isunread) goto L_NOT9;
      
      d=t->name;
      while(c=*s++)
      {
	if (c!=table_T9Key[*d++]) goto L_NOT9;
      }
      if (search_contact==t) return t; //Нашли контакт по адресу
      if (!(*i)) return(t);
      (*i)--;
    }
  L_NOT9:
    t=t->next;
  }
  return(t);
}

void UpdateCLheader(void)
{
  if (strlen(ContactT9Key))
  {
    strcpy(clm_hdr_text,key_clm_hdr_text);
    strcat(clm_hdr_text,ContactT9Key);
    strcpy(clmenu_sk_r,key_clmenu_sk_r);
  }
  else
  {
    strcpy(clm_hdr_text,def_clm_hdr_text);
    strcpy(clmenu_sk_r,def_clmenu_sk_r);
  }
}

void ClearContactT9Key(void)
{
  zeromem(ContactT9Key,sizeof(ContactT9Key));
}

void AddContactT9Key(int chr)
{
  int l=strlen(ContactT9Key);
  if (l<(sizeof(ContactT9Key)-1))
  {
    ContactT9Key[l]=chr;
  }
}

void BackSpaceContactT9(void)
{
  int l=strlen(ContactT9Key);
  if (l)
  {
    l--;
    ContactT9Key[l]=0;
  }
}

int CountContacts(void)
{
  int l=-1;
  FindContactByNS(&l,IS_ANY,2,NULL);
  l=-1-l;
  return l;
}

CLIST *FindContactByN(int i)
{
  CLIST *t;
  int f=IsActiveUp;
  if (f)
  {
    t=FindContactByNS(&i,IS_ANY,1,NULL); if ((!i)&&(t)) return (t);
    f=0;
  }
  else f=2;
  t=FindContactByNS(&i,IS_ANY,f,NULL);
  return t;
}

int FindContactByContact(CLIST *p)
{
  int l=-1;
  //  CLIST *t;
  int f=IsActiveUp;
  if (f)
  {
    if (FindContactByNS(&l,IS_ANY,1,p)) return -1-l;
    f=0;
  }
  else f=2;
  FindContactByNS(&l,IS_ANY,f,p);
  return -1-l;
}

void SwapContacts(CLIST *first, CLIST *second)
{
  CLIST *tp;
  if ((tp=second->next)) tp->prev=first;
  if ((tp=first->prev)) tp->next=second;
  first->next=second->next;
  second->next=first;
  second->prev=first->prev;
  first->prev=second;
}

void ChangeContactPos(CLIST *p)
{
  CLIST *t;
  if ((t=p->prev))
  {
    //Проверяем, не надо ли всплывать
    while(CompareContacts(t,p)>0)
    {
      //Всплываем вверх списка
      SwapContacts(t,p);
      if (!(t=p->prev)) return; //Всплыли
    }
  }
  if ((t=p->next))
  {
    //Проверяем, не надо ли углубляться
    while(CompareContacts(p,t)>0)
    {
      SwapContacts(p,t);
      if (!(t=p->next)) return; //Углубились нах ;)
    }
  }
}

//Прописать всех в offline
void FillAllOffline(void)
{
  CLIST *cl=(CLIST*)cltop;
  while(cl)
  {
    if ((cl->state!=0xFFFF)&&
	(!cl->isgroup))     //by BoBa 2.05.2007
    {
      CLIST *p=cl;
      p->state=0xFFFF;
      FreeXText(p);
      cl=cl->next;
      ChangeContactPos(p);
      continue;
    }
    cl=(CLIST*)(cl->next);
  }
}

void create_contactlist_menu(void)
{
  int i;
  i=CountContacts();
  prev_clmenu_itemcount=i;
  UpdateCLheader();
  patch_header(&contactlist_menuhdr);
#ifdef USE_MLMENU
  contactlist_menu_id=CreateMultiLinesMenu(0,0,&contactlist_menu,&contactlist_menuhdr,0,i);
#else
  contactlist_menu_id=CreateMenu(0,0,&contactlist_menu,&contactlist_menuhdr,0,i,0,0);
#endif
}

void contactlist_menu_ghook(void *data, int cmd)
{
  PNGTOP_DESC *pltop=PNG_TOP();
  if (cmd==9)
  {
    pltop->dyn_pltop=NULL;
  }
  if (cmd==0x0A)
  {
    silenthide=0;
    pltop->dyn_pltop=XStatusesImgList;
    DisableIDLETMR();
  }
}

void RecountMenu(CLIST *req)
{
  int i;
  int j;
  void *data;
  UpdateCLheader();
  if (!contactlist_menu_id) return; //Нечего считать
  data=FindGUIbyId(contactlist_menu_id,NULL);
  if (req==NULL)
  {
    j=0;
  }
  else
  {
    j=FindContactByContact(req);
  }
  i=CountContacts();
  if (j>=i) j=i-1;
  if (j<0) j=0;
  if (i!=prev_clmenu_itemcount)
  {
    prev_clmenu_itemcount=i;
    Menu_SetItemCountDyn(data,i);
  }
  SetCursorToMenuItem(data,j);
  if (IsGuiOnTop(contactlist_menu_id)) RefreshGUI();
}

int contactlist_menu_onkey(void *data, GUI_MSG *msg)
{
  CLIST *t;
  int i;
  i=GetCurMenuItem(data);
  t=FindContactByN(i);
  if (msg->keys==0x18)
  {
    ShowMainMenu();
    return(-1);
  }
  if (msg->keys==0x3D)
  {
    if (t)
    {
      if (t->isgroup)
      {
	t->state^=0xFFFF;
	RecountMenu(t);
	return(-1);
      }
      if (strlen(ContactT9Key))
      {
	ClearContactT9Key();
	RecountMenu(NULL);
      }
      CreateEditChat(t);
    }
    return(-1);
  }
  if (msg->keys==1)
  {
    if (strlen(ContactT9Key))
    {
      BackSpaceContactT9();
      RecountMenu(NULL);
      return(-1);
    }
  }
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    int key=msg->gbsmsg->submess;
    if (((key>='0')&&(key<='9'))||(key=='#')||(key=='*'))
    {
      AddContactT9Key(key);
      RecountMenu(NULL);
      return(-1);
    }
    if (key==GREEN_BUTTON)
    {
      IsActiveUp=!IsActiveUp;
      RecountMenu(NULL);
      return(-1);
    }
    if (key==RIGHT_BUTTON)
    {
      //Послать запрос текста XStatus
      if (t)
      {
	if (!t->isgroup)
	{
	  TPKT *p;
	  FreeXText(t);
	  p=malloc(sizeof(PKT));
	  p->pkt.uin=t->uin;
	  p->pkt.type=T_XTEXT_REQ;
	  p->pkt.data_len=0;
	  SUBPROC((void *)SendAnswer,0,p);
	  RefreshGUI();
	  return(-1);
	}
      }
    }
  }
  if (msg->gbsmsg->msg==LONG_PRESS)
  {
    if (msg->gbsmsg->submess=='#')
    {
      ClearContactT9Key();
      RecountMenu(NULL);
      gipc.name_to=ipc_xtask_name;
      gipc.name_from=ipc_my_name;
      gipc.data=0;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_XTASK_IDLE,&gipc);
      if (IsUnlocked())
      {
        KbdLock();
      }
      return(-1);
    }    
    if (msg->gbsmsg->submess=='*'){
      ClearContactT9Key();
      RecountMenu(NULL);
      silenthide=1;
      gipc.name_to=ipc_xtask_name;
      gipc.name_from=ipc_my_name;
      gipc.data=0;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_XTASK_IDLE,&gipc);
    }    
  }
  return(0);
}

void GetOnTotalContact(int group_id,int *_onlinetotal)
{
  CLIST *t;
  t=(CLIST *)&cltop;
  int online=0,total=0;
  while((t=t->next))
  {
    if(t->group==group_id && !t->isgroup)
    {
      total++;
      if (t->state!=0xFFFF) online++;
    }
  }
  _onlinetotal[0]=online;
  _onlinetotal[1]=total;
}


void contactlist_menu_iconhndl(void *data, int curitem, void *unk)
{
  CLIST *t;
#ifdef USE_MLMENU
  void *item=AllocMLMenuItem(data);
#else
  void *item=AllocMenuItem(data);
#endif
  int icon;
  
  WSHDR *ws2;
#ifdef USE_MLMENU
  WSHDR *ws4;
#endif
  WSHDR ws1loc, *ws1;
  unsigned short num[128];
  ws1=CreateLocalWS(&ws1loc,num,128);
#ifdef USE_MLMENU
  WSHDR ws3loc, *ws3;
  unsigned short num3[128];
  ws3=CreateLocalWS(&ws3loc,num3,128);
  char s[64];
#endif
  t=FindContactByN(curitem);
  if (t)
  {
    icon=GetIconIndex(t);
    if (icon!=IS_GROUP)
    {
      wsprintf(ws1,percent_t,t->name);
      if (t->isactive)
      {
        wsInsertChar(ws1,0x0002,1);
        wsInsertChar(ws1,0xE008,1);
      }
#ifdef USE_MLMENU
      if (t->xtext)
      {
	int i;
	zeromem(s,64);
	i=t->xtext[0];
	if (i>63) i=63;
	strncpy(s,t->xtext+1,i);
	wsprintf(ws3,"%c%t",0xE012,s);
      }
      else
	wsprintf(ws3,percent_d,t->uin);
      if ((t->xstate<total_xstatuses)&&(t->xstate))
      {
	wsInsertChar(ws3,FIRST_UCS2_BITMAP+t->xstate,1);
      }
#endif
    }
    else
    {
      int onlinetotal[2];
      GetOnTotalContact(t->group,onlinetotal);
#ifdef USE_MLMENU
      wsprintf(ws1,percent_t,t->name);
      wsprintf(ws3,"(%d/%d)",onlinetotal[0],onlinetotal[1]);
#else
      wsprintf(ws1,"%t%c%c(%d/%d)",t->name,0xE01D,0xE012,onlinetotal[0],onlinetotal[1]);
#endif
      if (t->state) icon++; //Модификация иконки группы
    }
  }
  else
  {
    wsprintf(ws1, LG_CLERROR);
  }
  ws2=AllocMenuWS(data,ws1->wsbody[0]);
  wstrcpy(ws2,ws1);
  
  SetMenuItemIconArray(data, item, S_ICONS+icon);
#ifdef USE_MLMENU
  ws4=AllocMenuWS(data,ws3->wsbody[0]);
  wstrcpy(ws4,ws3);
  SetMLMenuItemText(data, item, ws2, ws4, curitem);
#else
  SetMenuItemText(data, item, ws2, curitem);
#endif
}


CLIST *AddContactOrGroup(CLIST *p)
{
  CLIST *t;
  CLIST *pr;
  t=(CLIST *)cltop;
  if (t)
  {
    //Не первый
    while(CompareContacts(t,p)<0)
    {
      if (!(pr=t->next))
      {
	//добавляем в конец
	t->next=p;
	p->prev=t;
	return(p);
      }
      t=pr;
    }
    if ((pr=t->prev))
      pr->next=p;
    else
      cltop=p;
    p->prev=pr;
    p->next=t;
    t->prev=p;
  }
  else
  {
    //Первый
    cltop=p;
  }
  return(p);
}


CLIST *AddContact(unsigned int uin, char *name)
{
  CLIST *p=malloc(sizeof(CLIST));
  zeromem(p,sizeof(CLIST));
  p->uin=uin;
  p->group=GROUP_CACHE;
  strncpy(p->name,name,sizeof(p->name)-1);
  p->state=0xFFFF;
  return AddContactOrGroup(p);
}

CLIST *AddGroup(unsigned int grp, char *name)
{
  CLIST *p=malloc(sizeof(CLIST));
  zeromem(p,sizeof(CLIST));
  //  p->uin=0;
  p->group=grp;
  p->isgroup=1;
  strncpy(p->name,name,sizeof(p->name)-1);
  if (!Is_Show_Groups) p->state=0;//Открыть группу
  else
    p->state=0xFFFF;
  return AddContactOrGroup(p);
}

//===============================================================================================
int DNR_ID=0;
int DNR_TRIES=3;

extern const char NATICQ_HOST[];
extern const unsigned int NATICQ_PORT;


void create_connect(void)
{
  int ***p_res=NULL;
  void do_reconnect(void);
  SOCK_ADDR sa;
  //Устанавливаем соединение
  connect_state = 0;
  int err;
  unsigned int ip;
  GBS_DelTimer(&reconnect_tmr);
  if (!IsGPRSEnabled())
  {
    is_gprs_online=0;
    strcpy(logmsg,LG_GRWAITFORGPRS);
    SMART_REDRAW();
    return;
  }
  DNR_ID=0;
  *socklasterr()=0;
  ip=str2ip(NATICQ_HOST);
  if (ip!=0xFFFFFFFF)  
  {
    sa.ip=ip;
    strcpy(logmsg,"Connect by IP!");
    SMART_REDRAW();
    goto L_CONNECT;
  }  
  strcpy(logmsg,LG_GRSENDDNR);
  SMART_REDRAW();
  err=async_gethostbyname(NATICQ_HOST,&p_res,&DNR_ID); //03461351 3<70<19<81
  if (err)
  {
    if ((err==0xC9)||(err==0xD6))
    {
      if (DNR_ID)
      {
	return; //Ждем готовности DNR
      }
    }
    else
    {
      snprintf(logmsg,255,LG_GRDNRERROR,err);
      SMART_REDRAW();
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
      return;
    }
  }
  if (p_res)
  {
    if (p_res[3])
    {
      strcpy(logmsg,LG_GRDNROK);
      SMART_REDRAW();
      DNR_TRIES=0;
      sa.ip=p_res[3][0][0];
    L_CONNECT:
      sock=socket(1,1,0);
      if (sock!=-1)
      {
	sa.family=1;
	sa.port=htons(NATICQ_PORT);
	//    sa.ip=htonl(IP_ADDR(82,207,89,182));
	if (connect(sock,&sa,sizeof(sa))!=-1)
	{
	  connect_state=1;
	  TOTALRECEIVED=0;
	  TOTALSENDED=0;
	  SMART_REDRAW();
	}
	else
	{
	  closesocket(sock);
	  sock=-1;
	  LockSched();
	  ShowMSG(1,(int)LG_MSGCANTCONN);
	  UnlockSched();
	  GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
	}
      }
      else
      {
	LockSched();
	ShowMSG(1,(int)LG_MSGCANTCRSC);
	UnlockSched();
	//Не осилили создания сокета, закрываем GPRS-сессию
	GPRS_OnOff(0,1);
      }
    } 
  }
  else
  {
    DNR_TRIES--;
    LockSched();
    ShowMSG(1,(int)LG_MSGHOSTNFND);
    UnlockSched();
  }
}

#ifdef SEND_TIMER
GBSTMR send_tmr;
#endif

void ClearSendQ(void)
{
  mfree((void *)sendq_p);
  sendq_p=NULL;
  sendq_l=NULL;
#ifdef SEND_TIMER
  GBS_DelTimer(&send_tmr);
#endif
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
#ifdef SEND_TIMER
  GBS_DelTimer(&send_tmr);
#endif
}

#ifdef SEND_TIMER
static void resend(void)
{
  void SendAnswer(int dummy, TPKT *p);
  SUBPROC((void*)SendAnswer,0,0);
}
#endif

void SendAnswer(int dummy, TPKT *p)
{
  int i;
  int j;
  if (connect_state<2)
  {
    mfree(p);
    return;
  }
  if (p)
  {
    j=sizeof(PKT)+p->pkt.data_len; //Размер пакета
    TOTALSENDED+=j;
    //Проверяем, не надо ли добавить в очередь
    if (sendq_p)
    {
      //Есть очередь, добавляем в нее
      sendq_p=realloc((void *)sendq_p,sendq_l+j);
      memcpy((char *)sendq_p+sendq_l,p,j);
      mfree(p);
      sendq_l+=j;
      return;
    }
    sendq_p=p;
    sendq_l=j;
  }
  //Отправляем уже существующее в очереди
  while((i=sendq_l)!=0)
  {
    if (i>0x400) i=0x400;
    j=send(sock,(void *)sendq_p,i,0);
    snprintf(logmsg,255,"send res %d",j);
    SMART_REDRAW();
    if (j<0)
    {
      j=*socklasterr();
      if ((j==0xC9)||(j==0xD6))
      {
	//Передали что хотели
	strcpy(logmsg,"Send delayed...");
	return; //Видимо, надо ждать сообщения ENIP_BUFFER_FREE
      }
      else
      {
	//Ошибка
	LockSched();
	ShowMSG(1,(int)"Send error!");
	UnlockSched();
	end_socket();
	return;
      }
    }
    memcpy((void *)sendq_p,(char *)sendq_p+j,sendq_l-=j); //Удалили переданное
    if (j<i)
    {
      //Передали меньше чем заказывали
#ifdef SEND_TIMER
      GBS_StartTimerProc(&send_tmr,216*5,resend);
#endif
      return; //Ждем сообщения ENIP_BUFFER_FREE1
    }
    tenseconds_to_ping=0; //Чего-то послали, можно начинать отсчет времени до пинга заново
  }
  mfree((void *)sendq_p);
  sendq_p=NULL;
}

void send_login(int dummy, TPKT *p)
{
  connect_state=2;
  SendAnswer(dummy,p);
  RXstate=-(int)sizeof(PKT);
}

void do_ping(void)
{
  TPKT *pingp=malloc(sizeof(PKT));
  pingp->pkt.uin=UIN;
  pingp->pkt.type=0;
  pingp->pkt.data_len=0;
  SendAnswer(0,pingp);
}

void SendMSGACK(int i)
{
  TPKT *ackp=malloc(sizeof(PKT));
  ackp->pkt.uin=i;
  ackp->pkt.type=T_MSGACK;
  ackp->pkt.data_len=0;
  SendAnswer(0,ackp);
}

void get_answer(void)
{
  void *p;
  int i=RXstate;
  int j;
  if (connect_state<2) return;
  if (i==EOP) return;
  if (i<0)
  {
    j=recv(sock,RXbuf.data+i,-i,0);
    if (j>0) i+=j;
    if (i==0)
    {
      if (RXbuf.pkt.data_len==0) goto LPKT;
    }
  }
  else
  {
    if (RXbuf.pkt.data_len>16383)
    {
      strcpy(logmsg,LG_GRBADPACKET);
      end_socket();
      RXstate=EOP;
      return;
    }
    j = recv(sock,RXbuf.data+i,RXbuf.pkt.data_len-i,0);
    if (j>0) i+=j;
  LPKT:
    if (i==RXbuf.pkt.data_len)
    {
      TOTALRECEIVED+=(i+8);
      //Пакет удачно принят, можно разбирать...
      RXbuf.data[RXbuf.pkt.data_len]=0; //Конец строки
      switch(RXbuf.pkt.type)
      {
      case T_LOGIN:
        //Удачно залогинились
	//Посылаем в MMI
        j=i+sizeof(PKT)+1;
        p=malloc(j);
        memcpy(p,&RXbuf,j);
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
	Play(sndStartup);
//        GBS_StartTimerProc(&tmr_ping,120*TMR_SECOND,call_ping);
        snprintf(logmsg,255,LG_GRLOGINMSG,RXbuf.data);
        connect_state=3;
        SMART_REDRAW();
        break;
      case T_XTEXT_ACK:
      case T_GROUPID:
      case T_GROUPFOLLOW:
      case T_CLENTRY:
	//Посылаем в MMI
        j=i+sizeof(PKT)+1;
        p=malloc(j);
        memcpy(p,&RXbuf,j);
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        //snprintf(logmsg,255,"CL: %s",RXbuf.data);
        break;
      case T_STATUSCHANGE:
        j=i+sizeof(PKT);
        p=malloc(j);
        memcpy(p,&RXbuf,j);
        snprintf(logmsg,255,LG_GRSTATUSCHNG,RXbuf.pkt.uin,*((unsigned short *)(RXbuf.data)));
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        break;
      case T_ERROR:
        snprintf(logmsg,255,LG_GRERROR,RXbuf.data);
        SMART_REDRAW();
        break;
      case T_RECVMSG:
        j=i+sizeof(PKT)+1;
        p=malloc(j);
        memcpy(p,&RXbuf,j);
	{
	  char *s=p;
	  s+=sizeof(PKT);
	  int c;
	  while((c=*s))
	  {
	    if (c<3) *s=' ';
	    s++;
	  }
	}
        snprintf(logmsg,255,LG_GRRECVMSG,RXbuf.pkt.uin,RXbuf.data);
	SendMSGACK(TOTALRECEIVED);
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        SMART_REDRAW();
	Play(sndMsg);
        break;
      case T_SSLRESP:
        LockSched();
        ShowMSG(1,(int)RXbuf.data);
        UnlockSched();
        break;
      case T_SRV_ACK:
	if (FindContactLOGQByAck(&RXbuf)) Play(sndMsgSent);
      case T_CLIENT_ACK:
	p=malloc(sizeof(PKT)+2);
	memcpy(p,&RXbuf,sizeof(PKT)+2);
	GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
	break;
      case T_ECHORET:
	{
	  TDate d;
	  TTime t;
	  TTime *pt=(TTime *)(RXbuf.data);
	  int s1;
	  int s2;
	  GetDateTime(&d,&t);
	  s1=t.hour*3600+t.min*60+t.sec;
	  s2=pt->hour*3600+pt->min*60+pt->sec;
	  s1-=s2;
	  if (s1<0) s1+=86400;
	  snprintf(logmsg,255,"Ping %d-%d seconds!",s1,s1+1);
	  LockSched();
	  ShowMSG(1,(int)logmsg);
	  UnlockSched();
	}
	break;
      }
      i=-(int)sizeof(PKT); //А может еще есть данные
    }
  }
  RXstate=i;
  //  GBS_StartTimerProc(&tmr_dorecv,3000,dorecv);
  //  SMART_REDRAW();
}

void AddStringToLog(CLIST *t, int code, char *s, const char *name, unsigned int IDforACK)
{
  char hs[128];
  TTime tt;
  TDate d;
  GetDateTime(&d,&tt);
  int i;
  
  snprintf(hs,127,"%02d:%02d %02d-%02d %s:\r\n",tt.hour,tt.min,d.day,d.month,name);
  Add2History(t, hs, s); // Запись хистори
  LOGQ *p=NewLOGQ(s);
  snprintf(p->hdr,79,"%02d:%02d %02d-%02d %s:",tt.hour,tt.min,d.day,d.month,name);
  p->type=code;
  p->ID=IDforACK; //0-32767
  i=AddLOGQ(&t->log,p);
  while(i>MAXLOGMSG)
  {
    if (t->log==t->last_log) t->last_log=t->last_log->next;
    RemoveLOGQ(&t->log,t->log);
    i--;
  }
  t->msg_count=i;
  if (!t->last_log) t->last_log=p;
  if (!t->isunread) total_unread++;
  t->isunread=1;
  ChangeContactPos(t);
}

void ParseAnswer(WSHDR *ws, const char *s);

int time_to_stop_t9;

//Добавление итемов в чат при получении нового сообщения
void AddMsgToChat(void *data)
{
  LOGQ *p;
  EDITCONTROL ec;
  EDITC_OPTIONS ec_options;
  EDCHAT_STRUCT *ed_struct;
  int j;
  int color;
  int zc;
  if (!data) return;
  ed_struct=EDIT_GetUserPointer(data);
  if (!ed_struct) return;
  if (!ed_struct->ed_contact->isunread) return;
  p=ed_struct->ed_contact->last_log;
  if (p)
  {
    while(p)
    {
      ascii2ws(ews,p->hdr);
      ConstructEditControl(&ec,1,ECF_APPEND_EOL,ews,ews->wsbody[0]);
      PrepareEditCOptions(&ec_options);
      if ((zc=p->acked&3))
      {
	if (zc==1)
	  color=4; //Зеленый
	else
	  color=I_COLOR;
      }
      else
      {
	if (p->ID==0xFFFFFFFF)
	  color=p->type==1?I_COLOR:TO_COLOR;
	else
	  color=20; //Серый
      }
      SetPenColorToEditCOptions(&ec_options,color/*p->type==1?I_COLOR:TO_COLOR*/);
      SetFontToEditCOptions(&ec_options,2);
      CopyOptionsToEditControl(&ec,&ec_options);
      //AddEditControlToEditQend(eq,&ec,ma);
      EDIT_InsertEditControl(data,ed_struct->ed_answer-1,&ec);
      ed_struct->ed_answer++;
      ParseAnswer(ews,p->text);
      ConstructEditControl(&ec,3,ECF_APPEND_EOL|ECF_DISABLE_T9,ews,ews->wsbody[0]);
      PrepareEditCOptions(&ec_options);
      SetFontToEditCOptions(&ec_options,ED_FONT_SIZE);
      CopyOptionsToEditControl(&ec,&ec_options);
      EDIT_InsertEditControl(data,ed_struct->ed_answer-1,&ec);
      ed_struct->ed_answer++;
      p=p->next;
    }
    j=((ed_struct->ed_contact->msg_count+1)*2); //Ожидаемый ed_answer
    while(j<ed_struct->ed_answer)
    {
      EDIT_RemoveEditControl(ed_struct->ed_chatgui,1);
      ed_struct->ed_answer--;
    }
  }
  ed_struct->ed_contact->last_log=NULL;
  if (IsGuiOnTop(edchat_id)) 
    total_unread--;
  else
    ed_struct->requested_decrement_total_unread++;
  ed_struct->ed_contact->isunread=0;
  ChangeContactPos(ed_struct->ed_contact);
  //  EDIT_SetFocus(data,ed_struct->ed_answer);
}

//Рисуем подтверждения непосредственно
void DrawAck(void *data)
{
  LOGQ *p;
  EDITCONTROL ec;
  EDITC_OPTIONS ec_options;
  EDCHAT_STRUCT *ed_struct;
  int j;
  int color;
  int dorefresh=0;
  if (!data) return;
  ed_struct=EDIT_GetUserPointer(data);
  if (!ed_struct) return;
  p=ed_struct->ed_contact->log;
  j=ed_struct->ed_answer;
  j-=(ed_struct->ed_contact->msg_count*2)+1; //Номер собственно хедера верхнего итема
  while(p)
  {
    if (j>=1) //Пока возможно лишнее
    {
      if (!(p->acked&4))
      {
	ExtractEditControl(data,j,&ec);
	PrepareEditCOptions(&ec_options);
	if (p->acked)
	{
	  if (p->acked==1)
	    color=4; //Зеленый
	  else
	    color=I_COLOR;
	}
	else
	{
	  if (p->ID==0xFFFFFFFF)
	    color=p->type==1?I_COLOR:TO_COLOR;
	  else
	    color=20; //Серый
	}
	SetPenColorToEditCOptions(&ec_options,color/*p->type==1?I_COLOR:TO_COLOR*/);
	SetFontToEditCOptions(&ec_options,2);
	CopyOptionsToEditControl(&ec,&ec_options);
	StoreEditControl(data,j,&ec);
	p->acked|=4; //Обработали
	dorefresh=1;
      }
    }
    j+=2;
    p=p->next;
  } 
  if (IsGuiOnTop(edchat_id)&&dorefresh) RefreshGUI();
}

void ask_my_info(void)
{
  /*  TPKT *p;
  CLIST *t;
  p=malloc(sizeof(PKT));
  p->pkt.uin=UIN;
  p->pkt.type=T_REQINFOSHORT;
  p->pkt.data_len=0;
  //  AddStringToLog(t,0x01,"Request info...",I_str);
  SUBPROC((void *)SendAnswer,0,p);*/
}

void set_my_status(void)
{
  TPKT *p;
  p=malloc(sizeof(PKT)+1);
  p->pkt.uin=0;               // Никому; поле нужно проигнорировать на сервере
  p->pkt.type=T_MY_STATUS_CH; // Тип пакета: изменение статуса
  p->pkt.data_len=1;          // Длина пакета: 1 байт
  p->data[0]=CurrentStatus;
  SUBPROC((void *)SendAnswer,0,p);
}

void set_my_xstatus(void)
{
  TPKT *p;
  p=malloc(sizeof(PKT)+1);
  p->pkt.uin=0;               // Никому; поле нужно проигнорировать на сервере
  p->pkt.type=T_MY_XSTATUS_CH; // Тип пакета: изменение статуса
  p->pkt.data_len=1;          // Длина пакета: 1 байт
  p->data[0]=CurrentXStatus;
  SUBPROC((void *)SendAnswer,0,p);
}

void to_develop(void)
{
  if (silenthide) return;
  gipc.name_to=ipc_xtask_name;
  gipc.name_from=ipc_my_name;
  gipc.data=(void *)maincsm_id;
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_XTASK_SHOW_CSM,&gipc);  
}

ProcessPacket(TPKT *p)
{
  CLIST *t;
  LOGQ *q;
  char s[256];
  switch(p->pkt.type)
  {
  case T_LOGIN:
    set_my_status();
    set_my_xstatus();
    break;
  case T_CLENTRY:
    if (p->pkt.uin)
    {
      if ((t=FindContactByUin(p->pkt.uin)))
      {
	//        t->state=0xFFFF;
        strncpy(t->name,p->data,63);
	t->group=GROUP_CACHE;
	ChangeContactPos(t);
	RecountMenu(t);
      }
      else
      {
        RecountMenu(AddContact(p->pkt.uin,p->data));
      }
    }
    else
    {
      vibra_count=1;
      GROUP_CACHE=0;
      start_vibra();
      ask_my_info();
      if (contactlist_menu_id)
      {
	RecountMenu(NULL);
      }
      else
        create_contactlist_menu();
    }
    break;
  case T_GROUPID:
    if (t=FindGroupByID(GROUP_CACHE=p->pkt.uin))
    {
      strncpy(t->name,p->data,63);
      ChangeContactPos(t);
      RecountMenu(t);
    }
    else
    {
      RecountMenu(AddGroup(p->pkt.uin,p->data));
    }
    break;
  case T_GROUPFOLLOW:
    GROUP_CACHE=p->pkt.uin;
    break;
  case T_STATUSCHANGE:
    t=FindContactByUin(p->pkt.uin);
    if (t)
    {
      int i=t->state;
      CLIST *oldt=NULL;
      t->xstate=p->data[2];
      if (contactlist_menu_id)
      {
	oldt=FindContactByN(GetCurMenuItem(FindGUIbyId(contactlist_menu_id,NULL)));
      }
      t->state=*((unsigned short *)(p->data));
      LogStatusChange(t);
      ChangeContactPos(t);
      RecountMenu(oldt);
      if ((t->state!=0xFFFF)&&(i==0xFFFF))//Звук
      {
	Play(sndGlobal);
      }
      if ((i!=0xFFFF)&&(t->state==0xFFFF))//Звук
      {
	Play(sndSrvMsg);
      }
    }
    if (edchat_id)
    {
      void *data=FindGUIbyId(edchat_id,NULL);
      {
	EDCHAT_STRUCT *ed_struct;
	ed_struct=EDIT_GetUserPointer(data);
	if (ed_struct) 
	{
	  if (ed_struct->ed_contact==t)
	  {
	    DirectRedrawGUI_ID(edchat_id);
	  }
	}
      }
    }
    break;
  case T_RECVMSG:
    t=FindContactByUin(p->pkt.uin);
    if (!t)
    {
      sprintf(s,percent_d,p->pkt.uin);
      t=AddContact(p->pkt.uin,s);
    }
    t->isactive=ACTIVE_TIME;
    //    ChangeContactPos(t);
    vibra_count=1;
    start_vibra();
    IlluminationOn(ILL_DISP_RECV,ILL_KEYS_RECV,ILL_RECV_TMR,ILL_RECV_FADE); //Illumination by BoBa 19.04.2007
    if (t->name[0]=='#')
    {
      //Если это конференция, патчим имя
      char *s=strchr(p->data,'>');
      //Если нашли символ > и после него пробел и ник короче 16 символов
      if (s)
      {
	if ((s[1]==' ')&&((s-p->data)<16))
	{
	  *s=0; //Режем строку
	  AddStringToLog(t,0x02,s+2,p->data,0xFFFFFFFF); //Добавляем имя из текста сообщения
	  goto L1;
	}
      }
    }
    AddStringToLog(t,0x02,p->data,t->name,0xFFFFFFFF);
    L1:
    //Разворачиваем группу, в которой пришло сообщение
    {
      CLIST *g=FindGroupByID(t->group);
      if (g)
      {
	if (g->state)
	{
	  g->state=0;
	}
      }
    }
    if (edchat_id)
    {
      void *data=FindGUIbyId(edchat_id,NULL);
      {
	EDCHAT_STRUCT *ed_struct;
	ed_struct=EDIT_GetUserPointer(data);
	if (ed_struct) 
	{
	  if (ed_struct->ed_contact==t)
	  {
	    if (EDIT_IsBusy(data))
	    {
	      t->req_add=1;
	      time_to_stop_t9=3;
	    }
	    else
	    {
	      AddMsgToChat(data);
	      DirectRedrawGUI_ID(edchat_id);
	    }
	  }
	}
      }
    }
    RecountMenu(t);
    extern const int DEVELOP_IF;
    switch (DEVELOP_IF)
    {
    case 0:
      if ((((CSM_RAM *)(CSM_root()->csm_q->csm.last))->id!=maincsm_id)) to_develop();
      break;
    case 1:
      if ((((CSM_RAM *)(CSM_root()->csm_q->csm.last))->id!=maincsm_id)&&(IsUnlocked())) to_develop();
      break;
    case 2:
      break;
    }
    break;
  case T_SRV_ACK:
  case T_CLIENT_ACK:
    q=FindContactLOGQByAck(p);
    if (q&&(p->pkt.type==T_SRV_ACK)) IlluminationOn(ILL_DISP_SEND,ILL_KEYS_SEND,ILL_SEND_TMR,ILL_RECV_FADE); //Illumination by BoBa 19.04.2007
/*    if ((
	IsGuiOnTop(contactlist_menu_id)||
	  IsGuiOnTop(edchat_id)
	    )&&(q))
    {
      DrawRoundedFrame(ScreenW()-8,YDISP,ScreenW()-1,YDISP+7,0,0,0,
		       GetPaletteAdrByColorIndex(0),
		       GetPaletteAdrByColorIndex(p->pkt.type==T_SRV_ACK?3:4));
    }*/
    if (q)
    {
      q->acked=p->pkt.type==T_SRV_ACK?1:2;
      t=FindContactByUin(p->pkt.uin);
      if (edchat_id)
      {
	void *data=FindGUIbyId(edchat_id,NULL);
	if (data)
	{
	  EDCHAT_STRUCT *ed_struct;
	  ed_struct=EDIT_GetUserPointer(data);
	  if (ed_struct) 
	  {
	    if (ed_struct->ed_contact==t)
	    {
	      if (EDIT_IsBusy(data))
	      {
		t->req_drawack=1;
		time_to_stop_t9=3;
	      }
	      else
		DrawAck(data);
	    }
	  }
	}
      }
    }
    break;
  case T_XTEXT_ACK:
    t=FindContactByUin(p->pkt.uin);
    if (t)
    {
      int i;
      int j;
      FreeXText(t);
      i=p->pkt.data_len;
      memcpy(t->xtext=malloc(i),p->data,i);
      zeromem(s,256);
      strcpy(s,t->name);
      strcat(s,":\n");
      i=strlen(s);
      j=p->data[0];
      if (j>(255-i)) j=255-i;
      strncpy(s+i,p->data+1,j);
      i+=j;
      if (i<255)
      {
	s[i]='\n';
	i++;
	j=p->pkt.data_len-p->data[0]-1;
	if (j>(255-i)) j=255-i;
	strncpy(s+i,p->data+p->data[0]+1,j);
      }
//      if (IsGuiOnTop(contactlist_menu_id)) RefreshGUI();
      ShowMSG(0,(int)s);
    }
    break;
  }
  mfree(p);
}


IPC_REQ tmr_gipc;
void process_active_timer(void)
{
  if (connect_state>2)
  {
    if (++tenseconds_to_ping>12)
    {
      tenseconds_to_ping=0;
      SUBPROC((void *)do_ping);
    }
  }
  tmr_gipc.name_to=ipc_my_name;
  tmr_gipc.name_from=ipc_my_name;
  tmr_gipc.data=NULL;
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_TENSECONDS,&tmr_gipc);  
  GBS_StartTimerProc(&tmr_active,TMR_SECOND*10,process_active_timer);
}

//===============================================================================================
void method0(MAIN_GUI *data)
{
  int scr_w=ScreenW();
  int scr_h=ScreenH();
/*  DrawRoundedFrame(0,YDISP,scr_w-1,scr_h-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(20));*/

  DrawRectangle(0,YDISP,scr_w-1,scr_h-1,0,
		   GetPaletteAdrByColorIndex(1),
		   GetPaletteAdrByColorIndex(1));
  DrawImg(0,0,S_ICONS[ICON_LOGO]);
  wsprintf(data->ws1,LG_GRSTATESTRING,connect_state,RXstate,logmsg);
  if (total_smiles)
  {
    wstrcatprintf(data->ws1,"\nLoaded %d smiles",total_smiles);
  }
  if (xstatuses_load)
  {
    wstrcatprintf(data->ws1,"\nLoaded %d xstatus",total_xstatuses);
  }
  DrawString(data->ws1,3,3+YDISP,scr_w-4,scr_h-4-GetFontYSIZE(FONT_MEDIUM_BOLD),
	     FONT_SMALL,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  wsprintf(data->ws2,percent_t,LG_GRSKEYEXIT);
  DrawString(data->ws2,(scr_w >> 1),scr_h-4-GetFontYSIZE(FONT_MEDIUM_BOLD),
	     scr_w-4,scr_h-4,FONT_MEDIUM_BOLD,TEXT_ALIGNRIGHT,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  wsprintf(data->ws2,percent_t,cltop?LG_GRSKEYCLIST:empty_str);
  DrawString(data->ws2,3,scr_h-4-GetFontYSIZE(FONT_MEDIUM_BOLD),
	     scr_w>>1,scr_h-4,FONT_MEDIUM_BOLD,TEXT_ALIGNLEFT,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
}

void method1(MAIN_GUI *data,void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  data->gui.state=1;
}

void method2(MAIN_GUI *data,void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void method3(MAIN_GUI *data,void *(*malloc_adr)(int),void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void method4(MAIN_GUI *data,void (*mfree_adr)(void *))
{
  if (data->gui.state!=2)
    return;
  data->gui.state=1;
}

int method5(MAIN_GUI *data,GUI_MSG *msg)
{
  DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case LEFT_SOFT:
      if (cltop) create_contactlist_menu();
      //      if (cltop) remake_clmenu();
      break;
    case RIGHT_SOFT:
      return(1); //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI
    case GREEN_BUTTON:
      if ((connect_state==0)&&(sock==-1))
      {
        GBS_DelTimer(&reconnect_tmr);
	DNR_TRIES=3;
        SUBPROC((void *)create_connect);
      }
      break;
    }
  }
  return(0);
}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)method0,  //Redraw
  (void *)method1,  //Create
  (void *)method2,  //Close
  (void *)method3,  //Focus
  (void *)method4,  //Unfocus
  (void *)method5,  //OnKey
  0,
  (void *)kill_data, //method7, //Destroy
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,0,0};

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  patch_rect((RECT*)&Canvas,0,0,ScreenW()-1,ScreenH()-1);
  main_gui->gui.canvas=(void *)(&Canvas);
//  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  maingui_id=csm->gui_id=CreateGUI(main_gui);
  ews=AllocWS(16384);
  msg_buf=malloc(16384);
  //  MutexCreate(&contactlist_mtx);
  DNR_TRIES=3;
  //  SUBPROC((void *)InitSmiles);
  //  SUBPROC((void *)create_connect);
  GBS_StartTimerProc(&tmr_active,TMR_SECOND*10,process_active_timer);
  sprintf((char *)ipc_my_name+6,percent_d,UIN);
  gipc.name_to=ipc_my_name;
  gipc.name_from=ipc_my_name;
  gipc.data=(void *)-1;
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_CHECK_DOUBLERUN,&gipc);
}

extern const int MY_DEF_STATUS;
extern const unsigned int MY_DEF_XSTATUS;
extern const int DEF_VIBRA_STATUS;
extern const int DEF_SOUNDS_STATUS;
extern const int DEF_SHOWOFF_STATUS;
extern const int DEF_SHOW_GROUPS;

void maincsm_onclose(CSM_RAM *csm)
{
  *((int *)&DEF_VIBRA_STATUS)=Is_Vibra_Enabled;
  *((int *)&DEF_SOUNDS_STATUS)=Is_Sounds_Enabled;
  *((int *)&DEF_SHOWOFF_STATUS)=Is_Show_Offline;
  *((int *)&DEF_SHOW_GROUPS)=Is_Show_Groups;
  *((int *)&MY_DEF_STATUS)=CurrentStatus-1;
  *((int *)&MY_DEF_XSTATUS)=CurrentXStatus;
  
  SaveConfigData(successed_config_filename);
  
/*
  #pragma segment="CONFIG_C"
  unsigned int ul;
  int f;
  extern const CFG_HDR cfghdr0; //first var in CONFIG
  void *cfg=(void*)&cfghdr0;
  unsigned int len=(int)__segment_end("CONFIG_C")-(int)__segment_begin("CONFIG_C");

  if ((f=fopen("4:\\ZBin\\etc\\NATICQ.bcfg",A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))==-1){
   f=fopen("0:\\ZBin\\etc\\NATICQ.bcfg",A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul);
  }
  fwrite(f,cfg,len,&ul);
  fclose(f,&ul);
*/

  //  GBS_DelTimer(&tmr_dorecv);
  GBS_DelTimer(&tmr_active);
//  GBS_DelTimer(&tmr_ping);
  GBS_DelTimer(&tmr_vibra);
  GBS_DelTimer(&reconnect_tmr);
  GBS_DelTimer(&tmr_illumination);
  SetVibration(0);
  FreeTemplates();
  FreeCLIST();
  free_ICONS();
  //  FreeSmiles();
  mfree(msg_buf);
  FreeWS(ews);
  //  MutexDestroy(&contactlist_mtx);
  SUBPROC((void *)FreeSmiles);
  SUBPROC((void *)FreeXStatusesImg);
  SUBPROC((void *)end_socket);
  SUBPROC((void *)ClearSendQ);
  SUBPROC((void *)ElfKiller);
}

void do_reconnect(void)
{
  if (is_gprs_online)
  {
    DNR_TRIES=3;
    SUBPROC((void*)create_connect);
  }
}

void CheckDoubleRun(void)
{
  int csm_id;
  if ((csm_id=(int)(gipc.data))!=-1)
  {
    gipc.name_to=ipc_xtask_name;
    gipc.name_from=ipc_my_name;
    gipc.data=(void *)csm_id;
    GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_XTASK_SHOW_CSM,&gipc);  
    LockSched();
    CloseCSM(maincsm_id);
    //ShowMSG(1,(int)LG_ALREADY_STARTED);
    UnlockSched();
  }
  else
  {
    InitXStatusesImg();
    //InitSmiles(); Это вызовется из InitXStatusesImg
    create_connect();
  }
}

int maincsm_onmessage(CSM_RAM *data,GBS_MSG *msg)
{
  //  char ss[100];
  MAIN_CSM *csm=(MAIN_CSM*)data;
  {
    //IPC
    if (msg->msg==MSG_IPC)
    {
      IPC_REQ *ipc;
      if ((ipc=(IPC_REQ*)msg->data0))
      {
	if (strcmp_nocase(ipc->name_to,ipc_my_name)==0)
	{
	  switch (msg->submess)
	  {
	  case IPC_CHECK_DOUBLERUN:
	    //Если приняли свое собственное сообщение, значит запускаем чекер
	    if (ipc->name_from==ipc_my_name) SUBPROC((void *)CheckDoubleRun);
            else ipc->data=(void *)maincsm_id;
	    break;
	  case IPC_SMILE_PROCESSED:
	    //Только собственные смайлы ;)
	    if (ipc->name_from==ipc_my_name) SUBPROC((void *)ProcessNextSmile);
	    SMART_REDRAW();
	    break;
	  case IPC_XSTATUSIMG_PROCESSED:
	    //Только собственные иксстатусы ;)
	    if (ipc->name_from==ipc_my_name) SUBPROC((void *)ProcessNextXStatImg);
	    SMART_REDRAW();
	    break;
	  case IPC_TENSECONDS:
	    //Только свое сообщение
	    if (ipc->name_from==ipc_my_name)
	    {
	      CLIST *t=(CLIST *)cltop;
	      int f=0;
	      while(t)
	      {
		if (t->isactive)
		{
		  if (!(--(t->isactive))) f=1; //Если дошли до 0 хотя бы один раз - надо перерисовать меню
		}
		t=(CLIST *)(t->next);
	      }
	      if (f) 
	      {
		CLIST *oldt=NULL;
		if (contactlist_menu_id)
		{
		  oldt=FindContactByN(GetCurMenuItem(FindGUIbyId(contactlist_menu_id,NULL)));
		}
		RecountMenu(oldt);
	      }
	      if (time_to_stop_t9)
	      {
		if (!(--time_to_stop_t9))
		{
		  if (IsGuiOnTop(edchat_id)) RefreshGUI();
		}
	      }
	    }
	    break;
     	  case IPC_SENDMSG: ;                                   //IPC_SENDMSG by BoBa 26.06.07
            int l=strlen(((IPCMsg *)(ipc->data))->msg);
            TPKT *msg=malloc(sizeof(PKT)+l);
            msg->pkt.uin=((IPCMsg *)(ipc->data))->uin;
            msg->pkt.type=T_SENDMSG;
            msg->pkt.data_len=l;
            memcpy(msg->data,((IPCMsg *)(ipc->data))->msg,l);
            //slientsend=1;
	    SENDMSGCOUNT++; //Номер сообщения
            SUBPROC((void *)SendAnswer,0,msg);
            break;
	  }
	}
      }
    }
    //Нарисуем иконочку моего статуса
#define idlegui_id (((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4])
    CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
    if (IsGuiOnTop(idlegui_id)/*&&IsUnlocked()*/) //Если IdleGui на самом верху
    {
      GUI *igui=GetTopGUI();
      if (igui) //И он существует
      {
	void *canvasdata=BuildCanvas();
	int icn;
	if (total_unread)
	  icn=IS_MSG;
	else
	{
	  switch(connect_state)
	  {
	  case 0:
	    icn=IS_OFFLINE; break;
	  case 3:
	    icn=CurrentStatus; //IS_ONLINE;
	    break;
	  default:
	    icn=IS_UNKNOWN; break;
	  }
	}
	//Тут трохи поменял
	// by Rainmaker: Рисуем канву только для иконки и выводим в своих координатах
	DrawCanvas(canvasdata,IDLEICON_X,IDLEICON_Y,IDLEICON_X+GetImgWidth((int)S_ICONS[icn])-1,
		   IDLEICON_Y+GetImgHeight((int)S_ICONS[icn])-1,1);
	DrawImg(IDLEICON_X,IDLEICON_Y,S_ICONS[icn]);
      }
    }
  }
  if (msg->msg==MSG_RECONFIGURE_REQ)
  {
    extern const char *successed_config_filename;
    if (strcmp(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"NatICQ config updated!");
      InitConfig();
      free_ICONS();
      setup_ICONS();
      //      InitSmiles();
    }
  }
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
    if ((int)msg->data0==contactlist_menu_id)
    {
      contactlist_menu_id=0;
      prev_clmenu_itemcount=0;
    }
    if ((int)msg->data0==edchat_id)
    {
      edchat_id=0;
    }
  }
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    switch((int)msg->data0)
    {
    case LMAN_DISCONNECT_IND:
      is_gprs_online=0;
      return(1);
    case LMAN_CONNECT_CNF:
      vibra_count=3;
      start_vibra();
      is_gprs_online=1;
      strcpy(logmsg,LG_GRGPRSUP);
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*10,do_reconnect);
      return(1);
    case ENIP_DNR_HOST_BY_NAME:
      if ((int)msg->data1==DNR_ID)
      {
	if (DNR_TRIES) SUBPROC((void *)create_connect);
      }
      return(1);
    }
    if ((int)msg->data1==sock)
    {
      //Если наш сокет
      if ((((unsigned int)msg->data0)>>28)==0xA)
      {
	//Принят пакет
	ProcessPacket((TPKT *)msg->data0);
	return(0);
      }
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
	if (connect_state==1)
	{
	  vibra_count=2;
	  start_vibra();
	  //Соединение установленно, посылаем пакет login
	  strcpy(logmsg, LG_GRTRYLOGIN);
	  {
	    int i=strlen(PASS);
	    TPKT *p=malloc(sizeof(PKT)+i);
	    p->pkt.uin=UIN;
	    p->pkt.type=T_REQLOGIN;
	    p->pkt.data_len=i;
	    memcpy(p->data,PASS,i);
	    SUBPROC((void *)send_login,0,p);
	  }
	  GROUP_CACHE=0;
	  SENDMSGCOUNT=0; //Начинаем отсчет
	  if (!FindGroupByID(0)) AddGroup(0,LG_GROUPNOTINLIST);
	  if (!FindContactByUin(UIN)) AddContact(UIN, LG_CLLOOPBACK);
	  SMART_REDRAW();
	}
	else
	{
	  ShowMSG(1,(int)LG_MSGILLEGMSGCON);
	}
	break;
      case ENIP_SOCK_DATA_READ:
	if (connect_state>=2)
	{
	  //Если посылали send
	  SUBPROC((void *)get_answer);
	  //SMART_REDRAW();
	}
	else
	{
	  ShowMSG(1,(int)LG_MSGILLEGMSGREA);
	}
	break;
      case ENIP_BUFFER_FREE:
	if (!sendq_p)
	{
	  ShowMSG(1,(int)"Illegal ENIP_BUFFER_FREE!");
	  SUBPROC((void *)end_socket);
	}
	else
	{
	  //Досылаем очередь
	  snprintf(logmsg,255,"ENIP_BUFFER_FREE");
	  SMART_REDRAW();
	  SUBPROC((void *)SendAnswer,0,0);
	}
	break;
      case ENIP_BUFFER_FREE1:
	if (!sendq_p)
	{
	  ShowMSG(1,(int)"Illegal ENIP_BUFFER_FREE1!");
	  SUBPROC((void *)end_socket);
	}
	else
	{
	  //Досылаем очередь
	  SUBPROC((void *)SendAnswer,0,0);
	}
	break;
      case ENIP_SOCK_REMOTE_CLOSED:
	//Закрыт со стороны сервера
	if (connect_state)
	  SUBPROC((void *)end_socket);
	break;
      case ENIP_SOCK_CLOSED:
	//strcpy(logmsg, "No connection");
	FillAllOffline();
	RecountMenu(NULL);
	connect_state=0;
	sock=-1;
	vibra_count=4;
	start_vibra();
	if (sendq_p)
	{
	  snprintf(logmsg,255,"Disconnected, %d bytes not sended!",sendq_l);
	}
	SMART_REDRAW();
	SUBPROC((void *)ClearSendQ);
	GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
	break;
      }
    }
  }
  return(1);
}


const int minus11=-11;

unsigned short maincsm_name_body[140];

const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM =
{
  {
    maincsm_onmessage,
    maincsm_oncreate,
#ifdef NEWSGOLD
0,
0,
0,
0,
#endif
maincsm_onclose,
sizeof(MAIN_CSM),
1,
&minus11
  },
  {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};

void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name), "NATICQ: %d",UIN);
}


int main()
{
  char dummy[sizeof(MAIN_CSM)];
  
  InitConfig();
  
  Is_Vibra_Enabled=DEF_VIBRA_STATUS;
  Is_Sounds_Enabled=DEF_SOUNDS_STATUS;
  Is_Show_Offline=DEF_SHOWOFF_STATUS;
  Is_Show_Groups=DEF_SHOW_GROUPS;
  CurrentStatus=MY_DEF_STATUS+1;
  CurrentXStatus=MY_DEF_XSTATUS;
  
  setup_ICONS();
  
  //  InitSmiles();
  
  if (!UIN)
  {
    LockSched();
    ShowMSG(1,(int)LG_MSGNOUINPASS);
    UnlockSched();
    return 0;
  }
  UpdateCSMname();
  LockSched();
  maincsm_id=CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;
}

//===========================================================
// Edit chat
//===========================================================
typedef struct
{
  unsigned short u;
  char dos;
  char win;
  char koi8;
} TUNICODE2CHAR;

const TUNICODE2CHAR unicode2char[]=
{
  // CAPITAL Cyrillic letters (base)
  0x410,0x80,0xC0,0xE1, // А
  0x411,0x81,0xC1,0xE2, // Б
  0x412,0x82,0xC2,0xF7, // В
  0x413,0x83,0xC3,0xE7, // Г
  0x414,0x84,0xC4,0xE4, // Д
  0x415,0x85,0xC5,0xE5, // Е
  0x416,0x86,0xC6,0xF6, // Ж
  0x417,0x87,0xC7,0xFA, // З
  0x418,0x88,0xC8,0xE9, // И
  0x419,0x89,0xC9,0xEA, // Й
  0x41A,0x8A,0xCA,0xEB, // К
  0x41B,0x8B,0xCB,0xEC, // Л
  0x41C,0x8C,0xCC,0xED, // М
  0x41D,0x8D,0xCD,0xEE, // Н
  0x41E,0x8E,0xCE,0xEF, // О
  0x41F,0x8F,0xCF,0xF0, // П
  0x420,0x90,0xD0,0xF2, // Р
  0x421,0x91,0xD1,0xF3, // С
  0x422,0x92,0xD2,0xF4, // Т
  0x423,0x93,0xD3,0xF5, // У
  0x424,0x94,0xD4,0xE6, // Ф
  0x425,0x95,0xD5,0xE8, // Х
  0x426,0x96,0xD6,0xE3, // Ц
  0x427,0x97,0xD7,0xFE, // Ч
  0x428,0x98,0xD8,0xFB, // Ш
  0x429,0x99,0xD9,0xFD, // Щ
  0x42A,0x9A,0xDA,0xFF, // Ъ
  0x42B,0x9B,0xDB,0xF9, // Ы
  0x42C,0x9C,0xDC,0xF8, // Ь
  0x42D,0x9D,0xDD,0xFC, // Э
  0x42E,0x9E,0xDE,0xE0, // Ю
  0x42F,0x9F,0xDF,0xF1, // Я
  // CAPITAL Cyrillic letters (additional)
  0x402,'_',0x80,'_', // _ .*.*
  0x403,'_',0x81,'_', // _ .*.*
  0x409,'_',0x8A,'_', // _ .*.*
  0x40A,'_',0x8C,'_', // _ .*.*
  0x40C,'_',0x8D,'_', // _ .*.*
  0x40B,'_',0x8E,'_', // _ .*.*
  0x40F,'_',0x8F,'_', // _ .*.*
  0x40E,0xF6,0xA1,'_', // Ў ...*
  0x408,0x4A,0xA3,0x4A, // _ .*.*
  0x409,0x83,0xA5,0xBD, // _ .*..
  0x401,0xF0,0xA8,0xB3, // Ё
  0x404,0xF2,0xAA,0xB4, // Є
  0x407,0xF4,0xAF,0xB7, // Ї
  0x406,0x49,0xB2,0xB6, // _ .*..
  0x405,0x53,0xBD,0x53, // _ .*.*
  // SMALL Cyrillic letters (base)
  0x430,0xA0,0xE0,0xC1, // а
  0x431,0xA1,0xE1,0xC2, // б
  0x432,0xA2,0xE2,0xD7, // в
  0x433,0xA3,0xE3,0xC7, // г
  0x434,0xA4,0xE4,0xC4, // д
  0x435,0xA5,0xE5,0xC5, // е
  0x436,0xA6,0xE6,0xD6, // ж
  0x437,0xA7,0xE7,0xDA, // з
  0x438,0xA8,0xE8,0xC9, // и
  0x439,0xA9,0xE9,0xCA, // й
  0x43A,0xAA,0xEA,0xCB, // к
  0x43B,0xAB,0xEB,0xCC, // л
  0x43C,0xAC,0xEC,0xCD, // м
  0x43D,0xAD,0xED,0xCE, // н
  0x43E,0xAE,0xEE,0xCF, // о
  0x43F,0xAF,0xEF,0xD0, // п
  0x440,0xE0,0xF0,0xD2, // р
  0x441,0xE1,0xF1,0xD3, // с
  0x442,0xE2,0xF2,0xD4, // т
  0x443,0xE3,0xF3,0xD5, // у
  0x444,0xE4,0xF4,0xC6, // ф
  0x445,0xE5,0xF5,0xC8, // х
  0x446,0xE6,0xF6,0xC3, // ц
  0x447,0xE7,0xF7,0xDE, // ч
  0x448,0xE8,0xF8,0xDB, // ш
  0x449,0xE9,0xF9,0xDD, // щ
  0x44A,0xEA,0xFA,0xDF, // ъ
  0x44B,0xEB,0xFB,0xD9, // ы
  0x44C,0xEC,0xFC,0xD8, // ь
  0x44D,0xED,0xFD,0xDC, // э
  0x44E,0xEE,0xFE,0xC0, // ю
  0x44F,0xEF,0xFF,0xD1, // я
  // SMALL Cyrillic letters (additional)
  0x452,'_',0x90,'_', // _ .*.*
  0x453,'_',0x83,'_', // _ .*.*
  0x459,'_',0x9A,'_', // _ .*.*
  0x45A,'_',0x9C,'_', // _ .*.*
  0x45C,'_',0x9D,'_', // _ .*.*
  0x45B,'_',0x9E,'_', // _ .*.*
  0x45F,'_',0x9F,'_', // _ .*.*
  0x45E,0xF7,0xA2,'_', // ў ...*
  0x458,0x6A,0xBC,0x6A, // _ .*.*
  0x491,0xA3,0xB4,0xAD, // _ .*..
  0x451,0xF1,0xB8,0xA3, // ё
  0x454,0xF3,0xBA,0xA4, // є
  0x457,0xF5,0xBF,0xA7, // ї
  0x456,0x69,0xB3,0xA6, // _ .*..
  0x455,0x73,0xBE,0x73, // _ .*.*
  0x0A0,'_',0xA0,0x20, // space .*..
  0x0A4,'_',0xA4,0xFD, // ¤   .*..
  0x0A6,'_',0xA6,'_', // ¦   .*.*
  0x0B0,0xF8,0xB0,0x9C, // °
  0x0B7,0xFA,0xB7,0x9E, // ·
  // 0x2022,,0x95,0x95, //    .*..
  // 0x2116,0xFC,0xB9,0x23, // №   ...*
  // 0x2219,,0xF9,0x9E, //    .*..
  // 0x221A,0xFB,,0x96, // v   ..*.
  // 0x25A0,0xFE,,0x94, // ¦
  0x0000,0,0,0
};

const char wintranslation[128]=
{
  0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
  0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
  0xFF,0xF6,0xF7,0x5F,0xFD,0x83,0xB3,0x15,0xF0,0x63,0xF2,0x3C,0xBF,0x2D,0x52,0xF4,
  0xF8,0x2B,'I' ,'i' ,0xA3,0xE7,0x14,0xFA,0xF1,0xFC,0xF3,0x3E,0x5F,0x5F,0x5F,0xF5,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
};
const unsigned short dos2unicode[128]=
{
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
  0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
  0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
  0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
  0x002D,0x002D,0x002D,0x00A6,0x002B,0x00A6,0x00A6,0x00AC,
  0x00AC,0x00A6,0x00A6,0x00AC,0x002D,0x002D,0x002D,0x00AC,
  0x004C,0x002B,0x0054,0x002B,0x002D,0x002B,0x00A6,0x00A6,
  0x004C,0x0433,0x00A6,0x0054,0x00A6,0x003D,0x002B,0x00A6,
  0x00A6,0x0054,0x0054,0x004C,0x004C,0x002D,0x0433,0x002B,
  0x002B,0x002D,0x002D,0x002D,0x002D,0x00A6,0x00A6,0x002D,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
  0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
  0x0401,0x0451,0x0404,0x0454,0x0407,0x0457,0x040E,0x045E,
  0x00B0,0x2022,0x00B7,0x0076,0x2116,0x00A4,0x00A6,0x00A0
};

#pragma inline
unsigned int char8to16(int c)
{
  if (c>=128)
  {
    //Win->Dos
    c=wintranslation[c-128];
    if (c<128) return(c);
    return(dos2unicode[c-128]);
  }
  return(c);
}

unsigned int char16to8(unsigned int c)
{
  const TUNICODE2CHAR *p=unicode2char;
  unsigned int i;
  if (c<128) return(c);
  while((i=p->u))
  {
    if (c==i)
    {
      return(p->win);
    }
    p++;
  }
  c&=0xFF;
  if (c<32) return(' ');
  return(c);
}

void ascii2ws(WSHDR *ws, const char *s)
{
  char c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c));
  }
}

void edchat_locret(void){}

void ExtractAnswer(WSHDR *ws)
{
  S_SMILES *t;
  int c;
  int scur=0;
  int wcur=0;
  unsigned short *wsbody=ws->wsbody;
  int wslen=wsbody[0];
  do
  {
    if (wcur>=wslen) break;
    c=wsbody[wcur+1];
    if (c==10) c=13;
    if (c>=0xE100)
    {
      t=FindSmileByUni(c);
      if (t)
      {
        int w;
        char *s;
	if (t->lines)
	{
	  s=t->lines->text;
	  while ((w=*s++) && scur<16383)
	  {
	    msg_buf[scur]=w;
	    scur++;
	  }
	}
      }
      else
      {
        msg_buf[scur]=char16to8(c);
        scur++;
      }
    }
    else
    {
      msg_buf[scur]=char16to8(c);
      scur++;
    }
    wcur++;
  }
  while(scur<16383);
  msg_buf[scur]=0;
  return;
}

CLIST *FindNextActiveContact(CLIST *t)
{
  while(t=(CLIST *)(t->next))
  {
    if (t->isactive) return t;
  }
  t=(CLIST *)(&cltop);
  while(t=(CLIST *)(t->next))
  {
    if (t->isactive) return t;
  }
  return NULL;
}

CLIST *FindPrevActiveContact(CLIST *t)
{
  CLIST *cl;
  CLIST *cl_active=NULL;
  cl=(CLIST *)(&cltop);
  
  
  while(cl=cl->next)
  {
    if (cl==t)
    {
      if (cl_active==NULL) break;
      else return (cl_active);
    }
    else
    {
      if (cl->isactive) cl_active=cl;
    }
  }
  while(t)
  {
    if (t->isactive) cl_active=t;
    t=t->next;
  }
  return cl_active;
}


void ed_options_handler(USR_MENU_ITEM *item)
{
  EDCHAT_STRUCT *ed_struct=item->user_pointer;
  CLIST *t;
  int i=item->cur_item;
  if (item->type==0)
  {
    switch(i)
    {
    case 0:
      ascii2ws(item->ws,LG_MNUEDNEXTACT);
      break;
    case 1:
      ascii2ws(item->ws,LG_MNUEDPREVACT);
      break;
    default:
      i-=2;
      if (i<ed_struct->loaded_templates) ascii2ws(item->ws,templates_lines[i]);
      break;
    }
  }
  if (item->type==1)
  {
    switch(i)
    {
    case 0:
      t=FindNextActiveContact(ed_struct->ed_contact);
      if (t && t!=ed_struct->ed_contact)
      {
        GeneralFunc_flag1(edchat_id,1);
        CreateEditChat(t);
      }
      break;
    case 1:
      t=FindPrevActiveContact(ed_struct->ed_contact);
      if (t && t!=ed_struct->ed_contact)
      {
        GeneralFunc_flag1(edchat_id,1);
        CreateEditChat(t);
      }
      break;
    default:
      i-=2;
      if (i<ed_struct->loaded_templates)
      {
	EDITCONTROL ec;
	WSHDR *ed_ws;
	int c;
	char *p=templates_lines[i];
	ExtractEditControl(ed_struct->ed_chatgui,ed_struct->ed_answer,&ec);
	ed_ws=AllocWS(ec.pWS->wsbody[0]+strlen(p));
	wstrcpy(ed_ws,ec.pWS);
        if (EDIT_GetFocus(ed_struct->ed_chatgui)==ed_struct->ed_answer)
        {
          int pos=EDIT_GetCursorPos(ed_struct->ed_chatgui);
          while(c=*p++)
          {
            wsInsertChar(ed_ws,char8to16(c),pos++);
          }
          EDIT_SetTextToEditControl(ed_struct->ed_chatgui,ed_struct->ed_answer,ed_ws);
          EDIT_SetCursorPos(ed_struct->ed_chatgui,pos);
        }
        else
        {
          while(c=*p++)
          {
            wsAppendChar(ed_ws,char8to16(c));
          }
          EDIT_SetTextToEditControl(ed_struct->ed_chatgui,ed_struct->ed_answer,ed_ws);
        }
	FreeWS(ed_ws);
      }
      break;
    }
  }
}

int edchat_onkey(GUI *data, GUI_MSG *msg)
{
  //-1 - do redraw
  GBS_DelTimer(&tmr_illumination);
  CLIST *t;
  TPKT *p;
  int l=msg->gbsmsg->submess;
  EDCHAT_STRUCT *ed_struct=EDIT_GetUserPointer(data);
  
  if (msg->keys==0xFFF)
  {
    void ec_menu(EDCHAT_STRUCT *);
    ec_menu(ed_struct);
    return(-1);
  }
  if (msg->keys==0xFF0)  return (1);
  if (msg->gbsmsg->msg==LONG_PRESS)
  {
    if (l==RIGHT_BUTTON)
    {
      if (EDIT_GetFocus(data)==ed_struct->ed_answer)
      {
	EDITCONTROL ec;
	ExtractEditControl(data,ed_struct->ed_answer,&ec);
	if (ec.pWS->wsbody[0]==(EDIT_GetCursorPos(data)-1))
	{
	  t=FindNextActiveContact(ed_struct->ed_contact);
	  if (t && t!=ed_struct->ed_contact)
	  {
	    CreateEditChat(t);
	    return(1);
	  }
	}
      }
    }
  }
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    if ((l>='0')&&(l<='9'))
    {
      if (EDIT_GetFocus(data)!=ed_struct->ed_answer)
	EDIT_SetFocus(data,ed_struct->ed_answer);
    }
    if (l==GREEN_BUTTON)
    {
      if (connect_state==3)
      {
	if ((t=ed_struct->ed_contact))
	{
	  LOGQ *lp=t->answer;
	  if (lp)
	  {
	    char *s=lp->text;
	    if (strlen(s))
	    {
	      t->isactive=ACTIVE_TIME;
	      p=malloc(sizeof(PKT)+(l=strlen(s))+1);
	      p->pkt.uin=t->uin;
	      p->pkt.type=T_SENDMSG;
	      p->pkt.data_len=l;
	      strcpy(p->data,s);
	      AddStringToLog(t,0x01,p->data,I_str,(++SENDMSGCOUNT)&0x7FFF); //Номер сообщения
	      SUBPROC((void *)SendAnswer,0,p);
	      mfree(t->answer);
	      t->answer=NULL;
	      //        request_remake_edchat=1;
	      EDIT_SetFocus(data,ed_struct->ed_answer);
	      CutWSTR(ews,0);
	      EDIT_SetTextToFocused(data,ews);
	      AddMsgToChat(data);
	      RecountMenu(t);
	      return(-1);
	    }
	  }
	}
      }
    }
    if (l==ENTER_BUTTON)
    {
      //      t=FindNextActiveContact(ed_struct->ed_contact);
      //      if ((t!=ed_struct->ed_contact) && t)
      {
	int i=ed_struct->loaded_templates=LoadTemplates(ed_struct->ed_contact->uin);
        EDIT_OpenOptionMenuWithUserItems(data,ed_options_handler,ed_struct,i+2);
        return (-1);
      }
    }
    if (l==VOL_UP_BUTTON)
    {
      int pos;
 
      if ((pos=EDIT_GetFocus(data)-2)>=2)
      {
        EDIT_SetFocus(data, pos);
        EDIT_SetCursorPos(data, 1);
      }
      return (-1);
    }
    if (l==VOL_DOWN_BUTTON)
    {
      int pos;

      if ((pos=EDIT_GetFocus(data)+2)<=ed_struct->ed_answer)
      {
        EDIT_SetFocus(data, pos);
        EDIT_SetCursorPos(data, 1);
      }
      return (-1);
    }
  }
  return(0); //Do standart keys
  //1: close
}


void ParseAnswer(WSHDR *ws, const char *s)
{
  S_SMILES *t;
  S_SMILES *t_root=(S_SMILES *)s_top;
  STXT_SMILES *st;
  unsigned int wchar;
  unsigned int ulb=s[0]+(s[1]<<8)+(s[2]<<16)+(s[3]<<24);
  CutWSTR(ws,0);
  int i;
  while(wchar=*s)
  {
    t=t_root;
    while(t)
    {
      st=t->lines;
      while(st)
      {
	if ((ulb&st->mask)==st->key)
	{
	  if (!strncmp(s,st->text,strlen(st->text))) goto L1;
	}
	st=st->next;
      }
      t=t->next;
    }
  L1:
    if (t)
    {
      wchar=t->uni_smile;
      s+=strlen(st->text);
      ulb=s[0]+(s[1]<<8)+(s[2]<<16)+(s[3]<<24);
    }
    else
    {
      wchar=char8to16(wchar);
      s++;
      ulb>>=8;
      ulb+=s[3]<<24;
    }
    if (wchar!=10) wsAppendChar(ws,wchar);
  }
  i=ws->wsbody[0];
  while(i>1)
  {
    if (ws->wsbody[i--]!=13) break;
    ws->wsbody[0]=i;
  }
}

static const HEADER_DESC edchat_hdr={0,0,NULL,NULL,NULL,0,LGP_NULL};

void (*old_ed_redraw)(void *data);
void my_ed_redraw(void *data)
{
  void *edchat_gui;
  EDCHAT_STRUCT *ed_struct;
  if (old_ed_redraw) old_ed_redraw(data);
  edchat_gui=FindGUIbyId(edchat_id,NULL);
  if (edchat_gui)
  {
    ed_struct=EDIT_GetUserPointer(edchat_gui);
    if (ed_struct)
    {
      int icon, width;
#ifndef	NEWSGOLD
      icon=*(S_ICONS+GetIconIndex(ed_struct->ed_contact));
      ((HEADER_DESC *)&edchat_hdr)->rc.x2=ScreenW()-1-(width=GetImgWidth(icon));
      DrawImg(ScreenW()-1-width,1,icon);
#else
      icon=*(S_ICONS+GetIconIndex(ed_struct->ed_contact));
      width=GetImgWidth(icon);
      DrawImg(2,((HeaderH()-width)>>1)+YDISP,icon);
/*      DrawRoundedFrame(ScreenW()-8,YDISP,ScreenW()-1,YDISP+7,0,0,0,
		       GetPaletteAdrByColorIndex(0),
		       GetPaletteAdrByColorIndex(EDIT_IsBusy(edchat_gui)?3:4));*/
#endif
    }
  }  
}

void edchat_ghook(GUI *data, int cmd)
{
  
  static const SOFTKEY_DESC sk={0x0FFF,0x0000,(int)LG_MENU};
  static const SOFTKEY_DESC sk_cancel={0x0FF0,0x0000,(int)LG_CLOSE};
  //  static SOFTKEY_DESC sk={0x0018,0x0000,(int)"Menu"};
  int j;
  EDITCONTROL ec;
  EDCHAT_STRUCT *ed_struct=EDIT_GetUserPointer(data);
  PNGTOP_DESC *pltop=PNG_TOP();
  if (cmd==9)
  {
    GBS_DelTimer(&tmr_illumination);          //by BoBa 25.06.07
    pltop->dyn_pltop=NULL;
  }
  if (cmd==2)
  {
    ed_struct->ed_chatgui=data;
//    edgui_data=data;
    EDIT_SetFocus(data,ed_struct->ed_answer);

//#ifdef NEWSGOLD
//#else
    static void *methods[16];
#ifdef NEWSGOLD
    void **m=GetDataOfItemByID(data,2);
#else
    void **m=GetDataOfItemByID(data,4);
#endif
    if (m)
    {
      if (m[1])
      {
        memcpy(methods,m[1],sizeof(methods));
        old_ed_redraw=(void (*)(void *))(methods[0]);
        methods[0]=(void *)my_ed_redraw;
        m[1]=methods;
      }      
    }
//#endif
  }
  if (cmd==3)
  {
//    if (edgui_data==data) edgui_data=NULL;
    RecountMenu(ed_struct->ed_contact);
    mfree(ed_struct);
  }
  if (cmd==0x0A)
  {
    pltop->dyn_pltop=SmilesImgList;
    DisableIDLETMR();
    total_unread-=ed_struct->requested_decrement_total_unread;
    ed_struct->requested_decrement_total_unread=0;
/*    if (request_close_edchat)
    {
      request_close_edchat=0;
      GeneralFunc_flag1(edchat_id,1);
      return;
    }*/
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
    ExtractEditControl(data,ed_struct->ed_answer,&ec);
    ExtractAnswer(ec.pWS);
    if (ec.pWS->wsbody[0]==0)
      SetSoftKey(data,&sk_cancel,SET_SOFT_KEY_N==0?1:0);
    if (ed_struct->ed_contact)
    {
      mfree(ed_struct->ed_contact->answer);
      ed_struct->ed_contact->answer=NewLOGQ(msg_buf);
    }
    if (!EDIT_IsBusy(data))
    {
      time_to_stop_t9=0;
      if (ed_struct->ed_contact->req_add)
      {
	ed_struct->ed_contact->req_add=0;
	AddMsgToChat(data);
	RecountMenu(ed_struct->ed_contact);
      }
      if (ed_struct->ed_contact->req_drawack)
      {
	ed_struct->ed_contact->req_drawack=0;
	DrawAck(data);
      }
    }
  }
  if (cmd==0x0C)
  {
    j=EDIT_GetFocus(data);
    if ((EDIT_GetUnFocus(data)<j)&&(j!=ed_struct->ed_answer))
      EDIT_SetCursorPos(data,1);
  }
}



static const INPUTDIA_DESC edchat_desc =
{
  1,
  edchat_onkey,
  edchat_ghook,
  (void *)edchat_locret,
  0,
  &menu_skt,
  {0,NULL,NULL,NULL},
  FONT_SMALL,
  100,
  101,
  0,
  //  0x00000001 - Выровнять по правому краю
  //  0x00000002 - Выровнять по центру
  //  0x00000004 - Инверсия знакомест
  //  0x00000008 - UnderLine
  //  0x00000020 - Не переносить слова
  //  0x00000200 - bold
  0,
  //  0x00000002 - ReadOnly
  //  0x00000004 - Не двигается курсор
  //  0x40000000 - Поменять местами софт-кнопки
  0x40000000
};

void CreateEditChat(CLIST *t)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  EDITC_OPTIONS ec_options;
  int color;
  int zc;
  
  LOGQ *lp=t->log;
  
//  edcontact=t;
  int edchat_toitem=0;
  
  *((int *)(&edchat_hdr.lgp_id))=(int)t->name;
//  *((int **)(&edchat_hdr.icon))=(int *)S_ICONS+GetIconIndex(t);
  *((int **)(&edchat_hdr.icon))=(int *)S_ICONS+IS_NULLICON;
  
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  
  while(lp)
  {
    ascii2ws(ews,lp->hdr);
    ConstructEditControl(&ec,1,ECF_APPEND_EOL,ews,ews->wsbody[0]);
    if ((zc=lp->acked&3))
    {
      if (zc==1)
	color=4; //Зеленый
      else
	color=I_COLOR;
    }
    else
    {
      if (lp->ID==0xFFFFFFFF)
	color=lp->type==1?I_COLOR:TO_COLOR;
      else
	color=20; //Серый
    }
    PrepareEditCOptions(&ec_options);
    SetPenColorToEditCOptions(&ec_options,color);
    SetFontToEditCOptions(&ec_options,2);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    edchat_toitem++;
    //    wsprintf(ews,percent_t,msg_buf);
    ParseAnswer(ews,lp->text);
    ConstructEditControl(&ec,3,ECF_APPEND_EOL|ECF_DISABLE_T9,ews,ews->wsbody[0]);
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,ED_FONT_SIZE);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    edchat_toitem++;
    lp=lp->next;
  }
  if (t->isunread) total_unread--;
  t->isunread=0;
  ChangeContactPos(t);
  wsprintf(ews, "-------");
  ConstructEditControl(&ec,1,0x40,ews,ews->wsbody[0]);
  PrepareEditCOptions(&ec_options);
  SetFontToEditCOptions(&ec_options,ED_FONT_SIZE);
  CopyOptionsToEditControl(&ec,&ec_options);
  AddEditControlToEditQend(eq,&ec,ma);
  edchat_toitem++;
  
  ParseAnswer(ews,t->answer?t->answer->text:empty_str);
  ConstructEditControl(&ec,3,0x00,ews,1024);
  PrepareEditCOptions(&ec_options);
  SetFontToEditCOptions(&ec_options,ED_FONT_SIZE);
  CopyOptionsToEditControl(&ec,&ec_options);
  AddEditControlToEditQend(eq,&ec,ma);
  edchat_toitem++;
  
  EDCHAT_STRUCT *ed_struct=malloc(sizeof(EDCHAT_STRUCT));
  ed_struct->ed_contact=t;
  ed_struct->ed_answer=edchat_toitem;
  ed_struct->requested_decrement_total_unread=0;
  t->req_add=0;
  t->last_log=NULL;
  
  //  int scr_w=ScreenW();
  //  int scr_h=ScreenH();
  //  int head_h=HeaderH();
  
  patch_header(&edchat_hdr);
  patch_input(&edchat_desc);
  //  edchat_desc.font=ED_FONT_SIZE;
  edchat_id=CreateInputTextDialog(&edchat_desc,&edchat_hdr,eq,1,ed_struct);
}

//-----------------------------------------------------------------------------
#define EC_MNU_MAX 8

void Quote(GUI *data)
{
  int q_n;
  EDITCONTROL ec;
  EDITCONTROL ec_ed;
  WSHDR *ed_ws;
  WSHDR *ws;
  
  EDCHAT_STRUCT *ed_struct;
  ed_struct=MenuGetUserPointer(data);
  
  q_n=EDIT_GetFocus(ed_struct->ed_chatgui);
  ExtractEditControl(ed_struct->ed_chatgui,q_n,&ec);
  ExtractEditControl(ed_struct->ed_chatgui,ed_struct->ed_answer,&ec_ed);
  ed_ws=AllocWS(ec_ed.maxlen);
  if (EDIT_IsMarkModeActive(ed_struct->ed_chatgui))
  {
    EDIT_GetMarkedText(ed_struct->ed_chatgui,ed_ws);
  }
  else
  {
    wstrcpy(ed_ws,ec.pWS);
  }
  int ed_pos=0;
  do
  {
    ed_pos++;
    wsInsertChar(ed_ws,'>',ed_pos++);
    wsInsertChar(ed_ws,' ',ed_pos);
  }
  while((ed_pos=wstrchr(ed_ws,ed_pos,'\r'))!=0xFFFF);
  wsAppendChar(ed_ws,'\r');
  ws=AllocWS(ec_ed.pWS->wsbody[0]+ed_ws->wsbody[0]);
  wstrcpy(ws,ec_ed.pWS);
  wstrcat(ws,ed_ws);
  FreeWS(ed_ws);
  CutWSTR(ws,ec_ed.maxlen);
  EDIT_SetFocus(ed_struct->ed_chatgui,ed_struct->ed_answer);
  EDIT_SetTextToFocused(ed_struct->ed_chatgui,ws);
  FreeWS(ws);
  GeneralFuncF1(1);
}


void GetShortInfo(GUI *data)
{
  EDCHAT_STRUCT *ed_struct;
  ed_struct=MenuGetUserPointer(data);
  
  TPKT *p;
  CLIST *t;
  if ((t=ed_struct->ed_contact)&&(connect_state==3))
  {
    p=malloc(sizeof(PKT));
    p->pkt.uin=t->uin;
    p->pkt.type=T_REQINFOSHORT;
    p->pkt.data_len=0;
    AddStringToLog(t, 0x01, "Request info...", I_str,0xFFFFFFFF);
    AddMsgToChat(ed_struct->ed_chatgui);
    RecountMenu(t);
    SUBPROC((void *)SendAnswer,0,p);
  }
  GeneralFuncF1(1);
}

void AskNickAndAddContact(EDCHAT_STRUCT *);
void AddCurContact(GUI *data)
{
  EDCHAT_STRUCT *ed_struct;
  ed_struct=MenuGetUserPointer(data);
  
  if ((ed_struct->ed_contact)&&(connect_state==3)) AskNickAndAddContact(ed_struct);
  GeneralFuncF1(1);
}

void SendAuthReq(GUI *data)
{
  EDCHAT_STRUCT *ed_struct;
  ed_struct=MenuGetUserPointer(data);
  
  TPKT *p;
  CLIST *t;
  int l;
  const char s[]=LG_AUTHREQ;
  if ((t=ed_struct->ed_contact)&&(connect_state==3))
  {
    p=malloc(sizeof(PKT)+(l=strlen(s))+1);
    p->pkt.uin=t->uin;
    p->pkt.type=T_AUTHREQ;
    p->pkt.data_len=l;
    strcpy(p->data,s);
    AddStringToLog(t,0x01,p->data,I_str,0xFFFFFFFF);
    AddMsgToChat(ed_struct->ed_chatgui);
    RecountMenu(t);
    SUBPROC((void *)SendAnswer,0,p);
  }
  GeneralFuncF1(1);
}

void SendAuthGrant(GUI *data)
{
  EDCHAT_STRUCT *ed_struct;
  ed_struct=MenuGetUserPointer(data);
  
  TPKT *p;
  CLIST *t;
  int l;
  const char s[]=LG_AUTHGRANT;
  if ((t=ed_struct->ed_contact)&&(connect_state==3))
  {
    p=malloc(sizeof(PKT)+(l=strlen(s))+1);
    p->pkt.uin=t->uin;
    p->pkt.type=T_AUTHGRANT;
    p->pkt.data_len=l;
    strcpy(p->data,s);
    AddStringToLog(t,0x01,p->data,I_str,0xFFFFFFFF);
    AddMsgToChat(ed_struct->ed_chatgui);
    RecountMenu(t);
    SUBPROC((void *)SendAnswer,0,p);
  }
  GeneralFuncF1(1);
}

void OpenLogfile(GUI *data)
{
  EDCHAT_STRUCT *ed_struct;
  ed_struct=MenuGetUserPointer(data);
  
  extern const char HIST_PATH[64];
  CLIST *t;
  WSHDR *ws=AllocWS(256);
  if ((t=ed_struct->ed_contact))
  {
    wsprintf(ws,"%s\\%u.txt",HIST_PATH,t->uin);
    ExecuteFile(ws,NULL,NULL);
  }
  FreeWS(ws);
  GeneralFuncF1(1);
}

void ClearLog(GUI *data/*,void *dummy*/)
{
  EDCHAT_STRUCT *ed_struct;
  ed_struct=MenuGetUserPointer(data);
  
  CLIST *t;
  if ((t=ed_struct->ed_contact))
  {
    if (t->log)
    {
      FreeLOGQ(&t->log);
      t->msg_count=0;
      if (ed_struct->ed_answer>=2&&ed_struct->ed_chatgui)
      {
        while(ed_struct->ed_answer!=2)
        {
          EDIT_RemoveEditControl(ed_struct->ed_chatgui,1);
          ed_struct->ed_answer--;
        }
      }
    }
    t->isactive=0;		//by BoBa  18.06.07
    RecountMenu(t);
    GeneralFuncF1(1);
  }
}

void ecmenu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

static const MENUITEM_DESC ecmenu_ITEMS[EC_MNU_MAX]=
{
  {NULL,(int)LG_MNUQUOTE,    LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUADDSML,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUSHINFO,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUADDREN,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUSAUTHREQ, LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUSAUTHGRT, LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUOPENLOG,  LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUCLEARCHT, LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2}
};

extern void AddSmile(GUI *data);
static const MENUPROCS_DESC ecmenu_HNDLS[EC_MNU_MAX]=
{
  Quote,
  AddSmile,
  GetShortInfo,
  AddCurContact,
  SendAuthReq,
  SendAuthGrant,
  OpenLogfile,
  ClearLog,
};

char ecm_contactname[64];

static const HEADER_DESC ecmenu_HDR={0,0,NULL,NULL,NULL,(int)ecm_contactname,LGP_NULL};

static const MENU_DESC ecmenu_STRUCT=
{
  8,NULL,ecmenu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  ecmenu_ITEMS,
  ecmenu_HNDLS,
  EC_MNU_MAX
};

void ec_menu(EDCHAT_STRUCT *ed_struct)
{
  CLIST *t;
  int to_remove[EC_MNU_MAX+1];
  int remove=0;
  if ((t=ed_struct->ed_contact))
  {
    if (t->name)
    {
      strncpy(ecm_contactname,t->name,63);
    }
    else
    {
      sprintf(ecm_contactname,"%u",t->uin);
    }
    if (EDIT_GetFocus(ed_struct->ed_chatgui)==ed_struct->ed_answer)
    {
      to_remove[++remove]=0;
    }
    else
    {
      to_remove[++remove]=1;
    }
    
    if (ed_struct->ed_answer<=2) to_remove[++remove]=7;
    if (!ed_struct->ed_contact || connect_state!=3)
    {
      to_remove[++remove]=2;
      to_remove[++remove]=3;
      to_remove[++remove]=4;
      to_remove[++remove]=5;
    }
    
    patch_header(&ecmenu_HDR);
    to_remove[0]=remove;
    CreateMenu(0,0,&ecmenu_STRUCT,&ecmenu_HDR,0,EC_MNU_MAX,ed_struct,to_remove);
  }
}

void anac_locret(void){}

int anac_onkey(GUI *data, GUI_MSG *msg)
{
  EDCHAT_STRUCT *ed_struct=EDIT_GetUserPointer(data);
  
  CLIST *t;
  TPKT *p;
  int l;
  char s[64];
  int w;
  EDITCONTROL ec;
  if (msg->keys==0xFFF)
  {
    if (connect_state==3)
    {
      if ((t=ed_struct->ed_contact))
      {
	ExtractEditControl(data,2,&ec);
	l=0;
	while(l<ec.pWS->wsbody[0])
	{
	  w=char16to8(ec.pWS->wsbody[l+1]);
	  if (w<32) w='_';
	  s[l++]=w;
	  if (l==63) break;
	}
	s[l]=0;
	if (strlen(s))
	{
	  p=malloc(sizeof(PKT)+(l=strlen(s))+1);
	  p->pkt.uin=t->uin;
	  p->pkt.type=T_ADDCONTACT;
	  p->pkt.data_len=l;
	  strcpy(p->data,s);
	  AddStringToLog(t, 0x01, LG_ADDCONT, I_str,0xFFFFFFFF);
	  AddMsgToChat(ed_struct->ed_chatgui);
	  RecountMenu(t);
	  SUBPROC((void *)SendAnswer,0,p);
	  return(1);
	}
      }
    }
  }
  return(0);
}

void anac_ghook(GUI *data,int cmd)
{
  static const SOFTKEY_DESC sk={0x0FFF,0x0000,(int)LG_DOIT};
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
  }
}

static const HEADER_DESC anac_hdr={0,0,NULL,NULL,NULL,(int)LG_ADDREN,LGP_NULL};

static const INPUTDIA_DESC anac_desc=
{
  1,
  anac_onkey,
  anac_ghook,
  (void *)anac_locret,
  0,
  &menu_skt,
  {0,NULL,NULL,NULL},
  4,
  100,
  101,
  0,
  //  0x00000001 - Выровнять по правому краю
  //  0x00000002 - Выровнять по центру
  //  0x00000004 - Инверсия знакомест
  //  0x00000008 - UnderLine
  //  0x00000020 - Не переносить слова
  //  0x00000200 - bold
  0,
  //  0x00000002 - ReadOnly
  //  0x00000004 - Не двигается курсор
  //  0x40000000 - Поменять местами софт-кнопки
  0x40000000
};

void AskNickAndAddContact(EDCHAT_STRUCT *ed_struct)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  WSHDR *ews=AllocWS(256);
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  wsprintf(ews,LG_SETNICK,ed_struct->ed_contact->uin);
  ConstructEditControl(&ec,1,0x40,ews,ews->wsbody[0]);
  AddEditControlToEditQend(eq,&ec,ma);
  wsprintf(ews,percent_t,ed_struct->ed_contact->name);
  ConstructEditControl(&ec,3,0x40,ews,63);
  AddEditControlToEditQend(eq,&ec,ma);
  //  int scr_w=ScreenW();
  //  int head_h=HeaderH();
  patch_header(&anac_hdr);
  patch_input(&anac_desc);
  CreateInputTextDialog(&anac_desc,&anac_hdr,eq,1,ed_struct);
  FreeWS(ews);
}

void as_locret(void){}

int as_onkey(GUI *data,GUI_MSG *msg)
{
  EDCHAT_STRUCT *ed_struct=EDIT_GetUserPointer(data);
  if ((msg->gbsmsg->msg==KEY_DOWN)||(msg->gbsmsg->msg==LONG_PRESS))
  {
    switch(msg->gbsmsg->submess)
    {
    case GREEN_BUTTON: //insert smile by GREEN_BUTTON by BoBa 19.04.2007
    case ENTER_BUTTON:
      msg->keys=0xFFF;
    }
  }
  if (msg->keys==0xFFF)
  {
    int uni_smile;
    WSHDR *ed_ws;
    EDITCONTROL ec;
    int pos;
    pos=EDIT_GetCursorPos(data);
    EDIT_ExtractFocusedControl(data,&ec);
    uni_smile=ec.pWS->wsbody[pos];
    
    ExtractEditControl(ed_struct->ed_chatgui,ed_struct->ed_answer,&ec);
    ed_ws=AllocWS(ec.pWS->wsbody[0]+1);
    wstrcpy(ed_ws,ec.pWS);
    pos=EDIT_GetCursorPos(ed_struct->ed_chatgui);
    wsInsertChar(ed_ws,uni_smile,pos);
    EDIT_SetTextToEditControl(ed_struct->ed_chatgui,ed_struct->ed_answer,ed_ws);
    EDIT_SetCursorPos(ed_struct->ed_chatgui,pos+1);
    FreeWS(ed_ws);
    return (1);
  }
  return(0);
}

void as_ghook(GUI *data, int cmd)
{
  static SOFTKEY_DESC ask={0x0FFF,0x0000,(int)LG_PASTESM};
  PNGTOP_DESC *pltop=PNG_TOP();
  if (cmd==9)
  {
    pltop->dyn_pltop=NULL;
  }
  if (cmd == 0x0A)
  {
    pltop->dyn_pltop=SmilesImgList;
    DisableIDLETMR();
  }
  if (cmd == 7)
  {
    EDITCONTROL ec;
    int pos;
    
    SetSoftKey(data,&ask,SET_SOFT_KEY_N);
    EDIT_ExtractFocusedControl(data,&ec);
    pos=EDIT_GetCursorPos(data);
    EDIT_SetTextInvert(data,pos,1);
  }
  if (cmd==0x0C)
  {
    EDIT_SetCursorPos(data,1);
  }
}

HEADER_DESC as_hdr={0,0,NULL,NULL,NULL,(int)LG_ADDSMIL,LGP_NULL};

INPUTDIA_DESC as_desc=
{
  1,
  as_onkey,
  as_ghook,
  (void *)as_locret,
  0,
  &menu_skt,
  {0,NULL,NULL,NULL},
  4,
  100,
  101,
  2,
  //  0x00000001 - Выровнять по правому краю
  //  0x00000002 - Выровнять по центру
  //  0x00000004 - Инверсия знакомест
  //  0x00000008 - UnderLine
  //  0x00000020 - Не переносить слова
  //  0x00000200 - bold
  0,
  //  0x00000002 - ReadOnly
  //  0x00000004 - Не двигается курсор
  //  0x40000000 - Поменять местами софт-кнопки
  0x40000000
};

void AddSmile(GUI *data)
{
  EDCHAT_STRUCT *ed_struct=MenuGetUserPointer(data);
  S_SMILES *smiles,*st;
  WSHDR *ws1;
  int n;
  
  st=smiles=(S_SMILES *)&s_top;
  n=0;
  while((st=st->next)) n++;
  if (!n) return;
  ws1=AllocWS(n);
  
  st=smiles;
  while((st=st->next)) wsAppendChar(ws1,st->uni_smile);
  
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,ws1,ws1->wsbody[0]);
  AddEditControlToEditQend(eq,&ec,ma);
  patch_header(&as_hdr);
  patch_input(&as_desc);
  CreateInputTextDialog(&as_desc,&as_hdr,eq,1,ed_struct);
  FreeWS(ws1);
  GeneralFuncF1(1);
}
