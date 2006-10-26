#include "../inc/swilib.h"
#include "../inc/cfg_items.h"

#define TMR_SECOND 216

#define T_REQLOGIN 1
#define T_SENDMSG 2
#define T_RECVMSG 3
#define T_LOGIN 4
#define T_ERROR 6
#define T_CLENTRY 7
#define T_STATUSCHANGE 9

#define IS_OFFLINE 0
const char S_OFFLINE[]="Offline";
#define IS_INVISIBLE 1
const char S_INVISIBLE[]="Invisible";
#define IS_AWAY 2
const char S_AWAY[]="Away";
#define IS_NA 3
const char S_NA[]="N/A";
#define IS_OCCUPIED 4
const char S_OCCUPIED[]="Occupied";
#define IS_DND 5
const char S_DND[]="DND";
#define IS_ONLINE 6
const char S_ONLINE[]="Online";
#define IS_FFC 7
const char S_FFC[]="FFC";
#define IS_MSG 8

#define IS_UNKNOWN 9

int S_ICONS[11];

#define EOP -10

WSHDR *ews;

extern const unsigned int UIN;
extern const char PASS[];
extern const unsigned int ICON0;
extern const unsigned int ICON1;
extern const unsigned int ICON2;
extern const unsigned int ICON3;
extern const unsigned int ICON4;
extern const unsigned int ICON5;
extern const unsigned int ICON6;
extern const unsigned int ICON7;
extern const unsigned int ICON8;
extern const unsigned int ICON9;

const char percent_t[]="%t";
const char empty_str[]="";

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

typedef struct
{
  unsigned long uin;
  unsigned short type;
  unsigned short data_len;
}PKT;

int RXstate=EOP; //-sizeof(RXpkt)..-1 - receive header, 0..RXpkt.data_len - receive data

typedef struct
{
  PKT pkt;
  char data[16384];
}TPKT;

char *msg_buf;
TPKT RXbuf;
TPKT TXbuf;
char logmsg[256];

int connect_state=0;

int sock=-1;

extern void kill_data(void *p, void (*func_p)(void *));

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

//===============================================================================================
typedef struct
{
  void *next;
  unsigned int uin;
  char name[64];
  unsigned short state;
  int isunread;
  char *log;
  char *answer;
}CLIST;

volatile CLIST *cltop;

volatile int contactlist_menu_id;
volatile int request_remake_clmenu;

GBSTMR tmr_vibra;

volatile int edchat_id;
volatile int request_remake_edchat;
volatile int edchat_toitem;
volatile int edchat_answeritem;
CLIST *edcontact;

//MUTEX contactlist_mtx;

HEADER_DESC contactlist_menuhdr={0,0,131,21,NULL,(int)"Contacts...",0x7FFFFFFF};
int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Options"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

void contactlist_menu_ghook(void *data, int cmd);
int contactlist_menu_onkey(void *data, GUI_MSG *msg);
void contactlist_menu_iconhndl(void *data, int curitem, int *unk);

MENU_DESC contactlist_menu=
{
  8,(void *)contactlist_menu_onkey,(void*)contactlist_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  (void *)contactlist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

GBSTMR tmr_ping;

//Уничтожить список
void FreeCLIST(void)
{
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  while(cl)
  {
    CLIST *p;
    if (cl->log) mfree(cl->log);
    if (cl->answer) mfree(cl->answer);
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
    if (t->isunread)
      return(IS_MSG);
    else
    {
      if (s==0xFFFF) return(IS_OFFLINE);
      if (s & 0x0020) return(IS_FFC);
      if (s & 0x0001) return(IS_AWAY);
      if (s & 0x0004) return(IS_NA);
      if (s & 0x0010) return(IS_OCCUPIED);
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
    if (t->uin==uin) break;
    t=t->next;
  }
  return(t);
}

CLIST *FindContactByNS(int *i, int si)
{
  CLIST *t;
  t=(CLIST *)cltop;
  while(t)
  {
    if (GetIconIndex(t)==si)
    {
      if (!(*i)) return(t);
      (*i)--;
    }
    t=t->next;
  }
  return(t);
}


CLIST *FindContactByN(int i)
{
  CLIST *t;
  t=FindContactByNS(&i,8); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,7); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,6); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,5); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,4); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,3); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,2); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,1); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,0); if ((!i)&&(t)) return(t);
  t=FindContactByNS(&i,9); if ((!i)&&(t)) return(t);
  return t;
}

