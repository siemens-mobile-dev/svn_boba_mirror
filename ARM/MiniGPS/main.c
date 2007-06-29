#include "..\inc\swilib.h"
#include "conf_loader.h"

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
CSM_DESC icsmd;
void (*old_icsm_onClose)(CSM_RAM*);
int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);

extern const int cfgx;
extern const int cfgy;
extern const char cfgcolor[];
extern const char cfgcvcolor[];
extern const int cfgfont;
extern const int cfgtext_attribute;
extern const int cfgupdate_time;
extern const char gps_dir[];
extern const char drun[];

char filename[128];
WSHDR *corr_name;
GBSTMR mytmr;

RAMNET GetNetParams()
{
  RAMNET Result;
  zeromem(&Result, sizeof(RAMNET));
  Result = *RamNet();
  return Result;
}

void Savenew()
{
  unsigned int errcode;
  volatile int hFile = -1;
  RAMNET Res = GetNetParams();
  sprintf(filename,"%s%u\\%u-%u.tmo",gps_dir,Res.lac,Res.ci,Res.lac);
  hFile = fopen(filename,A_WriteOnly+A_Create,P_WRITE,&errcode);
  if(hFile==-1)
  {
    char *dir=malloc(128);
    sprintf(dir,"%s%u",gps_dir,Res.lac);
    mkdir(dir, &errcode);
    mfree(dir);
    hFile = fopen(filename,A_WriteOnly+A_Create,P_WRITE,&errcode);
  }
  WSHDR *ws0 = AllocWS(128);
  wsprintf(ws0,"%t %u-%u",drun,Res.ci,Res.lac);
  unsigned int len = wstrlen(ws0);
  fwrite(hFile,ws0->wsbody,len*2+2,&errcode);
  unsigned int crc = (int)ws0->wsbody[0];
  void *temp;
  char *arr;
  temp = ws0->wsbody;
  arr = temp;
  for(int i=2;i<(len+1)*2;i++)
  {
    crc = crc ^ arr[i];
  }
  fwrite(hFile,&crc,2,&errcode);
  FreeWS(ws0);
  fclose(hFile,&errcode);
}

void load_name()
{
  unsigned int errcode;
  volatile int hFile=-1;
  RAMNET Res=GetNetParams();
  sprintf(filename,"%s%u\\%u-%u.tmo",gps_dir,Res.lac,Res.ci,Res.lac);
  hFile = fopen(filename,A_ReadOnly,P_READ,&errcode);
  if(hFile==-1)
  {
    wsprintf(corr_name,"new: %u-%u",Res.ci,Res.lac);
    Savenew();
  }
  else
  {
    int str_len = 0;
    fread(hFile,&str_len,2,&errcode);
    WSHDR *tmostr=AllocWS(str_len);
    errcode=0;
    lseek(hFile,0,0,&errcode,&errcode);
    fread(hFile,tmostr->wsbody,(str_len+1)*2,&errcode);

    if(errcode)
    {
      wsprintf(corr_name,"%t %u","error ", errcode);
    }
    else
    {
      str_len=wstrlen(tmostr)-2;
      wstrcpybypos(corr_name,tmostr,3,str_len);
    }
    FreeWS(tmostr);
    fclose(hFile, &errcode);
  }
}

void tmrproc()
{
  load_name();
  GBS_StartTimerProc(&mytmr,262*2,tmrproc);
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
  GBS_StopTimer (&mytmr);
  GBS_DelTimer(&mytmr);
  FreeWS(corr_name);
}

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

int MyIDLECSM_onMessage(CSM_RAM* data, GBS_MSG* msg)
{
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"MiniGPS config updated!");
      InitConfig();
    }
  }
  
  int csm_result=old_icsm_onMessage(data, msg);
  
  if (IsGuiOnTop(idlegui_id))
  {
    GUI *igui=GetTopGUI();
    if (igui&&IsUnlocked())
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
            DrawCanvas(canvasdata,cfgx,cfgy,cfgx+Get_WS_width(corr_name,cfgfont),cfgy+GetFontYSIZE(cfgfont),1);
            //DrawString(corr_name,cfgx,cfgy,cfgx+Get_WS_width(corr_name,cfgfont),cfgy+GetFontYSIZE(cfgfont), //не пашут текст атрибуты :(
            DrawString(corr_name,cfgx,cfgy,ScreenW(),ScreenH(),// а так пашут :)
                       cfgfont,cfgtext_attribute,cfgcolor,cfgcvcolor);
          }
      }
  }
  return(csm_result);
}

int main()
{
  InitConfig();
  
  unsigned int err;  
  if(!isdir(gps_dir,&err)){
    mkdir(gps_dir,&err);
  }
  
  corr_name=AllocWS(128);
  GBS_StartTimerProc(&mytmr,262*2,tmrproc);
  
      CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
      memcpy(&icsmd,icsm->constr,sizeof(icsmd));
      old_icsm_onMessage=icsmd.onMessage;
      icsmd.onMessage=MyIDLECSM_onMessage;
      old_icsm_onClose=icsmd.onClose;
      icsmd.onClose=MyIDLECSM_onClose;
      icsm->constr=&icsmd;
      
  return 0;
}
