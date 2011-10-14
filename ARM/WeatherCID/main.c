#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "string_works.h"
#include "WeatherD.h"

#define BUFFSIZE 0x200
#define MINTIMEOUT 1
#define NEGTIMEOUT 15
#define DEFTIMEOUT 24*60

//=============================================================================

WSHDR *ews;
int sock;
int connect_state;
char *buf=0;
int pbuf;
int scr_w;
int scr_h;
int ncell=0;
int oldlac=0;
int mnc;
int mcc;
int lac;
int ci;

void log_data(char *data){
  int hFile;
  unsigned int io_error = 0;
  char fullname[256];
  
  TTime time;
  TDate date;
  GetDateTime(&date, &time);
  
  sprintf(fullname, "%s%i%i%i%i%i", DATA_PATH, date.year, date.month, date.day, time.hour, time.min);
  hFile = fopen(fullname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE, &io_error);
  if(!io_error)
  {
    fwrite(hFile, data, strlen(data)-1, &io_error);
    fclose(hFile, &io_error);
  }  
}

//=============================================================================

void create_connect(void);

void do_start_connection(void){
  SUBPROC((void *)create_connect);
}

void CountTime(int m){
  TDate date;
  TTime time;
  unsigned int min_before_update;

  GetDateTime(&date, &time);
  int h=(time.hour*60+time.min)-GetTimeZoneShift(&date, &time, RamDateTimeSettings()->timeZone)+180;

  if (h >= 1230/*20.30 мск*/)
      min_before_update = 1590-h+10/*10 минут запаса*/;
   else
  if (h < 150/*2.30 мск*/)
      min_before_update = 150-h+10/*10 минут запаса*/;
   else  
  if (h >= 870/*14.30 мск*/)
      min_before_update = 1230-h+10/*10 минут запаса*/;
   else
  if (h >= 510/*8.30 мск*/)
      min_before_update = 870-h+10/*10 минут запаса*/;
   else  
  if (h >= 150/*2.30 мск*/)
      min_before_update = 510-h+10/*10 минут запаса*/;
  
  if (m>min_before_update){
     m=min_before_update;
     oldlac=0;
  }
   
  if (m<MINTIMEOUT) m=MINTIMEOUT;
  
  GBS_DelTimer(&update_tmr);
  GBS_StartTimerProc(&update_tmr, (216*60)*m, do_start_connection); 
}

void StartGPRS(void){
  GPRS_OnOff(1,1);
  GBS_StartTimerProc(&reconnect_tmr,216*30,do_start_connection);
}

RAMNET * ramnet;
void create_connect(void){
  ramnet=RamNet();
  char *p=((char*)ramnet)-11;
  char cc1=*p;
  char cc2=*(p+1);
  mcc=(cc1&0x0F)*100+(cc1>>4)*10+(cc2&0x0F);
  char nc=*(p+2);
  mnc=(nc&0x0F)*10+(nc>>4);
  do{
    if (ncell>6){
      ncell=0;
      CountTime(NEGTIMEOUT);
      return;
    }
    lac=ramnet[ncell++].lac;
  }while ((oldlac==lac)||(!lac));
  ci=ramnet[ncell-1].ci;

  SOCK_ADDR sa;
  //Устанавливаем соединение
  weath.Temp[0]=0;  
  connect_state=0;
  pbuf=0;

  GBS_DelTimer(&reconnect_tmr);
  
  if (!IsGPRSEnabled())
  {
    StartGPRS();
    if (!old_gprs_state[1])
    {
      old_gprs_state[0] = 0;
      old_gprs_state[1] = 1;
    }
    return;
  }
  else
  {
    if (!old_gprs_state[1])
    {
      old_gprs_state[0] = 1;
      old_gprs_state[1] = 1;
    }    
  }
  
  sock=socket(1,1,0);
  if (sock!=-1)
  {
    sa.family=1;
    sa.port=htons(80);
    sa.ip=htonl(IP_ADDR(78,24,218,208));
    if (connect(sock,&sa,sizeof(sa))!=-1)
    {
      connect_state=1;
      REDRAW();
    }
    else
    {
      closesocket(sock);
      sock=-1;
      GBS_StartTimerProc(&reconnect_tmr,216*10,do_start_connection);
    }
  }
  else
  {
    //Не осилили создания сокета, закрываем GPRS-сессию
    GPRS_OnOff(0,1);
    // и через 30 секунд запускаем жопорез
    GBS_StartTimerProc(&reconnect_tmr, 216*30, StartGPRS);    
  }
}

