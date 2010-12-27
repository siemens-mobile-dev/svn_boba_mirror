#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "string_works.h"
#include "WeatherD.h"
#include "CFontWork.h"

#define BUFFSIZE 0x200
//=============================================================================

WSHDR *ews;
int sock;
int connect_state;
char *buf=0;
int pbuf;
int scr_w;
int scr_h;

static IMGHDR TextIMG;
unsigned* TextIMG_bmp;

void create_connect(void);
void do_start_connection(void)
{
  SUBPROC((void *)create_connect);
}

void StartGPRS(void)
{
  GPRS_OnOff(1,1);
  GBS_StartTimerProc(&reconnect_tmr,216*30,do_start_connection);
}

void create_connect(void)
{
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

char req_buf[100];
void send_req(void){

  RAMNET * ramnet=RamNet();
  char *p=((char*)ramnet)-11;
  char cc1=*p;
  char cc2=*(p+1);
  char nc=*(p+2);

  snprintf(req_buf,99, "GET /w/w.php?mcc=%i&mnc=%i&lac=%i&ci=%i"
    " HTTP/1.0\r\nHost: igps.boba.su\r\n\r\n",
    (cc1&0x0F)*100+(cc1>>4)*10+(cc2&0x0F),
    (nc&0x0F)*10+(nc>>4),
    ramnet[0].lac,
    ramnet[0].ci
  );  
 
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
//    utf82win(sss,(const char *)sss);
    ascii2ws(ews, sss);
    for(int i = 0; i < TextIMG.h*TextIMG.w; i++)
      ((unsigned*)TextIMG.bitmap)[i] = 0;
    unsigned fc, bc;
    fc = ((((unsigned)FONT_COLOR[3])*255/100)<<24) | (((unsigned)FONT_COLOR[0]) << 16) | (((unsigned)FONT_COLOR[1]) << 8) | ((unsigned)FONT_COLOR[2]);
    bc = ((((unsigned)BORDER_COLOR[3])*255/100)<<24) | (((unsigned)BORDER_COLOR[0]) << 16) | (((unsigned)BORDER_COLOR[1]) << 8) | ((unsigned)BORDER_COLOR[2]);
    RenderString(&TextIMG, 0, 0, ews, 1, fc, bc);
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
    if ((!buf)||(!pbuf)) return; 
    if (!strstr(buf,"200 OK")) return;
    
    int vmid;
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
    vmid = valuemid(tempmin,tempmax);
    snprintf(weath.Temp, 16, "%c%d \xB0\x43\n", (vmid>0)?'+':' ', vmid);

    //Давление
    char *pressmin=findtag(buf,"PRESSMIN:");
    char *pressmax=findtag(buf,"PRESSMAX:");
    snprintf(weath.Pressure, 16, "%d мм\n", valuemid(pressmin,pressmax));    
    
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
    snprintf(weath.Rewlet, 16, "%d %%\n", valuemid(rewletmin,rewletmax));        

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
}

//==============================================================================

const int minus11=-11;

typedef struct
{
  CSM_RAM csm;
}MAIN_CSM;

extern void kill_data(void *p, void (*func_p)(void *));

void CountTime(void){
  TDate date;
  TTime time;
  unsigned int min_before_update;

  GetDateTime(&date, &time);
  int h=(time.hour*60+time.min)-GetTimeZoneShift(&date, &time, RamDateTimeSettings()->timeZone)+180;

  if (h >= 1230) //20.30 мск
      min_before_update = 1590-h+10; //10 минут запаса
   else
  if (h < 150) //2.30 мск
      min_before_update = 150-h+10; //10 минут запаса
   else  
  if (h >= 870) //14.30 мск
      min_before_update = 1230-h+10; //10 минут запаса
   else
  if (h >= 510) //8.30 мск
      min_before_update = 870-h+10; //10 минут запаса
   else  
  if (h >= 150) //2.30 мск
      min_before_update = 510-h+10; //10 минут запаса

  GBS_DelTimer(&update_tmr);
  GBS_StartTimerProc(&update_tmr, (216*60)*min_before_update, do_start_connection); 
}
//==============================================================================

static DrwImg(IMGHDR *img, int x, int y, const char *pen, const char *brush)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc, x, y, img->w, img->h);
  SetPropTo_Obj5(&drwobj, &rc, 0, img);
  SetColor(&drwobj, pen, brush);
  DrawObject(&drwobj);
}
//==============================================================================

int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg)
{
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      InitConfig();
      UnloadFont();
      LoadFont(FONT_FILE);  

      int uiWidth  = scr_w - DATA_X + 1;
      int uiHeight = scr_h - DATA_Y + 1;

      mfree(TextIMG_bmp);
      TextIMG_bmp = malloc(4 * uiWidth * uiHeight);
      TextIMG.w = uiWidth;
      TextIMG.h = uiHeight;
      TextIMG.bpnum = 10;
      TextIMG.bitmap = (char*) TextIMG_bmp;
    
      GenerateString();
      ShowMSG(1,(int)"WeatherCID config updated!");
    }
  }
    #define idlegui_id (((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4])
    CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);    
    if (icsm&&IsGuiOnTop(idlegui_id)&&!IsScreenSaver()/*&&IsUnlocked()*/) //Если IdleGui на самом верху
    {
      GUI *igui=GetTopGUI();
      if (igui) //И он существует
      {
#ifdef ELKA
   {
     void *canvasdata=BuildCanvas();
#else
      void *idata=GetDataOfItemByID(igui,2);
      if (idata)
      {
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
          DrwImg(&TextIMG, DATA_X, DATA_Y, FONT_COLOR, BORDER_COLOR);
        
//        DrawString(ews, DATA_X, DATA_Y ,scr_w, scr_h,
//	         FONT_SIZE,0x20,FONT_COLOR,BORDER_COLOR);
      }
   }}    
  
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
        SUBPROC((void *)Parsing);
        CountTime();
        old_gprs_state[1] = 0;
        connect_state=0;
        sock=-1;
        break;
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

  int uiWidth  = scr_w - DATA_X + 1;
  int uiHeight = scr_h - DATA_Y + 1;

  mfree(TextIMG_bmp);
  TextIMG_bmp = malloc(4 * uiWidth * uiHeight);
  TextIMG.w = uiWidth;
  TextIMG.h = uiHeight;
  TextIMG.bpnum = 10;
  TextIMG.bitmap = (char*) TextIMG_bmp;
  for(int i = 0; i < TextIMG.h*TextIMG.w; i++)
  ((unsigned*)TextIMG.bitmap)[i] = 0;

  LoadFont(FONT_FILE);
  do_start_connection();
}

static void Killer(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *csm)
{
  mfree(TextIMG_bmp);
  UnloadFont();
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
