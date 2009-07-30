#include "..\inc\swilib.h"
#include "main.h"
#include "conf_loader.h"
#include "keyhook.h"
#include "revision.h"
#include "utils.h"
#include "idle.h"

#define UTF16_ALIGN_RIGHT (0xE01D)

const int minus11=-11;

unsigned short NSD_name_body[140];

void NSD_onCreate(CSM_RAM *data)
{
  LockSched();
  RereadSettings();
  SetHook();
  UnlockSched();
}

int NSD_onMessage(CSM_RAM* data, GBS_MSG* msg)
{
  
  extern const unsigned int KBD_STATE;
  extern const int active;
  extern const int count;
  

  if(msg->msg == MSG_RECONFIGURE_REQ)
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      RereadSettings();
      ShowMSG(1,(int)"IdleLinks config updated!");
    }
  }
 
  if ((KBD_STATE==0) && (IsUnlocked())) return 1;
  if ((KBD_STATE==1) && (!IsUnlocked())) return 1;

  void *icsm=FindCSMbyID(CSM_root()->idle_id);
  if ((IsGuiOnTop(((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4]))&&(active)&&(!IsScreenSaver()))
  {
  int i=0;
  do
  {
    Draw_NSD(i);  
    i++;
  }
  while (i<=count);  
  }  
  return 1;
}  

void NSD_onClose(CSM_RAM *data)
{
  DestroyHook();
  SUBPROC((void *)ElfKiller);
}

const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}NSDCSM =
{
  {
  NSD_onMessage,  
  NSD_onCreate, 
#ifdef NEWSGOLD
  0,
  0,
  0,
  0,
#endif
  NSD_onClose, 
  sizeof(MAIN_CSM),
  1,
  &minus11
  },
  {
    NSD_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};

void NSD_SetName(void)
{
  wsprintf((WSHDR *)(&NSDCSM.maincsm_name),"IdleLinks %cr%d",UTF16_ALIGN_RIGHT, __SVN_REVISION__);
}

void NSD_Create()
{
  CSM_RAM *save_cmpc;
  char dummy[sizeof(MAIN_CSM)];
  LockSched();
  save_cmpc=CSM_root()->csm_q->current_msg_processing_csm;
  CSM_root()->csm_q->current_msg_processing_csm=CSM_root()->csm_q->csm.first;
  CreateCSM(&NSDCSM.maincsm,dummy,0);
  CSM_root()->csm_q->current_msg_processing_csm=save_cmpc;
  UnlockSched();
  NSD_SetName();
}



