#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"

int GetCLST(void)
{
 return GetUniVars()->keys_vars.keysgreen;
}

void CLST_12(void)
{
 switch (GetCLST()) {
  case 1: CLST1();        break;
  case 2: CallDialList(); CallRecvList(); CallMissedList(); break;
  default: CallDialList(); 
 }
}
static const MENU_DESC CLST3_1 =
{
 NULL,CLST3_1_HNDL,NULL,NULL,
 (int far *)0xBA40E6,(SOFTKEYSTAB*)0xBA40E0,
 0x12,0x00,
 (void huge*)(0xDE2D6A),
 NULL,NULL,0
};

static const MENU_DESC CLST3_2 =
{
 NULL,CLST3_2_HNDL,NULL,NULL,
 (int far *)0xBA40E6,(SOFTKEYSTAB*)0xBA40E0,
 0x12,0x00,
 (void huge*)(0xDE2D6A),
 NULL,NULL, 0
};
static const MENU_DESC CLST3_3  =
{
 NULL,CLST3_3_HNDL,NULL,NULL,
 (int far *)0xBA40E6,(SOFTKEYSTAB*)0xBA40E0,
 0x12,0x00,
 (void huge*)(0xDE2D6A),
 NULL,NULL, 0
};

static const MENU_DESC CLST4_1 =
{
 NULL,CLST4_1_HNDL,NULL,NULL,
 (int far *)0xBA40E6,(SOFTKEYSTAB*)0xBA40E0,
 0x12,0x00,
 (void huge*)(0xDE2D6A),
 NULL,NULL,0
};

static const MENU_DESC CLST4_2 =
{
 NULL,CLST4_2_HNDL,NULL,NULL,
 (int far *)0xBA40E6,(SOFTKEYSTAB*)0xBA40E0,
 0x12,0x00,
 (void huge*)(0xDE2D6A),
 NULL,NULL, 0
};
static const MENU_DESC CLST4_3  =
{
 NULL,CLST4_3_HNDL,NULL,NULL,
 (int far *)0xBA40E6,(SOFTKEYSTAB*)0xBA40E0,
 0x12,0x00,
 (void huge*)(0xDE2D6A),
 NULL,NULL, 0
};

char far* CLST_34(int sel, int type)
{
 if (type==3) {
  switch (sel) {
   case 1: return (char far*)&CLST3_1;
   case 2: return (char far*)&CLST3_2;
   case 3: return (char far*)&CLST3_3;
  }
 } else {
  switch (sel) {
   case 1: return (char far*)&CLST4_1;
   case 2: return (char far*)&CLST4_2;
   case 3: return (char far*)&CLST4_3;
  }
 }
 return 0;
}

int PTTStart(int state)
{
 int sel=-1;
 if (state==KEY_LONG) sel=0;
 if (state==KEY_UP)   sel=4;
 if (sel==-1) return(-1);
 if (GetKeyLockState1()==1) sel+=2; // notlocked
 switch ((GetUniVars()->keys_vars.keysptt_set>>sel)&3) {
  case 0:  return(-1);
  case 1:  MP3Player(1);                        break;
  case 2:  DictaphoneRecording();       break;
  case 3:  VoiceDialing();                      break;
 }
 return(2);
}

int My_MainScreenDialog_onKey(void far* data, char far* key_data)
{
 char keysunlock=uni_vars.keys_vars.keysunlock;
 int unlock_key=uni_vars.keys_vars.unlock_key;
 unsigned char keyswhlock_bit=uni_vars.keys_vars.keyswhlock_bit;
 int unlock_state;
 int key=*(int far*)(0x06+*(char far**)(key_data+0x04));
 int state=*(int far*)(0x04+*(char far**)(key_data+0x04));
//===================================================================================
// PTT Button
 if (key==PTT_BUTTON) return PTTStart(state);
//===================================================================================
// Mainscreen when not locked 
 if (GetKeyLockState1()) {
stdMainScreenDialog_onKey:
  TurnLightIfON();
  return MainScreenDialog_onKey(data,key_data); 
 }
//===================================================================================
//Password for unlock
 if (uni_vars.passwords_vars.bit1&_PASS_UNLOCK) {
  PasswordDialog(0);
  return(2);
 }
//Keys when locked
 if (keyswhlock_bit&_KEY_MP3)   if (key==PLAY_BUTTON)   goto stdMainScreenDialog_onKey;
 if (keyswhlock_bit&_KEY_DICT)  if (key==RECORD_BUTTON) goto stdMainScreenDialog_onKey;
 if (keyswhlock_bit&_KEY_PLUS)  if (key==VOL_UP_BUTTON) goto stdMainScreenDialog_onKey;
 if (keyswhlock_bit&_KEY_UP)    if (key==UP_BUTTON)     goto stdMainScreenDialog_onKey;
 if (keyswhlock_bit&_KEY_DOWN)  if (key==DOWN_BUTTON)   goto stdMainScreenDialog_onKey;
 if (keyswhlock_bit&_KEY_LEFT)  if (key==LEFT_BUTTON)   goto stdMainScreenDialog_onKey;
 if (keyswhlock_bit&_KEY_RIGHT) if (key==RIGHT_BUTTON)  goto stdMainScreenDialog_onKey;
 if (keyswhlock_bit&_LEFT_SOFT)
  if ((key_data[2]==0x44)||(key_data[2]==0x46)||(key_data[2]==0x47)||(key_data[2]==0x4C))
   goto stdMainScreenDialog_onKey;
//===================================================================================
// When locked
 if (key==VOL_DOWN_BUTTON&&state==KEY_LONG) TurnLightON();
// Unlocking
  if ((keysunlock%2)==0) unlock_state=KEY_LONG; else unlock_state=KEY_DOWN;
  if ((state==unlock_state)&&(key==unlock_key)) {
   if (keysunlock<2) DialogConfirmUnlock(); 
    else { 
     UnLockKBD(); 
     doIDLE(); 
    }
   return(2); 
  }
//===================================================================================
 return(-1);

} 

int GetCallMenuState(void)
{
 if (uni_vars.keys_vars.other_bit&_CALLMENU) return 1;
  else return 0;
}

char far* GetCallMenuStr(void)
{
 return (STR)uni_vars.keys_vars.callmenu;
}

void AutoLockAtStart(void)
{
 if (uni_vars.keys_vars.other_bit&_AUTOLOCK_START) ScreenSaverStartAuto();
}

