#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"

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
      RemoveKeybMsgHook((void *)my_keyhook);
      ShowMSG(1,(int)"XTask отлючен!");
      SUBPROC((void *)ElfKiller);
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

#pragma segment="CONFIG_C"

int LoadConfigData(const char *fname)
{
  int f;
  unsigned int ul;
  char *buf;
  int result=0;
  void *cfg;

  extern const CFG_HDR cfghdr0; //first var in CONFIG
  cfg=(void*)&cfghdr0;

  unsigned int len=(int)__segment_end("CONFIG_C")-(int)__segment_begin("CONFIG_C");

  if (!(buf=malloc(len))) return -1;
  if ((f=fopen(fname,A_ReadOnly+A_BIN,0,&ul))!=-1)
  {
    if (fread(f,buf,len,&ul)==len)
    {
      memcpy(cfg,buf,len);
      fclose(f,&ul);
    }
    else
    {
      fclose(f,&ul);
      goto L_SAVENEWCFG;
    }
  }
  else
  {
  L_SAVENEWCFG:
    if ((f=fopen(fname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=-1)
    {
      if (fwrite(f,cfg,len,&ul)!=len) result=-1;
      fclose(f,&ul);
    }
    else
      result=-1;
  }
  mfree(buf);
  return(result);
}

void main(void)
{
  mode=0;

  if (LoadConfigData("4:\\ZBin\\etc\\XTask.bcfg")>=0)
  {
    LoadConfigData("0:\\ZBin\\etc\\XTask.bcfg");
  }
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
    ws_nogui=AllocWS(256);
    wsprintf(ws_nogui,"%t","Нет GUI!");
    under_idle=(FindCSMbyID(CSM_root()->idle_id))->prev; //Ищем idle_dialog
  }
  UnlockSched();
}