void create_contactlist_menu(void)
{
  CLIST *t;
  int i;

  t=(CLIST *)cltop;
  i=0;
  while(t)
  {
    t=t->next;
    i++;
  }
  if (!i) return; //Нечего создавать
  contactlist_menu_id=CreateMenu(0,0,&contactlist_menu,&contactlist_menuhdr,0,i,0,0);
}

void contactlist_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A) DisableIDLETMR();
}

int contactlist_menu_onkey(void *data, GUI_MSG *msg)
{
  CLIST *t;
  int i;
  if (msg->keys==0x18)
  {
    //    GeneralFunc_F1(1);
    ShowMSG(1,(int)"Under construction!");
    return(-1);
  }
  if (msg->keys==0x3D)
  {
    void CreateEditChat(CLIST *t);
    i=GetCurMenuItem(data);
    t=FindContactByN(i);
    if (t) CreateEditChat(t);
    //    GeneralFunc_F1(1);
    return(-1);
  }
  return(0);
}

void contactlist_menu_iconhndl(void *data, int curitem, int *unk)
{
  CLIST *t;
  WSHDR *ws;
  void *item=AllocMenuItem(data);

  t=FindContactByN(curitem);
  if (t)
  {
    ws=AllocMenuWS(data,strlen(t->name));
    wsprintf(ws,percent_t,t->name);
  }
  else
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,"error");
  }
  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(t));
}

void remake_clmenu(void)
{
  if (contactlist_menu_id)
  {
    request_remake_clmenu=1;
    GeneralFunc_flag1(contactlist_menu_id,1);
  }
  else
  {
    create_contactlist_menu();
  }
}

CLIST *AddContact(unsigned int uin, char *name)
{
  CLIST *p=malloc(sizeof(CLIST));
  CLIST *t;
  CLIST *pr;
  zeromem(p,sizeof(CLIST));
  p->uin=uin;
  strncpy(p->name,name,sizeof(p->name)-1);
  p->state=0xFFFF;
  t=(CLIST *)cltop;
  if (t)
  {
    //Не первый
    pr=(CLIST *)&cltop;
    while(strcmp(t->name,p->name)<0)
    {
      pr=t;
      t=t->next;
      if (!t) break;
    }
    p->next=t;
    pr->next=p;
  }
  else
  {
    //Первый
    cltop=p;
  }
  //  GBS_StartTimerProc(&tmr_contactlist_update,1000,remake_clmenu);
  return(p);
}

//===============================================================================================
void create_connect(void)
{
  SOCK_ADDR sa;
  //Устанавливаем соединение
  connect_state=0;
  sock=socket(1,1,0);
  if (sock!=-1)
  {
    sa.family=1;
    sa.port=htons(5050);
    sa.ip=htonl(IP_ADDR(82,207,89,182));
    if (connect(sock,&sa,sizeof(sa))!=-1)
    {
      connect_state=1;
      REDRAW();
    }
    else
      closesocket(sock);
  }
}

void send_login(void)
{
  int l=strlen(TXbuf.data);
  TXbuf.pkt.data_len=l;
  send(sock,&TXbuf,sizeof(PKT)+l,0);
  RXstate=-(int)sizeof(PKT);
  connect_state=2;
}

void do_ping(void)
{
  static PKT pingp;
  pingp.uin=UIN;
  pingp.type=0;
  pingp.data_len=0;
  send(sock,&pingp,sizeof(PKT),0);
}


