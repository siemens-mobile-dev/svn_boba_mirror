#include "..\inc\swilib.h"
#include "conf_loader.h"

#define RECONNECT_TIME (216*60)

CSM_DESC icsmd;
extern const int ENA_GPRSD;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

char binary_profile[0x204];

/*
Del by Kibab - бинарные профили конфигурируются
*/

NAP_PARAM_CONT *nap_container;

GBSTMR mytmr;
void reconnect(void)
{
  void do_connect(void);
  SUBPROC((void *)do_connect);
}

void do_connect(void)
{
  REGSOCKCEPID_DATA rsc;
  LMAN_DATA lmd;
  NAP_PARAM_CONT *nc;
  // Проверим, включён ли GPRS. Если нет - дальнейшие извращения бесмысленны
  if(!IsGPRSEnabled()){return;}
  
  // Перечитываем конфиг, дабы проверить, не откючены ли мы
  InitConfig();
  if(!ENA_GPRSD)
  {
    GBS_StartTimerProc(&mytmr,RECONNECT_TIME,reconnect);
    return; // Если не разрешён коннект, выходим
  }

  //Устанавливаем соединение
  rsc._0x0080=0x0080;
  rsc._0xFFFF=0xFFFF;
  rsc._0x0000=0x0000;
  RegisterCepidForSocketWork(&rsc);
  nap_container=nc=malloc(sizeof(NAP_PARAM_CONT));
  nc->len=sizeof(NAP_PARAM_CONT);
  nc->ctx_napdef=&(nc->napdef);
  nc->zero=0;
  FillProfile((char*)(&binary_profile)); // ADDED BY Kibab
  memcpy(&nc->napdef,binary_profile,sizeof(CTX_NAPDEF));
  lmd.zero=0;
  lmd.native_profile=NULL;
  lmd._1=1;
  lmd._4=4;
  lmd._0=0;
  lmd.NapParameterContainer=nc;
  RequestLMANConnect(&lmd);
  //  GBS_StartTimerProc(&mytmr,RECONNECT_TIME*2,reconnect);
}

void LogWriter(const char *s)
{
  TTime t;
  TDate d;
  char ss[100];
  unsigned int ul;
  int f=fopen("4:\\ZBin\\etc\\GprsD.log",A_ReadWrite+A_Create+A_Append,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    GetDateTime(&d,&t);
    sprintf(ss,"%02d:%02d %02d-%02d-%04d: ",
            t.hour,t.min,d.day,d.month,d.year);
    fwrite(f,ss,strlen(ss),&ul);
    fwrite(f,s,strlen(s),&ul);
    fwrite(f,"\r\n",2,&ul);
    fclose(f,&ul);
  }
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  char s[100];
  csm_result=old_icsm_onMessage(data,msg); //Вызываем старый обработчик событий
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    int m=(int)msg->data0;
    if (m==LMAN_CONNECT_CNF)
    {
      strcpy(s,"Session started!");
      LogWriter(s);
      ShowMSG(1,(int)s);
    }
    else
    {
      if ((m==LMAN_DISCONNECT_IND)||(m==LMAN_CONNECT_REJ_IND))
      {
        if (m==LMAN_DISCONNECT_IND) strcpy(s,"Session closed!");
        else
          strcpy(s,"Can't start session!");
        LogWriter(s);
        ShowMSG(1,(int)s);
        GBS_StartTimerProc(&mytmr,RECONNECT_TIME,reconnect);
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
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main()
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
  InitConfig(); // ADDED BY Kibab
  SUBPROC((void*)do_connect);
  return 0;
}
