#include "..\inc\swilib.h"


extern long  strtol (const char *nptr,char **endptr,int base);
extern unsigned long  strtoul (const char *nptr,char **endptr,int base);

extern const char MAIL_LOGIN[];
extern const char MAIL_PASS[];
extern const char HIST_PATH[];
extern const int LOAD_ONLY;
extern const char IP_ADRESS[];
void InitConfig();
// ------------------------------- Defines ----------------------------------------
#define TMR_SECOND 216
#define EOP -10
char tempfile[128];
const char mailer_db_name[]="mails.db";

#define T_SEND_LOGIN 1
#define T_SEND_PASS 2
#define T_GET_STAT 3
#define T_CREATE_STRUCT 4
#define T_GET_LIST 5
#define T_GET_UIDL_LIST 6
#define T_PROCESS_UIDL 7
#define T_PROCESS_MES 8
#define T_RECEIVE_MAIL 9
#define T_DELETE_MES 10
// -------------------------- Global Variables -------------------------------------

typedef struct
{
  unsigned short type;
  unsigned short is_readed;
  int size;
  char uid[40];
}MAIL_HIST;

#define FULL_MES 0   // normal mes
#define UNFULL_MES 1   // unfull mes
#define MES_DOWN 2   // mes for download
#define MES_DEL 3   // mes for delete

int file_exist=0;
MAIL_HIST* mail_hist_prev;
MAIL_HIST* mail_hist_cur;
const int minus11=-11;
char filename[128];
void ed1_locret(void){}
char errstr[80];
char* status[11]=
{
  "Disconnected",
  "Verificating Login",
  "Verificating Pass",
  "Getting Stat",
  "Creating Struct",
  "Loading List",
  "Loading UIDL List",
  "Process UID",
  "Ask for mail",
  "Receiving mail",
  "Deleting message"
};


char* log_buf;

int size_prev_hist;
int number_cur;
int inpop3;
int total_size;
int current_mes=0;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;

int reading_for_data;

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

char *eml_buf;
int pbuf=0;
int vbuf=0;

GBSTMR reconnect_tmr;
int connect_state=0;
// 1 - sock connected
// 2 - connect accepted, sending login
// 3 - login accepted, sending pass
// 4 - pass accepted, retrieve stats
// 5 - stats recieved, sending request for 1 mail
// 6 - recieved mail accepted 
// 7 - receiving mail

int RXstate=EOP; //-sizeof(RXpkt)..-1 - receive header, 0..RXpkt.data_len - receive data
int sock=-1;
volatile int is_gprs_online=1;


typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  int i;
}HISTORY;

unsigned short maincsm_name_body[140];
int if_rec=0;
// --------------------------- Constant Strings ------------------------------------
const char percent_t[]="%t";
const char empty_str[]="";
const char eol[]="\r\n";
// --------------------------- Global Functions ------------------------------------
extern void kill_data(void *p, void (*func_p)(void *));
void ShowErrMsg(char * msg)
{
  LockSched();
  ShowMSG(1,(int)msg);
  UnlockSched();
}

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

unsigned int win2unicode(int letter)
{
  if (letter<168) goto L_ENG;
  if (letter==168) goto L_BIG_YO;
  letter-=168;
  if (letter<24) goto L_UKR;
  if (letter>87) goto L_ENG;
//L_RUS:
  letter-=8;
  goto L_ADD_400;
L_UKR:
  switch(letter)
  {
  case 0x02:
    letter=0x04;
    break;
  case 10:
    letter=6;
    break;
  case 11:
    letter=0x56;
    break;
  case 16:
    letter=0x51;
    break;
  case 18:
    letter=0x54;
    break;
  case 23:
    letter=0x57;
    break;
  default:
    goto L_ENG;
  }
  goto L_ADD_400;
L_BIG_YO:
  letter=0x01;
L_ADD_400:
  return (letter+0x400);
  
L_ENG: return (letter);
}


void ascii2ws(WSHDR *ws, const char *s)
{
  char c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,win2unicode(c));
  }
}

// ---------------------------------- Mail List ------------------------------------

