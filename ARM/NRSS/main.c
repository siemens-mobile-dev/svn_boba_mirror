#include "..\inc\swilib.h"
#include "rect_patcher.h"
#include "decode_xml.h"
#include "conf_loader.h"
#include "config_data.h"
#include "strings.h"
#include "main.h"
#include "items_menu.h"

int sock;

int connect_state;
int is_gprs_online;
int DNR_TRIES;
int DNR_ID;

int ALLTOTALRECEIVED;
int ALLTOTALSENDED;

char logmsg[256];
GBSTMR mytmr;


typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
}MAIN_GUI;


SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Лев"},
  {0x0001,0x0000,(int)"Прав"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};



//=====================================================================


RSS_ITEM *rss_first;
RSS_ITEM *rss_last;


void FreeRssItems(void)
{
  RSS_ITEM *p=rss_first;
  LockSched();
  rss_first=NULL;
  UnlockSched();
  while(p)
  {
    RSS_ITEM *pr=p->next;
    mfree(p->title);
    mfree(p->description);
    p=pr;
  }
}

//=====================================================================
void create_connect(void)
{
  char *s, *d;
  int c;
  char rss_host[64];
  unsigned int rss_port=80;
  int ***p_res=NULL;
  SOCK_ADDR sa;
  //Устанавливаем соединение
  connect_state = 0;
  int err;
  unsigned int ip;
  if (!IsGPRSEnabled())
  {
    is_gprs_online=0;
    return;
  }
  DNR_ID=0;
  *socklasterr()=0;
  if ((s=strchr(RSS_URL,':')))
  {
    if (*(s+1)=='/' && *(s+2)=='/')
    {
      s+=3;
    }
    else s=0;
  }
  if (!s) s=(char *)RSS_URL;

  d=rss_host;
  while((c=*s++))
  {
    if (c=='/' || c==':') break;
    *d++=c;
  }
  *d=0;

  if ((s=strrchr(RSS_URL,':')))
  {
    if (*(s+1)!='/' || *(s+2)!='/')
    {
      rss_port=0;
      s++;
      while((c=*s++))
      {
        if (c=='/' || c==':' || !(c>='0' && c<='9')) break;
        rss_port*=10;
        rss_port+=c-'0';
      }
    }
  }
  sprintf(logmsg, "Connect to: %s Using port: %d", rss_host, rss_port);
  REDRAW();
  ip=str2ip(rss_host);
  if (ip!=0xFFFFFFFF)
  {
    sa.ip=ip;
    goto L_CONNECT;
  }
  err=async_gethostbyname(rss_host,&p_res,&DNR_ID); //03461351 3<70<19<81
  if (err)
  {
    if ((err==0xC9)||(err==0xD6))
    {
      if (DNR_ID)
      {
        strcpy(logmsg, "Wait DNR");
        REDRAW();
	return; //Ждем готовности DNR
      }
    }
    else
    {
      snprintf(logmsg,255,"DNR error %d",err);
      return;
    }
  }
  if (p_res)
  {
    if (p_res[3])
    {
      strcpy(logmsg,"DNR ok!");
      REDRAW();
      DNR_TRIES=0;
      sa.ip=p_res[3][0][0];
    L_CONNECT:
      sock=socket(1,1,0);
      if (sock!=-1)
      {
	sa.family=1;
	sa.port=htons(rss_port);
	if (connect(sock,&sa,sizeof(sa))!=-1)
	{
	  connect_state=1;
	}
	else
	{
	  closesocket(sock);
	  sock=-1;
          snprintf(logmsg,255,"Connect fault");
          REDRAW();
	}
      }
      else
      {
        snprintf(logmsg,255,"Error Create Socket");
        REDRAW();
      }
    }
  }
  else
  {
    DNR_TRIES--;
  }
}


void do_start_connection(void)
{
  SUBPROC((void *)create_connect);
}

char *recv_buf=NULL;
int recv_buf_len=0;

