#include "..\inc\swilib.h"

const int *ban_list;

GBSTMR ban_timer;

void banner(void)
{
  const int *ip=ban_list;
  int ch;
  if (ip)
  {
    while((ch=*ip++))
    {
      ((char*)0xA8E558CE)[ch]=0xEE;
    }
  }
  GBS_StartTimerProc(&ban_timer,20,banner);
}

static int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg)
{
  return 1;
}

void LoadBanList(void)
{
  static const int _banned[]={102,104,707-(512-175),699-(512-175),684-(512-175),685-(512-175),709-(512-175),0};
  ban_list=_banned;
}


static void maincsm_oncreate(CSM_RAM *data)
{
  LoadBanList();
  GBS_StartTimerProc(&ban_timer,20,banner);
}

#pragma segment="ELFBEGIN"
void ElfKiller(void)
{
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(__segment_begin("ELFBEGIN"),(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *csm)
{
  GBS_DelTimer(&ban_timer);
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"ChanBanner");
}

int main(void)
{
  CSMROOT *csmr;
  CSM_RAM *save_cmpc;
  CSM_RAM main_csm;
  //RereadSettings();
  UpdateCSMname();
  LockSched();
  csmr=CSM_root();
  save_cmpc=csmr->csm_q->current_msg_processing_csm;
  csmr->csm_q->current_msg_processing_csm=csmr->csm_q->csm.first;
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  csmr->csm_q->current_msg_processing_csm=save_cmpc;
  ShowMSG(1,(int)"ChanBanner installed!");
  UnlockSched();
  return (0);
}