// --------------------------------- Socket work -----------------------------------
int fill_sock_addr(SOCK_ADDR* sa)
{
  char str[32];
  char original[32];
  int i=0;
  int A;
  int B;
  int C;
  int D;
  int P;
  char* next;
  strcpy(original,IP_ADRESS);
  for (;original[i]!=0;i++)
  {
    if (original[i]<='9'&&original[i]>='0')
    {
      str[i]=original[i];
    }
    else
    {
      if (str[i-1])   str[i]=0;
      
    }
  }
  str[i]=0;
  A=strtoul(str,&next,10);
  B=strtoul(next+1,&next,10);
  C=strtoul(next+1,&next,10);
  D=strtoul(next+1,&next,10);
  P=strtoul(next+1,&next,10);
  sa->ip=htonl(IP_ADDR(A,B,C,D));
  sa->port=htons(P);
  sa->family=1;
  return 1;
  
}


void create_connect(void)
{
  void do_reconnect(void);
  SOCK_ADDR sa;
  //Устанавливаем соединение
  current_mes=0;
  connect_state=0;
  GBS_DelTimer(&reconnect_tmr);
  sock=socket(1,1,0);
  if (sock!=-1)
  {
    if (fill_sock_addr(&sa))
    {
      if (connect(sock,&sa,sizeof(sa))!=-1)
      {
        connect_state=1;
        REDRAW();
      }
      else
      {
        closesocket(sock);
        sock=-1;
        ShowErrMsg("Can't connect!");
        GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
      }
    }
  }
  else
  {
    ShowErrMsg("Can't create socket, GPRS restarted!");
    //Не осилили создания сокета, закрываем GPRS-сессию
    //GPRS_OnOff(0,1);
  }
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
    sock=-1;
  }
}

void add_to_log(char* log)
{
  if (!if_rec)
  {
    int len=strlen(log);
    vbuf=pbuf;
    strcpy(log_buf+pbuf,log);
    pbuf+=len;
  }
}
  
void send_get_mes(int i);

int resp_ok(char*buf)
{
  if (strncmp(buf,"+OK",3)) return -1;
  return (0);
}

int get_values(char* str)
{
  char* c;
  c=strchr(str,' ');
  inpop3=strtoul(c+1,&c,10);
  total_size=strtoul(c+1,0,10);
  return inpop3;
}


void send_str(char* str,int p);
void create_struct()
{
  int len=sizeof(MAIL_HIST)*inpop3;
  mail_hist_cur=malloc(len);
  zeromem(mail_hist_cur,len);
}


void write_letter(char* str) 
{
  unsigned int ul;
  int f;
  int len;
  char fname[128];
  snprintf(fname,sizeof(fname),"%s%s.eml",HIST_PATH,mail_hist_cur[current_mes].uid);
  if(!str)
  {
    f=fopen(fname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul);
    fclose(f,&ul);
  }
  else
  {
    if ((f=fopen(fname,A_ReadWrite+A_Append,P_READ+P_WRITE,&ul))!=-1)
    {
      len=strlen(str);
      fwrite(f,str,len,&ul);
      fclose(f,&ul);
    }
  }
}

void open_prev_hist()
{
  char hist_name[128];
  int f;
  int size;
  unsigned int er;
  snprintf(hist_name,sizeof(hist_name),"%s%s",HIST_PATH,mailer_db_name);
  f=fopen(hist_name,A_ReadOnly,P_READ,&er);
  if (f==-1)
  {
    file_exist=0;
    return;
  }
  else
  {
    if((size=lseek(f,0,S_END,&er,&er))!=0)
    {
      size_prev_hist=size;
      file_exist=1;
      lseek(f,0,S_SET,&er,&er);
      mail_hist_prev=malloc(size);
      fread(f,mail_hist_prev,size,&er);
    }
    else
    {
      file_exist=0;
      size=-1;
    }
    fclose(f,&er);
  }
}



int get_action_prev(int n)
{
  n=mail_hist_prev[n].type;
  return n;
}

int get_action_cur(int n)
{
  n=mail_hist_cur[n].type;
  return n;
}

void set_current_action(int n,int cur_item)
{
  mail_hist_cur[cur_item].type=n;
}

int get_prev_state(int n)
{
  return mail_hist_cur[n].is_readed;
}

void set_current_state(int state,int cur_item)
{
  mail_hist_cur[cur_item].is_readed=state;
}
  



