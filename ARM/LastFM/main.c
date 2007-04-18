#include "..\inc\swilib.h"
#include "conf_loader.h"

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
extern unsigned long  strtoul (const char *nptr,char **endptr,int base);

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

WSHDR *ws1;

extern const unsigned int cfgX;
extern const unsigned int cfgY;

extern const unsigned int cfgW;
extern const unsigned int cfgH;
extern const char CSMADR[];
unsigned int uiWidth, uiHeight;

unsigned int playercsmid=0;
void *playercsmadr=NULL;

GBSTMR mytmr;
int showstate=0;
volatile int readytochangestate=0;

const char msg_pa[]="Player active!";
const char percent_t[]="%t";

void RereadSettings()
{
  InitConfig();
  playercsmadr=(void *)strtoul(CSMADR,NULL,16);
  //========================
  uiWidth  = cfgW-1;
  uiHeight = cfgH-1;
}

void tmrproc_readychange(void)
{
  readytochangestate=1;
  GBS_StartTimerProc(&mytmr,3*216,tmrproc_readychange);
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"LastFMD config updated!");
      RereadSettings();
    }
  }
  if (playercsmid)
  {
    //Проверяем, есть ли плейер
    CSM_RAM *csmp=FindCSMbyID(playercsmid);
    if (csmp)
    {
      WSHDR *fn=((WSHDR **)csmp)[0x2C/4];
      if (!fn) goto L_ACTIVE;
      if (!fn->wsbody) goto L_ACTIVE;
      if (fn->wsbody[0])
      {
	if (readytochangestate)
	{
	  readytochangestate=0;
	  if (showstate)
	  {
#ifdef ELKA
	    WSHDR **wws=((WSHDR ***)csmp)[(0x18C/4)]-1;
#else
	    WSHDR **wws=((WSHDR ***)csmp)[(0x170/4)]-1;
#endif
	    if (wws)
	    {
	      WSHDR *tws;
	      while(1)
	      {
		if (showstate>4) goto ZEROSTATE;
		if (!(tws=wws[showstate++])) continue;
		if (!tws->wsbody) continue;
		if (tws->wsbody[0]) break;
	      }
	      wstrcpy(ws1,tws);
	      {
		//Патчим строку на предмет win1251
		int i=1;
		int c;
		while(i<=ws1->wsbody[0])
		{
		  c=ws1->wsbody[i];
		  if (c==0xA8) c=0x401;
		  if (c==0xAA) c=0x404;
		  if (c==0xAF) c=0x407;
		  if (c==0xB8) c=0x451;
		  if (c==0xBA) c=0x454;
		  if (c==0xBF) c=0x457;
		  if ((c>=0xC0)&&(c<0x100)) c+=0x350;
		  ws1->wsbody[i++]=c;
		}
	      }
	    }
	    else
	      goto ZEROSTATE;
	  }
	  else
	  {
	  ZEROSTATE:
	    showstate=0;
	    wstrcpy(ws1,fn);
	    showstate++;
	  }
	}
      }
      else
      {
      L_ACTIVE:
	wsprintf(ws1,percent_t,msg_pa);
	showstate=0;
      }
    }
    else
    {
      playercsmid=0;
      wsprintf(ws1,"Player not active");
    }
  }
  else
  {
    CSM_RAM *p=((void **)CSM_root()->csm_q)[2];
    while(p)
    {
      if (p->constr==playercsmadr) break;
      p=p->next;
    }
    if (p)
    {
      playercsmid=p->id;
      wsprintf(ws1,percent_t,msg_pa);
      showstate=0;
    }
  }
  csm_result = old_icsm_onMessage(data, msg); //Вызываем старый обработчик событий
  if (IsGuiOnTop(idlegui_id)) //Если IdleGui на самом верху
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
	  DrawString(ws1, cfgX, cfgY, cfgX + uiWidth, cfgY + uiHeight, SMALL_FONT, 0x20,
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
  ws1=AllocWS(128);
  wsprintf(ws1,"%t","LastFMD(C)Rst7");
  GBS_StartTimerProc(&mytmr,3*216,tmrproc_readychange);
  return 0;
}
