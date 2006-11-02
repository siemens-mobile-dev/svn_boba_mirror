#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

CSM_DESC icsmd;
void (*old_icsm_onClose)(CSM_RAM*);

WSHDR *ws_nogui;

CSM_RAM *under_idle;

extern const int RED_BUT_MODE;
extern const int ENA_LONG_PRESS;

extern void kill_data(void *p, void (*func_p)(void *));

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  FreeWS(ws_nogui);
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

extern void do_gui(int);

// -1 - XTask GUI present
// 0 - XTask GUI absent
// 1 - IBUT longpressed, ready for exit
int mode;

// 0 - no press
// 1 - long press REDBUT

int mode_red;

int my_keyhook(int submsg, int msg)
{
  void *icsm=FindCSMbyID(CSM_root()->idle_id);
  if ((submsg==RED_BUTTON)&&(RED_BUT_MODE))
  {
    if (IsGuiOnTop(((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4]))
    {
      if (msg==KEY_UP)
      {
	if (mode_red)
	{
	  mode_red=0;
	  return(2);
	}
      }
      mode_red=0;
    }
    else
    {
      if (msg==KEY_DOWN)
      {
	if (mode_red==1)
	{
	  mode_red=0;
	  return(0); //Long press, continue with REDB PRESS
	}
      }
      if (msg==KEY_UP)
      {
	if (mode_red==1)
	{
	  mode_red=0; //Release after longpress
	  return(0);
	}
	//Release after short press
	if (mode==0)
	{
          if (RED_BUT_MODE==1)
          {
            GBS_SendMessage(MMI_CEPID,KEY_DOWN,RIGHT_SOFT);
          }
          else
          {
            do_gui(1);
          }
	}
      }
      if (msg==LONG_PRESS)
      {
	mode_red=1;
	GBS_SendMessage(MMI_CEPID,KEY_DOWN,RED_BUTTON);
      }
      return(2);
    }
  }
  if (submsg!=INTERNET_BUTTON) return(0);
  if (mode==-1)
  {
    if (msg==KEY_UP)
    {
      GBS_SendMessage(MMI_CEPID,KEY_DOWN,ENTER_BUTTON);
    }
    return(2);
  }
  switch(msg)
  {
  case KEY_DOWN:
    break;
  case KEY_UP:
    if (mode==1)
    {
      mode=0;
      //RemoveKeybMsgHook((void *)my_keyhook);
      //ShowMSG(1,(int)"XTask отлючен!");
      //SUBPROC((void *)ElfKiller);
      break;
    }
    {
      do_gui(0);
    }
    break;
  case LONG_PRESS:
    if (ENA_LONG_PRESS) mode=1;
  }
  return(2);
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  FreeWS(ws_nogui);
  RemoveKeybMsgHook((void *)my_keyhook);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

void main(void)
{
  mode=0;
  InitConfig();
  LockSched();
  if (!AddKeybMsgHook_end((void *)my_keyhook))
  {
    ShowMSG(1,(int)"Невозможно зарегистрировать обработчик!");
    SUBPROC((void *)ElfKiller);
  }
  else
  {
    extern const int ENA_HELLO_MSG;
    if (ENA_HELLO_MSG) ShowMSG(1,(int)"XTask установлен!");
    {
      CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
      memcpy(&icsmd,icsm->constr,sizeof(icsmd));
      old_icsm_onClose=icsmd.onClose;
      icsmd.onClose=MyIDLECSM_onClose;
      icsm->constr=&icsmd;
    }
    ws_nogui=AllocWS(256);
    wsprintf(ws_nogui,"%t","Нет GUI!");
    under_idle=(FindCSMbyID(CSM_root()->idle_id))->prev; //Ищем idle_dialog
  }
  UnlockSched();
}