void write_current_hist()
{
  int f;
  char hist_name[128];
  unsigned int er;
  snprintf(hist_name,sizeof(hist_name),"%s%s",HIST_PATH,mailer_db_name);
  f=fopen(hist_name,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&er);
  if (f!=-1)
  {
    for (int i=0;i!=inpop3;i++)
    {
      if (get_action_cur(i)!=0xFFFF)
      {
        fwrite(f,&mail_hist_cur[i],sizeof(MAIL_HIST),&er);
      }
    }
    fclose(f,&er);
  }   
}


int find_uidl_in_hist(int cur_mes)
{
  int i;
  i=size_prev_hist/(sizeof(MAIL_HIST));
  for (int d=0; d!=i;d++)
  {
    if (!strcmp(mail_hist_cur[cur_mes].uid,mail_hist_prev[d].uid)) return d;
  }
  return (-1);
}



void set_state_for_delete(int i)
{
  mail_hist_cur[i].type=0xFFFF;
}

void fill_struct_list(char*str,MAIL_HIST* mail_hist)
{
  char*str1;
  int num;
  int size;
  num=strtoul(str,&str1,10);
  size=strtoul(str1+1,0,10); 
  mail_hist[num-1].size=size;
}

void fill_struct_uidl(char*str,MAIL_HIST* mail_hist)
{
  char*str1;
  char*str2;
  int num;
  str1=strchr(str,' ');
  str2=strstr(str,eol);
  num=strtoul(str,0,10);
  strncpy(mail_hist[num-1].uid,str1+1,str2-str1-1);
}

  
void send_del_mes(int i);
void send_get_headers(int i);
void send_pass();
void send_login();
void get_stat();
void get_uidl(int i);
void send_quit();
void get_list(void);
void get_uidl_list();
  
  
void process_line(char*mes)
{
  switch(connect_state)
  {
  case T_SEND_LOGIN:
    if(resp_ok(mes)) goto L_ERR;
    send_login();
    connect_state=T_SEND_PASS;
    break;
  case T_SEND_PASS:
    if(resp_ok(mes)) goto L_ERR;
    send_pass();
    connect_state=T_GET_STAT;
    break;
  case T_GET_STAT:
    if(resp_ok(mes)) goto L_ERR;
    inpop3=0;
    get_stat();
    connect_state=T_CREATE_STRUCT;
    break;
  case T_CREATE_STRUCT:
    if(resp_ok(mes)) goto L_ERR;
    number_cur=get_values(mes);
    open_prev_hist();
    create_struct();
    if (current_mes>=inpop3) goto L_MES_RECEIVED;
  case T_GET_LIST:
    connect_state=T_GET_UIDL_LIST;
    get_list();
    break;   
  case T_GET_UIDL_LIST:
    if (!if_rec)
    {
      if(resp_ok(mes)) goto L_ERR_FREE;
      else 
      {
        if_rec=1;
        break;
      }
    }
    else
    {
      if (strcmp(mes,".\r\n"))
      {
        fill_struct_list(mes,mail_hist_cur);
        break;
      }
      else
      {
        connect_state=T_PROCESS_UIDL;
        if_rec=0;
        get_uidl_list();
        break;
      }
    }
  case T_PROCESS_UIDL:
    if (!if_rec)
    {
      if(resp_ok(mes)) goto L_ERR_FREE;
      else 
      {
        if_rec=1;
        break;
      }
    }
    else
    {
      if (strcmp(mes,".\r\n"))
      {
        fill_struct_uidl(mes,mail_hist_cur);
        break;
      }
      else
      {
        connect_state=T_PROCESS_MES;
        if_rec=0;
        goto L_PROCESS_MES;
      }
    }    
    
    
  case T_PROCESS_MES:
  L_PROCESS_MES:
    connect_state=T_PROCESS_MES;
    REDRAW();
    if (current_mes>=inpop3) goto L_MES_RECEIVED;
    if (file_exist)
    {
      int i;
      if ((i=find_uidl_in_hist(current_mes))!=-1)
      {
        switch(get_action_prev(i))
        {
        case FULL_MES:
          set_current_action(FULL_MES,current_mes);
          if (get_prev_state(i)) set_current_state(1,current_mes);
          goto L_NEXT_MES;
        case UNFULL_MES:
          set_current_action(UNFULL_MES,current_mes);
          goto L_NEXT_MES;
        case MES_DOWN:
          connect_state=T_RECEIVE_MAIL;
          send_get_mes(current_mes);
          goto L_REDRAW;
        case MES_DEL:
          connect_state=T_DELETE_MES;
          send_del_mes(current_mes);
          goto L_REDRAW;
        }
      }
    }
    if (LOAD_ONLY)
    {
      send_get_mes(current_mes);
      set_current_action(FULL_MES,current_mes);
    }
    else
    {
      send_get_headers(current_mes);
      set_current_action(UNFULL_MES,current_mes);
    }
    connect_state=T_RECEIVE_MAIL;
    break;
    
  case T_RECEIVE_MAIL:
    if (if_rec)
    {
      if (strcmp(mes,".\r\n"))
      {
        write_letter(mes);
      }
      else
      {
      L_NEXT_MES:
        if_rec=0;
        current_mes++;
        goto L_PROCESS_MES;
      }
    }
    else
    {
      if(resp_ok(mes)) goto L_ERR_FREE;
      write_letter(0);
      if_rec=1;
    }
    break;
  case T_DELETE_MES:
    if(resp_ok(mes)) goto L_ERR_FREE;
    set_state_for_delete(current_mes);
    goto L_NEXT_MES;
    
  L_MES_RECEIVED:
    connect_state=0;    
    file_exist=0;
    write_current_hist();
  L_FREE:
    mfree(mail_hist_prev);
    mfree(mail_hist_cur);
  L_END:
    send_quit();
    end_socket();
  }
  
L_REDRAW:
  REDRAW();
  return;
  
L_ERR:
  connect_state=-1;
  strncpy(errstr,mes,sizeof(errstr));
  goto L_END;
L_ERR_FREE:
  connect_state=-1;
  strncpy(errstr,mes,sizeof(errstr));
  goto L_FREE;  
}

