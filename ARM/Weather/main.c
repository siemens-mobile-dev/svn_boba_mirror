#include "../inc/swilib.h"
#include "conf_loader.h"

#define ONE_SECOND (212)

CSM_DESC icsmd;
int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

int sock;

int connect_state;

char buf[16384];
int pbuf;

GBSTMR mytmr;

extern const unsigned int TID;

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
    sa.port=htons(80);
    sa.ip=htonl(IP_ADDR(212,48,138,18));
    if (connect(sock,&sa,sizeof(sa))!=-1)
    {
      connect_state=1;
      REDRAW();
    }
    else
      closesocket(sock);
  }
}

void do_start_connection(void)
{
  SUBPROC((void *)create_connect);
}

char req_buf[256];

void send_req(void)
{
  sprintf(req_buf,"GET /rss/%u.xml"
          " HTTP/1.0\r\nHost: informer.gismeteo.ru\r\n\r\n",TID);
  send(sock,req_buf,strlen(req_buf),0);
  connect_state=2;
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
  int i=pbuf;
  if (i==16383)
    end_socket();
  else
  {
    i=recv(sock,buf+i,16383-i,0);
    if (i>=0)
    {
      pbuf+=i;
    }
    else
    {
      end_socket();
    }
  }
}

void SaveXML(void)
{
  TTime t;
  TDate d;
  char ss[100];
  unsigned int ul;
  int f=fopen("4:\\Weather.data",A_ReadWrite+A_Create+A_Append,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    GetDateTime(&d,&t);
    sprintf(ss,"%02d:%02d %02d-%02d-%04d:\r\n",
            t.hour,t.min,d.day,d.month,d.year);
    fwrite(f,ss,strlen(ss),&ul);
    fwrite(f,buf,pbuf,&ul);
    fwrite(f,"\r\n",2,&ul);
    fclose(f,&ul);
  }
}

int tag(const char *p, const char *s)
{
  while((*s)==(*p++)) s++;
  return(!(*s)); //Если 0 - конец строки, возвращаем не 0
}

StructUSSDStr *ready_weather;

void ProcessXML(void)
{
  int state;
  int trf=pbuf;
  char *p=buf;
//  unsigned int ul;
  WSHDR *dst=AllocWS(1024);

  state=0;
  for(;trf>0;trf--,p++)
  {
    switch(state)
    {
    case 0:
      if (!tag(p,"<item>")) continue;
      state++;
      break;
    case 1:
//      if (tag(p,"<title>"))
//      {
//        state=2;
//      }
//      else
      {
        if (tag(p,"<description>"))
        {
          state=4;
        }
        else break;
      }
      while(*p!='>') {p++; trf--;}
      break;
    case 4:
    case 2:
      if (*p!='<')
      {
        wsAppendChar(dst,char8to16(*p));
      }
      else
      {
        if (state==2)
        {
          wsAppendChar(dst,':');
          state=1;
          break;
        }
        goto L2;
        wsAppendChar(dst,'.');
        wsAppendChar(dst,' ');
        state=1;
      }
      break;
    }
  }
L2:
//  {
//    int f=fopen("4:\\Weather.utf",A_ReadWrite+A_Create+A_Append,P_READ+P_WRITE,&ul);
//    fwrite(f,dst->wsbody+1,dst->wsbody[0]*2,&ul);
//    fclose(f,&ul);
//  }
  ready_weather=malloc(1024+10);
  wstrcopy(ready_weather->wstr,dst->wsbody);
  ready_weather->type=7;
  ready_weather->param2=0x7fffffff;
  ready_weather->param3=0;
  SetUSSD(ready_weather);
  FreeWS(dst);
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  csm_result=old_icsm_onMessage(data,msg); //Вызываем старый обработчик событий
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
          //Если посылали запрос
          SUBPROC((void *)send_req);
          REDRAW();
        }
        else
        {
          ShowMSG(1,(int)"Illegal message ENIP_SOCK_CONNECTED!");
        }
        break;
      case ENIP_SOCK_DATA_READ:
        if (connect_state==2)
        {
          //Если посылали send
          SUBPROC((void *)get_answer);
          REDRAW();
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
        //Закрыт вызовом closesocket
        ShowMSG(1,(int)"Weather recived!");
        SaveXML();
        ProcessXML();
        connect_state=0;
        sock=-1;
        break;
      }
    }
    return(1);
  }
  return(csm_result);
}


void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  GBS_DelTimer(&mytmr);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main()
{
  InitConfig();
  if (TID)
  {
    LockSched();
    CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
    memcpy(&icsmd,icsm->constr,sizeof(icsmd));
    old_icsm_onMessage=icsmd.onMessage;
    old_icsm_onClose=icsmd.onClose;
    icsmd.onMessage=MyIDLECSM_onMessage;
    icsmd.onClose=MyIDLECSM_onClose;
    icsm->constr=&icsmd;
    UnlockSched();
    GBS_StartTimerProc(&mytmr,ONE_SECOND*30,do_start_connection);
  }
  else
  {
    LockSched();
    ShowMSG(1,(int)"Please setup Town ID!");
    UnlockSched();
  }
  return 0;
}
