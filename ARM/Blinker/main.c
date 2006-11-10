#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

// Импорт переменных
extern const int bl_key;
extern const int bl_dis;
extern const int use_vib;
extern const int play_snd;
extern const int only_loc;
extern const unsigned int vibra_pow;
extern const unsigned int blink_delay;
extern const unsigned int check_each;
extern const unsigned int blink_light;
extern const int check;

int count;

GBSTMR mytmr;

void LightOff();
void Check(void);

//SetIllumination(unsigned char dev,unsigned long param1,unsigned short bright,unsigned long delay);
//0 - дисплей, 1 - клавиатура, 2 - динамический свет (x65)
void LightOn()
{
  if (bl_key)
    SetIllumination(1,1,blink_light,0);   
  if (bl_dis)
    SetIllumination(0,1,blink_light,0);   
  if (use_vib)
    SetVibration(vibra_pow);    
  GBS_StartTimerProc(&mytmr,blink_delay,LightOff);
}

void LightOff()
{
  if (bl_key)
    SetIllumination(1,1,0,0);   
  if (bl_dis)
    SetIllumination(0,1,0,0);   
  if (use_vib)
    SetVibration(0);    
  count--;
  if (count>0)
  {
    GBS_StartTimerProc(&mytmr,blink_delay,LightOn);
  }
  else
  {
    GBS_StartTimerProc(&mytmr,check_each,Check);    
  }
}

void Check(void)
{
  int missed_cnt=0;
  if ((!IsUnlocked() && only_loc)||(!only_loc))
  {
    if (check==0 || check==2)
      missed_cnt=GetMissedCallsCount();
    if ((check==1 || check==2) && HasNewSMS())
      missed_cnt++;
  }
  if (missed_cnt>0)
  {
    if (play_snd)
    {
/*      int one=1;
      WSHDR *ews, *ews1;
      ews=AllocWS(256);
      ews1=AllocWS(256);
      wsprintf(ews, "%t", "0:\\Sounds");       
      wsprintf(ews1, "%t", "notify.wav");       
      PlayFile(0xc, ews, ews1, 0, 167, &one);
      FreeWS(ews);
      FreeWS(ews1);*/
      PlaySound(0, 0, 0, 60, 0);

    }
    if (bl_key || bl_dis || use_vib)
    {
      count=missed_cnt;
      if (count>3)
        count=3;
      LightOn();
    }
  }
  else
    GBS_StartTimerProc(&mytmr,check_each,Check);
}

int main(void)
{
  InitConfig();
  Check();
  return 0;
}
