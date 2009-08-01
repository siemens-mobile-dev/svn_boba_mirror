#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "cpumon.h"
#include "render.h"

#define idlegui_id(icsm) (((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4])

#define MIN_WIDTH 30
#define MIN_HEIGHT 10
#define MIN_UPTIME 1
// #define START_DELAY (20 * 216)

#define MAX(a,b) (a)>(b)?(a):(b)

extern const RECT position;
extern const int cfgShowIn;
extern const int cfgStTxt;
extern const unsigned int cfgUpTime;
extern int const cfgLoadType;
extern TSensor cpu_sensor;
extern int const cfgRamType;
extern TSensor ram_sensor;
extern int const cfgNetType;
extern TSensor net_sensor;
extern int const cfgDiskType;
extern TSensor disk_sensor;

unsigned int uiWidth, uiHeight, uiUpdateTime, hhh;

TSensor Sensors[4];
int nSensors;

static IMGHDR img1;
unsigned short* img1_bmp;

static GBSTMR mytmr;
static WSHDR *ws1;
static int cop;

static const char ipc_my_name[]="SysMon";
#define IPC_UPDATE_STAT 1

#pragma inline=forced
static int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}

static int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

#pragma segment="ELFBEGIN"
void ElfKiller(void)
{
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(__segment_begin("ELFBEGIN"),(void (*)(void *))mfree_adr());
}

static void RereadSettings()
{
  unsigned int oldWidth=uiWidth,oldHeight=uiHeight;
  int oldcpu=cfgLoadType,oldram=cfgRamType,oldnet=cfgNetType;

  InitConfig();
  
  uiUpdateTime = MAX(((262*cfgUpTime)/ 10),MIN_UPTIME);
  uiWidth  = MAX(position.x2-position.x+1,MIN_WIDTH);
  uiHeight = MAX(position.y2-position.y+1,MIN_HEIGHT);
  
  if ((oldWidth!=uiWidth)||(oldHeight!=uiHeight)){
    mfree(img1_bmp);
    img1_bmp = malloc(4 * uiWidth * uiHeight);
//    zeromem(img1_bmp, 2 * uiWidth * uiHeight);
//    memset(img1_bmp,0xaaaa,2 * uiWidth * uiHeight);
    img1.w = uiWidth;
    img1.h = uiHeight;
    img1.bpnum = 8;
    img1.bitmap = (char*) img1_bmp;
  }
  
  nSensors=0;
  if (cfgLoadType){
    cpu_sensor.init(oldWidth!=uiWidth);
    Sensors[nSensors++]=cpu_sensor;
  }else if (oldcpu) cpu_sensor.deinit();

  if (cfgRamType){
    ram_sensor.init(oldWidth!=uiWidth);
    Sensors[nSensors++]=ram_sensor;
  }else if (oldram) ram_sensor.deinit();

  if (cfgNetType){
    net_sensor.init(oldWidth!=uiWidth);
    Sensors[nSensors++]=net_sensor;
  }else if (oldnet) net_sensor.deinit();

  if (cfgDiskType){
    disk_sensor.init(0);
    Sensors[nSensors++]=disk_sensor;
  };
}

static void FreeMem()
{
  for(int n=0; n<nSensors; n++)
    Sensors[n].deinit();
  mfree(img1_bmp);
  img1_bmp=0;
}

static DrwImg(IMGHDR *img, int x, int y, char *pen, char *brush)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc, x, y, img->w, img->h);
  SetPropTo_Obj5(&drwobj, &rc, 0, img);
  SetColor(&drwobj, pen, brush);
  DrawObject(&drwobj);
}

const IPC_REQ gipc={
  ipc_my_name,
  ipc_my_name,
  NULL
};

static void TimerProc(void)
{
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&gipc);
}

static int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg)
{
  int fShow;
  void *icsm;

  if(msg->msg == MSG_RECONFIGURE_REQ){
    if (strcmp_nocase((char*)successed_config_filename,(char *)msg->data0)==0){
      ShowMSG(1,(int)"SysMon config updated!");
//      FreeMem();
      RereadSettings();
    }
  }
  if (msg->msg==MSG_IPC){
    IPC_REQ *ipc;
    if ((ipc=(IPC_REQ*)msg->data0)){
      if (strcmp_nocase(ipc->name_to,ipc_my_name)==0){
        switch (msg->submess){
          case IPC_UPDATE_STAT:
            if (ipc->name_from==ipc_my_name){
              //Накапливаем значения
              for(int n=0; n<nSensors; n++)
                Sensors[n].tick(hhh);
              hhh++;
              if (hhh >= uiWidth){
                hhh = 0;
                cop = 1;
              }
              GBS_StartTimerProc(&mytmr, uiUpdateTime, TimerProc);
            }
        }
      }
    }
  }
  switch (cfgShowIn){
    case 0:
      fShow = !IsUnlocked();
      break;
    case 1:
      fShow = IsUnlocked();
      break;
    default:
      fShow = 1;
      break;
  }
  if (fShow){
  icsm=FindCSMbyID(CSM_root()->idle_id);
  if (icsm){
    if (IsGuiOnTop(idlegui_id(icsm))){ //Если IdleGui на самом верху
      GUI *igui = GetTopGUI();
      if (igui){ //И он существует
#ifdef ELKA
        {
          void *canvasdata = BuildCanvas();
#else
        void *idata = GetDataOfItemByID(igui, 2);
        if (idata){
          void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
#endif
          //рисуем нашу требуху
          DrawCanvas(canvasdata, position.x, position.y, position.x2, position.y2, 1);
          render();
          DrwImg((IMGHDR *)&img1, position.x, position.y, GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(1));
          if (!cop && cfgStTxt)
            DrawString(ws1, 0, position.y-(GetFontYSIZE(FONT_SMALL)+3), ScreenW()-1, ScreenH()-1, FONT_SMALL, 0x20,
                       GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(1));
        }
      }
    }
  }
  }
  return(1);
}

static void maincsm_oncreate(CSM_RAM *data)
{
  ws1=AllocWS(100);
  wsprintf(ws1,"%t","SysMon (C)BoBa,Rst7");
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&gipc);
}

static void maincsm_onclose(CSM_RAM *csm)
{
  GBS_DelTimer(&mytmr);
  FreeMem();
  FreeWS(ws1);
  SUBPROC((void *)ElfKiller);
}

static unsigned short maincsm_name_body[140];
const int minus11=-11;

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
  sizeof(CSM_RAM),
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"SysMon");
}

int main(void)
{
  CSMROOT *csmr;
  CSM_RAM *save_cmpc;
  CSM_RAM main_csm;
  RereadSettings();
  UpdateCSMname();
  LockSched();
  csmr=CSM_root();
  save_cmpc=csmr->csm_q->current_msg_processing_csm;
  csmr->csm_q->current_msg_processing_csm=csmr->csm_q->csm.first;
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  csmr->csm_q->current_msg_processing_csm=save_cmpc;
  UnlockSched();
  return (0);
}
