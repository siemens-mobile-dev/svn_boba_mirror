#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

extern char ussd_str[64];

void Killer(void){
  extern void *ELF_BEGIN;
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
//  ((void (*)(void *))(mfree_adr()))(&ELF_BEGIN);
}


void SendKeyPress(int key)
{
  GBS_SendMessage(MMI_CEPID,KEY_DOWN,key);
  GBS_SendMessage(MMI_CEPID,KEY_UP,key);
};

//* 1 2 3 4 5 6
//(6) 1 2 3 4 5 *
//
//
//

CSM_RAM *under_idle;
CSM_RAM *idlecsm;

void swap_csms(CSM_RAM*a,CSM_RAM*b)
{
  if(a==b) return;
  
  void * ap,*an,*bp,*bn;
  ap=a->prev;
  an=a->next;
  bp=b->prev;
  bn=b->next;
  a->prev=bp;
  a->next=bn;
  b->prev=ap;
  b->next=an;
};

CSM_RAM* TopCsmId()
{
CSM_RAM* icsm=under_idle;  
while(icsm->next!=0) icsm=icsm->next;
return icsm;
};

void SwitchToIdle()
{
/*  GBS_SendMessage(MMI_CEPID,KEY_DOWN,RED_BUTTON);
  GBS_SendMessage(MMI_CEPID,LONG_PRESS,RED_BUTTON);
  GBS_SendMessage(MMI_CEPID,KEY_UP,RED_BUTTON);*/ 
swap_csms(TopCsmId(),idlecsm);  
};

void SwitchFromIdle()
{
//swap_csms(TopCsmId(),idlecsm);
};

GBSTMR timer;

void timer_proc(void){
  
 SendKeyPress('*');
 for(char *s=ussd_str;*s!=0;s++)
   SendKeyPress(*s);
 SendKeyPress(GREEN_BUTTON); 
 
 GBS_StartTimerProc(&timer,(262/10)*10, SwitchFromIdle);   
 
 Killer();
}


int main(void)
{     
  InitConfig();

  idlecsm=FindCSMbyID(CSM_root()->idle_id);  
  under_idle=(FindCSMbyID(CSM_root()->idle_id))->prev; 
  
  SwitchToIdle();
  
  GBS_StartTimerProc(&timer,(262/10)*10,timer_proc);   
  
  return 0;
}