char req_buf[100];
void send_req(void){
  snprintf(req_buf,99, "GET /w/w.php?mcc=%i&mnc=%i&lac=%i&ci=%i&n=%i HTTP/1.0\r\nHost: igps.boba.su\r\n\r\n",
    mcc, mnc, lac, ci, ncell);  
  send(sock,req_buf,strlen(req_buf),0);
  connect_state=2;
  if (!buf) buf=malloc(BUFFSIZE);
}

void end_socket(void){
  if (sock>=0){
    shutdown(sock,2);
    closesocket(sock);
    buf[pbuf]=0;
    // выключаем жопорез, если он изначально был выключен
    if (!old_gprs_state[0]) GPRS_OnOff(0,1);
  }
}

void get_answer(void){
  int i=recv(sock,buf+pbuf,BUFFSIZE-1-pbuf,0);
  pbuf+=i;
  if ((i<0)||(pbuf>=BUFFSIZE-1)){
    end_socket();
  }
}

//==============================================================================

void GenerateString(){
    char sss[128];
    snprintf(sss, 127, "%s%s%s%s%s", 
                SHOW_TEMP     ? weath.Temp       : "", 
                SHOW_PRESSURE ? weath.Pressure   : "", 
                SHOW_WIND     ? weath.Wind       : "", 
                SHOW_REWLET   ? weath.Rewlet     : "",
                SHOW_CITY     ? weath.City       : ""
         );
    ascii2ws(ews, sss);
};

char *valuetag(char *src,char *dst, int maxlen){
 if (!src) return 0;
 if (!*src) return src;
 int c=0;
 dst=dst+strlen(dst);
 while (*src!='\n' && *src!='"' && *src!=0 && c < maxlen){
  *dst++=*src++;
  c++;
 }
 *dst=0;
 return src;
}

char * findtag(char *src, char *tag){
 char *s=strstr(src,tag);
 if (s)
   return s+strlen(tag);
 return 0;
}

int valuemid(char *min,char *max){
    char val[23];
    int vmin, vmax;
    *val = 0;
    valuetag(min, val, 22);
    vmin = atoi(val);
    *val = 0;
    valuetag(max, val, 22);
    vmax = atoi(val);
    return (vmax+vmin)>>1;
}