char *send_buf=NULL;
int send_buf_len=0;

char *xml_buf=NULL;
int xml_buf_len=0;


void free_xml_buf(void)
{
  char *p=xml_buf;
  xml_buf_len=0;
  xml_buf=NULL;
  mfree(p);
}

void free_recv_buf(void)
{
  char *p=recv_buf;
  recv_buf_len=0;
  recv_buf=NULL;
  mfree(p);
}

int receive_mode;
void get_answer(void)
{
  char buf[1024];
  int j;
  j=recv(sock,buf,sizeof(buf),0);
  if (j>0)
  {
    ALLTOTALRECEIVED+=j;
    if (receive_mode)
    {
      xml_buf=realloc(xml_buf, xml_buf_len+j+1);
      xml_buf[xml_buf_len+j]=0;
      memcpy(xml_buf+xml_buf_len, buf, j);
      xml_buf_len+=j;
    }
    else
    {
      char *end_answer;
      recv_buf=realloc(recv_buf, recv_buf_len+j+1);
      recv_buf[recv_buf_len+j]=0;
      memcpy(recv_buf+recv_buf_len, buf, j);
      recv_buf_len+=j;
      if (!(end_answer=strstr(recv_buf, "\r\n\r\n"))) return;
      receive_mode=1; //Остальное транслируем напрямую
      end_answer+=4;
      j=recv_buf_len-(end_answer-recv_buf);
      free_xml_buf();
      if (!j) return; //Нет данных, нечего посылать
      xml_buf=malloc(j+1);
      xml_buf[j]=0;
      memcpy(xml_buf, end_answer, j);
      xml_buf_len=j;
      free_recv_buf();
    }
  }
}

void free_send_buf(void)
{
  char *p=send_buf;
  send_buf_len=0;
  send_buf=NULL;
  mfree(p);
#ifdef SEND_TIMER
  GBS_DelTimer(&send_tmr);
#endif
}

#ifdef SEND_TIMER
static void resend(void)
{
  void send_answer(char *buf, int len);
  SUBPROC((void*)send_answer,0,0);
}
#endif

void send_answer(char *buf, int len)
{
  int i, j;
  char *p;
  if (buf)
  {
    if (send_buf)
    {
      send_buf=realloc(send_buf,send_buf_len+len);
      memcpy(send_buf+send_buf_len, buf, len);
      send_buf_len+=len;
      mfree(buf);
      return;
    }
    send_buf=buf;
    send_buf_len=len;
  }
  while((i=send_buf_len)!=0)
  {
    if (i>0x400) i=0x400;
    j=send(sock,send_buf,i,0);
    if (j<0)
    {
      j=*socklasterr();
      if ((j==0xC9)||(j==0xD6))
      {
	return; //Видимо, надо ждать сообщения ENIP_BUFFER_FREE
      }
      else
      {
	return;
      }
    }
    ALLTOTALSENDED+=j;
    send_buf_len-=j;
    memcpy(send_buf,send_buf+j,send_buf_len); //Удалили переданное
    if (j<i)
    {
      //Передали меньше чем заказывали
#ifdef SEND_TIMER
      GBS_StartTimerProc(&send_tmr,TMR_SECOND(5),resend);
#endif
      return; //Ждем сообщения ENIP_BUFFER_FREE1
    }
  }
  p=send_buf;
  send_buf=NULL;
  mfree(p);
}

static void end_socket(void)
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

static void free_buffers(void)
{
  free_recv_buf();
  free_send_buf();
}

static void free_socket(void)
{
  sock=-1;
  connect_state=0;
  free_buffers();
  REDRAW();
}

