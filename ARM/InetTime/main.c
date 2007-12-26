
#include "..\inc\swilib.h"
#include "..\inc\pnglist.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

const int minus11=-11;
unsigned short maincsm_name_body[140];
int MAINCSM_ID;
WSHDR *ews;

#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}








int sock;
int connect_state;
char buf[16384];
char logbuf[1024];
int pbuf;



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





int atoi(char *attr)
{
  int ret=0;
  int neg=1;
  for (int k=0; ; k++)
  {
    if ( attr[k]>0x2F && attr[k]<0x3A) {ret=ret*10+attr[k]-0x30;} else { if ( attr[k]=='-') {neg=-1;} else {return(ret*neg);}}
  }
}

void create_connect(void)
{
  SOCK_ADDR sa;
  //Устанавливаем соединение
  connect_state=0;
  pbuf=0;
  sock=socket(1,1,0);
  if (sock!=-1)
  {
    sa.family=1;
    sa.port=htons(37);
    sa.ip=htonl(IP_ADDR(212,192,122,109));    
    sa.port=htons(13);
    sa.ip=htonl(IP_ADDR(89,178,137,184));    
    
    if (connect(sock,&sa,sizeof(sa))!=-1)
    {
        //    ShowMSG(1,(int)"state 2");
      connect_state=1;
    strcat(logbuf,"Conect\n"); REDRAW();      
      REDRAW();
    }
    else{
      ShowMSG(1,(int)"state 3");
      closesocket(sock);
    }
  }
}


char req_buf[256];

void send_req(void)
{
  sprintf(req_buf,"\r\n");
  send(sock,req_buf,strlen(req_buf),0);
  connect_state=2;
  strcat(logbuf,"req\n"); REDRAW();        
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

void get_answer(void)
{
  strcat(logbuf,"ans1\n"); REDRAW();        
  char st[512];
  int i=pbuf;
  if (i==16383)
    end_socket();
  else
  {
    i=recv(sock,buf+i,16383-i,0);
    sprintf(st,"%04x-rc%d\n",sock,i);
  strcat(logbuf,st); REDRAW();            
    if (i>=0)
    {
      pbuf+=i;
    }
    else
    {

      end_socket();
      //sock=-1;
      
    }
  }
//  void Parsing();
//          Parsing();
}

typedef struct
{
  char yearNormBudd; //1 - norm, 2 - buddhist
  char dateFormat;
  char timeFormat; //0-24h, 1-12h
  char timeZone; //internal representation
  char isAutoTime1; 
  char isAutoTime2;
}TDateTimeSettings;


#pragma swi_number=0x230
__swi __arm int GetTimeZoneShift(TDate *, TTime *, int timeZone);
#pragma swi_number=0x822F
__swi __arm  TDateTimeSettings *RamDateTimeSettings(void);

void Parsing()
{
  strcat(logbuf,"Parsing\n"); REDRAW();
  //  strcat(logbuf,"Parsing\n"); REDRAW();
char st[1024];
  long  a;
  sscanf (buf,"%d",&a);
  sprintf(st,"%08d\n",a);

  strcat(logbuf,st); REDRAW();
  /*
  b=a-2208988800;
  char *s;
  struct tm *sd;

  //sd=gmtime(&a);
s=  ctime((const long*)&b);
//  ShowMSG(1,(int)s);  
  sprintf(st,"%08x\n%s",b,s);
  strcat(logbuf,st); REDRAW();*/
  char *s;
    int b;
  TDateTimeSettings *ts;
  ts=RamDateTimeSettings();
  
  TDate d;
  TTime t;  

  
   struct tm *dos;
dos=gmtime(&a);
t.hour=dos->tm_hour;
t.min=dos->tm_min;
t.sec=dos->tm_sec;
t.millisec=0;
d.day=dos->tm_yday;
d.year=dos->tm_year;
d.month=dos->tm_mon;

// GetDateTime(&d,&t);  
b=GetTimeZoneShift(&d,&t,ts->timeZone);
  a+=b*60;

dos=gmtime(&a);
t.hour=dos->tm_hour;
t.min=dos->tm_min;
t.sec=dos->tm_sec;
t.millisec=0;
d.day=dos->tm_yday;
d.year=dos->tm_year;
d.month=dos->tm_mon;
a-=b*60;
b=GetTimeZoneShift(&d,&t,ts->timeZone);
a+=b*60;
dos=gmtime(&a);
t.hour=dos->tm_hour;
t.min=dos->tm_min;
t.sec=dos->tm_sec;
t.millisec=0;
d.day=dos->tm_yday;
d.year=dos->tm_year;
d.month=dos->tm_mon;

SetDateTime(&d,&t);

s=  ctime((const long*)&a);
  sprintf(st,"%08x-%d\n%s",a,b,s);
  strcat(logbuf,st); REDRAW();


}


void DrawWait()
{
  WSHDR *ws = AllocWS(1024);
    wsprintf(ws,"%s",logbuf);
    DrawString(ws,2,YDISP+5,ScreenW()-1,ScreenH()-1,FONT_SMALL,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  FreeWS(ws);
}

void OnRedraw(MAIN_GUI *data)
{       
  DrawRoundedFrame(1 ,YDISP, ScreenW()-1, ScreenH()-1, 0,0,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(1));
  DrawWait();
}

void onCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->gui.state=1;
}

void onClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  data->gui.state=0;
}

void onFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{

//  DisableIconBar(1);

  data->gui.state=2;
  DisableIDLETMR();
}

void onUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

int OnKey(MAIN_GUI *data, GUI_MSG *msg)
{
  DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case RIGHT_SOFT: 
#ifndef NEWSGOLD
    case RED_BUTTON:       
#endif
      return(1);
    }
  }
  return(0);
}

extern void kill_data(void *p, void (*func_p)(void *));
void method7(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  kill_data(data, mfree_adr);
}

int method8(void){return(0);}
int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)OnRedraw,	
  (void *)onCreate,	
  (void *)onClose,	
  (void *)onFocus,	
  (void *)onUnfocus,
  (void *)OnKey,	
  0,
  (void *)method7,	
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
  //main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);    

  SUBPROC((void *)create_connect);
  ews=AllocWS(16384);
}

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

void maincsm_onclose(CSM_RAM *csm)
{

  FreeWS(ews);
  SUBPROC((void *)ElfKiller);
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if ((msg->msg==MSG_GUI_DESTROYED)&&((int)msg->data0==csm->gui_id))
  {
    csm->csm.state=-3;
  }
  
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    if ((int)msg->data1==sock)
    {
      //Если наш сокет
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
        if (connect_state==1)
        {
          strcat(logbuf,"Connected\n");
          REDRAW();
          //Если посылали запрос
          SUBPROC((void *)send_req);
          REDRAW();
        }
        else
        {
          strcat(logbuf,"Error, try again..\n"); 
          SUBPROC((void *)create_connect); 
          REDRAW();
        }
        break;
      case ENIP_SOCK_DATA_READ:
        if (connect_state==2)
        {
          strcat(logbuf,"Reading\n"); 
          REDRAW();
          //Если посылали send
          SUBPROC((void *)get_answer);
          REDRAW();
        }
        else
        {
          strcat(logbuf,"Error, try again..\n"); SUBPROC((void *)create_connect); REDRAW();
        }
        break;
      case ENIP_SOCK_REMOTE_CLOSED:
        //Закрыт со стороны сервера
        if (connect_state) SUBPROC((void *)end_socket);
        break;
      case ENIP_SOCK_CLOSED:
        //Закрыт вызовом closesocket
        if (connect_state) SUBPROC((void *)end_socket);
            SUBPROC((void *)Parsing);
        strcat(logbuf,"Receiving\n"); 
//        SUBPROC((void *)get_answer);
        REDRAW();
        connect_state=3;
        sock=-1;
        break;
      }
    }
    
  }
  return(1);
}

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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"Weather Forecast");
}



int main()
{
  int a,b,c;
  TDateTimeSettings *ts;
  ts=RamDateTimeSettings();
  
  a=GetTimeZoneShift(0,0,ts->timeZone);
  TDate d;
  TTime t;  
  GetDateTime(&d,&t);
  b=GetTimeZoneShift(&d,&t,ts->timeZone);
  char s[512];
  sprintf(s,"%d\n%d\n%d",a,b,c);
  ShowMSG(1,(int)s);
  
  InitConfig();
/*  if (TID)
  {  
*/
    char dummy[sizeof(MAIN_CSM)];
    UpdateCSMname();
    LockSched();
    MAINCSM_ID=CreateCSM(&MAINCSM.maincsm,dummy,0);
    UnlockSched();
    /*
  }
  else
  {
  
    LockSched();
    ShowMSG(1,(int)"Please setup Town ID!");
    UnlockSched();
  } 
  */
  return 0;
}

//---------------------------- Edit Control -----------------------------------
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





int itemnum, wd_id;


#define wslen(ws) ws->wsbody[0]
