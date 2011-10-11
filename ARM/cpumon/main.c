#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "cpumon.h"
#include "render.h"

#define idlegui_id(icsm) (((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4])

#define MIN_WIDTH 30
#define MIN_HEIGHT 10
#define MIN_UPTIME 1

extern const RECT position;
extern const int cfgShowIn;
extern const int cfgStTxt;
extern const int cfgRender;
extern const int cfgRenderBit;
extern const int cfgUpTime;
extern int const cfgLoadType;
extern TSensor cpu_sensor;
extern int const cfgRamType;
extern TSensor ram_sensor;
extern int const cfgNetType;
extern TSensor net_sensor;
extern int const cfgDiskType;
extern TSensor disk_sensor;
extern int const cfgGPRSType;
extern TSensor GPRS_sensor;
extern int const cfgCapaType;
extern TSensor Capa_sensor;
extern int const cfgTempType;
extern TSensor Temp_sensor;

int uiWidth, uiHeight, uiBits, uiUpdateTime;

TSensor *Sensors[NSENSORS];
int nSensors;
#define NTEXSENSORS 2
TSensor *TexSensors[NTEXSENSORS];
int nTexSensors;

IMGHDR img1;
char* img1_bmp;

static GBSTMR mytmr;
static const char ipc_my_name[]="SysMon";
#define IPC_UPDATE_STAT 392305998

#pragma segment="ELFBEGIN"
void ElfKiller(void)
{
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(__segment_begin("ELFBEGIN"),(void (*)(void *))mfree_adr());
}

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

static int RenderBit2Byte[3]={1,2,4};
static int RenderBit2bpnum[3]={5,8,10};

static void RereadSettings(){
  int oldWidth=uiWidth,oldHeight=uiHeight,oldBits=uiBits;
  int oldcpu=cfgLoadType,oldram=cfgRamType,oldnet=cfgNetType,oldgprs=cfgGPRSType;

  InitConfig();
  
  uiUpdateTime = MAX(((262*cfgUpTime)/ 10),MIN_UPTIME);
  uiWidth  = MAX(position.x2-position.x+1,MIN_WIDTH);
  uiHeight = MAX(position.y2-position.y+1,MIN_HEIGHT);

  if (cfgRender)
    uiBits=1;
   else
    uiBits=cfgRenderBit;

  int renew=(oldWidth!=uiWidth)||(oldBits!=uiBits);

  if (renew||(oldHeight!=uiHeight)){
 
    
    mfree(img1_bmp);
    img1_bmp = malloc(uiWidth * uiHeight * RenderBit2Byte[uiBits]);
    img1.w = uiWidth;
    img1.h = uiHeight;
    img1.bpnum = RenderBit2bpnum[uiBits];
    img1.bitmap = img1_bmp;
  }
  
  nSensors=0;
  nTexSensors=0;
  if (cfgLoadType){
    cpu_sensor.init(renew);
    Sensors[nSensors++]=&cpu_sensor;
  }else if (oldcpu) cpu_sensor.deinit();

  if (cfgRamType){
    ram_sensor.init(renew);
    Sensors[nSensors++]=&ram_sensor;
  }else if (oldram) ram_sensor.deinit();

  if (cfgNetType){
    net_sensor.init(renew);
    Sensors[nSensors++]=&net_sensor;
  }else if (oldnet) net_sensor.deinit();

  if (cfgGPRSType){
    GPRS_sensor.init(renew);
    Sensors[nSensors++]=&GPRS_sensor;
  }else if (oldgprs) GPRS_sensor.deinit();

  if (cfgDiskType){
    disk_sensor.init(0);
    Sensors[nSensors++]=&disk_sensor;
  };

  if (cfgCapaType){
    Capa_sensor.init(0);
    TexSensors[nTexSensors++]=&Capa_sensor;
  };
  if (cfgTempType){
    Temp_sensor.init(0);
    TexSensors[nTexSensors++]=&Temp_sensor;
  };
}

static void FreeMem(){
  for(int n=0; n<nSensors; n++)
    Sensors[n]->deinit();
  for(int n=0; n<nTexSensors; n++)
    TexSensors[n]->deinit();
  mfree(img1_bmp);
  img1_bmp=0;
}

const IPC_REQ gipc={
  ipc_my_name,
  ipc_my_name,
  NULL
};

static void TimerProc(void){
  GBS_StartTimerProc(&mytmr, uiUpdateTime, TimerProc);
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&gipc);
}

static int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg){
  int fShow;
  void *icsm;
  if(msg->msg == MSG_RECONFIGURE_REQ){
    if (strcmp_nocase((char*)successed_config_filename,(char *)msg->data0)==0){
      ShowMSG(1,(int)"SysMon config updated!");
      RereadSettings();
    }
  }else
  if (msg->msg==MSG_IPC){
/*    IPC_REQ *ipc;
    if ((ipc=(IPC_REQ*)msg->data0)){
      if (strcmp_nocase(ipc->name_to,ipc_my_name)==0){
        switch (msg->submess){
          case IPC_UPDATE_STAT:
            if (ipc->name_from==ipc_my_name){*/
    if(msg->submess==IPC_UPDATE_STAT){
      //Накапливаем значения
      for(int n=0; n<nSensors; n++){
        static int divs[NSENSORS]={1,1,1,1,1,1};
        if(--divs[n]==0){
          Sensors[n]->tick();
          divs[n]=Sensors[n]->div;
        }
      }
      for(int n=0; n<nTexSensors; n++){
        static int texdivs[NTEXSENSORS]={1,1};
        if(--texdivs[n]==0){
          TexSensors[n]->tick();
          texdivs[n]=TexSensors[n]->div;
        }
      }
    }
/*        }
      }
    }*/
  }else{
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
        if(igui){ //И он существует
#ifdef ELKA
            {
            void *canvasdata = BuildCanvas();
#else
          void *idata = GetDataOfItemByID(igui, 2);
          if (idata){
            void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
#endif
            DrawCanvas(canvasdata, position.x, position.y, position.x2, position.y2, 1);
            render();
          }
        }
      }
    }
  }}
  return(1);
}

static void maincsm_oncreate(CSM_RAM *data){
  TimerProc();
}

static void maincsm_onclose(CSM_RAM *csm){
  GBS_DelTimer(&mytmr);
  FreeMem();
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
