#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

BATT  bat;
u16   ws[193];
u16   timer; // ID таймера

void elf_exit(void)
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}
 
void onTimer (u16 *timerID , LPARAM lparam)
{
  GetBattaryState((int const*)1 , &bat);
  snwprintf(ws,196,(u16*)L"Temp = %d C",bat.SystemTemperature);
  StatusIndication_ShowNotes(Str2ID(ws,0,SID_ANY_LEN));
  Timer_ReSet(&timer,1000,onTimer,lparam);
}

int NewKey(int key, int r1 , int mode)
{
  if (key==KEY_POWER)
  {
    if(mode==KBD_SHORT_PRESS)
      {
        ModifyKeyHook(NewKey,0);
        Timer_Kill(&timer);
        StatusIndication_ShowNotes(0x6FFFFFFF);
        SUBPROC(elf_exit);
        return(0);
      }   
  }
  return(0);
}
  
int main(u16 *exename,u16 *filename)
{
  ModifyKeyHook(NewKey,1);
  timer=Timer_Set(1000,onTimer,0);
  return(0);
}