void send_req(void)
{
  char *p;
  char *s, *d;
  int c, len;
  char host[64], get_path[64];
  char req_buf[256];
  if ((s=strchr(RSS_URL, ':')))
  {
    if (*(s+1)=='/' && *(s+2)=='/') {
      s+=3;
    } else {
      s=0;
    }
  }
  if (!s) s=(char *)RSS_URL;
  d=host;
  while((c=*s))
  {
    if (c=='/') break;
    *d++=c;
    s++;
  }
  *d=0;

  d=get_path;
  while((c=*s))
  {
    if (c==':') break;
    *d++=c;
    s++;
  }
  *d=0;

  snprintf(req_buf,255,"GET %s"
          " HTTP/1.0\r\nHost: %s\r\n\r\n", get_path, host);
  p=malloc((len=strlen(req_buf))+1);
  strcpy(p, req_buf);
  send_answer(p, len);
}

char *html_decode(char *s)
{
  int l=strlen(s);
  int c;
  char *d, *dest;
  d=dest=malloc(l+1);
  while((c=*s++))
  {
    if (c=='<')
    {
      if (!strncmp(s,"br>",3))
      {
        *d++=13;
        s+=3;
        continue;
      }
      if (!strncmp(s,"br />",5))
      {
        *d++=13;
        s+=5;
        continue;
      }
      if (!strncmp(s,"/td>",4))
      {
        *d++=' ';
        s+=4;
        continue;
      }
      if (!strncmp(s,"p>",2))
      {
        *d++=13;
        s+=2;
        continue;
      }
    }
    if (c=='&')
    {
      if (!strncmp(s,"quote;",6))
      {
        *d++='\"';
        s+=6;
        continue;
      }
      if (!strncmp(s,"nbsp;",5))
      {
        *d++=' ';
        s+=5;
        continue;
      }
      if (!strncmp(s,"lt;",3))
      {
        *d++='<';
        s+=3;
        continue;
      }
      if (!strncmp(s,"gt;",3))
      {
        *d++='>';
        s+=3;
        continue;
      }
      if (!strncmp(s,"amp;",4))
      {
        *d++='&';
        s+=4;
        continue;
      }
      if (!strncmp(s,"copy;",5))
      {
        *d++='(';
        *d++='c';
        *d++=')';
        s+=5;
        continue;
      }
    }
    *d++=c;
  }
  *d=0;
  return dest;
}



void DecodeRSS()
{
  XMLNode *root, *rss, *channel, *items, *item;
  FreeRssItems();
  root=XMLDecode(xml_buf, xml_buf_len);
  if (root)
  {
    rss=root;
    while(rss)
    {
      if (!strcmp(rss->name, "rss")) break;
      rss=rss->next;
    }
    if (rss)
    {
      channel=rss->subnode;
      while(channel)
      {
        if (!strcmp(channel->name, "channel")) break;
        channel=channel->next;
      }
      if (channel)
      {
        items=channel->subnode;
        while(items)
        {
          if (!strcmp(items->name, "item"))
          {
            char *desc=0, *title=0;
            item=items->subnode;
            RSS_ITEM *p=malloc(sizeof(RSS_ITEM));
            zeromem(p, sizeof(RSS_ITEM));
            while(item)
            {
              if (!strcmp(item->name, "title")) title=item->value;
              if (!strcmp(item->name, "description")) desc=item->value;
              item=item->next;
            }
            if (title)
            {
              p->title=malloc(strlen(title)+1);
              strcpy(p->title, title);
            }
            if (desc)
            {
              p->description=html_decode(desc);
            }
            if (!rss_first)
            {
              rss_last=rss_first=p;
            }
            else
            {
              rss_last->next=p;
              rss_last=p;
            }
          }
          items=items->next;
        }
      }
    }
    DestroyTree(root);
  }
}