void call_ping(void)
{
  if (connect_state>2) SUBPROC((void *)do_ping);
  GBS_StartTimerProc(&tmr_ping,120*TMR_SECOND,call_ping);
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

GBSTMR tmr_dorecv;

void dorecv(void)
{
  void get_answer(void);
  if (connect_state>1)
  {
    SUBPROC((void *)get_answer);
  }
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
      snprintf(logmsg,255,"Bad packet");
      end_socket();
      RXstate=EOP;
      return;
    }
    j=recv(sock,RXbuf.data+i,RXbuf.pkt.data_len-i,0);
    if (j>0) i+=j;
  LPKT:
    if (i==RXbuf.pkt.data_len)
    {
      //Пакет удачно принят, можно разбирать...
      RXbuf.data[RXbuf.pkt.data_len]=0; //Конец строки
      switch(RXbuf.pkt.type)
      {
      case T_LOGIN:
        //Удачно залогинились
        GBS_StartTimerProc(&tmr_ping,120*TMR_SECOND,call_ping);
        snprintf(logmsg,255,"%s",RXbuf.data);
        connect_state=3;
        REDRAW();
        break;
      case T_CLENTRY:
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
        snprintf(logmsg,255,"SC%d: %04X",RXbuf.pkt.uin,*((unsigned short *)(RXbuf.data)));
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        break;
      case T_ERROR:
        snprintf(logmsg,255,"ERR: %s",RXbuf.data);
        REDRAW();
        break;
      case T_RECVMSG:
        j=i+sizeof(PKT)+1;
        p=malloc(j);
        memcpy(p,&RXbuf,j);
        snprintf(logmsg,255,"MSG%d: %s",RXbuf.pkt.uin,RXbuf.data);
        GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,p,sock);
        REDRAW();
        break;
      }
      i=-(int)sizeof(PKT); //А может еще есть данные
    }
  }
  RXstate=i;
  GBS_StartTimerProc(&tmr_dorecv,3000,dorecv);
  //  REDRAW();
}

void SendPreved(void)
{
  int l;
  strcpy(TXbuf.data,"Превед!");
  l=strlen(TXbuf.data);
  TXbuf.pkt.type=T_SENDMSG;
  TXbuf.pkt.uin=223642069;
  TXbuf.pkt.data_len=l;
  send(sock,&TXbuf,sizeof(PKT)+l,0);
}

void AddStringToLog(CLIST *t, char code, char *s, char *name)
{
  char hs[128];
  TTime tt;
  TDate d;
  char *ns;
  GetDateTime(&d,&tt);

  if (!t->log)
  {
    *(t->log=malloc(1))=0;
  }
  sprintf(hs,"%c%02d:%02d %02d-%02d %s:\n",code,tt.hour,tt.min,d.day,d.month,name);
  ns=malloc(strlen(t->log)+strlen(hs)+strlen(s)+1);
  strcpy(ns,t->log);
  strcat(ns,hs);
  strcat(ns,s);
  mfree(t->log);
  t->log=ns;
  t->isunread=1;
}

void SendAnswer(int dummy, TPKT *p)
{
  send(sock,p,sizeof(PKT)+p->pkt.data_len,0);
  mfree(p);
}

void stop_vibra(void)
{
  SetVibration(0);
}

ProcessPacket(TPKT *p)
{
  CLIST *t;
  char s[64];
  switch(p->pkt.type)
  {
  case T_CLENTRY:
    if (p->pkt.uin)
    {
      if ((t=FindContactByUin(p->pkt.uin)))
      {
        t->state=0xFFFF;
      }
      else
      {
        AddContact(p->pkt.uin,p->data);
      }
    }
    else
      remake_clmenu();
    break;
  case T_STATUSCHANGE:
    t=FindContactByUin(p->pkt.uin);
    if (t)
    {
      t->state=*((unsigned short *)(p->data));
      if (IsGuiOnTop(contactlist_menu_id)) RefreshGUI();
    }
    break;
  case T_RECVMSG:
    t=FindContactByUin(p->pkt.uin);
    if (t)
    {
      GBS_StartTimerProc(&tmr_vibra,TMR_SECOND*1,stop_vibra);
      SetVibration(100);
      AddStringToLog(t,0x02,p->data,t->name);
      if (edchat_id)
      {
        if (edcontact->isunread)
        {
          request_remake_edchat=1;
          GeneralFunc_flag1(edchat_id,1);
        }
      }
      else
      {
        if (IsGuiOnTop(contactlist_menu_id)) RefreshGUI();
      }
    }
    else
    {
      sprintf(s,"%d",p->pkt.uin);
      AddContact(p->pkt.uin,s);
    }
  }
  mfree(p);
}