void Parsing(){
  oldlac=lac;
  if ((!buf)||(!pbuf)||(!strstr(buf,"HTTP/1.1 200 OK"))){
    do_start_connection();
    return;
  }
    
    //главная картинка
    char *tod=findtag(buf,"TOD:");
    if (*tod=='1'||*tod=='2')
        weath.daytime = 0;//strcat(weath.MainPic.path,"d.sun");
      else
        weath.daytime = 1;//strcat(weath.MainPic.path,"n.moon");
    char *cloudiness=findtag(buf,"CLOUD:");
    weath.cloudness = *cloudiness - 0x30;
    if(weath.cloudness>4) weath.cloudness = 0;

    weath.rain = weath.snow = 0;
    char *rpower=findtag(buf,"RPWR:");
    char *precipitation=findtag(buf,"PREC:");
    weath.storm = 0;
    switch(*precipitation)  
    {
      case '4':
        weath.rain = *rpower - 0x30 + 1;
        break;
      case '5':
        weath.rain = *rpower - 0x30 + 3;
        break;
      case '6':
        weath.snow = *rpower - 0x30 + 1;
        break;
      case '7':
        weath.snow = *rpower - 0x30 + 3;
        break;
      case '8':
        weath.storm = 1;
        break;
      default:
        break;
     }
     if(weath.rain>4) weath.rain = 0;
     if(weath.snow>4) weath.snow = 0;

    char *spower=findtag(buf,"SPWR:");
    if (*spower=='1'){
      weath.storm = 1;
    }

    //Город
    char *city=findtag(buf,"CITY:");
    weath.City[0]=0;
    valuetag(city, weath.City, 15);
    
    //Температура
    char *tempmin=findtag(buf,"TEMPMIN:");
    char *tempmax=findtag(buf,"TEMPMAX:");
    snprintf(weath.Temp, 16, "%+d\xB0\x43\n", valuemid(tempmin,tempmax));

    //Давление
    char *pressmin=findtag(buf,"PRESSMIN:");
    char *pressmax=findtag(buf,"PRESSMAX:");
    snprintf(weath.Pressure, 16, "%dмм\n", valuemid(pressmin,pressmax));    
    
    //Ветер
    char *windmin=findtag(buf,"WINDMIN:");
    char *windmax=findtag(buf,"WINDMAX:");
    weath.Wind[0]=0;
    valuetag(windmin, weath.Wind, 7);
    strcat(weath.Wind, "-");     
    valuetag(windmax, weath.Wind, 7);
    strcat(weath.Wind, "м/с\n");     
    strcpy(weath.WindPic.path,ICON_PATH);
    char *winddir=findtag(buf,"WINDDIR:");
    int wpl = strlen(weath.WindPic.path);
    weath.WindPic.path[wpl]=(*winddir)+1; weath.WindPic.path[wpl+1] = 0;
    strcat(weath.WindPic.path,"w.png");
    weath.WindPic.height=GetImgHeight((int)weath.WindPic.path);
    weath.WindPic.width=GetImgWidth((int)weath.WindPic.path);
    
    //Влажность
    char *rewletmin=findtag(buf,"WETMIN:");
    char *rewletmax=findtag(buf,"WETMAX:");
    snprintf(weath.Rewlet, 16, "%d%%\n", valuemid(rewletmin,rewletmax));        

    mfree(buf);
    buf=0;

    strcpy(weath.dt.path,ICON_PATH);
    strcat(weath.dt.path,weath.daytime?"moon.png":"sun.png");
    weath.dt.height=GetImgHeight((int)weath.dt.path);
    weath.dt.width=GetImgWidth((int)weath.dt.path);
    
    char tmp[3];
    
    tmp[0] = 'c'; tmp[1] = weath.cloudness + '0'; tmp[2] = 0;
    strcpy(weath.c.path,ICON_PATH);
    strcat(weath.c.path, tmp);
    strcat(weath.c.path, ".png");

    tmp[0] = 'r'; tmp[1] = weath.rain + '0';
    strcpy(weath.r.path,ICON_PATH);
    strcat(weath.r.path, tmp);
    strcat(weath.r.path, ".png");

    tmp[0] = 's'; tmp[1] = weath.snow + '0';
    strcpy(weath.s.path,ICON_PATH);
    strcat(weath.s.path, tmp);
    strcat(weath.s.path, ".png");

    strcpy(weath.st.path,ICON_PATH);
    strcat(weath.st.path, "st.png");
    
    GenerateString();
    CountTime(DEFTIMEOUT);
}

//==============================================================================

const int minus11=-11;

typedef struct
{
  CSM_RAM csm;
}MAIN_CSM;

extern void kill_data(void *p, void (*func_p)(void *));

