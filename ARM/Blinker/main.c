#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

#define TICK 216
#define IN_TICKS(sec) (sec * TICK)

#define CFGUPD_INTV 5

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

// Импорт переменных
extern const int cfgEvents;
extern const unsigned int cfgMaxEv;
extern const unsigned int cfgInterval;
extern const int cfgLockOnly;
extern const unsigned int cfgPeriod;
extern const unsigned int cfgBright;

extern const int use_vib;
extern const unsigned int vibra_pow;
extern const int use_snd;
extern const unsigned int sndNum;

extern const int cfgKbd;
extern const int cfgDispl;
#ifndef NEWSGOLD
extern const int cfgDyn;
#else
extern const int cfgLighter;
#endif

int count = 0;
//int cycl_cnt = 0;

GBSTMR mytmr;

void LightOff(void);
void Check(void);

//SetIllumination(unsigned char dev,unsigned long param1,unsigned short bright,unsigned long delay);
//0 - дисплей, 1 - клавиатура, 2 - динамический свет (x65)
void LightOn()
{
	if (cfgDispl)
		SetIllumination(0, 1, cfgBright, 0);
	if (cfgKbd)
		SetIllumination(1, 1, cfgBright, 0);   
#ifndef NEWSGOLD
	if (cfgDyn)
		SetIllumination(2, 1, cfgBright, 0);
#else
	if (cfgLighter)
		SetIllumination(4, 1, cfgBright, 0);
#endif
  if (use_vib)
    SetVibration(vibra_pow);    
  
	GBS_StartTimerProc(&mytmr, IN_TICKS(cfgPeriod) / 20, LightOff);
}

void LightOff()
{
	if (cfgDispl)
		SetIllumination(0, 1, 0, 0);
	if (cfgKbd)
		SetIllumination(1, 1, 0, 0);   
#ifndef NEWSGOLD
	if (cfgDyn)
		SetIllumination(2, 1, 0, 0);
#else
	if (cfgLighter)
		SetIllumination(4, 1, 0, 0);
#endif
  if (use_vib)
    SetVibration(0); 
  
	if (--count)
		GBS_StartTimerProc(&mytmr, IN_TICKS(cfgPeriod) / 20, LightOn);
	else
		GBS_StartTimerProc(&mytmr, IN_TICKS(cfgInterval), Check); 
}
#ifdef NEWSGOLD
#pragma swi_number=0x9E
__swi __arm GetMissedEventCount(unsigned int Event);
#endif

void Check(void)
{
	if (!(IsUnlocked() && cfgLockOnly))
	{
#ifdef NEWSGOLD          
		if ((count = GetMissedEventCount(cfgEvents)) > 0
#else
                if (cfgEvents==0 || cfgEvents==2)
                     count=GetMissedCallsCount();
                if ((cfgEvents==1 || cfgEvents==2) && HasNewSMS())
                      count++;
                if ((count) > 0
#endif
			&& (cfgDispl || cfgKbd || 
#ifndef NEWSGOLD
				cfgDyn)
#else
				cfgLighter)
#endif
			)
		{
			if (count > cfgMaxEv)
				count = cfgMaxEv;
			LightOn();
                        if (use_snd)
                          PlaySound(0, 0, 0, sndNum, 0);
			return;
		}
	}

	GBS_StartTimerProc(&mytmr, IN_TICKS(cfgInterval), Check);
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

  csm_result=old_icsm_onMessage(data,msg);
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"Blinker config updated!");
      InitConfig();
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

int main(void)
{
  //ShowMSG(1, (int)"Blinker started!");
  InitConfig();

  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;  
  icsm->constr=&icsmd;        
        
  Check();
  return 0;
}