//===============================================================================================
void method0(MAIN_GUI *data)
{
  DrawRoundedFrame(0,0,ScreenW()-1,ScreenH()-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(20));
  wsprintf(data->ws1,"State: %d, RXstate: %d\n%t",connect_state,RXstate,logmsg);
  DrawString(data->ws1,3,3,ScreenW()-4,ScreenH()-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  wsprintf(data->ws2,percent_t,"Exit");
  DrawString(data->ws2,(ScreenW()>>2)*3,ScreenH()-4-14,ScreenW()-4,ScreenH()-4,MIDDLE_FONT,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  wsprintf(data->ws2,percent_t,cltop?"CList":empty_str);
  DrawString(data->ws2,3,ScreenH()-4-14,(ScreenW()>>2),ScreenH()-4,MIDDLE_FONT,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
}

void method1(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  data->gui.state=1;
}

void method2(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void method3(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void method4(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

int method5(MAIN_GUI *data, GUI_MSG *msg)
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
      if (connect_state==0)
      {
        SUBPROC((void *)create_connect);
      }
      break;
      /*    case '0':
      if (connect_state==3)
      {
      SUBPROC((void *)SendPreved);
    }
      break;*/
    }
  }
  //  method0(data);
  return(0);
}
//void method7(MAIN_GUI *data, void (*mfree_adr)(void *))
//{
//  mfree_adr(data);
//}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)method0,	//Redraw
  (void *)method1,	//Create
  (void *)method2,	//Close
  (void *)method3,	//Focus
  (void *)method4,	//Unfocus
  (void *)method5,	//OnKey
  0,
  (void *)kill_data, //method7,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,131,175};

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
  ews=AllocWS(16384);
  msg_buf=malloc(16384);
  //  MutexCreate(&contactlist_mtx);
  SUBPROC((void *)create_connect);
}

void maincsm_onclose(CSM_RAM *csm)
{
  GBS_DelTimer(&tmr_dorecv);
  GBS_DelTimer(&tmr_ping);
  GBS_DelTimer(&tmr_vibra);
  SetVibration(0);
  FreeCLIST();
  mfree(msg_buf);
  FreeWS(ews);
  //  MutexDestroy(&contactlist_mtx);
  SUBPROC((void *)end_socket);
  SUBPROC((void *)ElfKiller);
}


int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  //  char ss[100];
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
    if ((int)msg->data0==contactlist_menu_id)
    {
      contactlist_menu_id=0;
      if (request_remake_clmenu)
      {
        request_remake_clmenu=0;
        create_contactlist_menu();
      }
    }
    if ((int)msg->data0==edchat_id)
    {
      edchat_id=0;
      if (request_remake_edchat)
      {
        void CreateEditChat(CLIST *t);
        request_remake_edchat=0;
        CreateEditChat(edcontact);
      }
    }
  }
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
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
          //Соединение установленно, посылаем пакет login
          strcpy(logmsg,"Try to login...");
          TXbuf.pkt.uin=UIN;
          TXbuf.pkt.type=T_REQLOGIN;
          strcpy(TXbuf.data,PASS);
          SUBPROC((void *)send_login);
          if (!FindContactByUin(UIN)) AddContact(UIN,"Loopback");
          REDRAW();
        }
        else
        {
          ShowMSG(1,(int)"Illegal message ENIP_SOCK_CONNECTED!");
        }
        break;
      case ENIP_SOCK_DATA_READ:
        if (connect_state>=2)
        {
          //Если посылали send
          SUBPROC((void *)get_answer);
          //          REDRAW();
        }
        else
        {
          ShowMSG(1,(int)"Illegal message ENIP_DATA_READ");
        }
        break;
      case ENIP_SOCK_REMOTE_CLOSED:
        //Закрыт со стороны сервера
        if (connect_state) SUBPROC((void *)end_socket);
        break;
      case ENIP_SOCK_CLOSED:
        //        strcpy(logmsg,"No connection");
        if (edchat_id)
        {
          request_remake_edchat=0;
          GeneralFunc_flag1(edchat_id,1);
        }
        if (contactlist_menu_id)
        {
          request_remake_clmenu=0;
          GeneralFunc_flag1(contactlist_menu_id,1);
        }
        connect_state=0;
        sock=-1;
        REDRAW();
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
  WSHDR *ws=AllocWS(256);
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"NATICQ",ws);
  FreeWS(ws);
}

