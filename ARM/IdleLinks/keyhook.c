#include "..\inc\swilib.h"
#include "gui.h"

int hook_proc(int key, int m)
{
  extern const unsigned int CALL_BTN;
  extern const int active;
  void *icsm=FindCSMbyID(CSM_root()->idle_id);
  if ((IsGuiOnTop(((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4]))&&IsUnlocked()&&(m==KEY_DOWN)&&(active)&&(!IsScreenSaver()))
     if (key==CALL_BTN) 
       {
         CreateMainCSM();
       }
  
  return 0;
}

void SetHook()
{
  AddKeybMsgHook((void *)hook_proc);
}

void DestroyHook()
{
  RemoveKeybMsgHook((void *)hook_proc);   
}