static void OnRedraw(MAIN_GUI *data)
{
  int scr_w=ScreenW();
  int scr_h=ScreenH();
  unsigned long RX=ALLTOTALRECEIVED; unsigned long TX=ALLTOTALSENDED;			//by BoBa 10.07
  int n=0;
  RSS_ITEM *p=(RSS_ITEM *)&rss_first;
  while((p=p->next)) n++;
  DrawRoundedFrame(0,YDISP,ScreenW()-1,ScreenH()-1,0,0,0,GetPaletteAdrByColorIndex(4),GetPaletteAdrByColorIndex(1));
  wsprintf(data->ws1,
           "State: %d\n"
             "Rx: %db, Tx: %db\n"
               "Queue: %db\n"
                 "%t\n"
                   "Total items: %d",connect_state,RX,TX,send_buf_len,logmsg,n);
  DrawString(data->ws1,3,3+YDISP,scr_w-4,scr_h-4-GetFontYSIZE(FONT_MEDIUM_BOLD),
             FONT_SMALL,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));


}

static void onCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->gui.state=1;
}

static void onClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  data->gui.state=0;
  FreeWS(data->ws1);
}

static void onFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  data->gui.state=2;
  DisableIDLETMR();
}

static void onUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

static int OnKey(MAIN_GUI *data, GUI_MSG *msg)
{
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case '1':
      break;

    case GREEN_BUTTON:
      DNR_TRIES=3;
      do_start_connection();
      REDRAW();
      break;

    case LEFT_SOFT:
      if (rss_first) create_rssitems_menu();
      break;

    case RIGHT_SOFT:
      return (1);
    }

  }
  return(0);
}

extern void kill_data(void *p, void (*func_p)(void *));

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
  (void *)kill_data,
  (void *)method8,
  (void *)method9,
  0
};

static void maincsm_oncreate(CSM_RAM *data)
{
  static const RECT Canvas={0,0,0,0};
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  patch_rect((RECT*)&Canvas,0,YDISP,ScreenW()-1,ScreenH()-1);
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
}

#pragma segment="ELFBEGIN"
void ElfKiller(void)
{
  kill_data(__segment_begin("ELFBEGIN"),(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *csm)
{
  SUBPROC((void *)free_socket);
  SUBPROC((void *)free_xml_buf);
  FreeRssItems();
  GBS_DelTimer(&mytmr);
  SUBPROC((void *)ElfKiller);
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if ((msg->msg==MSG_GUI_DESTROYED)&&((int)msg->data0==csm->gui_id))
  {
    csm->csm.state=-3;
  }
  if (msg->msg==MSG_RECONFIGURE_REQ)
  {
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"NRSS config updated!");
      InitConfig();
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
      is_gprs_online=1;
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
      switch((int)msg->data0)
      {
      //Если наш сокет
      case ENIP_SOCK_CONNECTED:
        //Если посылали запрос
        free_buffers();
        if (connect_state==1)
        {
          receive_mode=0;
          connect_state=2;
          SUBPROC((void *)send_req);
        }
        break;

      case ENIP_SOCK_DATA_READ:
        //Если посылали send
        if (connect_state>=2)
        {
          SUBPROC((void *)get_answer);
        }
        break;

      case ENIP_BUFFER_FREE:
      case ENIP_BUFFER_FREE1:
	//Досылаем очередь
	SUBPROC((void *)send_answer,0,0);
	break;

      case ENIP_SOCK_REMOTE_CLOSED:
        //Закрыт со стороны сервера
        sprintf(logmsg,"Remote closed!");
        goto ENIP_SOCK_CLOSED_ALL;

      case ENIP_SOCK_CLOSED:
        //Закрыт вызовом closesocket
        sprintf(logmsg,"Local closed!");
      ENIP_SOCK_CLOSED_ALL:
	switch(connect_state)
	{
	case -1:
	  connect_state=0;
          DecodeRSS();
	  SUBPROC((void*)free_socket);	
	  break;
	case 0:
	  break;
	default:
	  connect_state=-1;
	  SUBPROC((void*)end_socket);
	  break;
	}
        break;
      }
    }

  }
  return(1);
}

static unsigned short maincsm_name_body[140];
static const int minus11=-11;

static const struct
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"NRSS");
}


int main(char *exename, char *fname)
{
  MAIN_CSM main_csm;
  InitConfig();
  LockSched();
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  UnlockSched();
  return 0;
}
