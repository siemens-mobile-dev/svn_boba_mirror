#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "TextInfo.h"

#define TMR_SECOND 216
#define ELF_ID 0x3EE

CSM_DESC icsmd;

WSHDR *ws;

typedef struct{
  int enabled;
  RECT rc;
  WSHDR *ws;
  char pen[4];
  unsigned short font;
  unsigned short type;
} TInfo;

TInfo InfoData[7];
 
int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

GBSTMR mytmr;


// ----------------------------------------------------------------------------
void InitInfoData(void);
void TimerProc(void)
{
  InitInfoData();
  GBS_SendMessage(MMI_CEPID,ELF_ID);
  GBS_StartTimerProc(&mytmr,REFRESH*TMR_SECOND/10,TimerProc);
}

#pragma inline
int get_string_width(WSHDR *ws, int font)
{
  int width=0;
  unsigned short *body=ws->wsbody;
  int len=body[0];
  while(len)
  {
    width+=GetSymbolWidth(body[len],font);
    len--;
  }
  return (width);
}

void FillInfoData(TInfo *Info,int x_start,int y_start, int font,const char *color)
{  
  Info->rc.x=x_start;
  Info->rc.y=y_start;
  Info->rc.x2=x_start+get_string_width(Info->ws,font);
  Info->rc.y2=y_start+GetFontYSIZE(font);
  Info->font=font;
  memcpy(Info->pen,color,4);
}
  
int wsprintf_bytes(WSHDR *ws, unsigned int bytes)
{
	//char *str;
	if (bytes<=1024)
		return (wsprintf(ws,BYTES_FMT,bytes,BYTES_SG));
	 //str=BYTES_SG;
	else 
	{
		bytes>>=10;
		return (wsprintf(ws,BYTES_FMT,bytes,KBYTES_SG));
		//str=KBYTES_SG;
	}  
}

void InitInfoData(void)
{
  RAMNET *net_data;
  int c;
  
  if(NET_ENA)
  {
    InfoData[0].enabled=1;
    net_data=RamNet();
    c=(net_data->ch_number>=255)?'=':'-';
    wsprintf(InfoData[0].ws,NET_FMT,c,net_data->power);
    FillInfoData(&InfoData[0],NET_X,NET_Y,NET_FONT,NET_COLORS);
  }
  else
  {
    InfoData[0].enabled=0;
  }
  
  if (TEMP_ENA)
  {
    InfoData[1].enabled=1;
    c=GetAkku(1,3)-0xAAA+15;
    wsprintf(InfoData[1].ws,TEMP_FMT,c/10,c%10);
    FillInfoData(&InfoData[1],TEMP_X,TEMP_Y,TEMP_FONT,TEMP_COLORS);
  }
  else
  {
    InfoData[1].enabled=0;
  }
  
  if (VOLT_ENA)
  {
    InfoData[2].enabled=1;
    c=GetAkku(0,9);
    wsprintf(InfoData[2].ws,VOLT_FMT,c/1000,(c%1000)/10);
    FillInfoData(&InfoData[2],VOLT_X,VOLT_Y,VOLT_FONT,VOLT_COLORS);
  }
  else
  {
    InfoData[2].enabled=0;
  }
  
  if (CAP_ENA)
  {
    InfoData[3].enabled=1;
    c=*RamCap();
    wsprintf(InfoData[3].ws,CAP_FMT,c);
    FillInfoData(&InfoData[3],ACCU_X,ACCU_Y,ACCU_FONT,ACCU_COLORS);
  }
  else
  {
    InfoData[3].enabled=0;
  }
  
  if (CPU_ENA)
  {
    InfoData[4].enabled=1;
    c=GetCPULoad();
    wsprintf(InfoData[4].ws,CPU_FMT,c);
    FillInfoData(&InfoData[4],CPU_X,CPU_Y,CPU_FONT,CPU_COLORS);
  }
  else
  {
    InfoData[4].enabled=0;
  }

  if (GPRS_ENA)
  {
    InfoData[5].enabled=1;
    RefreshGPRSTraffic();
    c=*GetGPRSTrafficPointer();
    wsprintf_bytes(InfoData[5].ws,c);
    FillInfoData(&InfoData[5],GPRS_X,GPRS_Y,GPRS_FONT,GPRS_COLORS);
  }
  else
  {
    InfoData[5].enabled=0;
  }
  
  if (RAM_ENA)
  {
    InfoData[6].enabled=1;
    c=GetFreeRamAvail();
    wsprintf_bytes(InfoData[6].ws,c);
    FillInfoData(&InfoData[6],RAM_X,RAM_Y,RAM_FONT,RAM_COLORS);  
  }
  else
  {
    InfoData[6].enabled=0;
  }
}
  
// ----------------------------------------------------------------------------
#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])

#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}
#pragma inline
int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"TextInfo config updated!");
      InitConfig();
    }
  }
  csm_result=(msg->msg==ELF_ID)?0:old_icsm_onMessage(data,msg);
  if (IsGuiOnTop(idlegui_id)) //Если IdleGui на самом верху
  {
    GUI *igui=GetTopGUI();
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
        for (int i=0; i!=7; i++)
        {
          if (!InfoData[i].enabled)
			  continue;
          DrawCanvas(canvasdata, InfoData[i].rc.x, InfoData[i].rc.y, InfoData[i].rc.x2, InfoData[i].rc.y2, 1);
          DrawString(InfoData[i].ws, InfoData[i].rc.x, InfoData[i].rc.y, InfoData[i].rc.x2, InfoData[i].rc.y2, InfoData[i].font,
                     0,InfoData[i].pen, GetPaletteAdrByColorIndex(23));         
        }            
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
  for (int i=0;i!=7; i++)
  {
    FreeWS(InfoData[i].ws);
  }    
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

// ----------------------------------------------------------------------------

int main(void)
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
  InitConfig();
  for (int i=0;i<7; i++)
  {
    InfoData[i].ws=AllocWS(20);
  }    
  GBS_StartTimerProc(&mytmr,TMR_SECOND*10,TimerProc);
  return 0;
}