#pragma segment="CONFIG_C"

int LoadConfigData(const char *fname)
{
  int f;
  unsigned int ul;
  char *buf;
  int result=0;
  void *cfg;

  extern const CFG_HDR cfghdr0; //first var in CONFIG
  cfg=(void*)&cfghdr0;

  unsigned int len=(int)__segment_end("CONFIG_C")-(int)__segment_begin("CONFIG_C");

  if (!(buf=malloc(len))) return -1;
  if ((f=fopen(fname,A_ReadOnly+A_BIN,0,&ul))!=-1)
  {
    if (fread(f,buf,len,&ul)==len)
    {
      memcpy(cfg,buf,len);
      fclose(f,&ul);
    }
    else
    {
      fclose(f,&ul);
      goto L_SAVENEWCFG;
    }
  }
  else
  {
  L_SAVENEWCFG:
    if ((f=fopen(fname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=-1)
    {
      if (fwrite(f,cfg,len,&ul)!=len) result=-1;
      fclose(f,&ul);
    }
    else
      result=-1;
  }
  mfree(buf);
  return(result);
}

int main()
{
  char dummy[sizeof(MAIN_CSM)];
  if (LoadConfigData("4:\\ZBin\\etc\\NATICQ.bcfg")<0)
  {
    LoadConfigData("0:\\ZBin\\etc\\NATICQ.bcfg");
  }
  S_ICONS[0]=ICON0;
  S_ICONS[1]=ICON1;
  S_ICONS[2]=ICON2;
  S_ICONS[3]=ICON3;
  S_ICONS[4]=ICON4;
  S_ICONS[5]=ICON5;
  S_ICONS[6]=ICON7;
  S_ICONS[7]=ICON6;
  S_ICONS[8]=ICON8;
  S_ICONS[9]=ICON9;

  if (!UIN)
  {
    LockSched();
    ShowMSG(1,(int)"Please setup UIN/PASS!");
    UnlockSched();
    return 0;
  }
  UpdateCSMname();
  LockSched();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
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

unsigned int char16to8(unsigned int c)
{
  const TUNICODE2CHAR *p=unicode2char;
  unsigned int i;
  if (c<32) return(' ');
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

void edchat_locret(void){}

void ExtractAnswer(WSHDR *ws)
{
  int i=0;
  do
  {
    if (i>=ws->wsbody[0]) break;
    msg_buf[i]=char16to8(ws->wsbody[i+1]);
    i++;
  }
  while(i<16383);
  msg_buf[i]=0;
  return;
}

int edchat_onkey(GUI *data, GUI_MSG *msg)
{
  //-1 - do redraw
  CLIST *t;
  TPKT *p;
  char *s;
  int l;
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    l=msg->gbsmsg->submess;
    if ((l>='0')&&(l<='9'))
    {
      if (EDIT_GetFocus(data)!=edchat_answeritem)
        EDIT_SetFocus(data,edchat_answeritem);
    }
    if (l==GREEN_BUTTON)
    {
      if (connect_state==3)
      {
        if ((t=edcontact))
        {
          if ((s=t->answer))
          {
            if (strlen(s))
            {
              p=malloc(sizeof(PKT)+(l=strlen(s))+1);
              p->pkt.uin=t->uin;
              p->pkt.type=T_SENDMSG;
              p->pkt.data_len=l;
              strcpy(p->data,s);
              AddStringToLog(t,0x01,p->data,"I sad");
              SUBPROC((void *)SendAnswer,0,p);
              mfree(s);
              t->answer=0;
              request_remake_edchat=1;
              EDIT_SetFocus(data,edchat_answeritem);
              CutWSTR(ews,0);
              EDIT_SetTextToFocused(data,ews);
              return(1);
            }
          }
        }
      }
    }
  }
  return(0); //Do standart keys
  //1: close
}

void edchat_ghook(GUI *data, int cmd)
{
  //  static SOFTKEY_DESC sk={0x0018,0x0000,(int)"Menu"};
  char *s;
  EDITCONTROL ec;
  if (cmd==3)
  {
    //    EDIT_CURSOR_POS(data)=0x7FFF;
  }
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
  if (cmd==7)
  {
    if (edchat_toitem)
    {
      EDIT_SetFocus(data,edchat_toitem);
      edchat_toitem=0;
    }
    ExtractEditControl(data,edchat_answeritem,&ec);
    ExtractAnswer(ec.pWS);
    if (edcontact)
    {
      if ((s=edcontact->answer)) mfree(s);
      s=malloc(strlen(msg_buf)+1);
      strcpy(s,msg_buf);
      edcontact->answer=s;
    }
  }
}

HEADER_DESC edchat_hdr={0,0,131,21,NULL,0,0x7FFFFFFF};

INPUTDIA_DESC edchat_desc=
{
  1,
  edchat_onkey,
  edchat_ghook,
  (void *)edchat_locret,
  0,
  &menu_skt,
  {0,22,131,153},
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

void CreateEditChat(CLIST *t)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  int j;
  char hdr[128];

  char *s=t->log;

  //  if (!s) return;

  edcontact=t;
  edchat_toitem=0;

  edchat_hdr.lgp_id=(int)t->name;
  edchat_hdr.icon=(int *)S_ICONS+GetIconIndex(t);
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());

  if (s) while(*s)
  {
    s++; //Пропуск типа
    j=0;
    while((hdr[j]=*s++)!='\n') j++;
    hdr[j]=0;
    wsprintf(ews,percent_t,hdr);
    ConstructEditControl(&ec,1,0x40,ews,strlen(hdr));
    AddEditControlToEditQend(eq,&ec,ma);
    edchat_toitem++;
    j=0;
    while((msg_buf[j]=*s)>3) {if (msg_buf[j]!=10) j++; s++;}
    if (j)
    {
      while(msg_buf[j-1]==13) j--;
    }
    msg_buf[j]=0;
    wsprintf(ews,percent_t,msg_buf);
    ConstructEditControl(&ec,3,0x40,ews,strlen(msg_buf));
    AddEditControlToEditQend(eq,&ec,ma);
    edchat_toitem++;
  }
  t->isunread=0;
  wsprintf(ews,"-------");
  ConstructEditControl(&ec,1,0x40,ews,wstrlen(ews));
  AddEditControlToEditQend(eq,&ec,ma);
  edchat_toitem++;
  wsprintf(ews,percent_t,t->answer?t->answer:empty_str);
  ConstructEditControl(&ec,3,0x00,ews,1024);
  AddEditControlToEditQend(eq,&ec,ma);
  edchat_toitem++;
  edchat_answeritem=edchat_toitem;
  edchat_id=CreateInputTextDialog(&edchat_desc,&edchat_hdr,eq,1,0);
}
