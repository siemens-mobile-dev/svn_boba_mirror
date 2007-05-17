#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

extern const int on_off;
extern const unsigned int volume;
extern const char folder_path[128];

extern const TTime TimeFrom;
extern const TTime TimeTo;

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);


//=============================Проигрывание звука===============================
void Play(const char *fpath, const char *fname)
{
  /*char s[40];
  sprintf(s,RamMediaIsPlaying());
#ifdef NEWSGOLD
  if ((!IsCalling())&&(s[0]!=1)) // для SGOLD s[0]!=2
#else
  if ((!IsCalling())&&(s[0]!=2)) // для SGOLD s[0]!=2    
#endif    
  {*/
  
  WSHDR* sndPath=AllocWS(128);
  WSHDR* sndFName=AllocWS(128);

  wsprintf(sndPath, fpath);
  wsprintf(sndFName, fname);
  
  if (!IsCalling())
  {
    PLAYFILE_OPT _sfo1;
    zeromem(&_sfo1,sizeof(PLAYFILE_OPT));
    _sfo1.repeat_num=1;
    _sfo1.time_between_play=0;
    _sfo1.play_first=0;
    _sfo1.volume=volume;
#ifdef NEWSGOLD
    _sfo1.unk6=1;
    _sfo1.unk7=1;
    _sfo1.unk9=2;
    PlayFile(0x10, sndPath, sndFName, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
#else
#ifdef X75
    _sfo1.unk4=0x80000000;
    _sfo1.unk5=1;
    PlayFile(0xC, sndPath, sndFName, 0, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
#else
    _sfo1.unk5=1;
    PlayFile(0xC, sndPath, sndFName, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
#endif
#endif 

  }

  FreeWS(sndPath);
  FreeWS(sndFName);
  /*}*/
}

//==============================================================================

void RereadSettings()
{
  LockSched();
   InitConfig();  
  UnlockSched(); 
}
//==============================================================================
int my_keyhook(int key, int m)
{
  void *icsm=FindCSMbyID(CSM_root()->idle_id);
  if (on_off)
    if (IsGuiOnTop(((int *)FindCSMbyID(CSM_root()->idle_id))[DISPLACE_OF_EDGUI_ID/4]))  
      {
        TTime CurTime;
        TDate date;
    
        GetDateTime(&date, &CurTime);
        if (((CurTime.hour*60+CurTime.min)<=(TimeTo.hour*60+TimeTo.min))&&
            ((CurTime.hour*60+CurTime.min)>=(TimeFrom.hour*60+TimeFrom.min)))
              {
                if (IsUnlocked()&&(m==KEY_DOWN))
                  {
                    if (key=='#') Play(folder_path, "r.wav");
                    if (key=='*') Play(folder_path, "z.wav");
                
                    if ((key>='0')&&(key<='9'))
                      {
                        char key_name[5];
                        sprintf(key_name, "%d.wav",(key&0xf));  
                        Play(folder_path, key_name);                  
                      }
                  }         
              }
       }
  return 0;
}


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
  csm_result=old_icsm_onMessage(data,msg); //Вызываем старый обработчик событий  
  
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"TalkKbd config updated!");
      RereadSettings();
    }
  }
 
  return(csm_result);
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  RemoveKeybMsgHook((void *)my_keyhook);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  RereadSettings();
  
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;  
  icsm->constr=&icsmd;  
  
  #ifdef NEWSGOLD
    LockSched();
      AddKeybMsgHook((void *)my_keyhook);
    UnlockSched();  
  #else
    LockSched();
      if (!AddKeybMsgHook_end((void *)my_keyhook)) ShowMSG(1, (int) "TalkKbd. Невозможно зарегистрировать обработчик!"); 
    UnlockSched();  
  #endif
    
  return 0;
}
