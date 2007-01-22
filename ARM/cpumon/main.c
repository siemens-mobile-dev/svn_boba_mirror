#include "..\inc\swilib.h"
#include "conf_loader.h"

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])

#define MIN_WIDTH 30
#define MIN_HEIGHT 10
#define MIN_UPTIME 1
#define START_DELAY (20 * 216)

#define CLOCK_26     26
#define CLOCK_52     52
#define CLOCK_104    104

#define RGB16(R, G, B) ((B & 0x1F) | ((G & 0x3F) << 5) | ((R & 0x1F) << 11))

extern const unsigned int cfgX;
extern const unsigned int cfgY;

extern const unsigned int cfgW;
extern const unsigned int cfgH;

extern const int cfgShowIn;
extern const int cfgStTxt;
extern const unsigned int cfgUpTime;

extern const unsigned int cfgLoadR52;
extern const unsigned int cfgLoadG52;
extern const unsigned int cfgLoadB52;

extern const unsigned int cfgLoadR104;
extern const unsigned int cfgLoadG104;
extern const unsigned int cfgLoadB104;

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

unsigned char hhh;
char cop;

void RereadSettings()
{
    InitConfig();

  //========================
  uiUpdateTime = (262 * cfgUpTime) / 10;
  uiWidth  = (cfgW < MIN_WIDTH)  ? MIN_WIDTH  : cfgW;
  uiHeight = (cfgH < MIN_HEIGHT) ? MIN_HEIGHT : cfgH;

  img1_bmp = malloc(2 * uiWidth * uiHeight);
  zeromem(img1_bmp, 2 * uiWidth * uiHeight);

  img1.w = uiWidth;
  img1.h = uiHeight;
  img1.bpnum = 8;
//  img1.zero = 0;
  img1.bitmap = (char*) img1_bmp;

  loads = malloc(uiWidth);
  zeromem(loads, uiWidth);

  clocks = malloc(uiWidth);
  zeromem(clocks, uiWidth);
}

void FreeMem()
{
  mfree(img1_bmp);
  mfree(loads);
  mfree(clocks);
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

void TimerProc(void)
{
  if (!getCpuUsedTime_if_ena())
  {
    StartCpuUsageCount();
  }
  GBS_SendMessage(MMI_CEPID, 0xDEAD, GetCPULoad());
  GBS_StartTimerProc(&mytmr, (uiUpdateTime > MIN_UPTIME) ? uiUpdateTime : MIN_UPTIME, TimerProc);
}


unsigned int ClocksColour(unsigned int clock)
{
  switch (clock)
  {
  case CLOCK_26:
    return (/*RGB16(cfgLoadR, cfgLoadG, cfgLoadB)*/ 0xE000);
  case CLOCK_52:
    return (RGB16(cfgLoadR52, cfgLoadG52, cfgLoadB52));
  case CLOCK_104:
    return (RGB16(cfgLoadR104, cfgLoadG104, cfgLoadB104));
  default:
    return (/*RGB16(0, 0, 0)*/ 0xFFFF);
  }
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  unsigned char fShow;

  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    ShowMSG(1,(int)"Config updated!");
    FreeMem();
    RereadSettings();
  }
  
	//Накапливаем значения
  if (msg->msg == 0xDEAD)
  {
    LockSched();
    loads[hhh]  = uiHeight * msg->submess / 100;
#ifdef ELKA
    clocks[hhh]=104;
#else
    clocks[hhh] = GetCPUClock()/* / 26*/;
#endif
    UnlockSched();
    hhh++;
    if (hhh >= uiWidth)
    {
      hhh = 0;
      cop = 1;
    }
    csm_result = 0;
  }
  else
    csm_result = old_icsm_onMessage(data, msg); //Вызываем старый обработчик событий

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
        DrawCanvas(canvasdata, cfgX, cfgY, cfgX + uiWidth, cfgY + uiHeight, 1);
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
        DrwImg((IMGHDR *)&img1, cfgX, cfgY, GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(1));
        if (!cop && cfgStTxt)
          DrawString(ws1, 0, cfgY-(YFSIZE+3), ScreenW()-1, ScreenH()-1, SMALL_FONT, 0x20,
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
  mfree(loads);
  mfree(clocks);
  mfree(img1_bmp);
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
  GBS_StartTimerProc(&mytmr,START_DELAY,TimerProc);
  return 0;
}