int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg){
  if(msg->msg == MSG_RECONFIGURE_REQ){
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0){
      InitConfig();
      GenerateString();
      ShowMSG(1,(int)"WeatherCID config updated!");
    }
  }else
  if (msg->msg == MSG_HELPER_TRANSLATOR){
    if ((int)msg->data1==sock){
      //Если наш сокет
      switch((int)msg->data0){
        case ENIP_SOCK_CONNECTED:
          if (connect_state==1){
            //Если посылали запрос
            SUBPROC((void *)send_req);
          }else{
            ShowMSG(1,(int)"Illegal message ENIP_SOCK_CONNECTED!");
          }
          break;
        case ENIP_SOCK_DATA_READ:
          if (connect_state==2){
            //Если посылали send
            SUBPROC((void *)get_answer);
          }else{
            ShowMSG(1,(int)"Illegal message ENIP_DATA_READ");
          }
          break;
        case ENIP_SOCK_REMOTE_CLOSED:
          //Закрыт со стороны сервера
          if (connect_state) SUBPROC((void *)end_socket);
          break;
        case ENIP_SOCK_CLOSED:
          //Закрыт вызовом closesocket
          SUBPROC((void *)Parsing);
          old_gprs_state[1] = 0;
          connect_state=0;
          sock=-1;
          break;
      }
    }
  }else
  if(msg->msg != MSG_IPC){
    #define idlegui_id (((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4])
    CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);    
    if (icsm&&IsGuiOnTop(idlegui_id)&&!IsScreenSaver()/*&&IsUnlocked()*/){ //Если IdleGui на самом верху
      GUI *igui=GetTopGUI();
      if (igui){ //И он существует
#ifdef ELKA
        {
          void *canvasdata=BuildCanvas();
#else
        void *idata=GetDataOfItemByID(igui,2);
        if (idata){
          void *canvasdata=((void **)idata)[DISPLACE_OF_IDLECANVAS/4];
#endif        
          if (SHOW_PIC){
            DrawCanvas(canvasdata, PICT_X, PICT_Y, PICT_X + weath.dt.width, PICT_Y + weath.dt.height, 1);
            DrawImg(PICT_X, PICT_Y, (int)weath.dt.path);
            if(weath.cloudness) DrawImg(PICT_X, PICT_Y, (int)weath.c.path);
            if(weath.rain)      DrawImg(PICT_X, PICT_Y, (int)weath.r.path);
            if(weath.snow)      DrawImg(PICT_X, PICT_Y, (int)weath.s.path);
            if(weath.storm)     DrawImg(PICT_X, PICT_Y, (int)weath.st.path);
            DrawImg(PICT_X+weath.dt.width-weath.WindPic.width, PICT_Y, (int)weath.WindPic.path);
            //DrawImg(PICT_X+weath.dt.width-weath.WindPic.width, PICT_Y+weath.dt.height-weath.WindPic.height, (int)weath.WindPic.path);
          }
          DrawString(ews, TEXTRECT.x, TEXTRECT.y , TEXTRECT.x2, TEXTRECT.y2,
	             FONT_SIZE,TEXT_OUTLINE+TEXT_ALIGNMIDDLE,
                     GetPaletteAdrByColorIndex(PC_LIGHTTEXTFOREGROUND),
                     GetPaletteAdrByColorIndex(PC_LIGHTTEXTBACKGROUND));
        }
      }
    }
  }    
  return (1);
}

static void maincsm_oncreate(CSM_RAM *data)
{
  scr_w=ScreenW();
  scr_h=ScreenH();
  ews=AllocWS(128);
  do_start_connection();
}

static void Killer(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *csm)
{
  mfree(buf);
  FreeWS(ews);
  GBS_DelTimer(&update_tmr);
  GBS_DelTimer(&reconnect_tmr);
  SUBPROC((void *)Killer);
}

static unsigned short maincsm_name_body[140];

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

static void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"WeatherCID");
}

int main()
{
  InitConfig();
  CSM_RAM *save_cmpc;
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();  
  LockSched();
  save_cmpc=CSM_root()->csm_q->current_msg_processing_csm;
  CSM_root()->csm_q->current_msg_processing_csm=CSM_root()->csm_q->csm.first;
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  CSM_root()->csm_q->current_msg_processing_csm=save_cmpc;
  UnlockSched();

  return 0;
}
