#include "..\swilib.h"

WSHDR *ws_nogui;

CSM_RAM *under_idle;

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  FreeWS(ws_nogui);
  ((void (*)(void *))(mfree_adr()))(&ELF_BEGIN);
}

int mode;
int mode_red;

int my_keyhook(int submsg, int msg)
{
/*  void *icsm=FindCSMbyID(CSM_root()->idle_id);
  if (submsg==RED_BUTTON)
  {
    if (IsGuiOnTop(((int *)icsm)[0x2C/4]))
    {
      if (msg==KEY_UP)
      {
	if (mode_red)
	{
	  mode_red=0;
	  return(2);
	}
      }
    }
    else
    {
      if (msg==KEY_DOWN)
      {
	if (mode_red==1)
	{
	  mode_red=0;
	  return(0);
	}
	GBS_SendMessage(MMI_CEPID,KEY_DOWN,RIGHT_SOFT);
	mode_red=2;
	//GeneralFuncF0(0);
      }
      if (msg==LONG_PRESS)
      {
	mode_red=1;
	GBS_SendMessage(MMI_CEPID,KEY_DOWN,RED_BUTTON);
      }
      return(2);
    }
  }*/
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
      RemoveKeybMsgHook((void *)my_keyhook);
      ShowMSG(1,(int)"XTask отлючен!");
      SUBPROC((void *)ElfKiller);
      break;
    }
    {
      extern void do_gui(void);
      do_gui();
    }
    break;
  case LONG_PRESS:
    mode=1;
  }
  return(2);
}

void main(void)
{
  mode=0;
  LockSched();
  if (!AddKeybMsgHook_end((void *)my_keyhook))
  {
    ShowMSG(1,(int)"Невозможно зарегистрировать обработчик!");
    SUBPROC((void *)ElfKiller);
  }
  else
  {
    ShowMSG(1,(int)"XTask установлен!");
    ws_nogui=AllocWS(256);
    wsprintf(ws_nogui,"%t","Нет GUI!");
    under_idle=(FindCSMbyID(CSM_root()->idle_id))->prev; //Ищем idle_dialog
  }
  UnlockSched();
}