char *recived_line;


#define BUF_SIZE 1024
void get_answer(void)
{
  char msg_buf[BUF_SIZE];
  char *s=msg_buf;
  char *d; //Куда
  int c;
  int i;
  i=recv(sock,msg_buf,BUF_SIZE-1,0);
  if (i>0)
  {
    d=recived_line;
    if (d)
    {
      //Уже была строка
      d=recived_line=realloc(recived_line,strlen(d)+i+1); //Возможно, мы добавим весь буфер и \0
      d+=strlen(d);
    }
    else
    {
      //Заново
     L_NEWLINE:
       d=recived_line=malloc(i+1); //Возможно, это будет целый буфер и \0
    }
    while(i)
    {
      i--;
      c=*d++=*s++; //Копируем символ
      if (c=='\n')
      {
        //Конец строки
        *d=0;
        add_to_log(recived_line);
        process_line(recived_line);
        mfree(recived_line);
        goto L_NEWLINE;
      }
    }
    *d=0; //Временно закончили строку
  }
}


void send_str(char* str,int p)
{
  char buf[128];
  int len;
  snprintf(buf,sizeof(buf),str,p);
  add_to_log(buf);
  len=strlen(buf);
  send(sock,buf,len,0);
}
    
    
void send_login(void)
{
  send_str("USER %s\r\n",(int)MAIL_LOGIN);    
}

void send_pass(void)
{
  send_str("PASS %s\r\n",(int)MAIL_PASS);    
}

void get_stat(void)
{
  send_str("STAT\r\n",NULL);
}

void get_list(void)
{
  send_str("LIST\r\n",NULL);
}

void get_uidl_list(void)
{
  send_str("UIDL\r\n",NULL);
}
           
void send_get_mes(int i)
{
  send_str("RETR %u\r\n",i+1);
}

void send_get_headers(int i)
{
  send_str("TOP %u 0\r\n",i+1);
}


void send_del_mes(int i)
{
  char string[80];
  unsigned int err;
  sprintf(string,"%s%s.eml",HIST_PATH,mail_hist_cur[i].uid);
  unlink(string,&err);
  send_str("DELE %u\r\n",i+1);
}

void get_uidl(int i)
{
  send_str("UIDL %u\r\n",i+1);  
}
void send_quit(void)
{
  send_str("QUIT\r\n",NULL);
}
// --------------------------- Creating GUI -------------------------------

