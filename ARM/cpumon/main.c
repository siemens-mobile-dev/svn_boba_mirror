#include "..\inc\swilib.h"
#include "conf_loader.h"


#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])

#define MIN_WIDTH 30
#define MIN_HEIGHT 10
#define MIN_UPTIME 1
#define START_DELAY (20 * 216)

#define MAX(a,b) (a)>(b)?(a):(b)

#define CLOCK_26     26
#define CLOCK_52     52
#define CLOCK_104    104
#define CLOCK_208    208

#define RGB16(R, G, B) (((B>>3)&0x1F) | ((G<<3)&0x7E0) | ((R<<8)&0xF800))

extern const RECT position;

extern const int cfgShowIn;
extern const int cfgStTxt;
extern const unsigned int cfgUpTime;

extern const char cfgLoad104[4];
extern const char cfgLoad52[4];

unsigned int uiUpdateTime, uiWidth, uiHeight;

unsigned short* img1_bmp;

IMGHDR img1;

unsigned char* loads;
unsigned char* clocks;

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

GBSTMR mytmr;

WSHDR *ws1;

unsigned int hhh;
int cop;


const char ipc_my_name[]="CpuMon";
#define IPC_UPDATE_STAT 1

#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}

int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

void RereadSettings()
{
  InitConfig();
  
 //========================
  uiUpdateTime = MAX(((262*cfgUpTime)/ 10),MIN_UPTIME);
  uiWidth  = MAX(position.x2-position.x+1,MIN_WIDTH);
  uiHeight = MAX(position.y2-position.y+1,MIN_HEIGHT);
  
  img1_bmp = malloc(2 * uiWidth * uiHeight);
  zeromem(img1_bmp, 2 * uiWidth * uiHeight);
  img1.w = uiWidth;
  img1.h = uiHeight;
  img1.bpnum = 8;
  img1.bitmap = (char*) img1_bmp;

  loads = malloc(uiWidth);
  zeromem(loads, uiWidth);

  clocks = malloc(uiWidth);
  zeromem(clocks, uiWidth);
}

void FreeMem()
{
  mfree(img1_bmp);
  img1_bmp=0;
  mfree(loads);
  loads=0;
  mfree(clocks);
  clocks=0;
}

DrwImg(IMGHDR *img, int x, int y, char *pen, char *brush)
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

void TimerProc(void)
{
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&gipc);
}


unsigned int ClocksColour(unsigned int clock)
{
  switch (clock)
  {
  case CLOCK_26:
  case CLOCK_52:
    return (RGB16(cfgLoad52[0],cfgLoad52[1],cfgLoad52[2]));
  case CLOCK_104:
  case CLOCK_208:
    return (RGB16(cfgLoad104[0], cfgLoad104[1], cfgLoad104[2]));
  default:
    return (/*RGB16(0, 0, 0)*/ 0xFFFF);
  }
}



int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result=0;
  unsigned char fShow;

  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    if (strcmp_nocase((char*)successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"CPUMon config updated!");
      FreeMem();
      RereadSettings();
    }
  }
  if (msg->msg==MSG_IPC)
  {
    IPC_REQ *ipc;
    if ((ipc=(IPC_REQ*)msg->data0))
    {
      if (strcmp_nocase(ipc->name_to,ipc_my_name)==0)
      {
        switch (msg->submess)
        {
        case IPC_UPDATE_STAT:
          if (ipc->name_from==ipc_my_name) 
          {
            //Накапливаем значения
#ifdef NEWSGOLD
            if (!getCpuUsedTime_if_ena())
            {
              StartCpuUsageCount();
            }
#endif
            loads[hhh]  = uiHeight * GetCPULoad() / 100;
            clocks[hhh] = GetCPUClock()/* / 26*/;
            hhh++;
            if (hhh >= uiWidth)
            {
              hhh = 0;
              cop = 1;
            }
            csm_result = 1;
            GBS_StartTimerProc(&mytmr, uiUpdateTime, TimerProc);
          }
        }
      }
    }
  }
  if (!csm_result)  csm_result=old_icsm_onMessage(data, msg); //Вызываем старый обработчик событий

  switch (cfgShowIn)
  {
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

  if (IsGuiOnTop(idlegui_id) && fShow) //Если IdleGui на самом верху
  {
    GUI *igui = GetTopGUI();
    if (igui) //И он существует
    {
#ifdef ELKA
      {
        void *canvasdata = BuildCanvas();
#else
      void *idata = GetDataOfItemByID(igui, 2);
      if (idata)
      {
        void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
#endif
        DrawCanvas(canvasdata, position.x, position.y, position.x2, position.y2, 1);
        //рисуем нашу требуху
        int h = hhh;
        for(unsigned int x = 0; x < uiWidth; x++)
        {
          for(unsigned int y = 0; y < uiHeight; y++)
          {
            if (y < loads[h])
              img1_bmp[x + (uiHeight - 1 - y) * uiWidth] = ClocksColour(clocks[h]);
            else
              img1_bmp[x + (uiHeight - 1 - y) * uiWidth] = 0xE000; //RGB16(21, 42, 21);
          }
          h++;
          if (h >= uiWidth)
            h = 0;
        }
        DrwImg((IMGHDR *)&img1, position.x, position.y, GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(1));
        if (!cop && cfgStTxt)
          DrawString(ws1, 0, position.y-(GetFontYSIZE(FONT_SMALL)+3), ScreenW()-1, ScreenH()-1, FONT_SMALL, 0x20,
                     GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(1));
      }
    }
  }
  return(csm_result);
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  GBS_DelTimer(&mytmr);
  FreeMem();
  FreeWS(ws1);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  RereadSettings();
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onMessage=MyIDLECSM_onMessage;
  icsmd.onClose=MyIDLECSM_onClose;
  icsm->constr=&icsmd;
  UnlockSched();
  ws1=AllocWS(100);
  wsprintf(ws1,"%t","CPUMon (C)BoBa,Rst7");
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&gipc);
  return 0;
}
