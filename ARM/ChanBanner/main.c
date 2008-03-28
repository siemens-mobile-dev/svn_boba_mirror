#include "..\inc\swilib.h"

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

typedef struct CHAN_BAN_Q
{
  struct CHAN_BAN_Q *next;
  unsigned int (__interwork*proc)(int chan, unsigned int lvl);
}CHAN_BAN_Q;

char ban_tab[972];

unsigned int banner(int chan, unsigned int lvl)
{
  switch(ban_tab[chan])
  {
  case 1:
    lvl=0xEE;
    break;
  }
  return lvl;
}

CHAN_BAN_Q my_q={NULL,banner};

const char chanbantab_file[]="4:\\ZBin\\var\\chan_ban.tab";

void LoadBanList(void)
{
  unsigned int ul;
  int f;
  memset(ban_tab,0,sizeof(ban_tab));
  if ((f=fopen(chanbantab_file,A_ReadOnly+A_BIN,P_READ,&ul))==-1) return;
  fread(f,ban_tab,sizeof(ban_tab),&ul);
  fclose(f,&ul);
//  static const int _banned[]={102,104,707-(512-175),699-(512-175),684-(512-175),685-(512-175),709-(512-175),0};
//  ban_list=_banned;
}

static int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg)
{
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    if (strcmp_nocase(chanbantab_file,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"ChanBanner config updated!");
      LoadBanList();
    }
  }
  return 1;
}

static void maincsm_oncreate(CSM_RAM *data)
{
  CHAN_BAN_Q *p;
  LoadBanList();
  LockSched();
  p=DATA_N_SFB()->ChanBannerQ;
  if (p)
  {
    my_q.next=p;
  }
  DATA_N_SFB()->ChanBannerQ=&my_q;
  UnlockSched();
}

#pragma segment="ELFBEGIN"
void ElfKiller(void)
{
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(__segment_begin("ELFBEGIN"),(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *csm)
{
  CHAN_BAN_Q *p;
  CHAN_BAN_Q *pp=NULL;
  
  LockSched();
  p=DATA_N_SFB()->ChanBannerQ;  
  while(p)
  {
    if (p->proc==banner)
    {
      if (pp)
      {
	pp->next=p->next;
      }
      else
      {
	DATA_N_SFB()->ChanBannerQ=p->next;
      }
    }
    p=(pp=p)->next;
  }
  UnlockSched();
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