void OnRedraw(MAIN_GUI *data)
{ 
  int x=ScreenW();
  int y=ScreenH();
  if(connect_state==-1)
  {
    ascii2ws(data->ws2,errstr);
  }
  else
  {
  ascii2ws(data->ws2,status[connect_state]);
  }
  DrawRoundedFrame(0,0,x-1,y-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(20));
  wsprintf(data->ws1,"State: %d, RXstate: %d\n%w",connect_state,current_mes,data->ws2);
  DrawString(data->ws1,3,3,x-4,y-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  
  if (inpop3>=0)
  {
    if (inpop3>0)
    {
      wsprintf(data->ws2,"Message %d recived...\nTotal mail size %db",inpop3,total_size);
    }
    else
    {
      wsprintf(data->ws2,"POP3 connect!");
    }
    DrawString(data->ws2,3,50,x-4,y-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  }
  wsprintf(data->ws2,percent_t,"Exit");
  DrawString(data->ws2,(x>>2)*3,y-4-14,x-4,y-4,MIDDLE_FONT,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
}


void OnCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  data->gui.state=1;
}

void OnClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void OnFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void OnUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
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
    case LEFT_SOFT:
//      if (cltop) create_maillist_menu();
      //      if (cltop) remake_clmenu();
      break;
    case RIGHT_SOFT:
      return(1); //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI
    case GREEN_BUTTON:
      if ((connect_state==0)&&(sock==-1))
      {
        GBS_DelTimer(&reconnect_tmr);
        SUBPROC((void *)create_connect);
      }
      break;
    case '0':
      GPRS_OnOff(0,1);
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


int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)OnRedraw,	//Redraw
  (void *)OnCreate,	//Create
  (void *)OnClose,	//Close
  (void *)OnFocus,	//Focus
  (void *)OnUnfocus,	//Unfocus
  (void *)OnKey,	//OnKey
  0,
  (void *)kill_data, //method7,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

RECT Canvas={0,0,0,0};

// ------------------------  Creating CSM -------------------------------- //
void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  Canvas.x2=ScreenW()-1;
  Canvas.y2=ScreenH()-1;
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
  log_buf=malloc(0x4000);
  zeromem(log_buf,0x4000);
  inpop3=-1;
  //  MutexCreate(&contactlist_mtx);
 // SUBPROC((void *)create_connect);
}


void maincsm_onclose(CSM_RAM *csm)
{
//  GBS_DelTimer(&tmr_dorecv);
  int f;
  unsigned int ul;
  if ((f=fopen(tempfile,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=-1)
  {
    fwrite(f,log_buf,pbuf,&ul);
    fclose(f,&ul);
  }
  GBS_DelTimer(&reconnect_tmr);
  mfree(log_buf);
  if (recived_line)
  {
    mfree(recived_line); 
    recived_line=NULL;
  }
  //  MutexDestroy(&contactlist_mtx);
  SUBPROC((void *)send_quit);
  SUBPROC((void *)end_socket);
  SUBPROC((void *)ElfKiller);

}

void do_reconnect(void)
{
  if (is_gprs_online)
  {
    SUBPROC((void*)create_connect);
  }
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
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
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
      return(1);
    }
    if ((int)msg->data1==sock)
    {
      //Если наш сокет
      if ((((unsigned int)msg->data0)>>28)==0xA)
      {
        //Принят пакет
        //ProcessPacket((TPKT *)msg->data0);
        return(0);
      }
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
        if (connect_state==1)
        {
          //Соединение установленно, посылаем пакет login
          add_to_log("Try to login...\r\n");
          //SUBPROC((void *)send_login);
          REDRAW();
        }
        else
        {
          ShowMSG(1,(int)"Illegal message ENIP_SOCK_CONNECTED!");
        }
        break;
      case ENIP_SOCK_DATA_READ:
        if (connect_state>=1)
        {
          //Если посылали send
          SUBPROC((void *)get_answer);
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
        connect_state=0;
        sock=-1;
        REDRAW();
        GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"Mail Daemon");
}

int main(char *exename, char *fname)
{
  TDate d;
  TTime t;
  GetDateTime(&d,&t);
  char dummy[sizeof(MAIN_CSM)];
  snprintf(tempfile,sizeof(tempfile),"%s%d_%d_%d_%d_%d_%d.log",HIST_PATH,d.year,d.month,d.day,t.hour,t.min,t.param);
  InitConfig();
  LockSched();
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;  
}



