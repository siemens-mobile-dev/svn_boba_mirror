
#include "..\inc\swilib.h"
#include "..\inc\pnglist.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

const int minus11=-11;
unsigned short maincsm_name_body[140];
int MAINCSM_ID;



extern void kill_data(void *p, void (*func_p)(void *));
void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}




int sock;
int connect_state;
char buf[512];
char logbuf[1024];
int pbuf;



typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;
/*
typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;

*/


/*
int atoi(char *attr)
{
  int ret=0;
  int neg=1;
  for (int k=0; ; k++)
  {
    if ( attr[k]>0x2F && attr[k]<0x3A) {ret=ret*10+attr[k]-0x30;} else { if ( attr[k]=='-') {neg=-1;} else {return(ret*neg);}}
  }
}
*/
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
//    sa.port=htons(37);
//    sa.ip=htonl(IP_ADDR(212,192,122,109));    
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
      ShowMSG(1,(int)"Can`t conect");

      closesocket(sock);
     CloseCSM( MAINCSM_ID);
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
  int i=pbuf;
  if (i==511)
    end_socket();
  else
  {
    i=recv(sock,buf+i,511-i,0);
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
 void ConvertTime(TDate *d,TTime *t, struct tm *dos ){
  t->hour=dos->tm_hour;
  t->min=dos->tm_min;
  t->sec=dos->tm_sec;
  t->millisec=0;
  d->day=dos->tm_mday;
  d->year=dos->tm_year+1900;
  d->month=dos->tm_mon+1;
  
}
char ddd[]="%d-%d-%d %d-%d-%d\n";
char ps[]=  "%s";
void Parsing()
{
  if (pbuf<10){  ShowMSG(1,(int)"Not all data trnsmited");
//     CloseCSM( MAINCSM_ID);
  }
  strcat(logbuf,"Parsing\n"); REDRAW();
  //  strcat(logbuf,"Parsing\n"); REDRAW();
char st[512];
  long  a;
  sscanf (buf,"%d",&a);
  sprintf(st,"%08d\n",a);

  strcat(logbuf,st); REDRAW();
  char *s;
  int b;
  TDateTimeSettings *ts=RamDateTimeSettings();
  TDate d;
  TTime t;  
 
  struct tm *dos;
  dos=gmtime(&a);   
  ConvertTime(&d,&t,dos);
  b=GetTimeZoneShift(&d,&t,ts->timeZone);
  a+=b*60;
  dos=gmtime(&a);   
  ConvertTime(&d,&t,dos);
  
  sprintf(st,ddd,d.year,d.month,d.day,t.hour,t.min,t.sec);
 
  strcat(logbuf,st); REDRAW();  

SetDateTime(&d,&t);

s=  ctime((const long*)&a);
  sprintf(st,ps,s);
  strcat(logbuf,st); REDRAW();
  ShowMSG(1,(int)s);
//  SUBPROC((void *)ElfKiller);
// CloseCSM( MAINCSM_ID);
}








void maincsm_oncreate(CSM_RAM *data)
{
//  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
//  zeromem(main_gui,sizeof(MAIN_GUI));
/*  main_gui->gui.canvas=(void *)(&Canvas);
  //main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  */
  csm->csm.state=0;
  csm->csm.unk1=0;
//  csm->gui_id=CreateGUI(main_gui);    

  SUBPROC((void *)create_connect);

}


void maincsm_onclose(CSM_RAM *csm)
{
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
         
          //Если посылали запрос
          SUBPROC((void *)send_req);
         
        }
        else
        {
          strcat(logbuf,"Error, try again..\n"); 
          SUBPROC((void *)create_connect); 
         
        }
        break;
      case ENIP_SOCK_DATA_READ:
        if (connect_state==2)
        {
          strcat(logbuf,"Reading\n"); 
         
          //Если посылали send
          SUBPROC((void *)get_answer);
         
        }
        else
        {
          strcat(logbuf,"Error, try again..\n"); SUBPROC((void *)create_connect); REDRAW();
        }
        break;
      case ENIP_SOCK_REMOTE_CLOSED:
        //Закрыт со стороны сервера
        if (connect_state) SUBPROC((void *)end_socket);
//                CloseCSM( MAINCSM_ID);
        break;
      case ENIP_SOCK_CLOSED:
        //Закрыт вызовом closesocket
        if (connect_state) SUBPROC((void *)end_socket);
            SUBPROC((void *)Parsing);
        strcat(logbuf,"Receiving\n"); 

        connect_state=3;
        sock=-1;
         CloseCSM( MAINCSM_ID);
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"DayTime Protocol (13)");
}



int main()
{
//  InitConfig();
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  LockSched();
  MAINCSM_ID=CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;
}







int itemnum, wd_id;


#define wslen(ws) ws->wsbody[0]
